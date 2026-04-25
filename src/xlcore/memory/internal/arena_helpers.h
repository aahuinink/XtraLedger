#include <stdalign.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// internal header for testing purposes

// helper macros
#define CHECK_OVERFLOW_AND_BUMP(type)   do { \
                                            if(_check_overflow_then_bump(&offset, total_num_accts, sizeof(type))) { \
                                                SET_NOMEM(); \
                                                return false; \
                                            } \
                                        } while (0);

#define CHECK_OVERFLOW_AND_ALIGN(type)  do { \
                                            if(_check_overflow_then_align(&offset, alignof(type))) { \
                                                SET_NOMEM(); \
                                                return false; \
                                            } \
                                        } while(0);

#define SET_NOMEM()      xl_errno = XL_NOMEM

// aligns the offset to the next correct type alignment boundary
static inline size_t _align_bump(size_t current_offset, size_t type_alignment) {
    // ensure type alignment is not zero and is power of 2
    assert(type_alignment != 0);
    assert(((type_alignment % 2) == 0) || (type_alignment == 1));

    // if not on exact alignment boundary of next type, determine the amount required to align the current offset with the next type alignment boundary
    return (type_alignment - (current_offset & (type_alignment - 1))) & (type_alignment - 1);
}

// checks if the next block will cause size_t to overflow. 
// If it doesn't it adds it to the offset and returns false.
// If it overflows, it returns true
static inline bool _check_overflow_then_bump(size_t *current_offset, size_t num_accts, size_t type_size) {
    // check if the number of accounts * type size will overflow size_t
    if (num_accts > SIZE_MAX / type_size) { return true; };

    // check if the increase in arena size will overflow the current offset
    size_t bump = num_accts * type_size;
    if ( *current_offset > SIZE_MAX - bump ) { return true;};

    // if not, bump up the offset to make room for the new fields
    *current_offset += bump;

    return false;
}

// same as above but for aligning to the next type
static inline bool _check_overflow_then_align(size_t * current_offset, size_t type_alignment) {
    // determine the number of bytes required to align the offset to the next type
    size_t alignment_bump = _align_bump(*current_offset, type_alignment);

    // make sure it wont overflow size_t
    if (*current_offset > SIZE_MAX - alignment_bump) { return true; };

    // align the offset
    *current_offset += alignment_bump;
    return false;
}
