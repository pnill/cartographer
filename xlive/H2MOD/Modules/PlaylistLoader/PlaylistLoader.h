#pragma once
namespace playlist_loader
{

	enum e_playlist_reader_seek_mode
	{
		new_line = 0x0,
		header_start = 0x2,
		header_read = 0x3,
		property_name_read = 0x4,
		property_deliminator_scan = 0x5,
		property_value_read = 0x6,
		seek_to_next_line = 0x7,
	};
	struct __declspec(align(4)) variant_match
	{
		DWORD unk;
		wchar_t variant_name[16];
		DWORD data[302];
	};
	struct __declspec(align(4)) variant_matches
	{
		DWORD shuffle;
		DWORD pregame_team_selection_delay;
		DWORD pregame_delay;
		DWORD postgame_delay;
		variant_match matches[100];
		DWORD match_count;
		DWORD data2[7600];
		DWORD unk2;
		DWORD data3[10002];
	};
	struct playlist_match
	{
		DWORD unk;
		wchar_t map[16];
		DWORD data1[9];
		wchar_t variant[17];
		DWORD unk1;
		DWORD weight;
		__int16 minimum_players;
		__int16 maximum_players;
	};

	struct setting_buffer_item
	{
		wchar_t name[16];
		wchar_t value[16];
		__int16 unk1;
		__int16 unk2;
	};
	static_assert(sizeof(setting_buffer_item) == 0x44, "Size Incorrect");

	struct playlist_entry
	{
		variant_matches *variant_matches;
		playlist_match playlist_matches[100];
		DWORD match_count;
		wchar_t section_buffer[7616];
		DWORD section_buffer_current_index;
		wchar_t header_buffer[32];
		DWORD unk;
		DWORD current_section_type;
		e_playlist_reader_seek_mode reader_current_mode;
		DWORD reader_current_char_index;
		DWORD reader_current_line;
		DWORD data3[24407];
	};
	static_assert(sizeof(playlist_entry) == 0x1E81C, "Size incorrect");
	void apply_hooks();
	void initialize();
}