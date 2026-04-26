# Automata-Based NLP Tokenizer - Team Assignment

## Project Overview
This project implements an NLP tokenizer using Deterministic Finite Automata (DFA) with longest-match strategy. The complete system involves theoretical automata concepts, algorithm implementation, optimization, and a web-based frontend.

---

## Team Member Assignments

### **Member 1: Token Patterns & Regex Foundations**
**File References**: `token_patterns.h`, `script.js` (tokenColors)

**Topics to Cover in Presentation**:
1. **Regular Expression Patterns** - Understanding the 9 token classes
   - WORD, NUMBER, EMAIL, URL, HASHTAG, MENTION, UNKNOWN, WHITESPACE
   - How regex patterns define regular languages
   - Pattern examples and use cases

2. **Token Classification System**
   - Design decisions for token types
   - Why these specific patterns were chosen
   - Real-world NLP applications

3. **Color-Coded Token System**
   - Visual representation in UI (token badges, colors)
   - How colors are mapped to token types in the code

**Deliverables**:
- Slides showing regex patterns for each token type
- Explanation of how patterns recognize different text elements
- Demo of pattern matching with examples

---

### **Member 2: Thompson's NFA Construction Algorithm**
**File References**: `nfa.c`, `nfa.h`

**Topics to Cover in Presentation**:
1. **Thompson's Algorithm Fundamentals**
   - Fragment composition approach
   - Union, concatenation, and Kleene star operations
   - How each regex operator maps to NFA construction

2. **NFA Fragment Data Structure**
   - Components: states, transitions, start/accept states
   - Memory management for fragments
   - Fragment merging logic

3. **Building NFA Step-by-Step**
   - Converting simple patterns to NFA (e.g., "a", "ab", "a|b", "a*")
   - Compound pattern construction
   - Example: Building NFA for email pattern

**Deliverables**:
- Flowchart of Thompson's algorithm
- Visual NFA construction examples
- Code walkthrough of `nfa_from_regex()` function
- State diagram showing epsilon transitions

---

### **Member 3: Subset Construction & NFA-to-DFA Conversion**
**File References**: `dfa.c` (dfa_from_nfa), `nfa.c` (epsilon_closure)

**Topics to Cover in Presentation**:
1. **Subset Construction Theory**
   - Converting NFA states to DFA states
   - Concept of reachable state sets
   - Epsilon-closure and move operations

2. **Algorithm Implementation**
   - ε-closure computation
   - Computing move(S, a) for a set of states
   - State set enumeration and DFA state creation

3. **Why DFA is Necessary**
   - Efficiency advantage over NFA simulation
   - O(n) time complexity guarantee
   - Longest-match capability

**Deliverables**:
- Step-by-step subset construction example
- Diagram showing NFA → DFA transformation
- Algorithm pseudocode and C implementation
- Explanation of epsilon transitions handling

---

### **Member 4: Hopcroft Minimization Algorithm**
**File References**: `dfa.c` (dfa_minimize)

**Topics to Cover in Presentation**:
1. **DFA Minimization Principles**
   - Why minimize? (smaller state space, faster execution)
   - Distinguishable vs. indistinguishable states
   - Partition refinement concept

2. **Hopcroft's Algorithm**
   - Initial partitioning (accepting vs. non-accepting states)
   - Iterative refinement process
   - Termination condition

3. **Practical Impact**
   - Comparison: pre-minimization vs. post-minimization state counts
   - Memory efficiency gains
   - Performance implications

**Deliverables**:
- Algorithm flowchart with refinement steps
- Before/after DFA comparison showing state reduction
- Pseudocode and implementation walkthrough
- Performance metrics/benchmarks

---

### **Member 5: Longest-Match Engine & Frontend Integration**
**File References**: `tokenizer.c`, `main.c`, `index.html`, `app.py`

**Topics to Cover in Presentation**:
1. **Longest-Match Tokenization Strategy**
   - Running all 9 DFAs in parallel at each position
   - Greedy selection of longest matching token
   - Handling ambiguity and token priority

2. **Tokenizer Implementation**
   - Main tokenization loop: O(n) complexity proof
   - No backtracking required
   - Edge cases and special handling

3. **System Architecture**
   - C tokenizer core → Flask backend → Web frontend flow
   - Token flow from engine to JavaScript visualization
   - Real-time processing and display

4. **Frontend Visualization**
   - Token table display with color coding
   - Token stream with inline highlighting
   - Bar chart and entity detection
   - User experience considerations

**Deliverables**:
- Architecture diagram (C → Flask → Web)
- Algorithm walkthrough with example input
- Tokenization process visualization
- Live demo showing all UI components
- Proof of O(n) time complexity

---

## Presentation Structure (Suggested)

1. **Opening (Member 1 - 5 min)**
   - Project overview, token types, why automata?

2. **Theory (Members 2-4 - 20 min)**
   - Thompson's NFA (5 min)
   - Subset Construction (5 min)
   - Hopcroft Minimization (5 min)
   - Show how they connect: regex → NFA → DFA → optimized DFA

3. **Implementation (Member 5 - 10 min)**
   - Longest-match engine
   - System architecture
   - Live demo

4. **Q&A & Conclusion (All - 5 min)**

---

## Key Resources Each Member Should Know

**All Members Should Review**:
- Project README for context
- `main.c` - shows the complete pipeline
- `index.html` - understand the frontend integration

**Specific Code References**:
- **Member 1**: Lines 1-50 of each C file (pattern definitions)
- **Member 2**: `nfa_from_regex()` in `nfa.c`
- **Member 3**: `dfa_from_nfa()` and `epsilon_closure()` in `dfa.c`
- **Member 4**: `dfa_minimize()` in `dfa.c`
- **Member 5**: `tokenize()` in `tokenizer.c` + all frontend files

---

## Presentation Tips

- **Visual Aids**: Use state diagrams and flowcharts
- **Live Demo**: Run actual tokenization with the web interface
- **Code Snippets**: Show key functions (2-3 lines max per slide)
- **Examples**: Use real tokens (emails, URLs, hashtags)
- **Timing**: Practice to keep each section within allocated time

---

## Estimated Time Requirements per Member

| Member | Topic | Prep Time | Presentation |
|--------|-------|-----------|--------------|
| 1 | Token Patterns | 2-3 hours | 5 min |
| 2 | Thompson's NFA | 4-5 hours | 5 min |
| 3 | Subset Construction | 4-5 hours | 5 min |
| 4 | Hopcroft Minimization | 4-5 hours | 5 min |
| 5 | Engine & Frontend | 3-4 hours | 10 min |
| **Total** | **Combined** | **18-22 hours** | **30 min** |
