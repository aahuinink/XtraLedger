# HANDOFF — current state

> Single source of "what's next." Written/updated at `/session-end`, read first at
> `/session-start`. This is the *current* pointer; the weekly `journal/` files are the
> historical log. Keep this short and action-oriented — if a section is stale, fix it.

_Last updated: 2026-06-20 (Opus session)_

## Current focus
Milestone 2 (fix the lookup table) → Milestone 3 (`account_store`). Milestone 3 is
**blocked** on Milestone 2's resize path being correct.

## Next actions (in order — start at the top)
1. **Fix lookup-table rehash-on-resize.** `populate_lookup_table` only inserts the new
   keys; existing entries keep stale positions after a grow. Reinsert every entry using the
   stored `hash` field (don't re-run DJB2). See homework Q1.
2. **Switch entries allocation to `calloc`** and make `state == 0` mean "empty"; delete the
   `0xF364` sentinel gamble. See homework Q3.
3. **Add the resize-survival test:** after growing 8→16, re-query all original keys and
   assert each still resolves to its slot. This is the test that would have caught the bug.
   See homework Q2.
4. **Only then** stub `account_store` per `docs/decisions/0001-account-store-design.md`:
   start with `account_store_init`/`deinit` and the `_by_handle` accessors (slot-only,
   bounds-checked).

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
