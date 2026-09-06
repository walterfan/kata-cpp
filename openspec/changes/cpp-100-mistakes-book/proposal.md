## Why

C++ is famously "simple to learn but hard to master." Its manual memory model, undefined behavior, subtle move semantics, template intricacies, and concurrency hazards produce a long tail of mistakes that even experienced engineers repeatedly ship. Teiva Harsanyi's *100 Go Mistakes and How to Avoid Them* proved that a curated, categorized catalog of concrete pitfalls — each with a bad example, an explanation, and an idiomatic fix — is an effective learning format. No equally structured, Chinese-language equivalent exists for C++. This change authors **《100 个 C++ 语言典型错误》** to fill that gap.

## What Changes

- Author a book **《100 个 C++ 语言典型错误》** modeled on the structure of *100 Go Mistakes and How to Avoid Them*.
- Organize ~105 mistakes (100+; the title is a round-number brand) into ~13 thematic chapters (code organization, types, memory & RAII, resource ownership, move semantics, classes & inheritance, templates & generics, the STL, concurrency, error handling, the standard library & tooling, performance, and a Linux systems-programming/`epoll` chapter).
- Explicitly cover several high-value lifetime/systems pitfalls: `shared_ptr<T>(this)` misuse (fix via `enable_shared_from_this`), dangling references from escaping lambdas that capture by reference or `this`, and common `epoll` mistakes on Linux (edge-triggered draining, `epoll_ctl`/`close` races, dangling `data.ptr`).
- For each mistake, provide a consistent entry format: mistake ID + title, symptom/bad example, why it is wrong (including UB where relevant), the idiomatic fix, and a reference to the relevant ISO C++ Core Guideline where applicable.
- Deliver the manuscript as structured Markdown under a book directory, one file per chapter, plus a top-level table of contents/README.
- Target modern C++ (C++11 through C++20/23) while flagging legacy pitfalls that persist from C++98.

## Capabilities

### New Capabilities
- `cpp-mistakes-book`: The authored book manuscript — its chapter organization, the per-mistake entry format, the coverage of all 100 mistakes across thematic categories, and the build/table-of-contents deliverables.

### Modified Capabilities
<!-- None: this is a new authoring effort with no existing spec-level behavior changing. -->

## Impact

- **New content**: A `books/cpp-100-mistakes/` directory (or equivalent) containing 13 chapter Markdown files and a README/TOC.
- **No production code impact**: This is a documentation/authoring change; it introduces no runtime code, APIs, or dependencies.
- **Source material**: Draws on *100 Go Mistakes* (structure), the ISO C++ Core Guidelines, *Effective Modern C++*, and curated community pitfall catalogs (structure and content constraints only; all prose and examples are original).
- **Audience**: Chinese-speaking C++ developers seeking a concise, example-driven reference of common mistakes.
