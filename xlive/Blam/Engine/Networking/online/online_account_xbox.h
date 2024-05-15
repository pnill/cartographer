#pragma once

#include "xlivedefs.h"

constexpr bool ONLINE_USER_VALID(XUID account)
{
	return account != 0ULL;
}
constexpr bool online_xuid_is_guest_account(XUID xuid)
{
	return (xuid & 3ULL) != 0;
}
constexpr char online_xuid_get_guest_account_number(XUID xuid)
{
	return (xuid & 3ULL);
}