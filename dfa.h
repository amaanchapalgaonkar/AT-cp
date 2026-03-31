#ifndef DFA_H
#define DFA_H

#include "nfa.h"
#include "token_patterns.h"

#define MAX_DFA_STATES 1024
#define DEAD_STATE     -1

typedef struct {
    int id;
    int is_accept;
    int token_type;
    int next[ALPHA_SIZE];
} DFAState;

typedef struct {
    DFAState *states[MAX_DFA_STATES];
    int       state_count;
    int       start;
    int       token_type;
} DFA;

typedef struct {
    DFA *dfas[TOKEN_COUNT];
} DFATable;

DFA        *dfa_create(int token_type);

void        dfa_free(DFA *dfa);

DFAState   *dfa_new_state(DFA *dfa);
DFA        *dfa_from_nfa(NFA *nfa, int token_type);

DFA        *dfa_minimize(DFA *dfa);

DFATable   *dfa_table_build(void);

void        dfa_table_free(DFATable *table);

int         dfa_simulate(DFA *dfa, const char *input, int offset);

void        dfa_print(const DFA *dfa);

#endif
