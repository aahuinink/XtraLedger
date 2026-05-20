#ifndef XL_BASE_ARENA_H
#define XL_BASE_ARENA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define ARENA_ITEMS_MAX (uint16_t)-1

struct base_arena {
    uint16_t capacity;
    uint16_t size;
};


#define CHECK_OVERFLOW_AND_BUMP(offset, total_num_accts, type)   do { \
                                            if(check_overflow_then_bump(&offset, total_num_accts, sizeof(type))) { \
                                                xl_errno = XL_ENOMEM; \
                                                return false; \
                                            } \
                                        } while (0);

#define CHECK_OVERFLOW_AND_ALIGN(offset, type)  do { \
                                            if(check_overflow_then_align(&offset, alignof(type))) { \
                                                xl_errno = XL_ENOMEM; \
                                                return false; \
                                            } \
                                        } while(0);


size_t align_bump(size_t current_offset, size_t type_alignment);

// checks if the next block will cause size_t to overflow. 
// If it doesn't it adds it to the offset and returns false.
// If it overflows, it returns true
bool check_overflow_then_bump(size_t *current_offset, size_t num_blocks, size_t type_size);

// same as above but for aligning to the next type
bool check_overflow_then_align(size_t * current_offset, size_t type_alignment);

#endif // !XL_BASE_ARENA_H
