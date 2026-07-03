# XtraLedger — Learning Plan

A double-entry accounting engine in ANSI C, used as a vehicle to practice professional
patterns: data-oriented design, CMake, testing, profiling, SQLite, and cross-compilation.

_Last reviewed: 2026-07-01 (Sonnet)._

## Milestones

| # | Milestone | Status | Primary learning goal |
|---|-----------|--------|-----------------------|
| 1 | Memory arenas (base + account SoA) | 🚧 reopened by ADR 0002 — `generation`/`tags` removal pending | Data-oriented design |
| 2 | String→slot lookup table (hash map) | 🚧 core logic correct, passes — **test coverage still thin** | Open addressing, testing rigor |
| 3 | `account_store` wiring (arena + index behind handle API) | ⬜ next | API design, ownership/lifetimes |
| 4 | Test harness + debug instrumentation maturing | 🚧 ongoing | Test design, invariants over smoke tests |
| 5 | Profiling instrumentation (timers/counters in debug builds) | ⬜ planned | Measurement-driven optimization |
| 6 | SQLite storage plugin (`src/storage/plugins/sqlite.c`) | ⬜ stub exists | Databases, persistence |
| 7 | Cross-compile to Windows from WSL | ⬜ planned | Toolchains, CMake cross-compilation |
| 8 | REPL app surface (`app/repl`) | ⬜ skeleton | End-to-end integration |

## Current focus (this week)
1. **Harden milestone-2 tests before starting the arena rewrite.** The rewrite is correct
   and passes, but coverage is one happy path. Add, at minimum: a not-found lookup
   (`XL_ENFND` + `-1`), a duplicate-key insert (`XL_EDUPL`, partial `successfull_writes`
   count from `arena_lookup_try_update`), a regression test pinning the "empty slot at
   index 0" case (the exact ambiguity the `UINT32_MAX - index` encoding was written to
   fix), and a near-`UINT16_MAX` overflow-guard test in `increase_capacity_if_needed`.
   Do this now — `account_store` builds directly on top of this table, and a latent bug
   here becomes much harder to trace once it's wrapped behind a handle API.
2. **SoA cleanup (ADR 0002):** remove `generation`, remove the `tags` byte
   (`account-arena.h`/`.c` still have both), add the normality enum column; stretch —
   move `descs` into a relocation-safe string pool.
3. Tighten `account.h` (still open, 4th session running): drop `const` from
   `xl_account_snapshot` fields (lines 29–31), make `xl_account_view.norm` a pointer
   (line 37) to match every other view field.
4. Then, and only then, start the `account_store` stub (milestone 3).

## Recurring habits I'm grading you on
- **Tests assert properties, not absence of crashes.** Every data-structure test should
  encode an invariant.
- **Uninitialized memory is a hard error.** calloc/memset or initialize explicitly.
- **Readability before cleverness; cleverness only after a profile.** (Milestone 5 exists to
  give you the data to justify the clever versions you already wrote.)
- **One naming convention for filenames** (kebab vs snake — pick one).
- **Turn on `-Wall -Wextra`.** Neither is set anywhere in the CMake config. It would have
  caught the dead `old_table_size` variable in today's rewrite for free; add `-Wconversion`
  at some point too since narrowing bugs (like the `int32_t`→`uint16_t` one in `put()` you
  just fixed) live in exactly the kind of unsigned/signed index math this table is full of.

## Parking lot / future reading
- **Type/account "buckets":** partition accounts by type (cash/asset/liability/…); normality
  becomes *derived* from the bucket, superseding the normality column and D1's flat-slot
  handle. Needs a new ADR — touches handles, the lookup table, and lockstep growth.
- Robin Hood hashing & backward-shift deletion (once basic open addressing is solid).
- `restrict`, alignment, and SoA cache behavior (ties into milestone 5 profiling).
- CMake toolchain files for mingw-w64 (milestone 7).

## Progress log
- **2026-07-01** — Lookup-table rewrite finished and `TestInternalsLookupTable` passes.
  Two real bugs found and fixed en route: (1) `put()` stored `get()`'s signed "not found"
  return in a `uint16_t`, so the unsigned wraparound made every insert look like a
  duplicate — this is why `arena_lookup_table_initialize` failed outright the first time
  it was reviewed; (2) the original negative-index encoding for "not found, insert here"
  collided with a real match at index 0 (`-1 * 0 == 0`) — replaced with a
  `UINT32_MAX - index` encoding, which is unambiguous given valid slots never approach
  `UINT16_MAX`. Style pass done (typos, stray semicolons, an unused variable `-Wall`
  would've caught, `hash` name shadowing itself as both function and local/param name).
  Decision: harden lookup-table tests before starting the account-arena rewrite, since
  the arena work builds directly on this table.
- **2026-06-21** — SoA design session (Opus). Revisited account-arena assumptions before
  `account_store` cements them: drop `generation` + `tags` byte, normality → own enum column,
  descriptions → string pool (index, not pointer; names stay inline for now). Decisions
  captured in HANDOFF for **ADR 0002** (A. Huinink to write). `account_store` stub bumped to
  next week; rehash fix still the milestone-2 unblocker and unaffected by the SoA changes.
- **2026-06-20** — First mentored session. Reviewed weeks of arena + lookup-table work.
  Lookup table architecture is strong; resize path has a latent correctness bug masked by
  weak test assertions. Assigned homework `20-06-2026/QUESTIONS.md`. Plan established.
