#ifndef TOKEN_PATTERNS_H
#define TOKEN_PATTERNS_H

typedef enum {
    TOKEN_EMAIL      = 0,
    TOKEN_URL        = 1,
    TOKEN_HASHTAG    = 2,
    TOKEN_MENTION    = 3,
    TOKEN_NUMBER     = 4,
    TOKEN_WORD       = 5,
    TOKEN_PUNCT      = 6,
    TOKEN_WHITESPACE = 7,
    TOKEN_UNKNOWN    = 8,
    TOKEN_COUNT      = 9
} TokenType;

static const char *TOKEN_NAMES[TOKEN_COUNT] __attribute__((unused)) = {
    "EMAIL",
    "URL",
    "HASHTAG",
    "MENTION",
    "NUMBER",
    "WORD",
    "PUNCT",
    "WHITESPACE",
    "UNKNOWN"
};

static const char *TOKEN_PATTERNS[TOKEN_COUNT] __attribute__((unused)) = {
    "[a-zA-Z0-9._+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z]{2,}",
    "https?://[a-zA-Z0-9./?=#_&%-]+",
    "#[a-zA-Z0-9_]+",
    "@[a-zA-Z0-9_]+",
    "[0-9]+\\.?[0-9]*",
    "[a-zA-Z]+",
    "[.,!?;:'\"\-]",
    "[ \\t\\n\\r]+",
    "."
};

#endif
