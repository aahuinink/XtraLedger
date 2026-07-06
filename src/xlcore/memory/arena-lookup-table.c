#include "arena-lookup-table.h"
#include "xlcore/datatypes.h"
#include <assert.h>
#include <stdint.h>
#include <strings.h>
#include <sys/types.h>
#include <xlcore/errors.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LOAD_FACTOR 0.7f

// No state is stored. A hash of 0x0 means that the entry is empty
struct arena_lookup_entry {
    uint32_t hash;
    uint16_t arena_slot; };

// PRIVATE FUNCITON PROTOTYPES

// converts the cstr key in the buffer to all lowercase and then produces a hash using DJB2
// Will not allow a hash to be 0x00, since this is the empty state
static uint32_t hash(const xl_smallstr64 * key);

//converts a hash into a lookup index
static inline uint16_t hash_to_index(const uint32_t hash, const uint16_t table_capacity);

// returns the slot number associated with a key and a key hash.
// If the key does not exist, it returns an index into the entries array where the key could be inserted, but in the form 
// UINT32_MAX - [key index]. For example, if the key is not found, but it *could* be inserted at index 5, then get() returns
// UINT32_MAX - 5. the index can be recovered by subtracting the return value from UINT32_MAX.
// WARNING -> do not use this index unless you know for sure that the entries array has not changed (i.e. concurrent writers)
static uint32_t get(const struct arena_lookup_table * table, const xl_smallstr64 * key, const uint32_t key_hash);

// inserts the value into the table using its corresponding key
// ERRORS       
//      XL_EDUPL        The slot already exists in the table
static bool put(const struct arena_lookup_table * table, const uint16_t slot, const uint32_t key_hash);

// determines the required capacity of the lookup table to fit a certain number of keys without exceeding the load factor or the maximum capacity of the table
// returns a minimum of 128 (0x80)
static uint16_t determine_required_capacity(const uint16_t num_keys);

// checks if filling a certain number of entrys will exceed the max load factor and resizes the lookup table if required.
// returns true if the operation succeeds, false if not (check xl_errno)
// ERRORS 
//      XL_ENOMEM       The resize operation caused the lookup table or the operating system to run out of memory
static bool increase_capacity_if_needed(struct arena_lookup_table * table, const uint32_t num_new_keys);

// FUNCTION DEFINITIONS

bool arena_lookup_table_initialize(struct arena_lookup_table * table, const xl_smallstr64 * keys, uint16_t num_keys) {

    assert((table != NULL && keys != NULL) && "Attempted to initialize a lookuptable with a null ptr");
    assert((num_keys != 0) && "Attempted to initialize using an empty arena");

    table->capacity = 0;
    table->keys = keys;
    table->size = 0;
    table->entries = NULL;

    return arena_lookup_try_update(table, num_keys);
}

// returns an index that is smaller than the table capacity
static inline uint16_t hash_to_index(uint32_t hash, uint16_t table_capacity) {
    uint16_t index = hash & (table_capacity - (table_capacity != UINT16_MAX)); 
    return index * (index < table_capacity);
}

int32_t arena_lookup_try_get(const struct arena_lookup_table * table, const xl_smallstr64 * key) {

    uint32_t slot = get(table, key, hash(key));

    if ( slot >= UINT16_MAX ) {
        xl_errno = XL_ENFND;
        return -1;
    };

    return slot;
}

static uint32_t get(const struct arena_lookup_table * table, const xl_smallstr64 * key, const uint32_t key_hash) {

    uint32_t slot;

    uint_fast16_t possible_match_index = hash_to_index(key_hash, table->capacity);

    while (true) {

        struct arena_lookup_entry possible_match = table->entries[possible_match_index];

        // if we reach an empty entry, then the key does not exist
        if ( possible_match.hash == 0x0 ) {
            return UINT32_MAX - possible_match_index;
        }

        // check if the hash entries match 
        if ( key_hash == possible_match.hash ) {
            slot = possible_match.arena_slot;
            // check if the strings match, case insensitive
            if ( strncasecmp(key->data, table->keys[slot].data, sizeof(table->keys[slot].data)) == 0 ) {
                return slot;
            }
        }

        possible_match_index++;
        // wrap around to 0 to prevent overflows.
        possible_match_index *= (possible_match_index < table->capacity);
    }
}

uint16_t arena_lookup_try_update(struct arena_lookup_table * table, const uint16_t num_new_keys) {

    // check if resize is needed to add an account, return false on failure (resize will set xl_errno)
    bool resize_success = increase_capacity_if_needed(table, num_new_keys);

    if (!resize_success) {
        return false;
    }

    uint16_t successfull_writes = 0;

    uint16_t new_table_size = table->size + num_new_keys;

    while (table->size < new_table_size ) {

        bool write_success = put( table, table->size, hash( &table->keys[table->size] ));

        if (write_success) {
            ++table->size;
            ++successfull_writes;
        } else {
            break;
        }
    }

    return successfull_writes;
}

void arena_lookup_table_deinitialize(struct arena_lookup_table * table) {
    free(table->entries);
    table->capacity = 0;
    table->size = 0;
    table->entries = NULL;
    table->keys = NULL;
}

#define MIN_TABLE_CAPACITY      0x80            // 128 minimum starting capacity

// minimum of 128 entries
static uint16_t determine_required_capacity(uint16_t num_keys) {
    // find min capacity to meet max load factor requirement
    uint32_t required_capacity = num_keys / MAX_LOAD_FACTOR;

    // if greater than UINT16_MAX / 2, return max_capacity;
    if (required_capacity > (0x8000)) {
        return UINT16_MAX;
    }

    if (required_capacity < 0x80) {
        return MIN_TABLE_CAPACITY;
    }

    // round up to next power of 2
    uint16_t rounded_capacity = MIN_TABLE_CAPACITY;

    while (rounded_capacity <= required_capacity) {
        rounded_capacity <<= 1;
    }

    return rounded_capacity;

}

static uint32_t hash(const xl_smallstr64 * key) {

    uint32_t digest = 5381;
    char c;
    const char * key_ptr = key->data;

    while ((c = *key_ptr++)){
        // convert cstr to lowercase
        // 65-90 --> 97-122
        c += 32 * ((c <= 'Z') & (c >= 'A'));

        // DJB2 algo, thanks to http://www.cse.yorku.ca/~oz/hash.html
        digest = ((digest << 5) + digest) + c;
    }

    // protect 0x0 as the sentinel value for an empty entry
    return digest != 0x0 ? digest : 0x01;
}

static bool increase_capacity_if_needed(struct arena_lookup_table * table, const uint32_t num_new_keys) {

    if (((uint32_t)table->size + num_new_keys) > UINT16_MAX) {
        xl_errno = XL_ENOMEM;
        return false;
    }

    // save old table metadata
    const uint16_t old_table_capacity = table->capacity;
    struct arena_lookup_entry * old_table_entries = table->entries;

    uint16_t new_table_size = table->size + num_new_keys;

    uint16_t new_table_capacity = determine_required_capacity(new_table_size);

    if (table->capacity >= new_table_capacity) {
        return true;
    }

    // malloc new space
    struct arena_lookup_entry * new_table_entries = calloc(new_table_capacity, sizeof(struct arena_lookup_entry));

    if (new_table_entries == NULL) {
        xl_errno = XL_ENOMEM;
        return false;
    }

    table->entries = new_table_entries;
    table->capacity = new_table_capacity;

    // if starting with uninitialized memory, we don't need to re-hash, we're all done
    if (old_table_entries == NULL) {
        return true;
    }

    // iterate over all entries and re-index
    for ( uint16_t i = 0; i < old_table_capacity; ++i) {

        struct arena_lookup_entry old_entry = old_table_entries[i];

        if ( old_entry.hash != 0x00 ) {
            put(table, old_entry.arena_slot, old_entry.hash);
        }
    }

    // free old memory
    free(old_table_entries);
    
    return true;
}

static bool put(const struct arena_lookup_table * table, const uint16_t slot, const uint32_t key_hash) {

    uint32_t entry_index = get(table, &table->keys[slot], key_hash);

    if (entry_index < UINT16_MAX) {
        xl_errno = XL_EDUPL;
        return false;
    }

    // convert the value returned by get back into a valid index
    entry_index = UINT32_MAX - entry_index;

    table->entries[entry_index].hash = key_hash;
    table->entries[entry_index].arena_slot = slot;

    return true;
}
