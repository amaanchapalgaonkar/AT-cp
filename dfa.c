#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dfa.h"

/* ═══════════════════════════════════════════════════════════
   dfa.c
   Stage 3 — Subset Construction  (NFA → DFA)
   Stage 4 — Hopcroft Minimization (DFA → Min-DFA)
   ═══════════════════════════════════════════════════════════ */

/* ─────────────────────────────────────────────
   DFA memory management
   ───────────────────────────────────────────── */

DFA *dfa_create(int token_type) {
    DFA *dfa = calloc(1, sizeof(DFA));
    if (!dfa) { fprintf(stderr, "dfa_create: out of memory\n"); exit(1); }
    dfa->start       = 0;
    dfa->state_count = 0;
    dfa->token_type  = token_type;
    return dfa;
}

void dfa_free(DFA *dfa) {
    if (!dfa) return;
    for (int i = 0; i < dfa->state_count; i++)
        free(dfa->states[i]);
    free(dfa);
}

DFAState *dfa_new_state(DFA *dfa) {
    if (dfa->state_count >= MAX_DFA_STATES) {
        fprintf(stderr, "dfa_new_state: state limit reached\n");
        exit(1);
    }
    DFAState *s = calloc(1, sizeof(DFAState));
    if (!s) { fprintf(stderr, "dfa_new_state: out of memory\n"); exit(1); }
    s->id         = dfa->state_count;
    s->is_accept  = 0;
    s->token_type = DEAD_STATE;
    for (int i = 0; i < ALPHA_SIZE; i++) s->next[i] = DEAD_STATE;
    dfa->states[dfa->state_count++] = s;
    return s;
}

/* ─────────────────────────────────────────────
   Subset Construction helpers
   ───────────────────────────────────────────── */

#define MAX_SUBSETS 1024

typedef struct {
    int nfa_ids[MAX_NFA_STATES];
    int count;
    int dfa_id;
} Subset;

static Subset subsets[MAX_SUBSETS];
static int    subset_count = 0;

static int subset_equal(int *a, int na, int *b, int nb) {
    if (na != nb) return 0;
    for (int i = 0; i < na; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

static void sort_ids(int *ids, int n) {
    for (int i = 1; i < n; i++) {
        int key = ids[i], j = i - 1;
        while (j >= 0 && ids[j] > key) { ids[j+1] = ids[j]; j--; }
        ids[j+1] = key;
    }
}

static int find_or_add_subset(int *ids, int n, int *is_new) {
    int sorted[MAX_NFA_STATES];
    memcpy(sorted, ids, n * sizeof(int));
    sort_ids(sorted, n);

    for (int i = 0; i < subset_count; i++) {
        if (subset_equal(subsets[i].nfa_ids, subsets[i].count, sorted, n)) {
            *is_new = 0;
            return subsets[i].dfa_id;
        }
    }
    *is_new = 1;
    Subset *sub = &subsets[subset_count++];
    memcpy(sub->nfa_ids, sorted, n * sizeof(int));
    sub->count  = n;
    sub->dfa_id = subset_count - 1;
    return sub->dfa_id;
}

static int set_has_accept(NFA *nfa, StateSet *set) {
    for (int i = 0; i < set->count; i++) {
        int sid = set->ids[i];
        for (int j = 0; j < nfa->state_count; j++) {
            if (nfa->states[j]->id == sid && nfa->states[j]->is_accept)
                return 1;
        }
    }
    return 0;
}

/* ─────────────────────────────────────────────
   Stage 3 — Subset Construction  (NFA → DFA)
   ───────────────────────────────────────────── */
DFA *dfa_from_nfa(NFA *nfa, int token_type) {
    DFA *dfa = dfa_create(token_type);
    subset_count = 0;

    /* Start: ε-closure of NFA start state */
    int start_arr[1] = { nfa->start->id };
    StateSet start_set = nfa_epsilon_closure(nfa, start_arr, 1);

    int is_new;
    find_or_add_subset(start_set.ids, start_set.count, &is_new);

    /* Worklist (queue of subset indices to process) */
    int worklist[MAX_SUBSETS];
    int wl_head = 0, wl_tail = 0;
    worklist[wl_tail++] = 0;

    /* Pre-create start DFA state */
    DFAState *s0 = dfa_new_state(dfa);
    s0->is_accept  = set_has_accept(nfa, &start_set);
    s0->token_type = s0->is_accept ? token_type : DEAD_STATE;

    while (wl_head < wl_tail) {
        int sub_idx  = worklist[wl_head++];
        Subset *sub  = &subsets[sub_idx];

        /* Build a StateSet from this subset's NFA ids */
        StateSet cur_set;
        cur_set.count = sub->count;
        memcpy(cur_set.ids, sub->nfa_ids, sub->count * sizeof(int));

        for (int c = 1; c < ALPHA_SIZE; c++) {
            StateSet moved   = nfa_move(nfa, &cur_set, (char)c);
            if (moved.count == 0) continue;

            StateSet closure = nfa_epsilon_closure(nfa, moved.ids, moved.count);
            if (closure.count == 0) continue;

            int new_flag;
            int dst_id = find_or_add_subset(closure.ids, closure.count, &new_flag);

            if (new_flag) {
                DFAState *ns = dfa_new_state(dfa);
                ns->is_accept  = set_has_accept(nfa, &closure);
                ns->token_type = ns->is_accept ? token_type : DEAD_STATE;
                worklist[wl_tail++] = dst_id;
            }

            if (sub_idx < dfa->state_count)
                dfa->states[sub_idx]->next[c] = dst_id;
        }
    }

    dfa->start = 0;
    return dfa;
}

/* ─────────────────────────────────────────────
   Stage 4 — Hopcroft's Minimization
   ───────────────────────────────────────────── */

#define MAX_GROUPS MAX_DFA_STATES

typedef struct {
    int members[MAX_DFA_STATES];
    int count;
} Group;

static Group groups[MAX_GROUPS];
static int   group_count;
static int   state_to_group[MAX_DFA_STATES];

static void group_add(int gid, int sid) {
    groups[gid].members[groups[gid].count++] = sid;
    state_to_group[sid] = gid;
}

DFA *dfa_minimize(DFA *dfa) {
    int n = dfa->state_count;
    if (n == 0) return dfa;

    /* Initial partition: accepting vs non-accepting */
    group_count = 2;
    memset(groups, 0, sizeof(groups));

    for (int i = 0; i < n; i++) {
        if (dfa->states[i]->is_accept)
            group_add(1, i);
        else
            group_add(0, i);
    }

    /* Iteratively refine */
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int g = 0; g < group_count; g++) {
            if (groups[g].count <= 1) continue;

            for (int c = 1; c < ALPHA_SIZE; c++) {
                int first = groups[g].members[0];
                int dst0  = dfa->states[first]->next[c];
                int grp0  = (dst0 == DEAD_STATE) ? -1 : state_to_group[dst0];

                int split = 0;
                for (int mi = 1; mi < groups[g].count; mi++) {
                    int sid = groups[g].members[mi];
                    int dst = dfa->states[sid]->next[c];
                    int grp = (dst == DEAD_STATE) ? -1 : state_to_group[dst];
                    if (grp != grp0) { split = 1; break; }
                }
                if (!split) continue;

                /* Partition group g */
                int stay[MAX_DFA_STATES], stay_n = 0;
                int move[MAX_DFA_STATES], move_n = 0;

                for (int mi = 0; mi < groups[g].count; mi++) {
                    int sid = groups[g].members[mi];
                    int dst = dfa->states[sid]->next[c];
                    int grp = (dst == DEAD_STATE) ? -1 : state_to_group[dst];
                    if (grp == grp0) stay[stay_n++] = sid;
                    else             move[move_n++] = sid;
                }

                groups[g].count = stay_n;
                memcpy(groups[g].members, stay, stay_n * sizeof(int));
                for (int mi = 0; mi < stay_n; mi++)
                    state_to_group[stay[mi]] = g;

                int ng = group_count++;
                groups[ng].count = move_n;
                memcpy(groups[ng].members, move, move_n * sizeof(int));
                for (int mi = 0; mi < move_n; mi++)
                    state_to_group[move[mi]] = ng;

                changed = 1;
                break;
            }
        }
    }

    /* Build minimized DFA */
    DFA *min = dfa_create(dfa->token_type);
    for (int g = 0; g < group_count; g++) {
        if (groups[g].count == 0) continue;
        DFAState *ns = dfa_new_state(min);
        int rep = groups[g].members[0];
        ns->is_accept  = dfa->states[rep]->is_accept;
        ns->token_type = dfa->states[rep]->token_type;
        for (int c = 0; c < ALPHA_SIZE; c++) {
            int dst    = dfa->states[rep]->next[c];
            ns->next[c] = (dst == DEAD_STATE) ? DEAD_STATE : state_to_group[dst];
        }
    }
    min->start = state_to_group[dfa->start];

    dfa_free(dfa);
    return min;
}

/* ─────────────────────────────────────────────
   Build all token DFAs
   ───────────────────────────────────────────── */
DFATable *dfa_table_build(void) {
    DFATable *table = calloc(1, sizeof(DFATable));
    if (!table) { fprintf(stderr, "dfa_table_build: out of memory\n"); exit(1); }
    for (int t = 0; t < TOKEN_COUNT; t++) {
        NFA *nfa       = nfa_from_regex(TOKEN_PATTERNS[t]);
        DFA *dfa       = dfa_from_nfa(nfa, t);
        dfa            = dfa_minimize(dfa);
        nfa_free(nfa);
        table->dfas[t] = dfa;
    }
    return table;
}

void dfa_table_free(DFATable *table) {
    if (!table) return;
    for (int t = 0; t < TOKEN_COUNT; t++)
        dfa_free(table->dfas[t]);
    free(table);
}

/* ─────────────────────────────────────────────
   DFA simulation — longest match from input[offset]
   ───────────────────────────────────────────── */
int dfa_simulate(DFA *dfa, const char *input, int offset) {
    int cur              = dfa->start;
    int last_accept_len  = 0;
    int len              = 0;

    while (input[offset + len] != '\0') {
        unsigned char c = (unsigned char)input[offset + len];
        if (c >= ALPHA_SIZE) { len++; continue; }

        int nxt = dfa->states[cur]->next[c];
        if (nxt == DEAD_STATE) break;

        cur = nxt;
        len++;

        if (dfa->states[cur]->is_accept)
            last_accept_len = len;
    }
    return last_accept_len;
}

/* ─────────────────────────────────────────────
   Debug printer
   ───────────────────────────────────────────── */
void dfa_print(const DFA *dfa) {
    printf("DFA: %d states, start=q%d, token=%d\n",
           dfa->state_count, dfa->start, dfa->token_type);
    for (int i = 0; i < dfa->state_count; i++) {
        DFAState *s = dfa->states[i];
        printf("  q%d%s\n", s->id, s->is_accept ? " [ACCEPT]" : "");
        for (int c = 32; c < 127; c++) {
            if (s->next[c] != DEAD_STATE)
                printf("    '%c' -> q%d\n", c, s->next[c]);
        }
    }
}
