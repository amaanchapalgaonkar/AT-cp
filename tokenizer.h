#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "dfa.h"
#include "token_patterns.h"

/* ═══════════════════════════════════════════════════════════
   Tokenizer Engine
   Uses the Longest-Match (Maximal Munch) strategy:
   - At each position, run ALL token DFAs simultaneously
   - Accept the match that consumed the most characters
   - On a tie, the DFA with lower TokenType id wins (priority)
   - Advance position by match length; repeat until EOF
   ═══════════════════════════════════════════════════════════ */

#define MAX_TOKEN_LEN 512   /* max characters in one token lexeme */

/* ── A single recognized token ── */
typedef struct {
    TokenType   type;               /* class of this token            */
    char        lexeme[MAX_TOKEN_LEN]; /* actual matched text         */
    int         start;              /* byte offset in original input  */
    int         length;             /* number of characters matched   */
    int         line;               /* 1-based line number            */
    int         col;                /* 1-based column number          */
} Token;

/* ── Result of tokenizing a full input string ── */
typedef struct {
    Token  *tokens;    /* heap-allocated array of tokens             */
    int     count;     /* number of tokens recognized                */
    int     capacity;  /* current allocated capacity                 */
} TokenList;

/* ─────────────────────────────────────────────
   Public API
   ───────────────────────────────────────────── */

/* Initialise the tokenizer (builds DFA table internally).
   Must be called once before tokenize().                  */
void        tokenizer_init(void);

/* Release all resources held by the tokenizer.
   Must be called once at program exit.                    */
void        tokenizer_destroy(void);

/* ── Main entry point ──
   Tokenizes null-terminated input string using longest-match.
   Returns a heap-allocated TokenList; caller must free with
   token_list_free().                                       */
TokenList  *tokenize(const char *input);

/* Free a TokenList returned by tokenize() */
void        token_list_free(TokenList *list);

/* Print all tokens in a TokenList to stdout */
void        token_list_print(const TokenList *list);

#endif /* TOKENIZER_H */
