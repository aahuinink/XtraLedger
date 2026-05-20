#ifndef XL_CORE_DATATYPES_H
#define XL_CORE_DATATYPES_H

#include <stddef.h>
#include <stdint.h>

#define container_of(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	static_assert(__same_type(*(ptr), ((type *)0)->member) ||	\
		      __same_type(*(ptr), void),			\
		      "pointer type mismatch in container_of()");	\
	((type *)(__mptr - offsetof(type, member))); })

// A stack-allocated length-prefixed small string. 
// Payload size 62 chars + null term
#define XL_SMALLSTR64_PAYLOAD_SIZE     62
typedef struct {
    uint8_t length;
    char data[63];
} xl_smallstr64;

// A stack-allocated length-prefixed small string. 
// Payload size 126 chars (no null term)
#define XL_SMALLSTR128_PAYLOAD_SIZE     126
typedef struct {
    uint8_t length;
    char data[127];
} xl_smallstr128;

// a basic string type.
typedef struct {
    char * data;
    size_t length;
} xl_string;

/*
 * Factory functions for xl_smallstr types
 */
xl_smallstr64 xl_smallstr64_copy_from_cstr(const char * cstr);
xl_smallstr64 xl_smallstr64_move_from_cstr(char * cstr);
xl_smallstr128 xl_smallstr128_copy_from_cstr(const char * cstr);
xl_smallstr128 xl_smallstr128_move_from_cstr(char * cstr);

/* factories for xl_string */ 
xl_string xl_string_copy_from_cstr(const char * cstr);
// sets cstr to NULL
xl_string xl_string_move_from_cstr(char * cstr);

void xl_string_free(xl_string * in);

#endif          // XL_CORE_DATATYPES_H
