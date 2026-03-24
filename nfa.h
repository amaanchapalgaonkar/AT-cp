#ifndef NFA_H
#define NFA_H

#include <stddef.h>

/* ═══════════════════════════════════════════════════════════
   NFA (Nondeterministic Finite Automaton)
   Built via Thompson's Construction from a regex pattern.

   Key properties of Thompson NFAs:
   - Each state has at most 2 outgoing transitions
   - Epsilon (ε) transitions are represented by EPSILON sentinel
   - Every NFA has exactly one start and one accept state
   ═══════════════════════════════════════════════════════════ */

#define EPSILON        '\0'   /* sentinel for ε-transitions   */
#define MAX_NFA_STATES 4096   /* upper bound per token class   */
#define ALPHA_SIZE     128    /* ASCII printable range         */
#define MAX_TRANS      8      /* max transitions per state     */

/* ── Single NFA state ── */
typedef struct NFAState {
    int  id;                           /* unique state index         */
    int  is_accept;                    /* 1 = accepting state        */

    char  trans_char[MAX_TRANS];       /* characters (or EPSILON)   */
    struct NFAState *trans[MAX_TRANS]; /* destination states        */
    int   trans_count;                 /* number of transitions     */
} NFAState;

/* ── An NFA fragment (Thompson style) ──
   Represents a sub-NFA with one start and one dangling accept.
   Fragments are composed by union, concat, and kleene ops.      */
typedef struct {
    NFAState *start;
    NFAState *accept;
} NFAFragment;

/* ── Complete NFA for one token class ── */
typedef struct {
    NFAState *states[MAX_NFA_STATES]; /* all allocated states       */
    int       state_count;            /* how many states used       */
    NFAState *start;                  /* entry point                */
    NFAState *accept;                 /* single accept state        */
} NFA;

/* ── Epsilon-closure result (set of NFA state IDs) ── */
typedef struct {
    int ids[MAX_NFA_STATES];
    int count;
} StateSet;

/* ─────────────────────────────────────────────
   Public API
   ───────────────────────────────────────────── */

/* Allocate and initialise a fresh NFA */
NFA        *nfa_create(void);

/* Free all memory owned by an NFA */
void        nfa_free(NFA *nfa);

/* Allocate a new state inside an existing NFA */
NFAState   *nfa_new_state(NFA *nfa);

/* Add a transition (char or EPSILON) from src → dst */
void        nfa_add_transition(NFAState *src, char c, NFAState *dst);

/* ── Thompson's Construction ──
   Builds an NFA from a simplified regex string.
   Returns NULL on parse error.                    */
NFA        *nfa_from_regex(const char *regex);

/* ── Epsilon closure ──
   Computes ε*(state_ids[], count) → StateSet      */
StateSet    nfa_epsilon_closure(NFA *nfa, int *state_ids, int count);

/* ── Move ──
   Computes move(StateSet, char c) → StateSet      */
StateSet    nfa_move(NFA *nfa, StateSet *set, char c);

/* Debug: print NFA transition table to stdout */
void        nfa_print(const NFA *nfa);

#endif /* NFA_H */
