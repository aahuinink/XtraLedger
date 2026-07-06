#include "arena-lookup-table.c"
#include "arena-lookup-table.h"
#include "../debug_settings.h"
#include "xlcore/datatypes.h"
#include "xlcore/errors.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* TODO Add the following:
    (x) not-found lookup (XL_ENFND), DONE
    (x) duplicate-key insert (XL_EDUPL + correct partial write count), 
    (x) the index-0 regression test (pins the bug fixed yesterday), 
    (x) an overflow-guard test near UINT16_MAX hitting XL_ENOMEM.
    (x) setup function for boilerplate
    (x) synthetic test key generation and more robust resize/capacity testing
*/

// generates test keys
xl_smallstr64 * generate_test_keys(uint16_t num_keys);

bool hash_is_case_insensitive();

bool hash_to_index_conversion_works();

bool lookup_table_init_and_deinit_succeeds();

bool key_not_found();

bool duplicate_key_insert();

bool entry_indexes_head_and_tail();

// ensures that the hash will throw no-memory error if too many keys are requested
bool overflow_guard();

// generates test keys
xl_smallstr64 * generate_test_keys(uint16_t num_keys) {
    xl_smallstr64 * test_keys = (xl_smallstr64 *)calloc(sizeof(xl_smallstr64), num_keys);

    for (int i = 0; i < num_keys; ++i ) {
        int chars_written = snprintf(test_keys[i].data, sizeof(test_keys[i].data), "key%d", i + 1);
        assert (((chars_written >= 0) && (chars_written < sizeof(test_keys[i].data))) && "Error generating test keys");
        test_keys[i].length = chars_written;
    }

    return test_keys;
}


bool hash_to_index_conversion_works() {
    assert(hash_to_index(0x01, 0x8) == 0x1);
    assert(hash_to_index(0x10, 0x8) == 0x00);
    assert(hash_to_index(0xFFFF, UINT16_MAX) == 0x00);
    assert(hash_to_index(UINT32_MAX, UINT16_MAX) == 0x00);
    return true;
}

bool hash_is_case_insensitive() {
    xl_smallstr64 Three = { .length = 5, .data = "Three"};
    xl_smallstr64 tHreE = { .length = 5, .data = "tHreE"};

    return (hash(&Three) == hash(&tHreE));
}


bool lookup_table_init_and_deinit_succeeds() {


    struct arena_lookup_table table;

    xl_smallstr64 * test_keys = generate_test_keys(MIN_TABLE_CAPACITY);

    assert(arena_lookup_table_initialize(&table, test_keys, 8));        // just use 8 to start
    assert(table.capacity == 0x80);
    assert(table.size == 0x8);
    assert(table.entries != NULL);

    for (size_t i = 0; i < 8; ++i) {
        DEBUG_PRT("%s", test_keys[i].data);
        assert(arena_lookup_try_get(&table, &test_keys[i]) == i && "Failed to retrieve keys after init");
    }
    
    assert(arena_lookup_try_update(&table, MIN_TABLE_CAPACITY - 8));
    assert(table.capacity == (MIN_TABLE_CAPACITY << 1));
    assert(table.size == MIN_TABLE_CAPACITY);


    for (size_t i = 0; i < MIN_TABLE_CAPACITY; ++i) {
        DEBUG_PRT("%s", test_keys[i].data);
        assert(arena_lookup_try_get(&table, &test_keys[i]) == i && "Failed to retrieve keys after re-size");
    }

    arena_lookup_table_deinitialize(&table);

    assert(table.capacity == 0);
    assert(table.entries == NULL);
    assert(table.entries == NULL);
    assert(table.keys == NULL);

    free(test_keys);

    return true;
}

bool key_not_found() {

    struct arena_lookup_table table;

    xl_smallstr64 * test_keys = generate_test_keys(8);

    arena_lookup_table_initialize(&table, test_keys, 8);

    xl_smallstr64 nonexistent_key = { .length = 4, .data = "Nine"};

    xl_errno = XL_ENOERR;

    int result = arena_lookup_try_get(&table, &nonexistent_key);

    assert(result == -1 && "Looking up a non-existent key did not return -1");

    assert(xl_errno == XL_ENFND && "Looking up a non-existent key did not set xl_errno correctly.");

    arena_lookup_table_deinitialize(&table);
    free(test_keys);

    return true;
}

bool duplicate_key_insert() {

    xl_smallstr64 test_keys_with_duplicate[8] = {
        { .length = 3, .data = "One" },
        { .length = 3, .data = "Two" },
        { .length = 5, .data = "Three" },
        { .length = 4, .data = "Four" },  // duplicate here
        { .length = 4, .data = "Five" },
        { .length = 4, .data = "Four" },
        { .length = 5, .data = "Seven" },
        { .length = 5, .data = "Eight" }
    };

    struct arena_lookup_table table;

    // stop before the duplicate
    arena_lookup_table_initialize(&table, test_keys_with_duplicate, 4);

    // check that passing a key array corrupted with a duplicate key will also fail using the public API
    xl_errno = XL_ENOERR;

    int32_t successful_writes = arena_lookup_try_update(&table, 4);

    assert((successful_writes == 1) && "An incorrect amount of successful_writes");
    assert(xl_errno == XL_EDUPL && "Duplicate error is not raised when a duplicate key is inserted into the table");

    arena_lookup_table_deinitialize(&table);

    return true;

}

bool entry_indexes_head_and_tail() {
    // attempt to put something at index zero in an empty table
    struct arena_lookup_table table;

    xl_smallstr64 zero_index_test_keys[3] = {{ .length = 0, .data = "" }, { .length = 0, .data = "" }, { .length = 13, .data = "Segfault Test"}};

    // since test_keys[0] is null, the hash will be 5361, which will evaluate to a non-zero index
    arena_lookup_table_initialize(&table, zero_index_test_keys, 1);

    // next, put in a key with a hash that evaluates to a zero index
    assert(put(&table, 1, MIN_TABLE_CAPACITY) && "Failed to insert a zero-index-producing hash");

    // next, put in a key with a hash that should overflow the entries 
    // (i.e. 0xFFFF produces 0xFF as a hash, which would segfault a 255 byte array)
    assert(put(&table, 2, 0xFFFFFFFF) && "Failed to insert a hash that produces an index >= table capacity");

    // ensure we can retrieve these keys

    assert(get(&table, &zero_index_test_keys[0], hash(&zero_index_test_keys[0])) == 0 && "Failed to retrieve key");
    assert(get(&table, &zero_index_test_keys[1], MIN_TABLE_CAPACITY) == 1 && "Failed to retrieve key");
    assert(get(&table, &zero_index_test_keys[2], 0xFFFFFFFF) == 2 && "Failed to retrieve key");

    arena_lookup_table_deinitialize(&table);
    
    return true;
}


bool overflow_guard() {

    struct arena_lookup_table table;

    xl_smallstr64 * test_keys = generate_test_keys(8);

    arena_lookup_table_initialize(&table, test_keys, 8);

    struct arena_lookup_entry * old_entries = table.entries;
    uint16_t old_capacity = table.capacity;
    uint16_t old_size = table.size;

    xl_errno = XL_ENOERR;

    assert(!arena_lookup_try_update(&table, UINT16_MAX - 7));

    assert(xl_errno == XL_ENOMEM);

    xl_errno = XL_ENOERR;

    // ensure init didn't change state
    assert(table.entries == old_entries && "overflowing init request put a garbage pointer in table entries");
    assert(table.capacity == old_capacity && "Overflow request updated table capacity");
    assert(table.size == old_size && "Overflow init request updated table size");

    arena_lookup_table_deinitialize(&table);
    free(test_keys);

    return true;
}

int main() {

    // test that indexes hash_to_index_conversion_works
    DEBUG_PRT("Testing hash-to-index conversion - ");
    assert(hash_to_index_conversion_works());
    DEBUG_PRTLN("SUCCESS");

    // test that the hash function produces the same hash regardless of case
    DEBUG_PRT("Testing Hash Case Insensitivity - ");
    assert(hash_is_case_insensitive());
    DEBUG_PRTLN("SUCCESS");

    DEBUG_PRT("Testing lookup table Init/Deinit - ");
    assert(lookup_table_init_and_deinit_succeeds());
    DEBUG_PRTLN("SUCCESS");

    XL_RUN_TEST("Testing key-not-found handling - ", key_not_found, "SUCCESS");

    XL_RUN_TEST("Testing duplicate key handling - ", duplicate_key_insert, "SUCCESS");

    XL_RUN_TEST("Testing hashes that produces indexes at the start and end of the entries array - ", entry_indexes_head_and_tail, "SUCCESS");

    XL_RUN_TEST("Testing overflow guards", overflow_guard, "SUCCESS");

    return 0;
}

