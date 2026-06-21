# Homework — Week of 2026-06-20

Answer in this file under each question (add an `### Answer` block). Write prose, not just
code — I'm grading your *reasoning*. Aim for a paragraph each; cite line numbers from your
own code where relevant.

---

## Q1 — Rehashing on resize (connects to: arena-lookup-table resize bug #1)
When the table grows from capacity 8 to 16, why can't you keep entries in their existing
array positions? Walk through, with a concrete hash value, what `hash_to_index` returns
before and after the grow, and explain in words the algorithm you'll use to rebuild the
table. Then implement it. **Constraint:** do it without re-running the DJB2 hash — you
already store `hash` in each entry, so use it.

## Q2 — Designing the test that would have caught the bug (connects to: testing discipline)
Your current test asserts `capacity == 0x10` and `size == 0x8` after the grow but never
re-queries the original 5 keys. Write down the *property* a lookup-table test should
guarantee (phrase it as an invariant), then write the assertions that encode it across a
resize boundary. Why is "the code ran without crashing" a weaker guarantee than "every
inserted key is retrievable"?

## Q3 — Uninitialized memory & the sentinel (connects to: bug #3, calloc vs malloc)
Explain the difference between "0xF364 is unlikely to appear by chance" and "reading
uninitialized memory is undefined behavior." Why does the second statement make the first
irrelevant? Convert the table to `calloc` with `state == 0` meaning empty, and explain one
measurable cost of `calloc` vs `malloc` and when it would matter.

## Q4 — `const` correctness in out-parameters (connects to: account.h redesign)
Explain why `const`-qualified *members* of `xl_account_snapshot` make the
`try_get_*_snapshot` functions impossible to implement cleanly. What's the difference
between "the caller should treat this snapshot as read-only" and "this struct's fields are
const"? Show the corrected struct and a 3-line caller example that gets read-only behavior
the right way.

## Q5 (stretch) — When is branchless worth it? (connects to: readability constraint + profiling goals)
Pick ONE of your branchless expressions (the hash lowercasing, the probe wraparound, or the
capacity mask). Write the readable branching version next to it. Describe an experiment you
could run to decide which is actually faster on your target hardware — what would you
measure, how would you isolate this function, and what result would justify keeping the
clever version? You don't have to run it yet (we'll wire up profiling soon), just design it.

---

### How this connects to the project
Q1–Q3 directly repair the lookup table so it can back `account_store` lookups by name — the
next dependency in the arena work. Q4 unblocks the `account.h` accessor layer you started.
Q5 plants the seed for the profiling milestone in the learning plan; we'll come back and
actually measure it.
