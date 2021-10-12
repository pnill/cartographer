#pragma once


struct s_cache_header
{
	DWORD magic;
	int engine_gen;
	uint32_t file_size;
	int field_C;
	int tag_offset;
	int data_offset;
	int data_size;
	int tag_size;
	int tag_offset_mask;
	int field_24;
	BYTE padding[260];
	char version[32];
	enum e_scnr_type : int
	{
		SinglePlayerScenario = 0,
		MultiplayerScenario = 1,
		MainMenuScenario = 2,
		MultiplayerSharedScenario = 3,
		SinglePlayerSharedScenario = 4
	};
	e_scnr_type type;
	int crc;
	int crc_uiid;
	char field_158;
	char tracked__maybe;
	char field_15A;
	char field_15B;
	int field_15C;
	int field_160;
	int field_164;
	int field_168;
	int string_block_offset;
	int string_table_count;
	int string_table_size;
	int string_idx_offset;
	int string_table_offset;
	int extern_deps;
	FILETIME time;
	FILETIME main_menu_time;
	FILETIME shared_time;
	FILETIME campaign_time;
	char name[32];
	int field_1C4;
	char scenario_path[256];
	int minor_version;
	uint32_t TagNamesCount;
	uint32_t TagNamesBufferOffset;
	uint32_t TagNamesBufferSize;
	uint32_t TagIndicesToName;
	int LanguagePacksOffset;
	int LanguagePacksSize;
	int SecondarySoundGestaltDatumIndex;
	int FastLoadGeometryBlockOffset;
	int FastLoadGeometryBlockSize;
	int Checksum;
	int MoppCodesChecksum;
	BYTE field_2F8[1284];
	int foot;

	bool is_main_menu()			   const { return type == e_scnr_type::MainMenuScenario; };
	bool is_multiplayer()		   const { return type == e_scnr_type::MultiplayerScenario; };
	bool is_multi_player_shared()  const { return type == e_scnr_type::MultiplayerSharedScenario; };
	bool is_single_player()		   const { return type == e_scnr_type::SinglePlayerScenario; };
	bool is_single_player_shared() const { return type == e_scnr_type::SinglePlayerSharedScenario; };
};
static_assert(sizeof(s_cache_header) == 0x800, "Bad cache header size");