#include "stdafx.h"
#include "online_account_xbox.h"
#include "interface/user_interface_guide.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Utils/Utils.h"

/* public code */

bool online_xuid_is_guest_account(XUID id)
{
	// return INVOKE(0x1AC4C0, 0, online_xuid_is_guest_account, xuid);

	const uint16 high = (uint16)(id >> 48);
	return ((high & 0x0F) == 0x9) && ((high & 0xC0) != 0);
}

uint8 online_xuid_get_guest_account_number(XUID id)
{
	// return INVOKE(0x1AC4C6, 0, online_xuid_get_guest_account_number, xuid);
	return uint8(id >> 62);
}

bool online_xuid_same_account(XUID xuid1, XUID xuid2)
{
	return (xuid1 & XUSER_XUID_SAME_MASK) == (xuid2 & XUSER_XUID_SAME_MASK);
}

bool __cdecl online_connected_to_xbox_live()
{
	//return INVOKE(0x1AC4A3, 0x0, online_connected_to_xbox_live);
	return user_interface_guide_state_manager_get()->m_sign_in_state == eXUserSigninState_SignedInToLive;
}

void online_account_transition_to_offline()
{
	ConfigureLocalUserDetails();
	XUserSignInSetStatusChanged(0);
}