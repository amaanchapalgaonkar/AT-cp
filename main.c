#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

/* ═══════════════════════════════════════════════════════════
   main.c — Automata-Based NLP Tokenization Engine
   Entry point: reads input, runs tokenizer, prints results.
   ═══════════════════════════════════════════════════════════ */

/* Demo NLP input covering all token classes */
static const char *DEMO_INPUT =
    "Hello world! Contact us at support@example.com or visit https://www.example.com\n"
    "Follow us on Twitter @nlp_engine and use #AutomataTokenizer\n"
    "Price: $99.99, release on 2024-01-15. Really? Yes!\n"
    "user123 sent 42 messages. Email: test.user+tag@domain.org\n";

/* ─────────────────────────────────────────────
   Usage: ./tokenizer [input_string]
   If no argument, runs built-in demo.
   ───────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    printf("======================================================\n");
    printf("   Automata-Based NLP Tokenization Engine         \n");
    printf("   Thompson NFA → Subset DFA → Hopcroft Min-DFA   \n");
    printf("====================================================\n\n");

    /* Choose input: CLI argument or built-in demo */
    const char *input = (argc > 1) ? argv[1] : DEMO_INPUT;

    /* If reading from stdin flag */
    char *stdin_buf = NULL;
    if (argc > 1 && strcmp(argv[1], "-") == 0) {
        size_t cap = 4096, sz = 0;
        stdin_buf = malloc(cap);
        if (!stdin_buf) { fprintf(stderr, "out of memory\n"); return 1; }
        int ch;
        while ((ch = getchar()) != EOF) {
            if (sz + 1 >= cap) {
                cap *= 2;
                stdin_buf = realloc(stdin_buf, cap);
                if (!stdin_buf) { fprintf(stderr, "out of memory\n"); return 1; }
            }
            stdin_buf[sz++] = (char)ch;
        }
        stdin_buf[sz] = '\0';
        input = stdin_buf;
    }

    printf("Input:\n------\n%s\n", input);

    /* ── Step 1: Build all DFAs ── */
    printf("Building DFAs from regex patterns...\n");
    tokenizer_init();
    printf("DFA table ready. Token classes: %d\n\n", TOKEN_COUNT);

    /* ── Step 2: Tokenize ── */
    TokenList *tokens = tokenize(input);

    /* ── Step 3: Print results ── */
    token_list_print(tokens);

    /* ── Step 4: Summary by token type ── */
    int counts[TOKEN_COUNT] = {0};
    for (int i = 0; i < tokens->count; i++)
        counts[tokens->tokens[i].type]++;

    printf("\nToken class breakdown:\n");
    for (int t = 0; t < TOKEN_COUNT; t++) {
        if (counts[t] > 0)
            printf("  %-14s : %d\n", TOKEN_NAMES[t], counts[t]);
    }

    /* ── Cleanup ── */
    token_list_free(tokens);
    tokenizer_destroy();
    free(stdin_buf);

    return 0;
}
