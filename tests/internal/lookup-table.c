#include "arena-lookup-table.c"
#include "arena-lookup-table.h"
#include "../debug_settings.h"
#include "xlcore/datatypes.h"
#include <assert.h>
#include <stdint.h>

bool hash_is_case_insensitive();

bool hash_to_index_conversion_works();

bool lookup_table_init_and_deinit_succeeds();

bool hash_to_index_conversion_works() {
    assert(hash_to_index(0x01, 0x8) == 0x1);
    assert(hash_to_index(0x10, 0x8) == 0x00);
    assert(hash_to_index(0xFFFF, UINT16_MAX) == 0x00);
    assert(hash_to_index(UINT32_MAX, UINT16_MAX) == 0x00);
    return true;
}

bool hash_is_case_insensitive() {
    char Three[6] = "Three";
    char tHreE[6] = "tHreE";

    return (hash(Three) == hash(tHreE));
}


bool lookup_table_init_and_deinit_succeeds() {

    static const xl_smallstr64 test_keys[8] = {
        { 3, "One\0" },
        { 3, "Two\0" },
        { 5, "Three\0" },
        { 4, "Four\0" },
        { 4, "Five\0" },
        { 3, "Six\0" },
        { 5, "Seven\0" },
        { 5, "Eight\0" }
    };

    struct arena_lookup_table table;

    assert(arena_lookup_table_initialize(&table, test_keys, 5));
    assert(table.capacity == 0x8);
    assert(table.size == 0x5);
    assert(table.entries != NULL);

    for (uint_fast8_t i = 0; i < 5; ++i) {
        DEBUG_PRT("%s", test_keys[i].data);
        assert(arena_lookup_try_get(&table, &test_keys[i]) == i);
    }
    
    assert(arena_lookup_try_update(&table, 3));

    assert(table.capacity == 0x10);
    assert(table.size == 0x8);

    arena_lookup_table_deinitialize(&table);

    assert(table.capacity == 0);
    assert(table.entries == NULL);
    assert(table.entries == NULL);
    assert(table.keys == NULL);

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

    return 0;
}

