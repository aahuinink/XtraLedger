#ifndef XL_MEMORY_ARENA_H
#define XL_MEMORY_ARENA_H

#include <xlcore/datatypes.h>
#include <xlcore/account.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

// account arena struct of arrays
struct acct_arena {
    uint16_t capacity;
    uint16_t size;
    char * start;
    int32_t * balances;
    xl_smallstr64 * names;
    xl_smallstr128 * descs;
    uint8_t * tags;
    uint8_t * generation;
};

// global accessor function
struct acct_arena * _get_account_arena();

#define account_arena_ref (_get_account_arena())

// allocates the arena based on the number of accounts it must hold
bool initialize_arena(uint16_t num_accts);

// frees the arena, nulls all values in account_arena_ref
void deinitialize_arena(void);

// true upon success, false upon failure (check xl_errno)
// creates a new account based on the snapshot provided (snapshot is constructed by caller)
// writes out the handle of the new account to the 
bool alloc_new_acct(struct xl_account_handle * out_handle);

// no free because accounts never close. They must always exist.
// What happens if we reach the limit of 65k accounts? TODO

#endif                  // XL_MEMORY_ARENA_H
