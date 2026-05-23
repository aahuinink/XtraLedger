#include <stdint.h>
#include <string.h>
#include <xlcore/account.h>
#include "memory/account-arena.h"

struct xl_account_handle {
    uint16_t slot;      // slot in the memory arena
};

// ACCOUNT TAGS

// accounts can be normally credit or debit, and they can be liabilities, assets, or not contribute to net worth

// tags are bit packed into a single byte:
/* 
 * Bits         Meaning
 * 0            Normality 
 * 1-2          Effect on net worth (asset, liability, or none)
*/

// The normality of account (credit or debit)
#define ACCT_TAG_NORM_OFFSET    0       // 0th bit
#define ACCT_TAG_NORM_MASK      1
enum xl_acct_normality {
    NORM_CR_ACCT = 0x0,    // credit-normal account
    NORM_DR_ACCT = 0x1,    // debit-normal account
};

// Net-worth tag for the account type (asset, liability, or none)
#define ACCT_TAG_NETW_OFFSET    1       // 1st, 2nd bits
#define ACCT_TAG_NETW_MASK      0x3

enum xl_acct_nw_tag {
    NW_NONE = 0x0,
    NW_ASSET = 0x1,
    NW_LIABILITY = 0x3
};

// Account status tag
enum xl_acct_status_tag {
    STATUS_CLOSED = 0x0,
    STATUS_OPEN = 0x1
};

typedef uint8_t xl_acct_tag;

bool try_get_account_snapshot_by_handle(const struct xl_account_handle *handle, struct xl_account_snapshot *out) {
    memcpy(out->desc.data, account_arena_ref->descs[handle->slot].data, account_arena_ref->descs[handle->slot].length);
    return true;
}

