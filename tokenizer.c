#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

/* ═══════════════════════════════════════════════════════════
   tokenizer.c — Stage 5: Longest-Match (Maximal Munch) Engine

   Algorithm:
     pos = 0
     while pos < len(input):
       best_len  = 0
       best_type = TOKEN_UNKNOWN
       for each DFA in table:
           match_len = dfa_simulate(dfa, input, pos)
           if match_len > best_len:
               best_len  = match_len
               best_type = dfa.token_type
       if best_len == 0:
           emit TOKEN_UNKNOWN of length 1
           pos += 1
       else:
           emit token(best_type, input[pos..pos+best_len])
           pos += best_len
   ═══════════════════════════════════════════════════════════ */

/* Global DFA table — built once by tokenizer_init() */
static DFATable *g_table = NULL;

void tokenizer_init(void) {
    if (g_table) return; /* already initialised */
    g_table = dfa_table_build();
}

void tokenizer_destroy(void) {
    dfa_table_free(g_table);
    g_table = NULL;
}

/* ─────────────────────────────────────────────
   TokenList helpers
   ───────────────────────────────────────────── */

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

/* ─────────────────────────────────────────────
   Main tokenizer — Longest-Match
   ───────────────────────────────────────────── */
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

        /* Run all token DFAs, pick the longest match */
        for (int t = 0; t < TOKEN_COUNT - 1; t++) { /* skip TOKEN_UNKNOWN */
            int m = dfa_simulate(g_table->dfas[t], input, pos);
            if (m > best_len) {
                best_len  = m;
                best_type = t;
            }
        }

        /* If no DFA matched, consume one character as UNKNOWN */
        if (best_len == 0) {
            best_len  = 1;
            best_type = TOKEN_UNKNOWN;
        }

        /* Build token */
        Token tok;
        tok.type   = (TokenType)best_type;
        tok.start  = pos;
        tok.length = best_len;
        tok.line   = line;
        tok.col    = col;

        /* Copy lexeme (clamped to MAX_TOKEN_LEN-1) */
        int copy_len = best_len < MAX_TOKEN_LEN - 1 ? best_len : MAX_TOKEN_LEN - 1;
        memcpy(tok.lexeme, input + pos, copy_len);
        tok.lexeme[copy_len] = '\0';

        token_list_push(list, &tok);

        /* Advance position and track line/col */
        for (int i = 0; i < best_len; i++) {
            if (input[pos + i] == '\n') { line++; col = 1; }
            else                         col++;
        }
        pos += best_len;
    }

    return list;
}

/* ─────────────────────────────────────────────
   Pretty-print token list
   ───────────────────────────────────────────── */
void token_list_print(const TokenList *list) {
    printf("\n%-5s  %-14s  %-6s  %-4s  %s\n",
           "NO.", "TYPE", "LINE", "COL", "LEXEME");
    printf("%-5s  %-14s  %-6s  %-4s  %s\n",
           "---", "--------------", "------", "----", "------");

    for (int i = 0; i < list->count; i++) {
        Token *t = &list->tokens[i];
        /* Skip whitespace tokens in output for readability */
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
