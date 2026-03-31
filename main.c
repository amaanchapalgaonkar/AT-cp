#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

static const char *DEMO_INPUT =
    "Hello @john! Email me at support@example.com for details.\n"
    "Check out https://github.com and follow #NLP on Twitter @nlp_engine\n"
    "Price: $99.99, available from 2024-01-15 onwards.\n";

int main(int argc, char *argv[]) {
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
    printf("  - PUNCT: Match [.,!?;:\'\"\-]\n");
    printf("\n");

    const char *input = (argc > 1) ? argv[1] : DEMO_INPUT;

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

    printf("Building DFAs from regex patterns...\n");
    tokenizer_init();
    printf("DFA table ready. Token classes: %d\n\n", TOKEN_COUNT);

    TokenList *tokens = tokenize(input);

    token_list_print(tokens);

    int counts[TOKEN_COUNT] = {0};
    for (int i = 0; i < tokens->count; i++)
        counts[tokens->tokens[i].type]++;

    printf("\nToken class breakdown:\n");
    for (int t = 0; t < TOKEN_COUNT; t++) {
        if (counts[t] > 0)
            printf("  %-14s : %d\n", TOKEN_NAMES[t], counts[t]);
    }

    printf("\n");
    printf("=== FUTURE SCOPE ===\n");
    printf("  - Support for advanced regex (RFC-compliant emails, URLs)\n");
    printf("  - Unicode and multilingual tokenization\n");
    printf("  - DFA serialization for fast startup\n");
    printf("  - Integration with NLP pipelines (stemming, lemmatization)\n");
    printf("  - Machine learning-based token classification\n");
    printf("  - Context-aware disambiguation\n\n");

    token_list_free(tokens);
    tokenizer_destroy();
    free(stdin_buf);

    return 0;
}
