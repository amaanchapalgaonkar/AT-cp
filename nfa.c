#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nfa.h"

#define WILDCARD '\x01'

typedef struct {
    const char *src;
    int         pos;
    NFA        *nfa;
} Parser;

static NFAFragment parse_expr(Parser *p);
static NFAFragment parse_concat(Parser *p);
static NFAFragment parse_quantifier(Parser *p);
static NFAFragment parse_atom(Parser *p);

NFA *nfa_create(void) {
    NFA *nfa = calloc(1, sizeof(NFA));
    if (!nfa) { fprintf(stderr, "nfa_create: out of memory\n"); exit(1); }
    return nfa;
}

void nfa_free(NFA *nfa) {
    if (!nfa) return;
    for (int i = 0; i < nfa->state_count; i++) free(nfa->states[i]);
    free(nfa);
}

NFAState *nfa_new_state(NFA *nfa) {
    if (nfa->state_count >= MAX_NFA_STATES) {
        fprintf(stderr, "nfa_new_state: state limit reached\n"); exit(1);
    }
    NFAState *s = calloc(1, sizeof(NFAState));
    if (!s) { fprintf(stderr, "nfa_new_state: out of memory\n"); exit(1); }
    s->id = nfa->state_count;
    nfa->states[nfa->state_count++] = s;
    return s;
}

void nfa_add_transition(NFAState *src, char c, NFAState *dst) {
    if (src->trans_count >= 2) {
        fprintf(stderr, "nfa_add_transition: exceeded 2-edge limit\n"); exit(1);
    }
    src->trans_char[src->trans_count] = c;
    src->trans[src->trans_count]      = dst;
    src->trans_count++;
}

static NFAFragment frag_char(NFA *nfa, char c) {
    NFAState *s = nfa_new_state(nfa);
    NFAState *a = nfa_new_state(nfa);
    nfa_add_transition(s, c, a);
    a->is_accept = 1;
    return (NFAFragment){s, a};
}

static NFAFragment frag_wildcard(NFA *nfa) {
    return frag_char(nfa, WILDCARD);
}

static NFAFragment frag_union(NFA *nfa, NFAFragment f1, NFAFragment f2) {
    NFAState *start  = nfa_new_state(nfa);
    NFAState *accept = nfa_new_state(nfa);
    accept->is_accept = 1;
    f1.accept->is_accept = 0;
    f2.accept->is_accept = 0;
    nfa_add_transition(start,     EPSILON, f1.start);
    nfa_add_transition(start,     EPSILON, f2.start);
    nfa_add_transition(f1.accept, EPSILON, accept);
    nfa_add_transition(f2.accept, EPSILON, accept);
    return (NFAFragment){start, accept};
}

static NFAFragment frag_class(NFA *nfa, const char *chars, int n) {
    if (n == 0) {
        NFAState *s = nfa_new_state(nfa);
        s->is_accept = 1;
        return (NFAFragment){s, s};
    }
    NFAFragment result = frag_char(nfa, chars[0]);
    for (int i = 1; i < n; i++)
        result = frag_union(nfa, result, frag_char(nfa, chars[i]));
    return result;
}

static NFAFragment frag_concat(NFA *nfa, NFAFragment f1, NFAFragment f2) {
    (void)nfa;
    f1.accept->is_accept = 0;
    nfa_add_transition(f1.accept, EPSILON, f2.start);
    return (NFAFragment){f1.start, f2.accept};
}

static NFAFragment frag_star(NFA *nfa, NFAFragment f) {
    NFAState *start  = nfa_new_state(nfa);
    NFAState *accept = nfa_new_state(nfa);
    accept->is_accept = 1;
    f.accept->is_accept = 0;
    nfa_add_transition(start,     EPSILON, f.start);
    nfa_add_transition(start,     EPSILON, accept);
    nfa_add_transition(f.accept,  EPSILON, f.start);
    nfa_add_transition(f.accept,  EPSILON, accept);
    return (NFAFragment){start, accept};
}

static NFAFragment frag_plus(NFA *nfa, NFAFragment f) {
    NFAState *accept = nfa_new_state(nfa);
    accept->is_accept = 1;
    f.accept->is_accept = 0;
    nfa_add_transition(f.accept, EPSILON, f.start);
    nfa_add_transition(f.accept, EPSILON, accept);
    return (NFAFragment){f.start, accept};
}

static NFAFragment frag_question(NFA *nfa, NFAFragment f) {
    NFAState *start  = nfa_new_state(nfa);
    NFAState *accept = nfa_new_state(nfa);
    accept->is_accept = 1;
    f.accept->is_accept = 0;
    nfa_add_transition(start,     EPSILON, f.start);
    nfa_add_transition(start,     EPSILON, accept);
    nfa_add_transition(f.accept,  EPSILON, accept);
    return (NFAFragment){start, accept};
}

static int parse_char_class(Parser *p, char *buf) {
    int n = 0;
    if (p->src[p->pos] == '^') p->pos++;

    while (p->src[p->pos] && p->src[p->pos] != ']') {
        char lo = p->src[p->pos++];
        if (lo == '\\' && p->src[p->pos]) {
            lo = p->src[p->pos++];
            if      (lo == 'n') lo = '\n';
            else if (lo == 't') lo = '\t';
            else if (lo == 'r') lo = '\r';
            buf[n++] = lo;
        } else if (p->src[p->pos] == '-' && p->src[p->pos+1] != ']') {
            p->pos++;
            char hi = p->src[p->pos++];
            for (char c = lo; c <= hi; c++) buf[n++] = c;
        } else {
            buf[n++] = lo;
        }
    }
    if (p->src[p->pos] == ']') p->pos++;
    return n;
}

static NFAFragment parse_expr(Parser *p) {
    NFAFragment left = parse_concat(p);
    while (p->src[p->pos] == '|') {
        p->pos++;
        NFAFragment right = parse_concat(p);
        left = frag_union(p->nfa, left, right);
    }
    return left;
}

static NFAFragment parse_concat(Parser *p) {
    NFAFragment result = parse_quantifier(p);
    while (p->src[p->pos] && p->src[p->pos] != '|' && p->src[p->pos] != ')') {
        NFAFragment next = parse_quantifier(p);
        result = frag_concat(p->nfa, result, next);
    }
    return result;
}

static NFAFragment parse_quantifier(Parser *p) {
    NFAFragment f = parse_atom(p);
    char q = p->src[p->pos];
    if      (q == '*') { p->pos++; f = frag_star(p->nfa, f);     }
    else if (q == '+') { p->pos++; f = frag_plus(p->nfa, f);     }
    else if (q == '?') { p->pos++; f = frag_question(p->nfa, f); }
    return f;
}

static NFAFragment parse_atom(Parser *p) {
    char c = p->src[p->pos];

    if (c == '(') {
        p->pos++;
        NFAFragment f = parse_expr(p);
        if (p->src[p->pos] == ')') p->pos++;
        return f;
    }
    if (c == '[') {
        p->pos++;
        char buf[256];
        int n = parse_char_class(p, buf);
        return frag_class(p->nfa, buf, n);
    }
    if (c == '.') { p->pos++; return frag_wildcard(p->nfa); }
    if (c == '\\' && p->src[p->pos+1]) {
        p->pos++;
        char esc = p->src[p->pos++];
        if      (esc == 'n') esc = '\n';
        else if (esc == 't') esc = '\t';
        else if (esc == 'r') esc = '\r';
        return frag_char(p->nfa, esc);
    }
    p->pos++;
    return frag_char(p->nfa, c);
}

NFA *nfa_from_regex(const char *regex) {
    NFA *nfa    = nfa_create();
    Parser p    = { regex, 0, nfa };
    NFAFragment f = parse_expr(&p);
    nfa->start  = f.start;
    nfa->accept = f.accept;
    f.accept->is_accept = 1;
    return nfa;
}

static int set_contains(StateSet *s, int id) {
    for (int i = 0; i < s->count; i++)
        if (s->ids[i] == id) return 1;
    return 0;
}

StateSet nfa_epsilon_closure(NFA *nfa, int *ids, int count) {
    StateSet result; result.count = 0;
    int stack[MAX_NFA_STATES], top = 0;

    for (int i = 0; i < count; i++) {
        if (!set_contains(&result, ids[i])) {
            result.ids[result.count++] = ids[i];
            stack[top++] = ids[i];
        }
    }
    while (top > 0) {
        int cur_id = stack[--top];
        NFAState *cur = NULL;
        for (int i = 0; i < nfa->state_count; i++)
            if (nfa->states[i]->id == cur_id) { cur = nfa->states[i]; break; }
        if (!cur) continue;
        for (int t = 0; t < cur->trans_count; t++) {
            if (cur->trans_char[t] == EPSILON) {
                int nid = cur->trans[t]->id;
                if (!set_contains(&result, nid)) {
                    result.ids[result.count++] = nid;
                    stack[top++] = nid;
                }
            }
        }
    }
    return result;
}

StateSet nfa_move(NFA *nfa, StateSet *set, char c) {
    StateSet result; result.count = 0;
    for (int i = 0; i < set->count; i++) {
        int sid = set->ids[i];
        NFAState *s = NULL;
        for (int j = 0; j < nfa->state_count; j++)
            if (nfa->states[j]->id == sid) { s = nfa->states[j]; break; }
        if (!s) continue;
        for (int t = 0; t < s->trans_count; t++) {
            char tc = s->trans_char[t];
            if (tc == c || (tc == WILDCARD && c != '\0')) {
                int nid = s->trans[t]->id;
                if (!set_contains(&result, nid))
                    result.ids[result.count++] = nid;
            }
        }
    }
    return result;
}

void nfa_print(const NFA *nfa) {
    printf("NFA: %d states, start=q%d, accept=q%d\n",
           nfa->state_count, nfa->start->id, nfa->accept->id);
    for (int i = 0; i < nfa->state_count; i++) {
        NFAState *s = nfa->states[i];
        for (int t = 0; t < s->trans_count; t++) {
            char c = s->trans_char[t];
            if      (c == EPSILON)  printf("  q%d --ε--> q%d\n", s->id, s->trans[t]->id);
            else if (c == WILDCARD) printf("  q%d --.--> q%d\n", s->id, s->trans[t]->id);
            else                    printf("  q%d --%c--> q%d\n", s->id, c, s->trans[t]->id);
        }
        if (s->is_accept) printf("  q%d [ACCEPT]\n", s->id);
    }
}
