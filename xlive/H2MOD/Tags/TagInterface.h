#pragma once

#include "Blam\Common\Common.h"

#include "Blam\Cache\DataTypes\BlamTag.h"

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
	static_assert(sizeof(cache_header) == 0x800, "Bad cache header size");

	struct tag_instance
	{
		blam_tag type;
		datum datum_index;
		size_t data_offset;
		size_t size;
	};

	struct tag_parent_info
	{
		blam_tag tag;
		blam_tag parent;
		blam_tag grandparent;
	};

	struct tag_offset_header
	{
		void* parent_info;
		int tag_parent_info_count;
		tag_instance* tag_instances;
		datum scenario_datum;
		datum globals_datum;
		int field_14;
		int tag_count;
		int type;
	};

	struct tag_data_block
	{
		int block_count;
		DWORD block_data_offset;
	};

	/*
		Tag Interface

		These functions shouldn't be called while a new cache is being loaded and as such it's not recommended you call them from any thread other than the main one.
		If you want to run code just after a map load register a callback using tags::on_map_load
	*/

	/* Run callbacks after the cache file has been loaded */
	void run_callbacks();

	/* Register callback on map data load */
	void on_map_load(void (*callback)());

	/* tag data in currently loaded map (merged cache and shared cache data afaik) */
	char* get_tag_data();

	/* gets the globals\globals aka matg for the current map/cache file (TODO: add the matg structure) */
	char* get_matg_globals_ptr();

	/* header for the current .map/cache file */
	cache_header* get_cache_header();

	/* Returns a handle to the map file currently loaded */
	HANDLE get_cache_handle();

	/* Is a cache loaded? */
	bool cache_file_loaded();

	/*
		Load tag names from cache file.
		Automatically called on load.
	*/
	bool load_tag_debug_name();

	/* helper function for getting a pointer to data at offset in tag data */
	template <typename T>
	T* get_at_tag_data_offset(size_t offset)
	{
		return reinterpret_cast<T*>(&get_tag_data()[offset]);
	}

	/* header containing information about currently loaded tags */
	inline tag_offset_header* get_tags_header()
	{
		return get_at_tag_data_offset<tag_offset_header>(get_cache_header()->tag_offset_mask);
	}

	/* Returns a pointer to the tag instance array */
	tag_instance* get_tag_instances();

	/* Returns the number of tags, pretty self explanatory */
	inline long get_tag_count()
	{
		return get_tags_header()->tag_count;
	}

	/* Convert a tag index to a tag datum */
	inline datum index_to_datum(signed short idx)
	{
		if (idx >= get_tag_count())
		{
			LOG_ERROR_FUNC("Index out of bounds");
			return NONE;
		}
		auto instance = get_tag_instances()[idx];
		datum tag_datum = instance.datum_index;
		LOG_CHECK(DATUM_ABSOLUTE_INDEX(tag_datum) == idx); // should always be true
		return tag_datum;
	}

	inline tag_instance* datum_to_instance(datum datum)
	{
		return &get_tag_instances()[DATUM_ABSOLUTE_INDEX(datum)];
	}

	/* Get parent tag groups for a tag group */
	inline const tag_parent_info* get_tag_parent_info(const blam_tag& tag_type)
	{
		auto* header = get_tags_header();
		if (!header)
		{
			LOG_ERROR_FUNC("Tags header not loaded");
			return nullptr;
		}
		auto compare_parent_info = [](const void* a, const void* b) -> int
		{
			auto* info_a = static_cast<const tag_parent_info*>(a);
			auto* info_b = static_cast<const tag_parent_info*>(b);
			return info_a->tag.as_int() - info_b->tag.as_int();
		};
		const tag_parent_info search_for{ tag_type, blam_tag::none(), blam_tag::none() };
		return static_cast<tag_parent_info*>(
			bsearch(
				&search_for,
				header->parent_info,
				header->tag_parent_info_count,
				sizeof(tag_parent_info),
				compare_parent_info
			));
	}

	/* Returns true if check is the same tag as main or a parent tag */
	inline bool is_tag_or_parent_tag(const blam_tag& main, const blam_tag& check)
	{
		if (main == check)
			return true;
		auto* parent_info = get_tag_parent_info(main);
		if (LOG_CHECK(parent_info))
		{
			if (check == parent_info->tag || check == parent_info->parent || check == parent_info->grandparent)
				return true;
		}
		return false;
	}

	/*
		gets the name of a tag
		debug names must be loaded or it will fail
	*/
	std::string get_tag_name(datum tag);

	/*
		Returns a pointer to a tag, if type is set in template it checks if the real type matches the requested type.
		Using the injectedTag flag will skil the max tag count check as injected tags are placed after the limit.
		Returns null on error
	*/
	template <blam_tag::tag_group_type request_type = blam_tag::tag_group_type::none, typename T = void>
	inline T* get_tag(datum tag, bool injectedTag = false)
	{
		tag_offset_header* header = get_tags_header();

		if (DATUM_IS_NONE(tag))
		{
			LOG_ERROR_FUNC("Bad tag datum - null datum: {}, tag count: {}", DATUM_ABSOLUTE_INDEX(tag), header->tag_count);
			return nullptr;
		}

		// out of bounds check
		if (DATUM_ABSOLUTE_INDEX(tag) > header->tag_count && !injectedTag)
		{
			LOG_CRITICAL_FUNC("Bad tag datum - index out of bounds (idx: {}, bounds: {})", DATUM_ABSOLUTE_INDEX(tag), header->tag_count);
			return nullptr;
		}

		//tag_instance instance = header->tag_instances[tag.Index];
		tag_instance instance = get_tag_instances()[DATUM_ABSOLUTE_INDEX(tag)];
		if (request_type != blam_tag::tag_group_type::none && !is_tag_or_parent_tag(instance.type, request_type))
		{
			LOG_ERROR_FUNC("tag type doesn't match requested type - to disable check set requested type to 'none' in template");
			return nullptr;
		}

		return get_at_tag_data_offset<T>(instance.data_offset);
	}

	template <typename T = void>
	inline T* get_tag_fast(datum tag)
	{
		return reinterpret_cast<T*>(&get_tag_data()[get_tag_instances()[DATUM_ABSOLUTE_INDEX(tag)].data_offset]);
	}

	/*
		Returns the tag datum or a null datum
	*/
	datum find_tag(blam_tag type, const std::string& name);
	std::map<datum, std::string> find_tags(blam_tag type);

	struct ilterator
	{
		ilterator() {};
		ilterator(blam_tag _type) : type(_type) {};

		blam_tag type = blam_tag::none(); // type we are searching for
		long current_index = 0; // current tag idx
		datum m_datum = DATUM_NONE; // last tag datum we returned

		datum next()
		{
			while (current_index < get_tag_count())
			{
				auto tag_instance = &get_tag_instances()[current_index++];
				if (tag_instance && !tag_instance->type.is_none() && !DATUM_IS_NONE(tag_instance->datum_index))
				{
					if (type.is_none() || is_tag_or_parent_tag(tag_instance->type, type))
					{
						m_datum = tag_instance->datum_index;
						return m_datum;
					}
				}
			}

			return DATUM_NONE;
		}

	};
}