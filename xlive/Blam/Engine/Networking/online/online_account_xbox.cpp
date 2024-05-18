#include "stdafx.h"
#include "online_account_xbox.h"

/* public code */

bool online_xuid_is_guest_account(XUID xuid)
{
	return (xuid & 3ULL) != 0;
}

uint8 online_xuid_get_guest_account_number(XUID xuid)
{
	return (xuid & 3ULL);
}
