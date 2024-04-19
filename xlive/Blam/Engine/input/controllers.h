#pragma once

enum e_controller_index : int32
{
	_controller_index_0 = 0,
	_controller_index_1 = 1,
	_controller_index_2 = 2,
	_controller_index_3 = 3,
	k_number_of_controllers,
	k_no_controller = NONE
};

struct s_controller_button_state
{
	bool plugged_in;
	bool field_1;
	bool field_2;
	int8 pad_3;
	int8 field_4[60];
};
ASSERT_STRUCT_SIZE(s_controller_button_state, 64);

struct keybind_map
{
	int8 field_0[100];
};
ASSERT_STRUCT_SIZE(keybind_map, 100);

struct s_controller_profile
{
	real32 N01DA8A10;
	real32 N01DA8A11;
	real32 N01DA8A12;
	real32 N01DA8A13;
	int32 N01DA8A14;
	keybind_map fix_this_2[57];	// keybind map?
	int32 field_1658;
	real32 field_165C;
	real32 field_1660;
	real32 field_1664;
	real32 field_1668;
	real32 N01E30345;
	real32 field_1670;
	real32 N01E30347;
	int16 N01E30348;
	int16 N01E45C19;
	int16 N01E30349;
	int16 N01E4664D;
};
ASSERT_STRUCT_SIZE(s_controller_profile, 5760);

s_controller_profile* controller_profile_get(e_controller_index index);

s_controller_button_state* controller_button_state_get(e_controller_index index);


e_controller_index first_controller();
e_controller_index next_controller(e_controller_index controller);