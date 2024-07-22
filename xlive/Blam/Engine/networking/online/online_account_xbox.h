#pragma once

/* macros */

constexpr bool ONLINE_USER_VALID(XUID account)
{
	return account != 0ULL;
}

/* public code */

bool online_xuid_is_guest_account(XUID xuid);

uint8 online_xuid_get_guest_account_number(XUID xuid);

bool __cdecl online_connected_to_xbox_live();

void online_account_transition_to_offline();