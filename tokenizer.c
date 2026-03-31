#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

static DFATable *g_table = NULL;

static int is_stopword(const char *word) {
    const char *sw[] = {"the","is","at","on","and","a","an","in","to"};
    int n = sizeof(sw)/sizeof(sw[0]);
    for (int i = 0; i < n; i++) {
        if (strcmp(word, sw[i]) == 0)
            return 1;
    }
    return 0;
}

void tokenizer_init(void) {
    if (g_table) return;
    g_table = dfa_table_build();
}

void tokenizer_destroy(void) {
    dfa_table_free(g_table);
    g_table = NULL;
}

static TokenList *token_list_create(void) {
    TokenList *list = malloc(sizeof(TokenList));
    if (!list) { fprintf(stderr, "token_list_create: out of memory\n"); exit(1); }
    list->capacity = 64;
    list->count    = 0;
    list->tokens   = malloc(list->capacity * sizeof(Token));
    if (!list->tokens) { fprintf(stderr, "token_list_create: out of memory\n"); exit(1); }
    return list;
}

static void token_list_push(TokenList *list, Token *tok) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, list->capacity * sizeof(Token));
        if (!list->tokens) { fprintf(stderr, "token_list_push: out of memory\n"); exit(1); }
    }
    list->tokens[list->count++] = *tok;
}

void token_list_free(TokenList *list) {
    if (!list) return;
    free(list->tokens);
    free(list);
}

TokenList *tokenize(const char *input) {
    if (!g_table) {
        fprintf(stderr, "tokenize: call tokenizer_init() first\n");
        exit(1);
    }

    TokenList *list = token_list_create();
    int pos  = 0;
    int line = 1;
    int col  = 1;
    int len  = (int)strlen(input);

    while (pos < len) {
        int best_len  = 0;
        int best_type = TOKEN_UNKNOWN;

        for (int t = 0; t < TOKEN_COUNT - 1; t++) {
            int m = dfa_simulate(g_table->dfas[t], input, pos);
            if (m > best_len || (m == best_len && t < best_type)) {
                best_len  = m;
                best_type = t;
            }
        }

        if (best_len == 0) {
            best_len  = 1;
            best_type = TOKEN_UNKNOWN;
        }

        Token tok;
        tok.type   = (TokenType)best_type;
        tok.start  = pos;
        tok.length = best_len;
        tok.line   = line;
        tok.col    = col;

        int copy_len = best_len < MAX_TOKEN_LEN - 1 ? best_len : MAX_TOKEN_LEN - 1;
        memcpy(tok.lexeme, input + pos, copy_len);
        tok.lexeme[copy_len] = '\0';

        token_list_push(list, &tok);

        for (int i = 0; i < best_len; i++) {
            if (input[pos + i] == '\n') { line++; col = 1; }
            else                         col++;
        }
        pos += best_len;
    }

    return list;
}

void token_list_print(const TokenList *list) {
    printf("\nTOKENIZATION RESULTS:\n");
    printf("======================\n\n");
    printf("%-5s  %-14s  %-6s  %-4s  %s\n",
           "NO.", "TYPE", "LINE", "COL", "LEXEME");
    printf("%-5s  %-14s  %-6s  %-4s  %s\n",
           "---", "--------------", "------", "----", "------");

    for (int i = 0; i < list->count; i++) {
        Token *t = &list->tokens[i];
        if (t->type == TOKEN_WHITESPACE) continue;
        printf("%-5d  %-14s  %-6d  %-4d  %s\n",
               i + 1,
               TOKEN_NAMES[t->type],
               t->line,
               t->col,
               t->lexeme);
    }
    printf("\nTotal tokens (incl. whitespace): %d\n", list->count);
}
