#ifndef DFA_H
#define DFA_H

#include "nfa.h"
#include "token_patterns.h"

/* ═══════════════════════════════════════════════════════════
   DFA (Deterministic Finite Automaton)
   Built from an NFA via Subset Construction, then minimized
   using Hopcroft's Partition Refinement algorithm.

   Key properties:
   - Each state has exactly one transition per input symbol
   - No epsilon transitions
   - Dead state (id = -1) used for missing transitions
   ═══════════════════════════════════════════════════════════ */

#define MAX_DFA_STATES 1024   /* upper bound after minimization */
#define DEAD_STATE     -1     /* sink / error state id          */

/* ── Single DFA state ── */
typedef struct {
    int id;                        /* unique state index              */
    int is_accept;                 /* 1 = accepting state             */
    int token_type;                /* which TokenType this accepts    */
    int next[ALPHA_SIZE];          /* next[c] = destination state id  */
                                   /* DEAD_STATE if no transition     */
} DFAState;

/* ── Complete DFA for one token class ── */
typedef struct {
    DFAState *states[MAX_DFA_STATES];
    int       state_count;
    int       start;               /* id of start state               */
    int       token_type;          /* which TokenType this DFA covers */
} DFA;

/* ── Array of all token DFAs (one per TOKEN_COUNT) ── */
typedef struct {
    DFA *dfas[TOKEN_COUNT];
} DFATable;

/* ─────────────────────────────────────────────
   Public API
   ───────────────────────────────────────────── */

/* Allocate a fresh DFA */
DFA        *dfa_create(int token_type);

/* Free all memory owned by a DFA */
void        dfa_free(DFA *dfa);

/* Allocate a new DFA state */
DFAState   *dfa_new_state(DFA *dfa);

/* ── Subset Construction (NFA → DFA) ──
   Converts nfa into an equivalent DFA.
   token_type is stored in every accept state.     */
DFA        *dfa_from_nfa(NFA *nfa, int token_type);

/* ── Hopcroft's Minimization (DFA → Min-DFA) ──
   Returns a new minimized DFA; frees the original. */
DFA        *dfa_minimize(DFA *dfa);

/* ── Build all token DFAs at startup ──
   Runs nfa_from_regex → dfa_from_nfa → dfa_minimize
   for every entry in TOKEN_PATTERNS[].             */
DFATable   *dfa_table_build(void);

/* Free all DFAs in a DFATable */
void        dfa_table_free(DFATable *table);

/* ── Single-DFA simulation ──
   Runs dfa on input starting at offset.
   Returns the length of the longest match (0 = no match). */
int         dfa_simulate(DFA *dfa, const char *input, int offset);

/* Debug: print DFA transition table to stdout */
void        dfa_print(const DFA *dfa);

#endif /* DFA_H */
