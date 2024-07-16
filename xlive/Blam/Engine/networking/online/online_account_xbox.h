#pragma once

#include "xlivedefs.h"

/* macros */

constexpr bool ONLINE_USER_VALID(XUID account)
{
	return account != 0ULL;
}

/* public code */

bool online_xuid_is_guest_account(XUID xuid);

uint8 online_xuid_get_guest_account_number(XUID xuid);
