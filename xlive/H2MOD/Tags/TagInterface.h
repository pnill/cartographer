#pragma once
#include "..\Blam\Cache\Cache.h"

namespace tags
{
	struct cache_header
	{
		int magic;
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
		uint32_t TagNamesCount;
		uint32_t TagNamesBufferOffset;
		uint32_t TagNamesBufferSize;
		uint32_t TagNamesIndicesOffset;
		int LanguagePacksOffset;
		int LanguagePacksSize;
		int SecondarySoundGestaltDatumIndex;
		int FastLoadGeometryBlockOffset;
		int FastLoadGeometryBlockSize;
		int Checksum;
		int MoppCodesChecksum;
		BYTE field_2F8[1284];
		int foot;
	};
	static_assert(sizeof(cache_header) == 0x800, "Bad cache header size");

	struct tag_instance
	{
		blam_tag type;
		DatumIndex tag;
		size_t data_offset;
		size_t field_C;
	};

	struct tag_offset_header
	{
		void *parent_info;
		int tag_parent_info_count;
		tag_instance *tag_instances;
		int scenario_datum;
		DatumIndex globals_datum;
		int field_14;
		int tag_count;
		int type;
	};

	/* Apply required patches to executable */
	void apply_patches();

	/* Register callback on map data load */
	void on_map_load(void (*callback)());

	/* tag data in currently loaded map (merged cache and shared cache data afaik) */
	char *get_tag_data();

	/* header for the current .map/cache file */
	cache_header *get_cache_header();

	/* Returns a handle to the map file currently loaded */
	HANDLE get_cache_handle();

	/* Is a cache loaded? */
	bool cache_file_loaded();

	/* Load tag names from cache file */
	bool load_tag_debug_name();

	/* helper function for getting a pointer to data at offset in tag data */
	template <typename T>
	T *get_at_tag_data_offset(size_t offset)
	{
		return reinterpret_cast<T*>(&get_tag_data()[offset]);
	}

	/* header containing information about currently loaded tags */
	inline tag_offset_header *get_tags_header()
	{
		return get_at_tag_data_offset<tag_offset_header>(get_cache_header()->tag_offset_mask);
	}

	inline tag_instance *get_tag_instances()
	{
		return get_tags_header()->tag_instances;
	}

	inline long get_tag_count()
	{
		return get_tags_header()->tag_count;
	}

	/* 
		gets the name of a tag
		debug names must be loaded or it will fail
	*/
	std::string get_tag_name(DatumIndex tag);

	/* 
		Returns a pointer to a tag, if type is set in template it checks if the real type matches the requested type.
		Returns null on error
	*/
	template <int request_type = 0xFFFFFFFF, typename T = void>
	inline T* get_tag(DatumIndex tag)
	{
		tag_offset_header *header = get_tags_header();

		if (tag.IsNull())
		{
			LOG_ERROR_FUNC("Bad tag datum - null datum", tag.Index, header->tag_count);
			return nullptr;
		}

		// out of bounds check
		if (tag.Index > header->tag_count)
		{
			LOG_CRITICAL_FUNC("Bad tag datum - index out of bounds (idx: %n, bounds: %n)", tag.Index, header->tag_count);
			return nullptr;
		}

		tag_instance instance = header->tag_instances[tag.Index];
		if (request_type != 0xFFFFFFFF && instance.type != request_type)
		{
			LOG_ERROR_FUNC("tag type doesn't match requested type - to disable check set requested type to 'none' in template");
			return nullptr;
		}

		return get_at_tag_data_offset<T>(instance.data_offset);
	}
}
