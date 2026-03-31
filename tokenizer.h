#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "dfa.h"
#include "token_patterns.h"

#define MAX_TOKEN_LEN 512

typedef struct {
    TokenType   type;
    char        lexeme[MAX_TOKEN_LEN];
    int         start;
    int         length;
    int         line;
    int         col;
} Token;

typedef struct {
    Token  *tokens;
    int     count;
    int     capacity;
} TokenList;

void        tokenizer_init(void);

void        tokenizer_destroy(void);

TokenList  *tokenize(const char *input);

void        token_list_free(TokenList *list);

void        token_list_print(const TokenList *list);

#endif
