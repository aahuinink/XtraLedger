#include "arena.h"
#include <assert.h>
#include <internal/arena_helpers.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <xlcore/datatypes.h>

#define DEBUG_PRNT
#ifdef DEBUG_PRNT
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif

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

    DBG("Checking 0 and integer multiple of alignments...\n");

    uint8_t i = 0;

    for (; i < ALIGNMENTS_LENGTH; i++) {
        uint8_t factor = rand() & 0xFF;     // random byte
        assert(0 == _align_bump( 0, alignments[i]));
        assert( 0 == _align_bump(factor * alignments[i], alignments[i]));
    }

    DBG("Checking non-aligned offsets\n");

    i= 0;
    for(; i < ALIGNMENTS_LENGTH; i++) {
        DBG("Checking alignment of: %lu\n", alignments[i]);
        uint8_t j = 0;
        for(; j < 8; j++ ) {
            DBG("Round %d...\n", j);
            uint8_t offset = rand() & 0xFF;     // random byte
            uint8_t modulo = offset % alignments[i]; // calculate difference between alignment boundary and offset
            size_t result = _align_bump(offset, alignments[i]);
            size_t check = (offset + result) % alignments[i];
            DBG("Offset: %d, modulo offset = %d, _align_bump result: %lu, (offset + result) mod alignment: %lu\n", offset, modulo, result, check);
            assert (check == 0);
        }
    }

#undef ALIGNMENTS_LENGTH
}

void test_arena_allocation() {

    // test size 0
    assert(initialize_arena(0));
    assert(account_arena_ref->size == 0);
    assert(account_arena_ref->capacity == EXTRA_CAPACITY_FACTOR);
    deinitialize_arena();

    // test mid-range size
    uint16_t rand_num = rand() & (uint8_t)-1;
    assert(initialize_arena(rand_num));
    assert(account_arena_ref->size == rand_num);
    assert(account_arena_ref->capacity == rand_num + EXTRA_CAPACITY_FACTOR);
    deinitialize_arena();
    // test max of uint16_t
    printf("%d\n", account_arena_ref->size);
    assert(initialize_arena((uint16_t)-1));
    assert(account_arena_ref->size == (uint16_t)-1);
    assert(account_arena_ref->capacity == (uint16_t)-1);
    deinitialize_arena();
}

void test_arena_alignment() {
    for(size_t i = 10; i-- > 0;) {
        uint16_t num_accts = rand() & (uint16_t)-1;
        initialize_arena(num_accts);

        #define CHECK_ARENA_FIELD_ALIGNMENT(field)  assert(is_aligned(account_arena_ref->field, alignof(*(account_arena_ref->field))))
        #define CHECK_ARENA_FIELD_SIZE(start, end)       assert(array_size_is_correct(account_arena_ref->start, account_arena_ref->end, sizeof(*(account_arena_ref->start)), num_accts))

        // verify size
        assert(account_arena_ref->size == num_accts);
        assert(account_arena_ref->capacity == (num_accts + EXTRA_CAPACITY_FACTOR));
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

        deinitialize_arena();
    }
}

int main(int argc,  char * argv[]) {

    DBG("*** Testing Arena Helpers ***\n");

    test_align_bump();
    test_arena_allocation();
    test_arena_alignment();

    return 0;
}

