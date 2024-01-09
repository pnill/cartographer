#pragma once

enum e_shell_tool_type
{
	_shell_tool_type_editing_tools = 0,
	_shell_tool_type_game = 2
};

e_shell_tool_type shell_tool_type(void);

enum e_startup_flags : int
{
	_startup_flag_windowed,
	_startup_flag_disable_voice_chat,
	_startup_flag_nosound,
	_startup_flag_d3d9ex_enabled, // allows the d3d ex version to be used instead
	_startup_flag_disable_hardware_vertex_processing, // force hardware vertex processing off
	_startup_flag_novsync,
	_startup_flag_unk6, // squad browser/xlive/ui?
	_startup_flag_nointro, // disables intro movie
	_startup_flag_unk8, // some tag thing?
	_startup_flag_unk9, // some tag thing?
	_startup_flag_unk10, // some tag thing?
	_startup_flag_unk11, // some tag thing?
	_startup_flag_unk12, // some tag thing?
	_startup_flag_unk13, // some tag thing?
	_startup_flag_unk14, // some tag thing?
	_startup_flag_custom_map_entry_test_map_name_instead_of_hash, // flag to test the map name instead of the hash of the custom map
	_startup_flag_unk16,
	_startup_flag_xbox_live_silver_account, // if true, disables 'gold-only' features, like quickmatch etc
	_startup_flag_unk18, // fuzzer/automated testing? (sapien)
	_startup_flag_ui_fast_test_no_start, // same as below but doesn't start a game
	_startup_flag_ui_fast_test, // auto navigates the UI selecting the default option
	_startup_flag_unk21, // player controls related, is checked when using a vehicle
	_startup_flag_monitor_count,
	_startup_flag_unk23,
	_startup_flag_unk24,
	_startup_flag_unk25, // something to do with game time?
	_startup_flag_unk26,
	_startup_flag_unk27, // network? value seems unused?
	_startup_flag_high_quality, // forced sound reverb ignoring CPU score and disable forcing low graphical settings (sapien)

	k_startup_flags_count
};
static_assert(e_startup_flags::k_startup_flags_count == 29, "bad startup flag count");

bool shell_startup_flag_is_set(e_startup_flags flag);
void shell_startup_flag_set(e_startup_flags flag, int32 state);
int32 shell_startup_flag_get(e_startup_flags flag);