#ifndef TEST_INTERNAL_ARENA_H
#define TEST_INTERNAL_ARENA_H

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <xlcore/datatypes.h>


void test_align_bump ();

void test_arena_allocation();


void test_arena_alignment();

#endif // !TEST_INTERNAL_ARENA_H
