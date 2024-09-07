#include "stdafx.h"
#include "screen_press_start_introduction.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"

// typedef

typedef void(__cdecl* t_screen_press_start_introduction_open)(s_screen_parameters* parameters);
t_screen_press_start_introduction_open p_screen_press_start_introduction_open;

// private code

void screen_press_start_introduction_open(s_screen_parameters* parameters)
{
	if (UserSignedIn(0)) {
		XUserSignOut(0);
		UpdateMasterLoginStatus();
	}
	return p_screen_press_start_introduction_open(parameters);
}

// public code

void screen_press_start_introduction_apply_patches()
{
	DETOUR_ATTACH(p_screen_press_start_introduction_open, Memory::GetAddress<t_screen_press_start_introduction_open>(0x23f6B7), screen_press_start_introduction_open);
}
