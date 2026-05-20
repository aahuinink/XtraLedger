#ifndef XL_MEMORY_ARENA_H
#define XL_MEMORY_ARENA_H

#include "base-arena.h"
#include "xlcore/datatypes.h"
#include <xlcore/account.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define ACCT_ARENA_START_FIELD balances

// account arena struct of arrays
struct acct_arena {
    int32_t * balances;
    xl_smallstr64 * names;
    xl_smallstr128 * descs;
    uint8_t * tags;
    uint8_t * generation;
    struct base_arena metadata;
};

// allocates the arena based on the number of accounts it must hold
bool account_arena_initialize(struct acct_arena * arena, uint16_t num_accts);

// frees the arena, nulls all values in account_arena_ref
void account_arena_deinitialize(struct acct_arena * arena);

// creates new accounts in the arena
// true upon success, false upon failure (check xl_errno)
// writes out the handles of the newly created account
bool account_arena_alloc(struct acct_arena * arena, const uint16_t num_new_accts, const int32_t * new_balances, const char * new_names, const char * new_descs, struct xl_account_handle ** out_handles);

#endif                  // XL_MEMORY_ARENA_H
