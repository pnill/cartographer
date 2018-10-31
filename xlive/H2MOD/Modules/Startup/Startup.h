#pragma once
#include "stdafx.h"

void InitH2Startup();
void InitH2Startup2();
void DeinitH2Startup();
int H2GetInstanceId();

enum flags : int
{
	windowed,
	unk, // some network thing
	nosound,
	unk1, // disable vista needed version check?
	disable_hardware_vertex_processing, // force hardware vertex processing off
	novsync,
	unk3, // squad browser/xlive/ui?
	nointro, // disables intro movie
	unk5, // some tag thing?
	unk6, // some tag thing?
	unk7, // some tag thing?
	unk8, // some tag thing?
	unk9, // some tag thing?
	unk10, // some tag thing?
	unk11, // some tag thing?
	unk12, // seen near xlive init code
	unk13,
	xbox_live_silver_account, // if true, disables 'gold-only' features, like quickmatch etc
	unk15, // fuzzer/automated testing? (sapien)
	ui_fast_test_no_start, // same as below but doesn't start a game
	ui_fast_test, // auto navigates the UI selecting the default option
	unk18, // player controls related (sapien)
	monitor_count,
	unk19,
	unk20,
	unk21, // something to do with game time?
	unk22,
	unk23, // network? value seems unused?
	high_quality, // forced sound reverb ignoring CPU score and disable forcing low graphical settings (sapien)
	unk24,

	count
};
static_assert(flags::count == 30, "Bad flags count");

enum H2Types
{
	H2Game,
	H2Server,
	Invalid
};

class ProcessInfo
{

public:
	HMODULE base;
	H2Types process_type = H2Types::Invalid;
};

extern ProcessInfo game_info;

extern bool H2IsDediServer;
extern DWORD H2BaseAddr;
extern wchar_t* H2ProcessFilePath;
extern wchar_t* H2AppDataLocal;
extern wchar_t* FlagFilePathConfig;
extern HWND H2hWnd;
