#include "stdafx.h"
#include "user_interface_utilities.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Utils/Utils.h"

datum __cdecl user_interface_get_widget_tag_index_from_screen_id(e_user_interface_screen_id screen_id)
{
	return INVOKE(0x20C701, 0x0, user_interface_get_widget_tag_index_from_screen_id, screen_id);
}

void user_interface_transition_to_offline()
{
	BYTE abEnet[6];
	BYTE abOnline[20];
	XNetRandom(abEnet, sizeof(abEnet));
	XNetRandom(abOnline, sizeof(abOnline));
	ConfigureUserDetails("[Username]", "12345678901234567890123456789012", rand(), 0, H2Config_ip_lan, ByteToHexStr(abEnet, sizeof(abEnet)).c_str(), ByteToHexStr(abOnline, sizeof(abOnline)).c_str(), false);

	H2Config_master_ip = inet_addr("127.0.0.1");
	H2Config_master_port_relay = 2001;
	XUserSignInSetStatusChanged(0);
}