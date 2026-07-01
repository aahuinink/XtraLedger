# HANDOFF — current state

> Single source of "what's next." Written/updated at `/session-end`, read first at
> `/session-start`. This is the *current* pointer; the weekly `journal/` files are the
> historical log. Keep this short and action-oriented — if a section is stale, fix it.

_Last updated: 2026-06-25 (Sonnet, session-end)_

## Completed this session (2026-06-25)
- Committed baseline (`fd4da54`): homework Q4–Q5, `account.h` redesign, broken partial patch, test.
- Wrote `resize_table_if_needed` — correct skeleton for the rehash. **Uncommitted. In progress.**
- Locked sentinel design: `hash == 0` means empty; `state` field to be removed; `hash()`
  will guarantee non-zero return.

## Current focus
**Finish rewriting `arena-lookup-table.c`** — `resize_table_if_needed` is written but has
known bugs still to fix; rest of the file still has old code.

## Next actions (in order — start at the top)
1. **Fix `resize_table_if_needed` (in progress, uncommitted):**
   - Add linear probe in rehash loop (line 88): before copying, probe `new_entries` for the
     next slot where `hash == 0`, same pattern as `populate_lookup_table`.
   - Fix overflow check (line 53): `table->capacity >= (UINT16_MAX - num_new_keys)` →
     `(uint32_t)table->size + num_new_keys > UINT16_MAX`.
   - Drop the C++ cast on `calloc` (line 68).
2. **Implement `hash()` non-zero guarantee:** `return hash ? hash : 1;` at end of function.
3. **Remove `state` field** from `arena_lookup_entry` struct.
4. **Update `try_get` and `populate_lookup_table`** to use `hash == 0` (not `ENTRY_STATE_ACTIVE`).
   Remove the `ENTRY_STATE_ACTIVE` macro and the stale malloc/UB comment in `populate_lookup_table`.
5. **Wire up:** replace `increase_capacity_if_needed` call in `arena_lookup_try_update` with
   `resize_table_if_needed`. Delete `increase_capacity_if_needed`.
6. **Delete dead declarations:** `lookup_table_get`, `lookup_table_put`, `get_starting_entry`.
7. **Bound probe loop** in `arena_lookup_try_get`: after `capacity` probes, return -1.
8. **Run the resize-survival test** — rewrite is done when it passes.
9. **`account.h` Q4 fix**: remove `const` from `xl_account_snapshot` fields; make
   `xl_account_view.norm` a pointer. (Three sessions overdue — do this before next feature.)
10. **SoA cleanup (ADR 0002):** remove `generation`; remove `tags` byte; add `normality` column.
11. **Descs string pool (stretch):** pool alloc/grow, `uint32_t` index column, wire accessors.

## Bumped / still blocked
- Stub `account_store` (`init`/`deinit` + `_by_handle` accessors) — still blocked on the
  lookup-table rewrite being correct.

## Spec / references
- `docs/decisions/0001-account-store-design.md` — the locked account_store spec (D1–D5, I1–I2).
- This week's homework: `homework/20-06-2026/QUESTIONS.md`.
- This week's journal: `journal/22-06-2026.md`.
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

## Design decisions (lookup-table rewrite, 2026-06-25)
- **Empty sentinel: `hash == 0`** (not `state`). `state` field will be removed from
  `arena_lookup_entry`. Requires `hash()` to guarantee it never returns 0 — add
  `return hash ? hash : 1;` at the end of the function. Without that guarantee the
  sentinel is just as broken as the old `0xF364` approach.
