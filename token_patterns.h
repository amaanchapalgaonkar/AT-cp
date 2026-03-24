#ifndef TOKEN_PATTERNS_H
#define TOKEN_PATTERNS_H

/* ─────────────────────────────────────────────
   Token class IDs
   Each ID maps to one regex pattern and one
   minimized DFA built at startup.
   ───────────────────────────────────────────── */
typedef enum {
    TOKEN_EMAIL      = 0,
    TOKEN_URL        = 1,
    TOKEN_HASHTAG    = 2,
    TOKEN_MENTION    = 3,   /* @username */
    TOKEN_NUMBER     = 4,   /* integer or float */
    TOKEN_WORD       = 5,   /* alphabetic word */
    TOKEN_PUNCT      = 6,   /* single punctuation */
    TOKEN_WHITESPACE = 7,
    TOKEN_UNKNOWN    = 8,
    TOKEN_COUNT      = 9    /* total number of classes */
} TokenType;

/* Human-readable names — index matches TokenType */
/* Marked unused to suppress -Wunused-variable when included in multiple TUs */
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

/* Simplified regex-like patterns (used by Thompson's construction).
   These are processed by our own mini regex engine, not libc regex.
   Supported operators: . | * + ? () and char classes [...]          */
static const char *TOKEN_PATTERNS[TOKEN_COUNT] __attribute__((unused)) = {
    /* EMAIL      */ "[a-zA-Z0-9._+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z]{2,}",
    /* URL        */ "https?://[a-zA-Z0-9./?=#_&%-]+",
    /* HASHTAG    */ "#[a-zA-Z0-9_]+",
    /* MENTION    */ "@[a-zA-Z0-9_]+",
    /* NUMBER     */ "[0-9]+\\.?[0-9]*",
    /* WORD       */ "[a-zA-Z]+",
    /* PUNCT      */ "[.,!?;:'\"-]",
    /* WHITESPACE */ "[ \\t\\n\\r]+",
    /* UNKNOWN    */ "."    /* catch-all: any single character */
};

#endif /* TOKEN_PATTERNS_H */
