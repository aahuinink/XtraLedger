#ifndef XL_ARENA_LOOKUP_TABLE_H
#define XL_ARENA_LOOKUP_TABLE_H

/*
 *      *** arena-lookup-table.h ***
 *      
 *      lookup table of account names to handles.
 */

#include <stdint.h>
#include <xlcore/datatypes.h>
#include <stdbool.h>

typedef struct arena_lookup_table;

// initialize a string-to-index lookup table with the provided keys
// Returns true if successful, false if not (see xl_errno)
bool arena_lookup_table_initialize(struct arena_lookup_table * table, xl_smallstr64 * keys, uint16_t num_keys);

// returns -1 if it if the key is not found, otherwise returns the slot in the arena associated with that key
int32_t arena_lookup_try_get(struct arena_lookup_table * table, const xl_smallstr64 * key);

// Updates the lookup table after new keys are added to the arena.
// returns true if the key creation was successful, false if not (see xl_errno)
// Assumes you have checked that duplicate keys do not exist.
//
// @param table         the lookup table to update
// @param num_new_keys  the number of new keys that have been added to the arena
//
// ERRORS
//      XL_ENOMEM       Creating new keys causes the lookup table or the operating system to run out of space
bool arena_lookup_try_update(struct arena_lookup_table * table, const uint32_t num_new_keys);

// frees the lookup table memory
void arena_lookup_table_deinitialize(struct arena_lookup_table * table);


#endif // XL_ARENA_LOOKUP_TABLE_H
