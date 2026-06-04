---
title: AAD and // Simulation
type: book
alias: [AAD]
---

[Available in Amazon](https://amzn.eu/d/07TqCXwI)

# Modern Computational Finance AAD and Parallel Simulations

---

## Pre-Chapter Foundations

- **Processes vs Threads**:
  - A process contains one or more threads.
  - Threads share the heap (global memory, allocated objects) AND code and open file handles
  - Each thread has its own stack (local variables, function call frames)
  - The heap/stack split is the crucial distinction for parallel programming

- **Race Conditions**:
  - Shared heap means multiple threads can read and write the same variable.
  - `counter++` looks like one operation but is actually 3 CPU steps: READ//ADD/WRITE
  - Two threads can interleave these steps → one increment gets lost silently, no error thrown.
  - Classic example: counter goes from 8 to 9 instead of 10.

- **Four Solutions Derived**:

| Tool | When to use it |
|---|---|
| **Atomic operations** | Single variable, simple operations — read-modify-write as one uninterruptible step |
| **Mutex / locks** | Protecting a whole block of operations — only one thread executes at a time |
| **Thread-local storage** | Each thread works privately on its own results, combine only at the end |
| **Path independence** | The architectural property that makes thread-local valid in Monte-Carlo |

- *Why thread-local works for standard Monte-Carlo:* each path is a self-contained simulation of one possible future — path 5 doesn't depend on path 3. So each thread can own a private subset of paths and never communicate during computation.

- **American Monte-Carlo Exception**: LSM (Longstaff-Schwartz) requires a regression **across all paths** at each exercise date to estimate the continuation value. This is a genuine cross-path dependency — path 5 depends on paths 1, 2, 3, 4...

  Solution: **synchronization barrier** pattern:

    ```text
    [threads run in parallel]
    [barrier — all threads pause — regression across all paths]
    [threads run in parallel again]
    [barrier again at next exercise date]
    ```

    The thread pool and synchronization primitives in Ch. 3 are the exact machinery needed to implement this correctly.

## Chapter 1 — Effective C++ Principles

- Simplicity is a prerequisite for long-term performance.
- Simple code avoids technical debt and keeps the codebase maintainable.
- Prefer clear naming over comments, avoid premature optimization, use STL aggressively.

## Chapter 2 — Modern C++

### 2.1 Lambda expressions

- Definitiona: anonymous functions with explicit capture.
- Key use case: passing work to thread pools (functional programming)
- Capture modes:
  - By value `[x]` -> protect original, work on private copy.
  - By reference `[&x]` -> modify original
  - By const reference `[&x]` -> read large objects without copying

    ```cpp
    // Capture by value
    auto f = [x, y]() { return x + y; };

    // Capture by reference  
    auto f = [&x, &y]() { return x + y; };

    // Capture everything by value
    auto f = [=]() { return x + y; };

    // Capture everything by reference — dangerous in parallel!
    auto f = [&]() { return x + y; };
    ```

- Danger in parallel: never capture local variables by reference if they may go out of scope before the thread finishes → dangling pointer / undefined behaviour

### 2.2 Functional programming

- Functions as first-class values that can be passed around
- Already implicit in using lambdas with thread pools

### 2.3 Move semantics

- Transfers ownership of internal pointer to new object, leaving original empty
- Avoids expensive data duplication for large objects
- Triggered explicitly with `std::move()` or automatically by compiler (RVO) when returning local objects from functions

---

#book #cpp