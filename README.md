# Automata-Based NLP Tokenization Engine

A deterministic tokenizer built from scratch in C using formal automata theory.  
Models each token class as a regular language and constructs NFAs and DFAs from regular expressions.

---

## Pipeline

```
Regex Pattern
     ↓
Thompson's Construction → NFA
     ↓
Subset Construction → DFA
     ↓
Hopcroft's Algorithm → Minimized DFA
     ↓
Longest-Match (Maximal Munch) → Tokens
```

---

## Token Classes Recognized

| Token | Example |
|---|---|
| EMAIL | `support@example.com` |
| URL | `https://www.example.com` |
| HASHTAG | `#AutomataTokenizer` |
| MENTION | `@nlp_engine` |
| NUMBER | `99.99`, `42` |
| WORD | `Hello`, `world` |
| PUNCT | `!`, `,`, `?` |
| WHITESPACE | spaces, tabs, newlines |
| UNKNOWN | `$`, `+` (unrecognized chars) |

---

## Project Structure

```
tokenizer/
├── main.c            # Entry point, demo input
├── nfa.c             # Thompson's Construction (Regex → NFA)
├── nfa.h
├── dfa.c             # Subset Construction + Hopcroft Minimization
├── dfa.h
├── tokenizer.c       # Longest-Match tokenizer engine
├── tokenizer.h
├── token_patterns.h  # Token class definitions and regex patterns
├── Makefile
└── README.md
```

---

## How to Run

### On Linux / Mac

**1. Compile:**
```bash
make
```

**2. Run built-in demo:**
```bash
./tokenizer
```

**3. Run with custom input:**
```bash
./tokenizer "Email: hello@gmail.com | URL: https://google.com | Tag: #demo"
```

**4. Clean build files:**
```bash
make clean
```

**5. Check memory leaks (requires Valgrind):**
```bash
make valgrind
```

---

### On Windows (PowerShell)

**1. Fix terminal encoding (run once per session for proper display):**
```powershell
chcp 65001
```

**2. Compile manually using GCC:**
```powershell
gcc -std=c99 -Wall -Wextra -g -c main.c
gcc -std=c99 -Wall -Wextra -g -c nfa.c
gcc -std=c99 -Wall -Wextra -g -c dfa.c
gcc -std=c99 -Wall -Wextra -g -c tokenizer.c
gcc -std=c99 -Wall -Wextra -g -o tokenizer.exe main.o nfa.o dfa.o tokenizer.o
```

**Or if you have MinGW installed:**
```powershell
mingw32-make
```

**3. Run built-in demo:**
```powershell
.\tokenizer.exe
```

**4. Run with custom input:**
```powershell
.\tokenizer.exe "Email: hello@gmail.com | URL: https://google.com | Tag: #demo"
```

**5. Clean build files:**
```powershell
del *.o *.exe
```

---

## Sample Output

```
Input:
------
Hello world! Contact us at support@example.com or visit https://www.example.com
Follow us on Twitter @nlp_engine and use #AutomataTokenizer
Price: $99.99, release on 2024-01-15. Really? Yes!

Building DFAs from regex patterns...
DFA table ready. Token classes: 9

NO.    TYPE            LINE    COL   LEXEME
---    --------------  ------  ----  ------
1      WORD            1       1     Hello
3      WORD            1       7     world
4      PUNCT           1       12    !
12     EMAIL           1       28    support@example.com
21     URL             1       57    https://www.example.com
31     MENTION         2       22    @nlp_engine
37     HASHTAG         2       42    #AutomataTokenizer
43     NUMBER          3       9     99.99

Token class breakdown:
  URL            : 1
  HASHTAG        : 1
  MENTION        : 1
  NUMBER         : 2
  WORD           : 10
  PUNCT          : 6
  WHITESPACE     : 15
  UNKNOWN        : 1
```

---

## Tech Stack

| Tool | Purpose |
|---|---|
| C (C99) | Implementation language |
| GCC | Compiler |
| Make | Build system |
| Valgrind | Memory debugging (Linux) |

---

## Team

| Member | Responsibility |
|---|---|
| M1 | Regular Languages & Token Modeling (`token_patterns.h`) |
| M2 | Thompson's Construction — Regex → NFA (`nfa.h`, `nfa.c`) |
| M3 | Subset Construction — NFA → DFA (`dfa_from_nfa()`) |
| M4 | Hopcroft's Minimization — DFA → Min-DFA (`dfa_minimize()`) |
| M5 | Longest-Match Tokenizer Engine (`tokenizer.c`, `main.c`) |
