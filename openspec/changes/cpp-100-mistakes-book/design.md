## Context

The proposal calls for authoring **《100 个 C++ 语言典型错误》**, a Chinese-language book cataloging 100 common C++ mistakes, modeled on *100 Go Mistakes and How to Avoid Them*. The reference book organizes ~100 mistakes into 12 categories (organization, data types, control structures, strings, functions, error management, concurrency foundations, concurrency practice, standard library, testing, optimizations). C++ differs materially from Go: it has manual memory management, RAII, move semantics, templates, multiple/virtual inheritance, and pervasive undefined behavior (UB). The chapter taxonomy must therefore be re-derived for C++ rather than copied.

This is an authoring (documentation) effort. There is no runtime code, no build pipeline change, and no external dependency. The primary design concerns are: (1) chapter taxonomy, (2) a consistent per-mistake entry format that makes the book scannable and teachable, (3) target-standard scoping (which C++ standards each fix assumes), and (4) file/directory layout for the manuscript.

## Goals / Non-Goals

**Goals:**
- Define a stable chapter taxonomy of ~13 categories that collectively cover ~105 distinct C++ mistakes.
- Define a single, repeatable per-mistake entry format: `#N 标题` → 现象/错误示例 (bad example) → 为什么错 (why, incl. UB) → 正确做法 (idiomatic fix) → 相关准则 (Core Guideline reference, when applicable).
- Ensure every code example is compilable-in-principle, minimal, and demonstrates exactly one mistake.
- Scope each fix to a concrete C++ standard (C++11/14/17/20/23) and flag legacy-from-C++98 pitfalls.
- Produce the manuscript as one Markdown file per chapter plus a top-level README/TOC that links all mistakes.
- Keep all prose and examples original; use reference works only for structure and topic coverage.

**Non-Goals:**
- Not a comprehensive C++ language tutorial or reference — it assumes working C++ knowledge.
- Not shipping runnable/tested sample projects or a compiler harness; examples illustrate, they are not a test suite.
- Not translating *100 Go Mistakes* text; only its organizational format is borrowed.
- Not covering compiler/vendor-extension pitfalls (MSVC/GCC/Clang-specific) beyond portable standard C++.
- Platform-specific systems-programming pitfalls are generally out of scope, **with one deliberate exception**: a dedicated Linux I/O chapter covers a small set of high-value `epoll` mistakes (see Decision 6). Such entries are explicitly labeled as Linux-only, non-portable.

## Decisions

**Decision 1 — Chapter taxonomy (13 categories, ~105 mistakes).**
Chosen distribution:
1. 代码与项目组织 (Code & project organization) — ~6 (#1–#6)
2. 基本类型与转换 (Fundamental types, conversions, integer/overflow) — ~9 (#7–#15)
3. 内存管理与 RAII (Memory, new/delete, leaks, dangling) — ~10 (#16–#25)
4. 资源与所有权 (Ownership, smart pointers, Rule of 0/3/5, `shared_from_this`) — ~11 (#26–#36)
5. 移动语义与值类别 (Move semantics, rvalue refs, moved-from state) — ~9 (#37–#45)
6. 类、对象与继承 (Constructors, virtual, slicing, virtual dtor) — ~10 (#46–#55)
7. 模板与泛型 (Templates, ADL, SFINAE/concepts, forwarding) — ~9 (#56–#64)
8. 标准库容器与算法 (STL containers, iterator invalidation, algorithms) — ~10 (#65–#74)
9. 并发 (Threads, data races, mutex/atomic, memory order, deadlock, lambda 悬垂捕获) — ~11 (#75–#85)
10. 错误处理 (Exceptions, noexcept, error codes, exception safety) — ~8 (#86–#93)
11. 标准库与 API 使用 (string_view/span lifetime, chrono, RNG, I/O) — ~5 (#94–#98)
12. 性能与优化 (Copies, allocations, cache, premature optimization) — ~4 (#99–#102)
13. 系统编程与 Linux I/O (epoll 边缘触发、fd 与 epoll_ctl 竞态、data.ptr 悬垂) — ~3 (#103–#105, Linux-only)

Rationale: mirrors *100 Go Mistakes* granularity while re-partitioning around C++'s distinctive hazard areas (memory, ownership, moves, templates), which have no Go analog. Chapter 13 is a deliberate, clearly-labeled exception to the portability scope because `epoll` misuse is a very common source of real-world bugs in Linux network services. Alternative considered: a 1:1 mapping to the Go chapters — rejected because Go has GC and no templates/move semantics, so several Go chapters (e.g., "Strings", "Control structures") are near-empty for C++ while C++'s biggest hazards would be underweighted.

**Decision 2 — Per-mistake entry format.**
Each mistake uses a fixed template so the book is scannable:
```
### #N 标题（一句话概括错误）
**现象 / 错误示例**  (minimal bad code block)
**为什么错**  (explanation; explicitly call out UB / 未定义行为 when applicable)
**正确做法**  (idiomatic fixed code block)
**适用标准**  (C++11/14/17/20/23; or "C++98 遗留")
**相关准则**  (ISO C++ Core Guidelines id, e.g., C.35, ES.20; optional)
```
Rationale: matches the "bad → why → fix" pedagogy proven by the Go book; adding 适用标准 and 相关准则 anchors advice to authoritative sources and prevents anachronistic suggestions. Alternative considered: free-form prose per mistake — rejected as inconsistent and hard to scan across 100 entries.

**Decision 3 — File/directory layout.**
```
books/cpp-100-mistakes/
  README.md            # 书名、前言、完整目录（100 条锚点链接）
  ch01-organization.md
  ch02-fundamental-types.md
  ...
  ch12-performance.md
  ch13-systems-linux-io.md    # Linux-only 平台相关章节
```
One file per chapter; README is the authoritative TOC linking each of the 100 mistakes by anchor. Rationale: one-file-per-chapter keeps files reviewable and lets chapters be drafted independently/in parallel. Alternative: single monolithic file — rejected (too large to review/edit).

**Decision 4 — Language and standard scoping.**
Prose is Simplified Chinese; code, identifiers, and standard-library names remain English. Each fix states its minimum C++ standard. Rationale: matches the target audience (Chinese C++ developers) while keeping code idiomatic and copy-pasteable.

**Decision 5 — Numbering scheme and total count.**
Mistakes are numbered globally #1–#105 and grouped by chapter (chapter N owns a contiguous range). The total is ~105 rather than exactly 100: the title 《100 个 C++ 语言典型错误》 is a round-number brand, and the extra entries (`shared_from_this` misuse, lambda dangling capture, and three `epoll` mistakes) are high-value enough to include rather than drop weaker entries. Rationale: a global number gives each mistake a stable citation id; contiguous per-chapter ranges keep the mapping predictable if counts shift during drafting; not forcing exactly 100 avoids filler or dropping strong content.

**Decision 6 — Linux systems chapter and platform labeling.**
Add Chapter 13 (系统编程与 Linux I/O) covering three `epoll` mistakes: (a) ET (edge-triggered) mode not draining the fd until `EAGAIN`/`EWOULDBLOCK`, causing lost events/starvation; (b) races between `epoll_ctl`/`close(fd)` and fd reuse (delete from epoll before close); (c) `epoll_event.data.ptr` pointing at an already-destroyed object. Every entry in this chapter is marked 平台相关 (Linux-only) in its 适用标准/平台 field and names required headers (`<sys/epoll.h>`). Rationale: these are among the most common real bugs in Linux servers and readers expect them; isolating them in one clearly-labeled chapter preserves the "portable standard C++" contract for chapters 1–12. Alternative considered: exclude all OS-specific content — rejected because the user explicitly requested epoll coverage and it has high practical value.

## Risks / Trade-offs

- **Title says 100 but content is ~105** → The title is a round-number brand, not a literal count; the preface notes the book contains "100+ 典型错误" so the extra high-value entries do not mislead. Chapter counts are targets, not hard quotas.
- **Anachronistic advice (suggesting features not in the target standard)** → Every entry records 适用标准; a review pass verifies each fix compiles under its stated standard.
- **Overlap/duplication between adjacent chapters (e.g., memory vs. ownership vs. moves)** → Assign each mistake a single primary chapter; cross-reference rather than duplicate. The README TOC surfaces the full list to catch dupes.
- **Copyright / originality** → Reference works inform structure and topic selection only; all prose and code are written fresh. No text is copied from *100 Go Mistakes* or other sources.
- **Examples that don't actually compile** → Keep examples minimal and self-contained; note required headers. A spot-check compile pass is recommended but out of scope for the manuscript itself (see Non-Goals).
- **Chinese/English mix readability** → Standardize on Chinese prose + English code/terms, with the English term in parentheses on first use per chapter.

## Migration Plan

Not applicable — this is additive documentation. Deliverables live under `books/cpp-100-mistakes/` and can be removed by deleting the directory; no rollback of running systems is involved.

## Open Questions

- Final exact per-chapter counts (targets above sum to ~100 and will be reconciled during drafting).
- Whether to add an appendix mapping each mistake to its ISO C++ Core Guideline id as a lookup table (nice-to-have; deferred).
