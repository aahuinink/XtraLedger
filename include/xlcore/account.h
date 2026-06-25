/* 
 * *** account.h ***
 *
 * Defines the account datatype and associated functionality.
 */

#ifndef XL_ACCOUNTS_H
#define XL_ACCOUNTS_H

#include <stdint.h>
#include <stdbool.h>
#include "datatypes.h"

enum xl_acct_normality {
    XL_ACCT_NORM_DEBIT,
    XL_ACCT_NORM_CREDIT,
};

// Account handle struct, used to look up accounts.
// Provides lifetime guarantees
struct xl_account_handle;

// Account store, uses/provides account handles
struct xl_account_store;

// object containing copies of all data from an account. No lifetime guarantee
struct xl_account_snapshot {
    int32_t balance;                    // account balance
    const enum xl_acct_normality norm;  // account normality (cr/dr)
    const xl_smallstr64 name;           // account name
    const xl_smallstr128 desc;          // account description
};

// pointer-based view of account data. Provides lifetime guarantee
struct xl_account_view {
    const int32_t * balance;            // account balance
    const enum xl_acct_normality norm;  // account normality
    const xl_smallstr64 * name ;        // account name
    const xl_smallstr128 * desc;        // account description
};

// initializes an account store
// 
// @return true if initialization succeeds, false if it fails (check xl_errno)
//
// ERRORS
//      XL_ENOMEM       No memory was available to allocate for the account store
bool account_store_init(struct xl_account_store * store);

// deinitializes the account store
void account_store_deinit(struct xl_account_store * store);

/*
 * Checks if an account exists at the index.
 * If it does, it retrieves a snapshot of the account at that index, otherwise it returns NULL.
 *
 * @param store         The account store to look up with
 * @param handle        The account handle to look up.
 * @param out           The account snapshot struct to write account details in
 *
 * @return True if the account is found and successfully writtent to the snapshot, false if it fails (see xl_errno)
 *
 * ERRORS 
 *      
 */
bool try_get_account_snapshot_by_handle(
        const struct xl_account_store * store, 
        const struct xl_account_handle * handle, 
        struct xl_account_snapshot * out);

/*
 * Checks if an account exists with the specified name.
 * If it does, it retrieves a snapshot of the account with that name, otherwise it returns NULL.
 *
 * @param store         The account store to look up with
 * @param name          The account name to look up
 * @param length        The length of the name string (max 64 char, will panic otherwise)
 * @param out_handle    Pointer to a handle to write the associated handle with the snapshot at the time of creation.
 * @param out_snapshot  The snapshot struct to copy the values out to.
 * @return A read-only account view structure.
 */
bool try_get_account_snapshot_by_name(
        const struct xl_account_store * store, 
        const xl_smallstr64 * name, 
        uint8_t length, 
        struct xl_account_handle * out_handle, 
        struct xl_account_snapshot * out_snapshot);

/*
 * Checks if an account exists with the specified name.
 * If it does, it retrieves a snapshot of the account with that name, otherwise it returns NULL.
 *
 * @param store         The account store to look up with
 * @param name          The account name to look up
 * @param out_handle    Pointer to a handle to write the associated handle with the snapshot at the time of creation.
 * @param out_snapshot  The snapshot struct to copy the values out to.
 * @return A read-only account view structure.
 */
bool try_get_account_view_by_name(
        const struct xl_account_store * store, 
        const xl_smallstr64 * name, 
        struct xl_account_handle * out_handle, 
        struct xl_account_view * out_view
);

/*
 * Checks if an account exists at the index.
 * If it does, it retrieves a view of the account at that index, otherwise it returns NULL.
 *
 * @param store         The account store to look up with
 * @param handle        The account handle to look up.
 * @param out           The account snapshot struct to write account details in
 *
 * @return True if the account is found and successfully writtent to the snapshot, false if it fails (see xl_errno)
 *
 * ERRORS 
 *      
 */
bool try_get_account_view_by_handle(
        const struct xl_account_store * store, 
        const struct xl_account_handle * handle, 
        struct xl_account_view * out_view);

/*
 * Checks if an account exists for the specified handle.
 * If it does, it writes the account name into the passed fixed byte array.
 *
 * @param store         The account store to look up with
 * @param handle        The account handle.
 * @param out_str       The string to write the name into.
 *
 * @return              True if the operation completed successfully, false if not (check xl_errno)
 */
bool try_get_account_name_by_handle (
        const struct xl_account_store * store, 
        const struct xl_account_handle * handle, 
        const xl_smallstr64 * out_str);

/*
 * Checks if an account with the specified name exists. 
 * If it does, it writes the handle associated with the name into out handle.
 *
 * @param store         The account store to look up with
 * @param name          The account name
 * @param out_handle    The out parameter
 *
 * @return              True if the operation completed successfully, false if not (check xl_errno)
 *
 * ERRORS       
 *              XL_ENFND  The account specified does not exist 
 */
bool try_get_account_handle_by_name (
        const struct xl_account_store * store, 
        const xl_smallstr64 * name, 
        struct xl_account_handle * out_handle);

/*
 * Checks if an account exists at the specified handle exists.
 * If it does, it copies out the the description into a string.
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param store         The account store to look up with
 * @param handle        The account handle to look up.
 * @param out_desc      The smallstr128 to write the description to.
 *
 * @return              True if the operation completed successfully, false if not (check xl_errno)
 *
 * ERRORS       
 *              XL_ENFND  The account specified does not exist 
 */
bool try_get_account_desc_by_handle (
        const struct xl_account_store * store, 
        const struct xl_account_handle * handle, 
        char * out_desc);

/*
 * Checks if an account with the specified name exists. 
 * If it does, it copies out the the description into a string and provides the handle to the containing account.
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param store         The account store to look up with
 * @param name          The account name to look up.
 *
 * @return              True if the operation completed successfully, false if not (check xl_errno)
 *
 * ERRORS       
 *              XL_ENFND  The account specified does not exist 
 */
bool try_get_account_desc_by_name (
        const struct xl_account_store * store, 
        const xl_smallstr64 * name, 
        struct xl_account_handle * out_handle, 
        char * out_desc);

/*
 * Checks if an account exists at the specified handle exists.
 * If it does, it copies out the account balance.
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param store         The account store to look up with
 * @param handle        The account handle to look up.
 * @param out_balance   The out parameter.
 *
 * @return              True if the operation completed successfully, false if not (check xl_errno)
 *
 * ERRORS       
 *              XL_ENFND  The account specified does not exist 
 */
bool try_get_account_balance_by_handle (
        const struct xl_account_store * store, 
        const struct xl_account_handle * handle, 
        int32_t * out_balance);

/*
 * Checks if an account with the specified name exists. 
 * If it does, copies out the account balance and provides the handle to the containing account..
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param store         The account store to look up with
 * @param handle        The account name to look up.
 *
 * @return              True if the operation completed successfully, false if not (check xl_errno)
 *
 * ERRORS       
 *              XL_ENFND  The account specified does not exist 
 */
bool try_get_account_balance_by_name (
        const struct xl_account_store * store, 
        const xl_smallstr64 * name, 
        struct xl_account_handle * out_handle, 
        int32_t * out_balance);

#endif          // XL_ACCOUNTS_H
