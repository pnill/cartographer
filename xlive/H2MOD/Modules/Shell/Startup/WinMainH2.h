#pragma once

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
bool rasterizer_initialize();
bool input_initialize();
void sound_initialize();
HANDLE init_data_checksum_info();

bool InitPCCInfo();
void run_main_loop();
void main_engine_dispose();

void show_error_message_by_id(int error_id);
void show_fatal_error(int error_id);