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

// using this value as the "entry is active" state.
// I picked something random so it is unlikely to exist in uninitialized memory
// See the populate_lookup_table function for reasoning
#define ENTRY_STATE_ACTIVE 0xF364

struct arena_lookup_entry {
    uint32_t hash;
    uint16_t arena_slot;
    uint16_t state;
};

// HELPER PROTOTYPES

// gets the position of the first possible matching entry for a key
static uint16_t get_starting_entry(const char * key);
// determines the required capacity of the lookup table to fit a certain number of keys without exceeding the load factor or the maximum capacity of the table
static uint16_t determine_required_capacity(uint16_t num_new_keys);
// populates the lookup table.
// assumes the size of the lookup table is correct
static void populate_lookup_table(struct arena_lookup_table * table, const uint16_t num_new_keys);

// converts the cstr key in the buffer to all lowercase and then produces a hash using DJB2
static uint32_t hash(char * key);

//converts a hash into a lookup index
static inline uint16_t hash_to_index(uint32_t hash, uint16_t table_capacity);

// checks if filling a certain number of entrys will exceed the max load factor and resizes the lookup table if required.
// returns true if the operation succeeds, false if not (check xl_errno)
// ERRORS 
//      XL_ENOMEM       The resize operation caused the lookup table or the operating system to run out of memory
static bool increase_capacity_if_needed(struct arena_lookup_table * table, const uint32_t num_new_keys);

// FUNCTION DEFINITIONS

bool arena_lookup_table_initialize(struct arena_lookup_table * table, const xl_smallstr64 * keys, uint16_t num_keys) {

    assert((table != NULL && keys != NULL) && "Attempted to initialize a lookuptable with a null ptr");
    assert((num_keys != 0) && "Attempted to initialize using an empty arena");

    table->size = 0;
    table->capacity = 0;
    table->keys = keys;
    table->entries = NULL;

    return arena_lookup_try_update(table, num_keys);
}

// returns an index that is smaller than the table capacity
static inline uint16_t hash_to_index(uint32_t hash, uint16_t table_capacity) {
    uint16_t index = hash & (table_capacity - (table_capacity != UINT16_MAX)); 
    return index * (index < table_capacity);
}

int32_t arena_lookup_try_get(const struct arena_lookup_table * table, const xl_smallstr64 * key) {

    int32_t slot;

    char key_buf[XL_SMALLSTR64_PAYLOAD_SIZE + 1];
    memcpy(key_buf, key->data, key->length);
    
    key_buf[key->length] = '\0';

    uint_fast32_t key_hash = hash(key_buf);

    uint_fast16_t possible_match_index = hash_to_index(key_hash, table->capacity);

    while (true) {

        struct arena_lookup_entry possible_match = table->entries[possible_match_index];

        // if we reach an empty entry, then the key does not exist
        if ( possible_match.state != ENTRY_STATE_ACTIVE) {
            return -1;
        }

        // check if the hash entries match 
        if ( key_hash == possible_match.hash ) {
            slot = possible_match.arena_slot;
            // check if the strings match, case insensitive
            if ( strncasecmp(key->data, table->keys[slot].data, XL_SMALLSTR64_PAYLOAD_SIZE ) == 0 ) {
                return slot;
            }
        }

        possible_match_index++;
        // wrap around to 0 to prevent overflows.
        possible_match_index *= (possible_match_index < table->capacity);
    }

}

bool arena_lookup_try_update(struct arena_lookup_table * table, const uint16_t num_new_keys) {

    // check if resize is needed to add an account, return false on failure (resize will set xl_errno)
    bool resize_success = increase_capacity_if_needed(table, num_new_keys);

    if (!resize_success) {
        return false;
    }

    populate_lookup_table(table, num_new_keys);

    return true;
}

void arena_lookup_table_deinitialize(struct arena_lookup_table * table) {
    free(table->entries);
    table->capacity = 0;
    table->size = 0;
    table->entries = NULL;
    table->keys = NULL;
}

static uint16_t determine_required_capacity(uint16_t num_keys) {
    // find min capacity to meet max load factor requirement
    uint16_t min_capacity = num_keys / MAX_LOAD_FACTOR;
    // if greater than UINT16_MAX / 2, return max_capacity;
    if (min_capacity > (0x8000)) {
        return UINT16_MAX;
    }

    // round up to next power of 2
    int num_shifts = 0;
    
    while ((min_capacity >> ++num_shifts));

    return 0x1 << num_shifts;

};

static void populate_lookup_table(struct arena_lookup_table * table, const uint16_t num_new_keys) {

    char key_buf[XL_SMALLSTR64_PAYLOAD_SIZE + 1];

    for (uint_fast16_t i = num_new_keys; i-- > 0;) {

        // we are calling this function because new keys have been added to keys. we update the table size as we successfully populate the table.
        xl_smallstr64 new_key = table->keys[table->size];

        memcpy(key_buf, new_key.data, new_key.length);

        key_buf[new_key.length] = '\0';

        uint_fast32_t key_hash = hash(key_buf);

        uint_fast16_t index = hash_to_index(key_hash, table->capacity);

        // false positives here just mean the entry never gets used, which is fine because our load factor allows for extra indexes.
        // A false positive would be when the uninitialized memory returned when the entries array is malloced contains ENTRY_STATE_ACTIVE already in the entry state field. It's highly unlikely to happen though
        while (table->entries[index].state == ENTRY_STATE_ACTIVE) {
            index++;
            index *= (index < table->capacity);
        }

        table->entries[index].state = ENTRY_STATE_ACTIVE;
        table->entries[index].hash = key_hash;
        table->entries[index].arena_slot = table->size;
        table->size++;

    }
}

static uint32_t hash(char * key_buf) {

    uint32_t hash = 5381;
    char c;
    while ((c = *key_buf++)){
        // convert cstr to lowercase
        // 65-90 --> 97-122
        c += 32 * ((c <= 'Z') & (c >= 'A'));

        // DJB2 algo, thanks to http://www.cse.yorku.ca/~oz/hash.html
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

static bool increase_capacity_if_needed(struct arena_lookup_table * table, const uint32_t num_new_keys) {

    if (((uint32_t)table->size + num_new_keys) > UINT16_MAX) {
        xl_errno = XL_ENOMEM;
        return false;
    }

    uint16_t new_min_capacity = determine_required_capacity(table->size + num_new_keys);

    if (table->capacity >= new_min_capacity) {
        return true;
    }

    table->capacity = new_min_capacity;

    // malloc new space
    struct arena_lookup_entry * new_entry_array = calloc(new_min_capacity, sizeof(struct arena_lookup_entry));

    if (new_entry_array == NULL) {
        xl_errno = XL_ENOMEM;
        return false;
    }

    if (table->entries == NULL) {
        table->entries = new_entry_array;
        return true;
    }

    // iterate over all entries and re-index

    // set new entry array for the lookup table
    table->entries = new_entry_array;
    // free old memory
    free(old_entry_array);
    
    return true;
}
