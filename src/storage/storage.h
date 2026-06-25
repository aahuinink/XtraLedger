/*
 * *** storage.h ***
 *
 * Interface for storage plugin drivers.
 *
 * *************************************
 *
 * Date: April 4, 2026
 * Author: A. Huinink
 * Version: 1.0.0
 *
 * Initial release.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <xlcore/account.h>

// Special cases of read sizes
#define DB_READ_SIZE_ALL     0

// Inclusive start and end of read range
struct db_i32_read_range {
    int32_t start;
    int32_t end;
};

struct db_u32_read_range {
    uint32_t start;
    uint32_t end;
};

// Configuration structs for a read operation.

//      Read account. Set pointer fields to null if you don't care
//      - By name 
//      - By description (contains)
//      - By type (normality and/or tag)
//      - Invert search
struct db_read_acct_criteria {
    const char * name;                  // the name is
    const char * desc;                  // the description contains
    enum xl_acct_normal * normality;    // the type is
    enum xl_acct_nw_tag * nw_tag;       // the tag is
    bool inverse;                       // whether or not search is inverse
};

enum xaction_effect_on_acct {
    ACCT_IS_CREDITED,
    ACCT_IS_DEBITED,
    ACCT_IS_EITHER
};

//      Search transaction
//      - By description contains
//      - By account effect (credited, debited, or either)
//      - By creater (blame)
//      - By date (or range)
//      - By amount (or range)
struct db_search_xaction_criteria {
    const char * desc_contains;                 // contained by the description
    struct db_u32_read_range * created_range;   // number of days since Jan 1, 2026
    struct db_i32_read_range * amt_range;       // the amount
    uint32_t * blame;                           // the id of the creator (0 for app)
    uint16_t * acct;                            // account id
    enum xaction_effect_on_acct acct_effect;    // the effect of the transaction on the account
};

// transaction search result struct
struct db_search_xaction_result {
    uint32_t * ids;             // the unique keys of the transactions matching the search criteria
    uint32_t size;              // the number of transactions matching the search criteria.
};

/* Counts the number of open accounts
 *
 * ERRORS
 *          DB_ERABRT       The read operation was aborted for some reason.
 */
uint16_t count_open_accounts();

/* Retrieves all open accounts
 * 
 * @param arena The memory arena to write the open accounts into.
 * @ret The number of accounts read into the arena.
 *
 * DESC 
 *          Finds all open accounts and writes them into a memory arena
 *
 * ERRORS
 *          DB_ERABRT       The search operation was aborted for some reason.
 */
uint16_t read_open_accounts(struct xl_accounts * arena);

/* Searches for transactions based on criteria. 
 *
 * @param   crit    The search criteria.
 * @param   res     The search results
 * @param   limit   The number of transactions to look for (0 is the special case, meaning all matching transactions)
 * @ret     True if some or all transactions are found, false if none are found matching the config criteria.
 *
 * DESC 
 *          Searches for transactions based on some criteria and returns their IDs.
 *          
 *          *** NOTE ***
 *          This function will heap-allocate an array of u32s to store the IDs in, which is in turn stored in the results structure.
 *          If you use read_xaction using the id array in the result structure, then you don't need to worry about freeing that memory, that function will handle it.
 *          If you don't (for some reason), then you need to free that memory elsewhere...
 *          Search results are the only place that the storage driver may head-allocate memory that it does not free before a DB transaction finishes.
 *
 * ERRORS
 *          DB_ERABRT       The search operation was aborted for some reason.
 */
bool try_search_xaction(struct db_search_xaction_criteria * crit, uint32_t limit, struct db_search_xaction_result * res);

/*
 * Fast lookup of transactions based on search results.
 * 
 * @param res       Search result struct to lookup with. This struct is consumed (moved)! DO NOT USE AFTER CALLING THIS
 * @param xactions_buffer  The transactions SoA to write the results into.
 * @ret                    The number of transactions successfully read into the xactions buffer struct
 *
 * DESC
 *          Provides a fast lookup of multiple transactions by ID. Faster than searching because it uses unique ID keys.
 *  ERRORS
 *          DB_ENFND        A transaction was not found, and the read operatin was aborted.
 *          DB_ERABRT       The read was aborted for some other reason.
 */
uint32_t read_xaction(struct db_search_xaction_result res, xl_transactions_committed * xactions_buffer);

/*
 * Opens an account in the database
 * @param   accounts The accounts SoA to create in the database
 * @ret     The number of acccounts successfully created in the database.
 *
 * DESC
 *          Opens accounts in the database and returns the number of successfully written accounts
 *
 * ERRORS
 *          DB_EDUPL A duplicate exists in the database.
 *          DB_ENOMEM No space for the new account in the database.
 */
uint16_t open_accts(struct xl_accounts * accounts);

/*
 * Creates transactions in the database.
 *
 * @param   xactions The transactions to write to the database.
 * @ret     The number of transactions successfully created in the database.
 *
 * DESC
 *          Creates transactions in the database. The 'ids' field will be filled in for each transaction in the xaction param.
 *
 * ERRORS
 *          DB_EDUPL A duplicate exists in the database.
 *          DB_ENOMEM No space for the new account in the database.
 */
uint32_t create_xactions(xl_transactions_uncommitted * xactions);

/*
 * Updates accounts in database
 *
 * @param   accounts Accounts to update.
 * @ret     The number of acccounts successfully updated in the database.
 *
 * DESC
 *          Updates existing accounts
 *
 *  ERRORS
 *          DB_EWABRT       The write operation was aborted (general)
 *          DB_ENFND        An account to update was not found in the database and the operation was aborted.
 */
uint16_t update_accts(struct xl_accounts * accounts);

/*
 * Updates transactions in the database.
 *
 * @param   xactions The transactions to update in the database.
 * @ret     The number of transactions successfully updated in the database.
 *
 * DESC
 *          Updates existing transactions in the database.
 *
 * ERRORS
 *          DB_EWABRT       The write operation was aborted (general)
 *          DB_ENFND        A transaction was not found in the database and the operation was aborted
 */
uint32_t update_xactions(xl_transactions_uncommitted * xactions);

/*
 * closes an account in the database
 *
 * @param   ids     The account ID to close
 *
 * DESC
 *          Deletes accounts in the database.
 *
 *  ERRORS
 *          DB_EWABRT       The deletion was aborted.
 *          DB_ENFND        An account was not found in the database and the operation was aborted.
 */
void close_acct(uint16_t id);

/*
 * Deletes transactions in the database.
 *
 * @param   ids     The transaction IDs to deleted in the database.
 * @ret     The number of transactions successfully deleted in the database.
 *
 * DESC
 *          Deletes existing transactions in the database.
 *
 * ERRORS
 *          DB_EWABRT       The deletion was aborted.
 *          DB_ENFND        An account was not found in the database
 */
uint32_t delete_xactions(uint32_t * ids);
