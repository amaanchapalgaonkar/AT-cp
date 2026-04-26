#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

static const char *DEMO_INPUT =
"Hello @john! Email me at [support@example.com](mailto:support@example.com) for details.\n"
"Check out https://github.com and follow #NLP on Twitter @nlp_engine\n"
"Price: $99.99, available from 2024-01-15 onwards.\n";

int main(int argc, char *argv[]) {


// Silent mode for Flask integration
int silent = (argc > 2 && strcmp(argv[2], "--silent") == 0);

if (!silent) {
    printf("=====================================================\n");
    printf(" Automata-Based NLP Tokenization Engine\n");
    printf(" Each token class = Regular Language (DFA)\n");
    printf(" Recognition Strategy: Longest Match (Maximal Munch)\n");
    printf("=====================================================\n\n");

    printf("Token Classes:\n");
    printf("  - EMAIL: Match [user@domain.com]\n");
    printf("  - URL: Match [https://example.com]\n");
    printf("  - HASHTAG: Match [#topic]\n");
    printf("  - MENTION: Match [@username]\n");
    printf("  - NUMBER: Match [123] or [45.67]\n");
    printf("  - WORD: Match [alphabetic sequences]\n");
    printf("  - PUNCT: Match [.,!?;:'\"-]\n\n");
}

const char *input = (argc > 1) ? argv[1] : DEMO_INPUT;

// Read from stdin if "-" is passed
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

if (!silent) {
    printf("Input:\n------\n%s\n", input);

    printf("Building DFAs from regex patterns...\n");
}

tokenizer_init();

if (!silent) {
    printf("DFA table ready. Token classes: %d\n\n", TOKEN_COUNT);

    // DEMO: Incorrect tokenization (for comparison)
    printf("--- DEMO: Without proper tokenization (incorrect) ---\n");
    printf("WORD: test\nUNKNOWN: @\nWORD: gmail\nUNKNOWN: .\nWORD: com\n");

    printf("\n--- DEMO: With DFA-based tokenization (correct) ---\n");

    printf("\nTOKENIZATION RESULTS:\n");
    printf("======================\n\n");
}

TokenList *tokens = tokenize(input);

// Output handling
if (silent) {
    // Clean output for Flask/UI
    for (int i = 0; i < tokens->count; i++) {
        printf("%s: %s\n",
               TOKEN_NAMES[tokens->tokens[i].type],
               tokens->tokens[i].lexeme);
    }
} else {
    token_list_print(tokens);
}

// Token statistics (only in normal mode)
if (!silent) {
    int counts[TOKEN_COUNT] = {0};
    for (int i = 0; i < tokens->count; i++)
        counts[tokens->tokens[i].type]++;

    printf("\nToken class breakdown:\n");
    for (int t = 0; t < TOKEN_COUNT; t++) {
        if (counts[t] > 0)
            printf("  %-14s : %d\n", TOKEN_NAMES[t], counts[t]);
    }
}

token_list_free(tokens);
tokenizer_destroy();
free(stdin_buf);

return 0;


}
