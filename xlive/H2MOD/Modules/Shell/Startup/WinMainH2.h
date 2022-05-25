#pragma once

enum e_startup_flags : int
{
	windowed,
	disable_voice_chat,
	nosound,
	allow_d3d_ex_version, // allows the d3d ex version to be used instead
	disable_hardware_vertex_processing, // force hardware vertex processing off
	novsync,
	unk6, // squad browser/xlive/ui?
	nointro, // disables intro movie
	unk8, // some tag thing?
	unk9, // some tag thing?
	unk10, // some tag thing?
	unk11, // some tag thing?
	unk12, // some tag thing?
	unk13, // some tag thing?
	unk14, // some tag thing?
	custom_map_entry_test_map_name_instead_of_hash, // flag to test the map name instead of the hash of the custom map
	unk16,
	xbox_live_silver_account, // if true, disables 'gold-only' features, like quickmatch etc
	unk18, // fuzzer/automated testing? (sapien)
	ui_fast_test_no_start, // same as below but doesn't start a game
	ui_fast_test, // auto navigates the UI selecting the default option
	unk21, // player controls related, is checked when using a vehicle
	monitor_count,
	unk23,
	unk24,
	unk25, // something to do with game time?
	unk26,
	unk27, // network? value seems unused?
	high_quality, // forced sound reverb ignoring CPU score and disable forcing low graphical settings (sapien)

	count
};
static_assert(e_startup_flags::count == 29, "Bad flags count");

void InitH2WinMainPatches();

bool engine_basic_init();
void real_math_initialize();
void async_initialize();
bool gfwl_gamestore_initialize();
void* runtime_state_initialize();
void game_preferences_initialize();
void font_initialize();
bool tag_files_open();
void timing_initialize(int a1);
void game_state_initialize();
bool rasterizer_initialize();
bool input_initialize();
void sound_initialize();
HANDLE init_data_checksum_info();

bool InitPCCInfo();
void run_main_loop();
void main_engine_dispose();

void show_error_message_by_id(int error_id);
void show_fatal_error(int error_id);