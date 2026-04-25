#ifndef XL_CORE_DATATYPES_H
#define XL_CORE_DATATYPES_H

#include <stdint.h>


// A stack-allocated length-prefixed small string. 
// Payload size 63 bytes (no null term)
typedef struct {
    uint8_t length;
    char data[63];
} xl_smallstr64;

// A stack-allocated length-prefixed small string. 
// Payload size 127 bytes (no null term)
typedef struct {
    uint8_t length;
    char data[127];
} xl_smallstr128;

/*
 * Factory functions for xl_smallstr types
 */
xl_smallstr64 smallstr64_from_cstr(const char * cstr);
xl_smallstr128 smallstr128_from_cstr(const char * cstr);

#endif          // XL_CORE_DATATYPES_H
