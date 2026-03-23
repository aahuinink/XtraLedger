# architecture.md

**Date Created**: March 6, 2026

**Date Updated**: March 23, 2026

**Author**: A. Huinink

<u>**Document Revision History**</u>

| Revision (M.m)| Revised By    | Date          | Comment       |
| ------------- | ------------- | ------------- | ------------- |
| 1.0           | A. Huinink    | March 6, 2026 | Initial Release| 

## 1. Introduction

Describes the general architecture of the ExtraLedger (XL) core library and applicaiton frontends.

### 1.1 Birds-Eye View

[assets/block-diagram.png]

The core library _xlcore_ provides the data types, the business logic, and a storage plugin interface.
The app library provides UI wrappers around the core library.

## 2. XL Core library

The core library provides a double-ledger system to handle finance data.

### 2.1 xlcore/domain

Core data types like Transaction and Account.

### 2.2 xlcore/service

Buisiness logic.

### 2.3 xl/core/storage

Storage plugin interface for interacting with the database.

## 2. App 

The app library provides UI wrappers around the business logic.

### 3.1 app/repl

A REPL CLI for interacting with the database (v1 of XL).

### 3.2 app/qt

A Qt GUI for interacting with budget data (TBD, probably v2).

## 4. Storage Plugins

The storage plugins implement interfaces provided in the _xlcore/storage_ library, which are different depending on the database implementation.

### 4.1 storage/sqlite

A SQLite implementation of the storage interface (v1).

### 4.2 storage/lmdb

An LMDB implemenation of the storage interface (TBD).
