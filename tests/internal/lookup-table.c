#include "arena-lookup-table.c"
#include "arena-lookup-table.h"
#include "xlcore/datatypes.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

void test_index_making() {

    printf("Testing hash-to-index...\n");
    printf("Happy path - ");
    assert(hash_to_index(0x01, 0x8) == 0x1);
    assert(hash_to_index(0x10, 0x8) == 0x00);
    printf("SUCCESS\nMax Capacity - ");
    assert(hash_to_index(0xFFFF, UINT16_MAX) == 0x00);
    assert(hash_to_index(UINT32_MAX, UINT16_MAX) == 0x00);
    printf("SUCCESS\n");
}

void test_put_and_get() {
    xl_smallstr64 keys[8] = {
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

    char Three[6] = "Three";
    char tHreE[6] = "tHreE";

    printf("Test Lowercasing - ");
    assert(hash(Three) == hash(tHreE));
    printf("SUCCESS\nInitialization - ");
    assert(arena_lookup_table_initialize(&table, keys, 5));
    assert(table.capacity == 0x8);
    for (uint_fast8_t i = 0; i < 5; i++) {
        printf("%s", keys[i].data);
        assert(arena_lookup_try_get(&table, &keys[i]) == i);
    }
    printf("SUCCESS\nResize and Update - ");

    assert(arena_lookup_try_update(&table, 3));

    assert(table.capacity == 0x10);
    assert(table.size == 0x8);
    
}
