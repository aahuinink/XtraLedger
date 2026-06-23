# HANDOFF — current state

> Single source of "what's next." Written/updated at `/session-end`, read first at
> `/session-start`. This is the *current* pointer; the weekly `journal/` files are the
> historical log. Keep this short and action-oriented — if a section is stale, fix it.

_Last updated: 2026-06-21 (Opus, mid-session design discussion)_

## Completed last session (2026-06-20)
Mentoring workflow + skills stood up; `account_store` design locked as ADR 0001; week's
code reviewed; setup committed/pushed. No production code written yet.

## This session (2026-06-21, Opus) — SoA assumptions revisited
Design discussion before `account_store` cements the SoA. Decisions reached (raw material
for **ADR 0002**, which A. Huinink writes; supersedes parts of 0001):
- **Drop the `generation` column.** Aligns the arena with D1 (slot-only handles); the column
  was dead weight nothing read. *Not* a reversal of D1 — it enforces it.
- **Drop the `tags` byte entirely** (net-worth class + status bit). Net-worth class is
  subsumed by the future type/bucket dimension (below); the reserved status bit is dropped —
  a **conscious reversal of D4**, accepting a future format migration if deactivation returns.
- **Normality → its own 1-byte `enum xl_acct_normality` column.** Wastes 7 bits for now;
  accepted. Supersedes D5 (no longer packed in a tag byte). Stored directly, *not* derived.
- **Descriptions → a string pool (sequencing option (a)).** `descs` column becomes a 4-byte
  index (`uint32_t`) into a separate `xl_smallstr128` pool; **names stay inline in the SoA**
  for now (keeps the lookup table / I2 untouched this week). Store the **index, not a
  pointer** — the pool reallocs on grow. Snapshots copy out (safe); desc *views* resolve
  through `pool_base + idx` or are documented as valid only until the next pool-growing insert.
- **Capacity policy: grow-on-demand, geometric, sized to the DB row count at load.**
  Pre-allocating the full uint16 range is **rejected** (~5–13 MB to hold what is realistically
  hundreds of rows). At startup the store pre-fetches all accounts and inits capacity to the
  loaded count rounded up to the next power of two → steady state does **zero** resizes. Keep
  doubling, not linear (+N): linear is O(n²) total copy work, and — decisive — the lookup
  table is power-of-two-only and grows in lockstep (D3), so a linear arena can't stay aligned
  with it. "Not perf-critical" defeats the resize-latency worry; optimize footprint + simplicity.
  Future, only if profiled (milestone 5): chunked/segmented arena (stable addresses, no copy
  on grow) — also fixes the desc-pool view-lifetime hazard.
- **Future (reserved, not built): type/account "buckets."** Partition accounts by type
  (cash/asset/liability/…); normality would then be *derived* from the bucket, superseding
  both the normality column and D1's flat-slot handle. New ADR when we build it.

## Current focus
ADR 0002 + SoA cleanup, *then* finish Milestone 2 (lookup-table rehash), *then* Milestone 3
(`account_store`). Milestone 3 is still **blocked** on the rehash being correct — option (a)
deliberately keeps the lookup table out of this week's SoA changes so that fix is unchanged.

## Next actions (in order — start at the top)
1. **Write ADR 0002** from the decision list above. Mark it as superseding D4/D5 and
   *enforcing* D1; reserve the type/bucket direction in Consequences.
2. **Fix lookup-table rehash-on-resize** (unchanged by ADR 0002 — names stay in SoA).
   `populate_lookup_table` only inserts the new keys; existing entries keep stale positions
   after a grow. Reinsert every entry using the stored `hash` field (don't re-run DJB2). Q1.
3. **Switch entries allocation to `calloc`**, `state == 0` means "empty"; delete the
   `0xF364` sentinel gamble. Q3. **Bound the probe loop by capacity** (review bug #4).
4. **Add the resize-survival test:** after growing 8→16, re-query all original keys and
   assert each still resolves to its slot. The test that would have caught the bug. Q2.
5. **SoA cleanup (ADR 0002):** remove `generation`; remove the `tags` byte; add the
   `normality` enum column. Mechanical, low risk.
6. **Descs string pool (stretch — may slip):** pool alloc/grow, `uint32_t` index column,
   wire the `desc` accessors. Mind the relocation/view-lifetime note above.

## Bumped to next week
- Stub `account_store` (`init`/`deinit` + `_by_handle` accessors, slot-only, bounds-checked).
  Was the old action #4; displaced by ADR 0002 + SoA work. Still blocked on the rehash fix.

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
