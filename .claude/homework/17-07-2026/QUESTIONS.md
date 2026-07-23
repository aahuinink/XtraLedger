# Homework — Week of 2026-07-17

Answer in this file under each question (add an `### Answer` block). Write prose, not just
code — I'm grading your *reasoning*. Aim for a paragraph each; cite line numbers from your
own code where relevant. This week is the account-arena rewrite (ADR 0002), so the questions
are the design decisions you'll have to defend *while* writing it, not after.

---

## Q1 — Why the desc string pool is DEFERRED (connects to: ADR 0002 stretch, SoA vs AoS locality, profile-first)
We talked this through 2026-07-17 and landed on: keep `descs` inline as a fixed
`xl_smallstr128` column; defer the pool. Write the decision up as if it were the ADR 0002
amendment. Cover, in your own words: (1) why a relocating pool and the lifetime-guaranteeing
`view` API are in direct conflict, and how reserve-and-bump dissolves it; (2) why your original
cache-locality worry is the *AoS* failure mode that choosing SoA already eliminates — be
concrete about what a `balances[]` scan does and does not touch; (3) why `offset+len` is a
wash-to-worse for the one workload it would help (name the second cache miss); (4) the footprint
number (~6.5 MB at cap `0x8000`) and why it makes the pool a pre-profile optimization your own
constraints forbid. End with the exact condition (a profile result) that would REOPEN the pool.

### Answer
`desc` will stay as an inline column, not as a pool. This is an amendment to ADR 0002.

A relocating pool breaks the lifetime guarantee of a view API, because after the pool relocates, any view holding
a pointer into the old pool will be invalid. Reserving the entire description column memory up-front for a bump-allocating arena
means that any view into the arena via a pointer remains valid for the lifetime of the program.

Cache-locality is solved by an SoA arena because descriptions are held next to each-other in memory. 
For example, a `balances[]` scan will iterate through balances that stored in contiguous blocks of memory.
The scan will only ever load balances into the cache and touch those; it will not have to also load other
struct fields.

Using an `offset+len` scheme to index into a separate `desc` pool will actually make cache locality worse,
since there now can be two cache misses - one when reading the `offset+len` data from the arena,
and one when indexing into the `descs` pool. Now two cache lines may be filled, which also evicts other data
from the cache. It is better to just have one cache miss and only use the cache lines required to load the `desc`
small string.

Pre-allocating all the space needed to store a full arena requires about 5.0MB: 
```
(0.7 load factor * 0x8000 lookup table capacity * 197 bytes/account) + (0x8000 capacity * 8 bytes/lookup entry) = ~5.0MB
```

If a profile shows that fetching descriptions becomes the bottleneck AND it greatly disrupts user experience,
then we will reopen the description/name implementation and do some more optimizations.

## Q2 — Fixing the `initialize` return contract (connects to: review finding #1)
`arena_lookup_table_initialize` returns a `uint16_t` write-count coerced to `bool`
(`arena-lookup-table.c:62`), so "at least one key written" reads as success. Explain precisely
what goes wrong for (a) a duplicate in the *first* key and (b) a duplicate in the *third of
five* keys — what each caller sees and why both are wrong. Then decide the contract: should
`initialize` mean "all keys inserted" or something else? Rewrite the signature(s) and the
error reporting (return value + `xl_errno`) so a caller can tell "fully succeeded," "partially
succeeded, N written," and "failed, nothing written" apart. Implement it. Note: `account_store`
is the next caller — design the contract you'd want to consume, not the one that's easy to write.

### Answer

## Q3 — Make the max-capacity call and kill the cleverness (connects to: review finding #3, readability constraint)
`hash_to_index` (`arena-lookup-table.c:66-69`) uses two stacked branchless tricks to survive
the one capacity that isn't a power of two (`UINT16_MAX`), and biases slot 0 as a side effect.
Make the design decision with numbers: option A caps max capacity at `0x8000` (power of two,
`hash_to_index` becomes `return hash & (capacity - 1);`, account ceiling ~22.9k); option B keeps
`UINT16_MAX` and the special-casing (ceiling ~45.9k). Argue which is right *for a double-entry
ledger* — how many distinct accounts does a real chart of accounts have, and does that ceiling
ever bind? Whichever you pick, implement it and explain what test(s) change. If you keep B, you
must justify the unreadable index math against your own "readability over cleverness" constraint.

### Answer

## Q4 — normality: column now, derived later? (connects to: ADR 0002, learning-plan parking lot, YAGNI)
ADR 0002 adds a `normality` enum column to the account SoA *now*, but the parking lot says
normality will eventually be *derived* from an account "type/bucket" (cash/asset/liability/…),
superseding the column. Reconcile these. Is adding the column now the right call, or is it work
you'll throw away — and how do you tell the difference between "designing for a known change"
and "premature over-design (YAGNI)"? What would have to be true about the bucket ADR's timeline
for adding the column now to be clearly correct vs. clearly wasteful? Give the decision rule you
used, not just the verdict.

## Q5 (stretch) — Handles, deletion, and the generation you're deleting (connects to: I1 invariant, D1, generation removal)
ADR 0002 *removes* the `generation` field, and invariant I1 says "slots are never reused or
deleted." Explain the connection: why does I1 make a bare slot index safe to hand out as a
`xl_account_handle`, and why does that same invariant make `generation` redundant *today*?
Then break it: the moment a future feature allows account deletion + slot reuse, what exact bug
does a long-lived handle cause (the "ABA" problem — describe the sequence)? What does a
generation counter do to detect it, and how many bits do you need? Conclude: is removing
generation now correct, or are you removing the seatbelt before the crash? Defend either way.

### Answer

---

### How this connects to the project
Q1 and Q4 are the two live design calls in the ADR 0002 SoA rewrite you're starting this week —
answer them before you write the struct. Q2 and Q3 are the two review findings from the lookup
table that `account_store` will inherit if you don't fix them first; both are prerequisites to
the milestone-3 stub. Q5 pins down *why* the handle API is shaped the way it is, so that when
the bucket/deletion ADR eventually lands you'll recognize exactly what invariant it's spending.
