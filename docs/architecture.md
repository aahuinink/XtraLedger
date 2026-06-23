# architecture.md

**Date Created**: March 6, 2026

**Date Updated**: March 24, 2026

**Author**: A. Huinink

<u>**Document Revision History**</u>

| Revision (M.m)| Revised By    | Date          | Comment       |
| ------------- | ------------- | ------------- | ------------- |
| 1.0           | A. Huinink    | March 24, 2026 | Initial Release| 
| 1.1           | A. Huinink    | March 24, 2026 | Clarified some names and shuffled hierarchy.| 

## 1. Introduction

Describes the general architecture of the XtraLedger (XL) core library and application frontends.

### 1.1 Birds-Eye View

![Block Diagram](assets/birds-eye-view.png)

The core library _libxl_ provides the data types, the business logic, and a storage plugin interface.
The app library provides UI wrappers around the core library.

## 2. XL Core library

The core library provides a double-ledger system to handle finance data.

### 2.1 Public Includes

#### 2.1.1 xlcore/xlcore.h

Umbrella include.

#### 2.1.2 xlcore/errors.h

Defines errors.

#### 2.1.3 xlcore/account.h

Account datatype.

#### 2.1.4 xlcore/transaction.h

Transaction data type.

### 2.2 Source

#### 2.2.1 xlcore/api

Public frontend API.

#### 2.2.2 xlcore/domain

Core business logic, account, and transaction data types.

#### 2.2.3 xlcore/storage

Storage interface that decouples domain from storage plugins.

##### 2.2.3.1 storage_plugins/sqlite.cpp

SQLite implementation.

## 3. App 

The app library provides UI wrappers around the business logic.

### 3.1 app/repl

A REPL CLI for interacting with the database (v1 of XL).

### 3.2 app/qt

A Qt GUI for interacting with budget data (TBD, probably v2).

## 4. Memory Layout and Workflow

Minimize db transactions. Use a batch queue -> commit cycle. 

At startup, all existing accounts are pulled into a struct of arrays memory arena. The arena is accessed by an account view, which is an index to an existing account in the arena. A hashmap of account views based on account names is also created. Accounts can be searched by name. The arena allocates a bit of extra space, but it will expand as needed by adding a fixed amount in case new accounts are created.

A global staging area is created with memory arenas for all the CUD job types:
 1. Open account
 2. Updated accounts
 3. New transactions
 4. Updated transactions
 5. Deleted transactions

For now, transactions need to be fetched from the DB every time.
Eventually, I'd like to implement a cache of recently searched transactions.

Jobs are added to the staging area. 
Once the commit call is made, then the staging area is written to the database.
First, new accounts are opened.
Next, accounts are updated.
Next, transactions are updated.
Next, transactions are deleted,
Finally, new transactions are created,

NOTE - You cannot add transactions to an account until it is commited to the db.

At shutdown, the memory arenas and hashmap are all deallocated.
