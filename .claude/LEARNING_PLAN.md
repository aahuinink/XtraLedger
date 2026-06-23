# XtraLedger — Learning Plan

A double-entry accounting engine in ANSI C, used as a vehicle to practice professional
patterns: data-oriented design, CMake, testing, profiling, SQLite, and cross-compilation.

_Last reviewed: 2026-06-20 (Opus)._

## Milestones

| # | Milestone | Status | Primary learning goal |
|---|-----------|--------|-----------------------|
| 1 | Memory arenas (base + account SoA) | ✅ done | Data-oriented design |
| 2 | String→slot lookup table (hash map) | 🚧 in progress — **resize/rehash broken** | Open addressing, testing rigor |
| 3 | `account_store` wiring (arena + index behind handle API) | ⬜ next | API design, ownership/lifetimes |
| 4 | Test harness + debug instrumentation maturing | 🚧 ongoing | Test design, invariants over smoke tests |
| 5 | Profiling instrumentation (timers/counters in debug builds) | ⬜ planned | Measurement-driven optimization |
| 6 | SQLite storage plugin (`src/storage/plugins/sqlite.c`) | ⬜ stub exists | Databases, persistence |
| 7 | Cross-compile to Windows from WSL | ⬜ planned | Toolchains, CMake cross-compilation |
| 8 | REPL app surface (`app/repl`) | ⬜ skeleton | End-to-end integration |

## Current focus (this week)
1. **Write ADR 0002** — SoA assumptions revisited (drop `generation` + `tags` byte,
   normality as its own enum column, descriptions into a string pool). Supersedes 0001
   D4/D5; enforces D1. A. Huinink authors, Opus reviews.
2. **Fix milestone 2.** Rehash on resize, calloc the entries, bound the probe loop, and —
   most importantly — write the test whose assertions actually exercise a resize boundary.
   (Unchanged by ADR 0002: names stay inline in the SoA this week, so the lookup table /
   I2 is untouched.)
3. **SoA cleanup (ADR 0002):** remove `generation`, remove the `tags` byte, add the
   normality enum column; stretch — move `descs` into a relocation-safe string pool.
4. Tighten `account.h` (`const` placement on snapshot/view, out-param qualifiers) so
   milestone 3 can start on solid footing. *(`account_store` stub itself bumped to next week.)*

## Recurring habits I'm grading you on
- **Tests assert properties, not absence of crashes.** Every data-structure test should
  encode an invariant.
- **Uninitialized memory is a hard error.** calloc/memset or initialize explicitly.
- **Readability before cleverness; cleverness only after a profile.** (Milestone 5 exists to
  give you the data to justify the clever versions you already wrote.)
- **One naming convention for filenames** (kebab vs snake — pick one).

## Parking lot / future reading
- **Type/account "buckets":** partition accounts by type (cash/asset/liability/…); normality
  becomes *derived* from the bucket, superseding the normality column and D1's flat-slot
  handle. Needs a new ADR — touches handles, the lookup table, and lockstep growth.
- Robin Hood hashing & backward-shift deletion (once basic open addressing is solid).
- `restrict`, alignment, and SoA cache behavior (ties into milestone 5 profiling).
- CMake toolchain files for mingw-w64 (milestone 7).

## Progress log
- **2026-06-21** — SoA design session (Opus). Revisited account-arena assumptions before
  `account_store` cements them: drop `generation` + `tags` byte, normality → own enum column,
  descriptions → string pool (index, not pointer; names stay inline for now). Decisions
  captured in HANDOFF for **ADR 0002** (A. Huinink to write). `account_store` stub bumped to
  next week; rehash fix still the milestone-2 unblocker and unaffected by the SoA changes.
- **2026-06-20** — First mentored session. Reviewed weeks of arena + lookup-table work.
  Lookup table architecture is strong; resize path has a latent correctness bug masked by
  weak test assertions. Assigned homework `20-06-2026/QUESTIONS.md`. Plan established.
