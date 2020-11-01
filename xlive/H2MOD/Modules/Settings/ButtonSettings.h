#pragma once
namespace SettingsMenus
{
	namespace Controller
	{
		namespace ButtonLayout
		{
			enum e_button_strings : uint32_t
			{
				reload = 0xd001a35,
				left_weapon = 0x14001a30,
				crouch = 0x0d001a31,
				pause = 0xc001a33,
				flashlight_team_chat = 0x11001a39,
				score = 0xc001a32,
				right_weapon = 0xb001a34,
				switch_weapons = 0x15001a36,
				meele = 0xc001a37,
				jump = 0xb001a38,
				swap_grenades = 0x14001a3a,
				zoom = 0xb001a3b,
				meele_use_left_weapon = 0x12001a3e,
				throw_grenade = 0x14001a3f,

				DEFAULT = 0x7000001,
				SOUTH_PAW = 0x90003E4,
				GREEN_THUMB = 0xB0003E6,
				BOXER = 0x50003E5,
				BUMPER_JUMPER = 0x12341235
			};
			void Initialize();
		}
	}
}