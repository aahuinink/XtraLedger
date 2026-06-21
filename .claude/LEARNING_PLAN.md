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
1. **Fix milestone 2.** Rehash on resize, calloc the entries, bound the probe loop, and —
   most importantly — write the test whose assertions actually exercise a resize boundary.
2. Tighten `account.h` (`const` placement on snapshot/view, out-param qualifiers) so
   milestone 3 can start on solid footing.

## Recurring habits I'm grading you on
- **Tests assert properties, not absence of crashes.** Every data-structure test should
  encode an invariant.
- **Uninitialized memory is a hard error.** calloc/memset or initialize explicitly.
- **Readability before cleverness; cleverness only after a profile.** (Milestone 5 exists to
  give you the data to justify the clever versions you already wrote.)
- **One naming convention for filenames** (kebab vs snake — pick one).

## Parking lot / future reading
- Robin Hood hashing & backward-shift deletion (once basic open addressing is solid).
- `restrict`, alignment, and SoA cache behavior (ties into milestone 5 profiling).
- CMake toolchain files for mingw-w64 (milestone 7).

## Progress log
- **2026-06-20** — First mentored session. Reviewed weeks of arena + lookup-table work.
  Lookup table architecture is strong; resize path has a latent correctness bug masked by
  weak test assertions. Assigned homework `20-06-2026/QUESTIONS.md`. Plan established.
