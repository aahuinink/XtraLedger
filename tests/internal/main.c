#include "arena.h"
#include "lookup-table.h"
#include <stdio.h>

#define DEBUG_PRNT
#ifdef DEBUG_PRNT
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif


int main(int argc,  char * argv[]) {

    DBG("*** Testing Arena Helpers ***\n");

    test_align_bump();
    test_arena_allocation();
    test_arena_alignment();

    DBG("*** TESTING LOOKUP TABLE ***\n");

    test_index_making();
    test_put_and_get();

    return 0;
}

