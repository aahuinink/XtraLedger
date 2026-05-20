#include "base-arena.h"
#include <stddef.h>

static bool is_power_of_two(size_t type_size);

size_t align_bump(size_t current_offset, size_t type_alignment) {

    // ensure type alignment is not zero and is power of 2
    assert(type_alignment != 0);
    assert(is_power_of_two(type_alignment));

    // if not on exact alignment boundary of next type, determine the amount required to align the current offset with the next type alignment boundary
    return (type_alignment - (current_offset & (type_alignment - 1))) & (type_alignment - 1);
}

// checks if the next block will cause size_t to overflow. 
// If it doesn't it adds it to the offset and returns false.
// If it overflows, it returns true
bool check_overflow_then_bump(size_t *current_offset, size_t num_blocks, size_t type_size) {
    // check we are requesting too many blocks
    if (num_blocks > SIZE_MAX / type_size) { return true; };

    // check if the increase in arena size will overflow the current offset
    size_t bump = num_blocks * type_size;
    if ( *current_offset > (SIZE_MAX - bump) ) { return true;};

    // if not, bump up the offset to make room for the new fields
    *current_offset += bump;

    return false;
}

// same as above but for aligning to the next type
bool check_overflow_then_align(size_t * current_offset, size_t type_alignment) {
    // determine the number of bytes required to align the offset to the next type
    size_t alignment_bump = align_bump(*current_offset, type_alignment);

    // make sure it wont overflow size_t
    if (*current_offset > (SIZE_MAX - alignment_bump)) { return true; };

    // align the offset
    *current_offset += alignment_bump;
    return false;
}

static bool is_power_of_two(size_t type_size) {
    uint_fast8_t acc = 0;
    while (type_size > 0) {
        acc += type_size & 0x1;
        type_size >>= 1;
    }
    return (acc == 1);
}
