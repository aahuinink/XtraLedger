/* 
 * *** errors.h ***
 *
 * Defines the public errors something might throw
 * and an errno-style macro
 */

#ifndef XL_ERRORS_H
#define XL_ERRORS_H

// XL ERRORS - General errors related to XL library logic and memory management
// DB ERRORS - Errors specific to the database (not related to xl library logic)
//
// XL ERRORS
#define XL_ENOERR       0       // success
#define XL_ENOMEM       1       // no memory available to allocate
#define XL_ENFND        2       // item not found
#define XL_EDUPL        3
#define xl_errno (*get_xl_errno())

static inline int * get_xl_errno() {
    extern _Thread_local int _xl_errno;
    return &_xl_errno;
};


// DB ERRORS
#define DB_ENOERR       0       // no error
#define DB_EDUPL        1       // A duplicate exists in the database.
#define DB_ENOMEM       2       // No space in the database
#define DB_EWABRT       3       // The database write was aborted
#define DB_ERABRT       4       // The database read was aborted
#define DB_ENFND        5       // The item was not found in the database

#define db_errno (*get_db_errno())

static inline int * get_db_errno() {
    extern _Thread_local int _db_errno;
    return &_db_errno;
};

#endif          // XL_ERRORS_H
