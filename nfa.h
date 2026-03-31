#ifndef NFA_H
#define NFA_H

#include <stddef.h>

#define EPSILON        '\0'
#define MAX_NFA_STATES 4096
#define ALPHA_SIZE     128
#define MAX_TRANS      8

typedef struct NFAState {
    int  id;
    int  is_accept;

    char  trans_char[MAX_TRANS];
    struct NFAState *trans[MAX_TRANS];
    int   trans_count;
} NFAState;

typedef struct {
    NFAState *start;
    NFAState *accept;
} NFAFragment;

typedef struct {
    NFAState *states[MAX_NFA_STATES];
    int       state_count;
    NFAState *start;
    NFAState *accept;
} NFA;

typedef struct {
    int ids[MAX_NFA_STATES];
    int count;
} StateSet;

NFA        *nfa_create(void);

void        nfa_free(NFA *nfa);

NFAState   *nfa_new_state(NFA *nfa);

void        nfa_add_transition(NFAState *src, char c, NFAState *dst);

NFA        *nfa_from_regex(const char *regex);

StateSet    nfa_epsilon_closure(NFA *nfa, int *state_ids, int count);

StateSet    nfa_move(NFA *nfa, StateSet *set, char c);

void        nfa_print(const NFA *nfa);

#endif
