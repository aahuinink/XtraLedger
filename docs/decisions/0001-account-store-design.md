# ADR 0001 — account_store design

- **Status:** Accepted
- **Date:** 2026-06-20
- **Deciders:** A. Huinink (with Opus mentor review)
- **Relates to:** `requirements.md` V1B; `architecture.md` §4; milestones 2–3

> An Architecture Decision Record captures a single significant choice: the context
> that forced it, the decision, and the consequences we accept. ADRs are append-only —
> if we change our minds later, we add a new ADR that supersedes this one rather than
> editing history.

## Context

`account_store` is the layer that ties together the SoA account arena, the name→slot
lookup table, and handle validation, and presents the accessor API in `account.h`. Its
job is one guarantee: a handle resolves to the account it was minted for, or it fails —
never to a different account.

## Decisions

### D1 — Handles are slot-only (`{ uint16_t slot }`)
Accounts are never deleted and slots are never reused (see Invariant I1), so a slot
uniquely identifies an account for the life of the store. A generation counter would add
nothing. Lookups still bounds-check `slot < size` so a malformed handle fails loudly.

### D2 — `account_store` is heap-allocated and opaque
`account_store_init(void)` allocates the store and returns
`struct xl_account_store *` (or `NULL` + `xl_errno` on failure).
`account_store_deinit` frees it. The struct stays opaque to callers; only core code
sees its definition. (Supersedes the earlier `bool init(store*)` signature.)

### D3 — The store owns growth; arena and index resize in lockstep
The lookup table is never resized except by the store, immediately after an arena grow:
grow arena → re-point `table->keys` at the moved `names` column → `arena_lookup_try_update`.
This makes a correct rehash-on-resize (milestone 2) a hard prerequisite for the store.

### D4 — No account-closing operation in v1
v1 has no close/deactivate/delete operation. Justified because the V1B functional
requirements never mention closing — it appeared only as an architectural assumption.
One status bit is *reserved* in the tag byte for future deactivation (free now, avoids a
format migration later), but no logic reads or writes it yet. Period-end "closing the
books" is explicitly out of scope for v1.

### D5 — Normality is stored in the SoA tag column, surfaced as an enum
There is no account row struct at rest; an account is spread across arena columns.
Normality is one bit of `tags[i]`. It is materialized as `enum xl_acct_normality` only in
snapshots/views. Because normality is immutable, `xl_account_view.norm` may be a value
(not a pointer) — views need pointers only for mutable fields.

## Invariants this establishes

- **I1:** A slot, once assigned, is never reused or deleted. (D1 depends on this.)
- **I2:** The lookup table's `keys` always points at the arena's live `names` column. (D3)

## Consequences

- (+) No generation bookkeeping; simpler handles and lookups.
- (+) No-globals, testable store; clean opaque boundary for app/REPL.
- (−) Adding slot reuse or account deactivation later requires a new ADR and revisiting D1.
- (−) `architecture.md` §4 still describes a "closed accounts" commit step and must be
  updated to match D4. **(Action: A. Huinink)**
