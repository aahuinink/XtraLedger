# ADR 0002 - Account SoA Design

- **Status:** Accepted - Supersedes ADR 001 D4, D5, clarifies D1
- **Date:** 2026-06-21
- **Deciders:** A. Huinink (with Opus mentor review)
- **Relates to:** `requirements.md` V1B; `architecture.md` §4; Milestones 2–3

## Context

`account_store` contains an SoA account arena used to store ledger information. The account store manages the memory for the account arena for the user. This ADR defines how the arena is structured.

## Decisions

### D1 - Slots are never reused; Account IDs are never reused

Clarifying ADR 0001 D1. Accounts can never be deleted, since decoupling a closed account from other open accounts in a double ledger system will be nearly impossible. Therefore, slots shall not be reused. 

### D2 - Normality is stored

Account normality is stored as an enum with the account. Some of the bits of the enum will be wasted storage, but storing this information "out of band" would be too much work to be worth saving a few kB at most.

### D3 - String storage

Account names and descriptions are stored inline in the arena. Cache locality is not really an issues since we are using an SoA (i.e. not worried about a 128 byte description evicting other balances).

### D4 - Growth and resizing

The account arena grows geometrically, doubling on each resize, with a minimum capacity of 256.

### D5 - No account status/tag bits

Account tags will be stored out of band using a "bucketing" system to be used later. For example, a "Net Worth" bucket will store a list of all account slots that contribute to net worth, rather than tagging each account as an asset or liablity.

## Consequences

- (+) Updating the account store will have unpredictable latency spikes if the SoA needs to be resized.
- (+) Some bits of the normality enum will be wasted.
- (+) Buckets to be implemented in the future.
