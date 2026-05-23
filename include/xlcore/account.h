/* 
 * *** account.h ***
 *
 * Defines the account datatype and associated functionality.
 */

#ifndef XL_ACCOUNTS_H
#define XL_ACCOUNTS_H

#include <stdint.h>
#include <stdbool.h>

#define XL_ACCOUNT_NAME_MAX_LENGTH      64
#define XL_ACCOUNT_DESC_MAX_LENGTH      128

// Account handle struct, used to look up accounts.
// Provides lifetime guarantees
struct xl_account_handle;

// A stack-allocated object containing copies of all data from an account. No lifetime guarantee
struct xl_account_snapshot {
    int32_t balance;                                    // account balance
    const char name[XL_ACCOUNT_NAME_MAX_LENGTH];        // account name
    const char desc[XL_ACCOUNT_DESC_MAX_LENGTH];        // account description
};

// pointer-based view of account data. Provides lifetime guarantee
struct xl_account_view {
    const int32_t * balance;                            // account balance
    const char (*name)[XL_ACCOUNT_NAME_MAX_LENGTH];     // account name
    const char (*desc)[XL_ACCOUNT_DESC_MAX_LENGTH];     // account description
};

/*
 * Checks if an account exists at the index.
 * If it does, it retrieves a snapshot of the account at that index, otherwise it returns NULL.
 *
 * @param handle        The account handle to look up.
 * @param out           The account snapshot struct to write account details in
 *
 * @ret True if the account is found and successfully writtent to the snapshot, false if it fails (see xl_errno)
 *
 * ERRORS 
 *      
 */
bool try_get_account_snapshot_by_handle(const struct xl_account_handle * handle, struct xl_account_snapshot * out);

/*
 * Checks if an account exists with the specified name.
 * If it does, it retrieves a snapshot of the account with that name, otherwise it returns NULL.
 *
 * @param name          The account name to look up
 * @param length        The length of the name string (max 64 char, will panic otherwise)
 * @param out_handle    Pointer to a handle to write the associated handle with the snapshot at the time of creation.
 * @param out_snapshot  The snapshot struct to copy the values out to.
 * @ret A read-only account view structure.
 */
bool try_get_account_snapshot_by_name(const char * name, uint8_t length, struct xl_account_handle * out_handle, struct xl_account_snapshot * out_snapshot);

/*
 * Checks if an account exists with the specified name.
 * If it does, it retrieves a snapshot of the account with that name, otherwise it returns NULL.
 *
 * @param name          The account name to look up
 * @param length        The length of the name string (max 64 char, will panic otherwise)
 * @param out_handle    Pointer to a handle to write the associated handle with the snapshot at the time of creation.
 * @param out_snapshot  The snapshot struct to copy the values out to.
 * @ret A read-only account view structure.
 */
bool try_get_account_view_by_name(const char * name, uint8_t length, struct xl_account_handle * out_handle, struct xl_account_view * out_view);

/*
 * Checks if an account exists at the index.
 * If it does, it retrieves a view of the account at that index, otherwise it returns NULL.
 *
 * @param handle        The account handle to look up.
 * @param out           The account snapshot struct to write account details in
 *
 * @ret True if the account is found and successfully writtent to the snapshot, false if it fails (see xl_errno)
 *
 * ERRORS 
 *      
 */
bool try_get_account_view_by_handle(const struct xl_account_handle * handle, struct xl_account_view * out_view);

/*
 * Checks if an account exists for the specified handle.
 * If it does, it writes the account name into the passed fixed byte array.
 *
 * @param handle        The account handle.
 * @param out_str       The string to write the name into.
 *
 * @return              True if the operation concluded successfully, false if not (check xl_errno)
 */
bool try_get_account_name_by_handle (const struct xl_account_handle * handle, const char * out_str);

/*
 * Checks if an account with the specified name exists. 
 * If it does, it writes the handle associated with the name into out handle.
 *
 * @param name          The account name (length must be )
 *
 * @return              True if the operation concluded successfully, false if not (check xl_errno)
 */
bool try_get_account_handle_by_name (const char * name, struct xl_account_handle * out_handle);

/*
 * Checks if an account exists at the specified handle exists.
 * If it does, it copies out the the description into a string.
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param handle        The account handle to look up.
 * @param out_desc      The smallstr128 to write the description to.
 *
 * @return              True if the operation concluded successfully, false if not (check xl_errno)
 */
bool try_get_account_desc_by_handle (const struct xl_account_handle * handle, char * out_desc);

/*
 * Checks if an account with the specified name exists. 
 * If it does, it copies out the the description into a string and provides the handle to the containing account.
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param name          The account name to look up.
 *
 * @return              True if the operation concluded successfully, false if not (check xl_errno)
 */
bool try_get_account_desc_by_name (const char * name, struct xl_account_handle * out_handle, char * out_desc);

/*
 * Checks if an account exists at the specified handle exists.
 * If it does, it copies out the account balance.
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param handle         The account handle to look up.
 *
 * @return              A fixed byte array containing the account balanace.
 */
bool try_get_account_balance_by_handle (const struct xl_account_handle * handle, int32_t * out_balance);

/*
 * Checks if an account with the specified name exists. 
 * If it does, it copies out the account balance and provides the handle to the containing account..
 * If you need a lifetime guarantee, use the pointer-based view.
 *
 * @param handle         The account name to look up.
 * @
 *
 * @return              A fixed byte array containing the account balanace
 */
bool try_get_account_balance_by_name (const char * name, struct xl_account_handle * out_handle, int32_t * out_balance);

#endif          // XL_ACCOUNTS_H
