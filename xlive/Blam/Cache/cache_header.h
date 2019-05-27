#pragma once

namespace Blam
{
	namespace Cache
	{
		struct cache_header
		{
			int magic;
			int engine_gen;
			int file_size;
			int field_C;
			int tag_offset;
			int data_offset;
			int data_size;
			int tag_size;
			int tag_offset_mask;
			int field_24;
			char padding[260];
			char version[32];
			enum scnr_type : int
			{
				SinglePlayer = 0,
				Multiplayer = 1,
				MainMenu = 2,
				MultiplayerShared = 3,
				SinglePlayerShared = 4
			};
			scnr_type type;
			int shared_type;
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
			int time_low;
			int time_high;
			int main_menu_time_low;
			int main_menu_time_high;
			int shared_time_low;
			int shared_time_high;
			int campaign_time_low;
			int campaign_time_high;
			char name[32];
			int field_1C4;
			char tag_name[256];
			int minor_version;
			int TagNamesCount;
			int TagNamesBufferOffset;
			int TagNamesBufferSize;
			int TagNamesIndicesOffset;
			int LanguagePacksOffset;
			int LanguagePacksSize;
			int SecondarySoundGestaltDatumIndex;
			int FastLoadGeometryBlockOffset;
			int FastLoadGeometryBlockSize;
			int Checksum;
			int MoppCodesChecksum;
			char field_2F8[1284];
			int foot;
		};
		static_assert(sizeof(cache_header) == 0x800, "Bad cache header size");
	}
}