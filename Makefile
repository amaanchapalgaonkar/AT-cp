# ─────────────────────────────────────────────
# Makefile — Automata-Based NLP Tokenizer
# Compiler: GCC, Standard: C99
# ─────────────────────────────────────────────

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -g
TARGET  = tokenizer
SRCS    = main.c nfa.c dfa.c tokenizer.c
OBJS    = $(SRCS:.c=.o)

# ── Default build ──
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# ── Dependencies ──
main.o:      main.c tokenizer.h token_patterns.h
nfa.o:       nfa.c nfa.h
dfa.o:       dfa.c dfa.h nfa.h token_patterns.h
tokenizer.o: tokenizer.c tokenizer.h dfa.h nfa.h token_patterns.h

# ── Memory check via Valgrind ──
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# ── Clean build artifacts ──
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean valgrind
