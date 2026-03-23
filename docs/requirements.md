# Requirements

These are the requirements for this budget app.

**Document Creation**: March 6th, 2026

**Last Updated**: March 23, 2026

**Author**: Aaron Huinink

<u>**Document Revision History**</u>

| Revision (M.m)| Revised By    | Date          | Comment       |
| ------------- | ------------- | ------------- | ------------- |
| 1.0           | A. Huinink    | March 6, 2026 | Initial Release| 

## 1. Introduction
### 1.1. Purpose

Describes the requirements of my personal finance app, ExtraLedger (XL).

### 1.2. References

None at this time

### 1.3. Assumptions

None

## 2. Requirements

The app should replace our current Excel budget spreadsheet, with the possibility of new features being added later.

Functional use cases for each major version of the application are identified under second level headings with a functional ID of the format 
    
    **V<Major Version #><A-Z>**.

The testable requirements for each functional use case are identified with a requirement ID with the format **<Functional ID>-<Requirement #>**.

### 2.1 Version 1 - All Excel Features with a Basic REPL

Version 1 shall implement the basic features of my current Excel budget document:

**V1A** - Transactions & Issuers - I will be able to record both expenditure and income (E&I) transactions as one-time lump sum payments, or create an Issuer, which amortizes E&I as scheduled regular installment transactions with a certain frequency.
**V1B** - I will be able to create basic accounts which are chronological records of transactions. The accounts shall operate with a double-ledger principle and have hashes associated with each transaction, which allows me to go back and edit things (similar to git).
**V1C** - I will be able to see snapshots of transactions and accounts over time, like net weekly expenditures and income.
**V1D** - I will have a REPL with simple CRUD commands to interact with my accounts.

#### 2.1.1 v1.0 - Basic Functionality

This version will implement the following requirements for V1A, B, and C:

_V1A - Transactions & Issuers_ 
1) A Transaction shall be a pair of 1 credit account record and 1 debit account record.
2) A Transaction shall include an amount, a date, a description, a type (cr/dr), a sibling (the ID of the corresponding dr/cr account record), the current account balance (including this transaction), and an ID (the account ID followed by the transaction index).
3) A Transaction may include a parent Issuer ID.
4) An Issuer shall amortize E&I as recurring installments over a period of time.
5) An Issuer shall issue transactions to specific accounts according to a user-specified frequency with a granularity of a day.
6) A transaction shall allow expenses or income transactions to be repeated for a certain number of time periods, starting on a start date.

_V1B - Basic Accounts_
1) An account shall be a chronological sequence of transactions that results in a final ledger balance.
2) An account shall have an enumerated type (asset, liability, or n/a), a normality (credit or debit), a unique ID, and a sequence of transaction.
3) An account shall have a net ledger balance based on the sequence of its records.
4) An account shall raise a warning if the ledger balance goes below zero.
5) An account shall 

### 2.1.2 v1.1 - Snapshots by Time

This version will implement the following requirements for V1C:

_V1C - Transaction Snapshots_
1) A snapshot will provide all the account activity between a start and end time, inclusive, with a granularity of 1 day.

### 2.1.3 v1.2 - REPL UI

This version will implement the following requirements for V1D:

_V1D - REPL_
1) XL shall provide a REPL interface to create, read, update, and delete transactions, issuers, and accounts.

## Traceability

|Requirement|Completed|Test ID|
|-|-|-|
|MV1A.1| | |
|MV1A.2| | |
|MV1A.3| | |
|MV1A.4| | |
|MV1A.5| | |
|MV1A.6| | |
|MV1B.1| | |
|MV1B.2| | |
|MV1B.3| | |
|MV1B.4| | |
|MV1B.5| | |
|MV1C.1| | |
|MV1B.1| | |
