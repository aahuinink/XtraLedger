# HANDOFF — current state

> Single source of "what's next." Written/updated at `/session-end`, read first at
> `/session-start`. This is the *current* pointer; the weekly `journal/` files are the
> historical log. Keep this short and action-oriented — if a section is stale, fix it.

_Last updated: 2026-06-24 (Sonnet, session-end)_

## Completed this session (2026-06-24)
- Homework Q4–Q5 answered (uncommitted — stage and commit before starting code work).
- `account.h` redesigned: `xl_account_store` forward decl added, `xl_acct_normality` enum
  added, raw `char[]` → `xl_smallstr64`/`xl_smallstr128`, `store` param added to all
  accessors. **Uncommitted.**
- Resize-survival test written in `tests/internal/lookup-table.c` — failing as expected.
  **Uncommitted.**
- `increase_capacity_if_needed`: partial `calloc` switch — `malloc` → `calloc`, `memcpy`
  removed, but rehash NOT implemented (left as a comment). Implementation is currently
  broken (worse than before the patch). **Uncommitted.**
- Decision: **rewrite `arena-lookup-table`** rather than continue patching.

## Current focus
**Rewrite `arena-lookup-table`** — stacked bugs make patching worse than a clean rewrite.
Decision made 2026-06-24.

## Next actions (in order — start at the top)
0. **Commit working-tree changes** (homework Q4–Q5, `account.h` redesign, partial
   `calloc` patch, test): stage them, then commit before touching any code.
1. **Rewrite `arena-lookup-table`** with three goals:
   - **Better interface for `account_store`**: `account_store` (ADR 0001 D3) drives resize
     decisions. Expose a `resize`/`rebuild` entry point that `account_store` calls after
     growing its arena; remove self-triggering resize from inside the table.
   - **Iterator for resize**: iterator over live entries so the rehash walks existing entries
     cleanly — reinsert using `entry.hash`, never re-running DJB2.
   - **Fix the logic bugs** (`feedback/24-06-2026.md`): `memcpy` arg order, byte count,
     capacity stomped before copy, UB sentinel → `calloc`/`state==0`, unbound probe loop,
     dead `get_starting_entry` prototype.
2. **Resize-survival test already written** — make the rewrite pass it. Do not touch the test.
3. **`account.h` Q4 fix**: remove `const` from *fields* of `xl_account_snapshot`; make
   `xl_account_view.norm` a pointer (`const enum xl_acct_normality *`). Unblocks accessors.
4. **SoA cleanup (ADR 0002):** remove `generation`; remove `tags` byte; add `normality` enum
   column. Mechanical, low risk.
5. **Descs string pool (stretch):** pool alloc/grow, `uint32_t` index column, wire accessors.

## Bumped / still blocked
- Stub `account_store` (`init`/`deinit` + `_by_handle` accessors) — still blocked on the
  lookup-table rewrite being correct.

## Spec / references
- `docs/decisions/0001-account-store-design.md` — the locked account_store spec (D1–D5, I1–I2).
- This week's homework: `homework/20-06-2026/QUESTIONS.md`.
- This week's journal: `journal/15-06-2026.md`.
- `LEARNING_PLAN.md` — milestones.

## Open doc actions (owner: A. Huinink)
- Update `docs/architecture.md` §4 to drop the "closed accounts" job/step (per ADR 0001 D4).
- Add the I1 invariant comment ("slots are never reused or deleted") above
  `xl_account_handle` when `account.c` is rebuilt.

## Watch out for
- D3: the store re-points `table->keys` after every arena grow. Forgetting that line
  produces a dangling-pointer bug that *looks* like a hash bug. Comment it.
- The desc pool reallocs on grow → it relocates. Store **indices, not pointers** in the SoA
  column, and never hand out a long-lived raw pointer into the pool. Same hazard class as I2.
