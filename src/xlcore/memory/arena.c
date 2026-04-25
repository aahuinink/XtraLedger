#include "arena.h"
#include "internal/arena_helpers.h"
#include "xlcore/datatypes.h"
#include <stdint.h>
#include <xlcore/errors.h>
#include <stdlib.h>

static struct acct_arena g_account_arena;

// attempt to initialize account arena
// return true upon success, false upon failure (check xl_errno)
//
// ERRORS
//      XL_NOMEM        the system does not have enough memory to allocate the arena
bool initialize_arena(uint16_t num_accts) {

    size_t total_num_accts =  (size_t)num_accts + EXTRA_CAPACITY_FACTOR;
    if (total_num_accts > (uint16_t)-1) {
        total_num_accts = (uint16_t)-1;
    }

    size_t offset = 0;          // used to calculate SoA offsets
    

    // start of account balance array
    CHECK_OVERFLOW_AND_ALIGN(int32_t);
    size_t balance_offset = offset;
    CHECK_OVERFLOW_AND_BUMP(int32_t);

    // start of account names array
    CHECK_OVERFLOW_AND_ALIGN(xl_smallstr64);
    size_t names_offset = offset;
    CHECK_OVERFLOW_AND_BUMP(xl_smallstr64);

    // start of descriptions array 
    CHECK_OVERFLOW_AND_ALIGN(xl_smallstr128);
    size_t descs_offset = offset;
    CHECK_OVERFLOW_AND_BUMP(xl_smallstr128);

    // start of tags array
    CHECK_OVERFLOW_AND_ALIGN(uint8_t);
    size_t tags_offset = offset;
    CHECK_OVERFLOW_AND_BUMP(uint8_t);

    // start of generation array 
    CHECK_OVERFLOW_AND_ALIGN(uint8_t);
    size_t gen_offset = offset;
    CHECK_OVERFLOW_AND_BUMP(uint8_t);

    char * arena_start = malloc(offset);

    if (arena_start == NULL) {
        SET_NOMEM();
        return false;
    }

    // assign pointers
    g_account_arena.capacity = total_num_accts;
    g_account_arena.size = num_accts;
    g_account_arena.start = arena_start;
    g_account_arena.balances = (int32_t*)(arena_start + balance_offset);
    g_account_arena.names = (xl_smallstr64*)(arena_start + names_offset);
    g_account_arena.descs = (xl_smallstr128*)(arena_start + descs_offset);
    g_account_arena.tags = (uint8_t*)(arena_start + tags_offset);
    g_account_arena.generation = (uint8_t*)(arena_start + gen_offset);

    return true;
};

void deinitialize_arena(void) {
    free(g_account_arena.start);
    g_account_arena.start = NULL;
    g_account_arena.balances = NULL;
    g_account_arena.names = NULL;
    g_account_arena.descs = NULL;
    g_account_arena.tags = NULL;
    g_account_arena.generation = NULL;
    g_account_arena.size = 0;
    g_account_arena.capacity = 0;
}

const struct acct_arena * const _get_account_arena() {
    return &g_account_arena;
}
