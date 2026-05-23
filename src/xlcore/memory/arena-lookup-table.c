#include "arena-lookup-table.h"
#include "xlcore/datatypes.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <xlcore/errors.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LOAD_FACTOR 0.7f

struct arena_lookup_entry {
    uint32_t hash;
    uint16_t index_value;
};

struct arena_lookup_table {
    struct arena_lookup_entry * entries;
    uint16_t size;
    uint16_t capacity;
};

// HELPER PROTOTYPES

// gets the starting index of possible matching entries from a hashed key
static inline uint32_t get_starting_index(const char * key);
// determines the required capacity of the lookup table to fit a certain number of keys without exceeding the load factor or the maximum capacity of the table
static uint32_t determine_required_capacity(uint32_t num_keys, uint32_t max_capacity);
// populates the lookup table.
// assumes the size of the lookup table is correct
static void populate_table(arena_lookup_table table, const uint32_t num_new_keys, const char * new_keys);
// hashes a cstring using DJB2
static uint32_t hash(const char * key, const uint32_t max_key_length);
// checks if an active entry exists at an index in the lookup table. 
// returns the slot number associated with the active entry, else returns -1 if no active entry is at that index
static int64_t check_for_active_entry(arena_lookup_table map, uint32_t index);
// verifies if the key matches the one in the keystore slot. returns true if yes, false if no
static bool verify_key_match(const char * test_key, const uint32_t test_slot, const char * keystore, uint32_t max_key_length);
// checks if filling a certain number of entrys will exceed the max load factor and resizes the lookup table if required.
// returns true if the operation succeeds, false if not (check xl_errno)
// ERRORS 
//      XL_ENOMEM       The resize operation caused the lookup table or the operating system to run out of memory
static bool resize_if_needed(arena_lookup_table map, const uint32_t num_new_keys);
// inserts a entry containing the key slot in the lookup table at the specified index
static inline void put(arena_lookup_table map, const uint32_t key_slot, const uint32_t entry_index);

// FUNCTION DEFINITIONS

bool arena_lookup_table_initialize(struct arena_lookup_table * table, xl_smallstr64 * keys, uint16_t num_keys) {

    assert((table != NULL && keys != NULL) && "Attempted to initialize a lookuptable with a null ptr");
    assert((num_keys != 0) && "Attempted to initialize using an empty arena");

    return arena_lookup_try_update(table, keys, num_keys);
}

int32_t arena_lookup_try_get(const struct arena_lookup_table table, const xl_smallstr64 * key) {

    uint32_t starting_index = hash(key, sizeof(*(table->arena->lookup_keys))) & (table->arena->capacity - (1 * (table->arena->capacity == table->arena->max_capacity)));

    int64_t slot;

    do
        slot = check_for_active_entry(table, entry_index);

        // no active entry at index
        if (slot < 0) {
            return slot;
        }

        // verify key match

        if(
            
            strncasecmp()
        ) {
            return slot;
        }

        entry_index++;
        entry_index = entry_index * (entry_index >= table->capacity);

    } while (true);
}

bool arena_lookup_try_update(arena_lookup_table table, xl_smallstr64 * new_keys, uint16_t num_new_keys) {

    // check if resize is needed to add an account, return false on failure (resize will set xl_errno)
    bool resize_success = resize_if_needed(table, num_new_keys);

    if (!resize_success) {
        return false;
    }

    populate_lookup_table(table, num_new_keys, new_keys);

    return true;
}

void arena_lookup_table_deinitialize(arena_lookup_table map) {
    free(map->entries);
    map->arena_size = 0;
    map->capacity = 0;
    map->max_capacity = 0;
    map->max_key_length = 0;
}

static inline uint32_t get_starting_index(arena_lookup_table map, const char * key) {
    // lookup table capacity must be a power of 2 for this to work 
    return hash(key, map->max_key_length) & (map->capacity - (1 * (map->capacity == map->max_capacity)));
}

static uint32_t determine_required_capacity(uint32_t num_keys, uint32_t max_capacity) {
    // find min capacity to meet max load factor requirement
    uint32_t min_capacity = num_keys / MAX_LOAD_FACTOR;

    // if greater than max_capacity / 2, return max_capacity;
    if (min_capacity & ~(max_capacity >> 1)) {
        return max_capacity;
    }

    // round up to next power of 2
    int num_shifts = 0;
    
    while (~(max_capacity >> 1) & (min_capacity << ++num_shifts));

    return 0x1 << (16 - num_shifts);

};

static void populate_lookup_table(arena_lookup_table table, const uint32_t num_new_keys) {

    for (uint32_t i = num_new_keys; i-- > 0;) {

        uint32_t entry_index = get_entry_index(table, num_new_keys);
       
        // find next open slot
        while (check_for_active_entry(table, entry_index) >= 0) { 
            entry_index++;
            entry_index *= (entry_index >= table->max_capacity);
        }

        put(table, entry_index, table->size);
        (table->size)++;
        new_keys += sizeof(char) * map->max_key_length;

    }
}

static uint32_t hash(const char * key, uint32_t max_key_length) {

    // thanks to http://www.cse.yorku.ca/~oz/hash.html

    uint32_t hash = 5381;

    uint32_t counter = max_key_length;
    int c;
    while ((c = *key++) && (counter-- > 0)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

static int64_t check_for_active_entry(arena_lookup_table map, uint32_t index) {

    struct xl_arena_lookup_table_entry test_entry = map->entries[index];
    if (test_entry.state == ENTRY_STATE_EMPTY) {
        return -1;
    }

    return test_entry.slot;

}

static bool verify_key_match(arena_lookup_table arena, const char * test_key, const uint32_t test_slot) {
    return strncasecmp(test_key, keystore + (test_slot * max_key_length * sizeof(char)), max_key_length) == 0;
}

static bool resize_if_needed(arena_lookup_table map, const uint32_t entrys_to_be_filled) {
    uint16_t new_min_capacity = determine_required_capacity(map->capacity + entrys_to_be_filled, map->max_capacity);

    if (map->capacity == new_min_capacity) {
        return true;
    }

    if (new_min_capacity > map->max_capacity) {
        xl_errno = XL_ENOMEM;
        return false;
    }

    // malloc new space
    struct xl_arena_lookup_table_entry * new_entry_array = calloc(new_min_capacity, sizeof(struct xl_arena_lookup_table_entry));

    struct xl_arena_lookup_table_entry * old_entry_array = map->entries;

    if (new_entry_array == NULL) {
        xl_errno = XL_ENOMEM;
        return false;
    }

    // copy old stuff over
    memcpy(map->entries, new_entry_array, map->capacity);
    // set new entry array for the lookup table
    map->entries = new_entry_array;
    // update new capacity 
    map->capacity = new_min_capacity;
    // free old memory
    free(old_entry_array);
    
    return true;
}

static inline void put(arena_lookup_table map, const uint32_t entry_index, const uint32_t new_slot) {
    map->entries[entry_index].slot = new_slot;
    map->entries[entry_index].state = ENTRY_STATE_ACTIVE;
}

