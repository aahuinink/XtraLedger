#include "account-arena.h"
#include "base-arena.h"
#include "xlcore/datatypes.h"
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <xlcore/errors.h>
#include <stdlib.h>

// HELPER FUNCTIONS
static uint16_t determine_required_capacity(uint16_t num_accts);

// attempt to initialize account arena
// return true upon success, false upon failure (check xl_errno)
//
// ERRORS
//      XL_NOMEM        the system does not have enough memory to allocate the arena
bool account_arena_initialize(struct acct_arena * arena, uint16_t num_accts) {

    // determine what power of two is required to hold the capacity
    uint16_t required_capacity = determine_required_capacity(num_accts);

    size_t offset = 0;          // used to calculate SoA offsets
    

    // start of account balance array
    CHECK_OVERFLOW_AND_ALIGN(offset, int32_t);
    size_t balance_offset = offset;
    printf("Balance offset: %lu\n", offset);
    CHECK_OVERFLOW_AND_BUMP(offset, num_accts, int32_t);

    // start of account names array
    CHECK_OVERFLOW_AND_ALIGN(offset, *arena->names);
    size_t names_offset = offset;
    printf("Names offset: %lu\n", offset);
    CHECK_OVERFLOW_AND_BUMP(offset, num_accts, *arena->names);

    // start of descriptions array 
    CHECK_OVERFLOW_AND_ALIGN(offset, *arena->descs);
    size_t descs_offset = offset;
    printf("Descs offset: %lu\n", offset);
    CHECK_OVERFLOW_AND_BUMP(offset, num_accts, *arena->descs);

    // start of tags array
    CHECK_OVERFLOW_AND_ALIGN(offset, uint8_t);
    size_t tags_offset = offset;
    printf("Tags offset: %lu\n", offset);
    CHECK_OVERFLOW_AND_BUMP(offset, num_accts, uint8_t);

    // start of generation array 
    CHECK_OVERFLOW_AND_ALIGN(offset, uint8_t);
    size_t gen_offset = offset;
    printf("Generation offset: %lu\n", offset);
    CHECK_OVERFLOW_AND_BUMP(offset, num_accts, uint8_t);

    char * arena_start = malloc(offset);

    if (arena_start == NULL) {
        xl_errno = XL_ENOMEM;
        return false;
    }

    // assign pointers
    arena->metadata.capacity = required_capacity;
    arena->metadata.size = num_accts;
    arena->balances = (int32_t*)(arena_start + balance_offset);
    arena->names = (xl_smallstr64*)(arena_start + names_offset);
    arena->descs = (xl_smallstr128*)(arena_start + descs_offset);
    arena->tags = (uint8_t*)(arena_start + tags_offset);
    arena->generation = (uint8_t*)(arena_start + gen_offset);

    return true;
};

void account_arena_deinitialize(struct acct_arena * arena) {
    free(arena->ACCT_ARENA_START_FIELD);
    arena->balances = NULL;
    arena->names = NULL;
    arena->descs = NULL;
    arena->tags = NULL;
    arena->generation = NULL;
    arena->metadata.size = 0;
    arena->metadata.capacity = 0;
}


static uint16_t determine_required_capacity(uint16_t num_accts) {
    // round up to next power of 2
    if(num_accts >= (UINT16_MAX >> 1)) {
        return UINT16_MAX;
    }

    // minimum of 256
    uint_fast8_t num_shifts = 7;

    while (num_accts >> ++num_shifts);

    return (0x1 << num_shifts);
}
