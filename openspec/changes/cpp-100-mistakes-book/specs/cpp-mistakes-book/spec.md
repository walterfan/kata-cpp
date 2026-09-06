## ADDED Requirements

### Requirement: Book identity and language
The book SHALL be titled 《100 个 C++ 语言典型错误》 and SHALL be written with Simplified Chinese prose while keeping all code, identifiers, and standard-library names in English.

#### Scenario: Title and preface present
- **WHEN** a reader opens the book's README/TOC
- **THEN** the title 《100 个 C++ 语言典型错误》 and a short preface explaining the book's purpose and format are present

#### Scenario: Language convention applied
- **WHEN** a reader reads any chapter
- **THEN** explanatory prose is in Simplified Chinese and all code blocks use English identifiers and standard C++ syntax

### Requirement: Chapter taxonomy
The book SHALL organize its content into 13 thematic chapters covering: code & project organization; fundamental types & conversions; memory management & RAII; resources & ownership; move semantics & value categories; classes, objects & inheritance; templates & generics; STL containers & algorithms; concurrency; error handling; standard library & API usage; performance & optimization; and systems programming & Linux I/O.

#### Scenario: All thirteen chapters exist
- **WHEN** the manuscript directory is listed
- **THEN** exactly thirteen chapter files exist, one per theme, each with a clear title matching its theme

#### Scenario: Mistake assigned to a single primary chapter
- **WHEN** a given mistake is authored
- **THEN** it appears in exactly one chapter as its primary location, with cross-references (not duplicated entries) used for related mistakes in other chapters

### Requirement: Mistake count and numbering
The book SHALL contain approximately 105 distinct mistakes (at least 100), numbered globally starting at #1, contiguous with no gaps or duplicates, with each chapter owning a contiguous number range.

#### Scenario: Count is at least one hundred
- **WHEN** all mistakes across all chapters are counted
- **THEN** the total is at least 100 (target ~105) with no duplicate numbers and no gaps in the global sequence

#### Scenario: Title framing acknowledges 100+
- **WHEN** a reader reads the preface
- **THEN** it states the book contains 100+ 典型错误, so the round-number title does not misrepresent the actual count

#### Scenario: Contiguous per-chapter ranges
- **WHEN** the mistakes in any single chapter are listed
- **THEN** their numbers form a contiguous range that does not overlap any other chapter's range

### Requirement: High-value lifetime and systems pitfalls covered
The book SHALL include entries for these specific high-value pitfalls: returning/constructing a `shared_ptr<T>` from a raw `this` inside a member function (producing a second control block / double free); a lambda that captures by reference (`[&]`) or by `this` and then escapes its scope (stored in a callback, container, thread, or timer) causing a dangling reference; and `epoll` misuse on Linux (edge-triggered draining, `epoll_ctl`/`close` fd races, and dangling `epoll_event.data.ptr`).

#### Scenario: shared_from_this pitfall present
- **WHEN** a reader looks in the resources & ownership chapter
- **THEN** an entry explains the `shared_ptr<T>(this)` double-control-block bug and prescribes `enable_shared_from_this` + `shared_from_this()`, noting it must not be called from the constructor

#### Scenario: Lambda dangling capture pitfall present
- **WHEN** a reader looks in the concurrency chapter
- **THEN** an entry explains dangling reference caused by `[&]` / `this` capture in an escaping lambda and prescribes fixes (capture by value, capture a `shared_ptr`/`weak_ptr`, or extend lifetime via `enable_shared_from_this`)

#### Scenario: epoll pitfalls present and platform-labeled
- **WHEN** a reader looks in the systems programming & Linux I/O chapter
- **THEN** entries cover edge-triggered draining, `epoll_ctl`/`close` fd-reuse races, and dangling `data.ptr`, and each is labeled Linux-only (非可移植) with required headers noted

### Requirement: Per-mistake entry format
Each mistake entry SHALL follow a fixed format containing: a numbered title summarizing the error, a minimal bad-code example, an explanation of why it is wrong (explicitly identifying undefined behavior where applicable), an idiomatic fixed-code example, and the applicable C++ standard. It MAY additionally cite a relevant ISO C++ Core Guideline identifier.

#### Scenario: Required sections present
- **WHEN** a reader reads any of the 100 mistake entries
- **THEN** the entry contains a numbered title, a bad example, a "why it is wrong" explanation, an idiomatic fix, and an applicable-standard note

#### Scenario: Undefined behavior flagged
- **WHEN** a mistake causes undefined behavior
- **THEN** the explanation explicitly states that the code is undefined behavior (未定义行为)

#### Scenario: Standard scoping is accurate
- **WHEN** a fix relies on a language or library feature
- **THEN** the entry names the minimum C++ standard (C++11/14/17/20/23) that provides that feature, or marks the pitfall as "C++98 遗留" when it predates modern C++

### Requirement: Table of contents and navigation
The book SHALL provide a top-level README that serves as the authoritative table of contents, linking to every chapter and to each mistake by anchor.

#### Scenario: TOC links resolve
- **WHEN** a reader follows any table-of-contents link
- **THEN** the link resolves to the corresponding chapter or mistake within the manuscript

#### Scenario: Every mistake is reachable from the TOC
- **WHEN** the README table of contents is inspected
- **THEN** all mistakes are listed and each has a working anchor link

### Requirement: Original content and sourcing
The book's prose and code examples SHALL be original. Reference works (e.g., 100 Go Mistakes, ISO C++ Core Guidelines, Effective Modern C++) SHALL be used only for structure and topic coverage, not copied.

#### Scenario: No copied text
- **WHEN** the manuscript is reviewed against reference works
- **THEN** no prose or code is copied verbatim from those works; the shared elements are limited to organizational format and topic selection

### Requirement: File and directory layout
The manuscript SHALL be delivered as Markdown files under a dedicated book directory, with one file per chapter plus the README table of contents.

#### Scenario: Layout matches convention
- **WHEN** the book directory is inspected
- **THEN** it contains a README file and thirteen chapter Markdown files, each chapter file named to reflect its theme and ordering
