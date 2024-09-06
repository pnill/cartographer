#pragma once
#include "game_variant.h"

#include "game/players.h"
#include "input/input_abstraction.h"
#include "memory/static_arrays.h"

#define k_default_profiles_count 1

struct s_saved_game_profile_variant_info
{
	int8 data[192];
	s_game_variant variant;
};

enum e_saved_game_profile_input_preference_flags : uint32
{
	_saved_game_profile_input_preference_flag_controller_look_inversion = 0x1,
	_saved_game_profile_input_preference_flag_mouse_look_inversion = 0x2,
	_saved_game_profile_input_preference_flag_vibration_disabled = 0x3,
	_saved_game_profile_input_preference_flag_4 = 0x4,
	_saved_game_profile_input_preference_flag_controller_auto_look_centering = 0x5,
	_saved_game_profile_input_preference_flag_mouse_auto_look_centering = 0x6,
	_saved_game_profile_input_preference_flag_mouse_dual_wield_inversion = 0x7,
	k_saved_game_profile_input_preferences_count
};


struct s_saved_game_profile_input_binds
{
	uint32 m_bind_count;
	s_game_function_bind binds[5];
};

#pragma pack(push, 1)
struct s_saved_game_profile_input_preferences
{
	c_flags_no_init<e_saved_game_profile_input_preference_flags, uint32, k_saved_game_profile_input_preferences_count> flags;
	int8 controller_button_layout;
	int8 controller_thumbstick_layout;
	int8 controller_sensitivity;
	int8 mouse_sensitivity;
	int8 data[4];
	s_saved_game_profile_input_binds input_binds[57];
};
#pragma pack(pop)


struct s_saved_game_player_profile
{
	int32 valid_maybe;
	int32 unk;
	wchar_t name[16];
	int8 data[192];
	s_saved_game_profile_variant_info variant;
	int8 data2[20];
	s_saved_game_profile_input_preferences input_preferences;
	int8 data3[16];
	s_player_profile_traits profile_traits;
	int8 gap2[176];
};
ASSERT_STRUCT_SIZE(s_saved_game_player_profile, 4616);


void __cdecl saved_game_player_profile_set_default_variant(void* saved_game_variant);
void __cdecl saved_game_player_profile_set_input_preferences(s_gamepad_input_preferences* input_preferences, s_saved_game_profile_input_preferences* profile_input_preferences);

void saved_game_player_profile_default_new(s_saved_game_player_profile* profile, int32 default_profile_type);
bool saved_game_player_profile_read_file(uint32 enumerated_file_index, s_saved_game_player_profile* profile);
bool __cdecl saved_game_player_profile_read_post_verify_profile_traits(s_player_profile_traits* profile);
bool saved_game_player_profile_load(uint32 enumerated_file_index, s_saved_game_player_profile* profile);
