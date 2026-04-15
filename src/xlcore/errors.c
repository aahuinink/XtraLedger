/* 
 * *** errors.c ***
 *
 * Defines the macro for xl_errno
 */

#include <xlcore/errors.h>

_Thread_local int _xl_errno = XL_ENOERR;

_Thread_local int _db_errno = DB_ENOERR;
