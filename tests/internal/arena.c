#include "account-arena.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdalign.h>

void test_align_bump ();

void test_arena_allocation();


void test_arena_alignment();

// Helpers
static inline bool is_aligned(const void * ptr, size_t type_alignment) {
    return ((uintptr_t)ptr % type_alignment) == 0;
}

static inline bool array_size_is_correct(const void * start, const void * end, size_t array_member_size, size_t num_accts) {
    return (((uintptr_t)start - (uintptr_t)end) / array_member_size ) >= num_accts;     // there might be a few extra spaces because of alignemnt
}

void test_align_bump () {

#define ALIGNMENTS_LENGTH 5
   
    size_t alignments[ALIGNMENTS_LENGTH] = {
        alignof(xl_smallstr64),
        alignof(xl_smallstr128),
        alignof(int32_t),
        alignof(uint16_t),
        alignof(uint8_t),
    };

    printf("Checking 0 and integer multiple of alignments...\n");

    uint8_t i = 0;

    for (; i < ALIGNMENTS_LENGTH; i++) {
        uint8_t factor = rand() & 0xFF;     // random byte
        assert(0 == align_bump( 0, alignments[i]));
        assert( 0 == align_bump(factor * alignments[i], alignments[i]));
    }

    printf("Checking non-aligned offsets\n");

    for(i=0; i < ALIGNMENTS_LENGTH; i++) {
        printf("Checking alignment of: %lu\n", alignments[i]);
        for(uint_fast8_t j = 0; j < 8; j++ ) {
            printf("Round %d...\n", j);
            uint8_t offset = rand() & 0xFF;     // random byte
            uint8_t modulo = offset % alignments[i]; // calculate difference between alignment boundary and offset
            size_t result = align_bump(offset, alignments[i]);
            size_t check = (offset + result) % alignments[i];
            printf("Offset: %d, modulo offset = %d, _align_bump result: %lu, (offset + result) mod alignment: %lu\n", offset, modulo, result, check);
            assert (check == 0);
        }
    }

#undef ALIGNMENTS_LENGTH
}

void test_arena_allocation() {

    struct acct_arena arena;

    assert(account_arena_initialize(&arena,(uint16_t)-1));
    assert(arena.metadata.size == (uint16_t)-1);
    assert(arena.metadata.capacity == (uint16_t)-1);
    account_arena_deinitialize(&arena); 
}

void test_arena_alignment() {
    struct acct_arena arena;
    for(size_t i = 10; i-- > 0;) {
        uint16_t num_accts = rand() & (uint16_t)-1;
        account_arena_initialize(&arena,num_accts);

        #define CHECK_ARENA_FIELD_ALIGNMENT(field)  assert(is_aligned(arena.field, alignof(*(arena.field))))
        #define CHECK_ARENA_FIELD_SIZE(start, end)       assert(array_size_is_correct(arena.start, arena.end, sizeof(*(arena.start)), num_accts))

        // verify size
        assert(arena.metadata.size == num_accts);
		CHECK_ARENA_FIELD_ALIGNMENT(balances);
        CHECK_ARENA_FIELD_SIZE(balances, names);
		CHECK_ARENA_FIELD_ALIGNMENT(names);
        CHECK_ARENA_FIELD_SIZE(names, descs);
		CHECK_ARENA_FIELD_ALIGNMENT(descs);
        CHECK_ARENA_FIELD_SIZE(descs, tags);
		CHECK_ARENA_FIELD_ALIGNMENT(tags);
        CHECK_ARENA_FIELD_SIZE(tags, generation);
        CHECK_ARENA_FIELD_ALIGNMENT(generation);

        #undef CHECK_ARENA_FIELD_ALIGNMENT
        #undef CHECK_ARENA_FIELD_SIZE

        account_arena_deinitialize(&arena);
    }
}

int main() {
    test_align_bump();
    test_arena_allocation();
    test_arena_alignment();
}
