#pragma once
#include "cache/cache_files.h"
#include "tag_files/tag_groups.h"

namespace tags
{
	struct tag_instance
	{
		tag_group type;
		datum datum_index;
		size_t data_offset;
		size_t size;
	};
	
	struct tag_parent_info
	{
		tag_group tag;
		tag_group parent;
		tag_group grandparent;
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

	/* Returns a pointer to the tag instance array */
	tag_instance* get_tag_instances();

	/* Returns the number of tags, pretty self explanatory */
	inline long get_tag_count()
	{
		return cache_files_get_tags_header()->tag_count;
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
		LOG_CHECK(DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_datum) == idx); // should always be true
		return tag_datum;
	}

	inline tag_instance* datum_to_instance(datum datum)
	{
		return &get_tag_instances()[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum)];
	}

	/* Get parent tag groups for a tag group */
	inline const tag_parent_info* get_tag_parent_info(const tag_group& tag_type)
	{
		s_tags_header* header = cache_files_get_tags_header();
		if (!header)
		{
			LOG_ERROR_FUNC("Tags header not loaded");
			return nullptr;
		}
		auto compare_parent_info = [](const void* a, const void* b) -> int
		{
			auto* info_a = static_cast<const tag_parent_info*>(a);
			auto* info_b = static_cast<const tag_parent_info*>(b);
			return info_a->tag.group - info_b->tag.group;
		};
		const tag_parent_info search_for{ tag_type, (e_tag_group)NONE, (e_tag_group)NONE };
		return static_cast<tag_parent_info*>(
			bsearch(
				&search_for,
				header->tag_group_link_set,
				header->tag_group_link_set_count,
				sizeof(tag_parent_info),
				compare_parent_info
			));
	}

	/* Returns true if check is the same tag as main or a parent tag */
	inline bool is_tag_or_parent_tag(const tag_group& main, const e_tag_group& check)
	{
		if (main.group == check)
			return true;
		auto* parent_info = get_tag_parent_info(main);
		if (LOG_CHECK(parent_info))
		{
			if (check == parent_info->tag.group || check == parent_info->parent.group || check == parent_info->grandparent.group)
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
	template <uint32 request_type = NONE, typename T = void>
	inline T* get_tag(datum tag, bool injectedTag = false)
	{
		s_tags_header* header = cache_files_get_tags_header();

		if (tag == NONE)
		{
			LOG_ERROR_FUNC("Bad tag datum - null datum: {}, tag count: {}", DATUM_INDEX_TO_ABSOLUTE_INDEX(tag), header->tag_count);
			return nullptr;
		}

		// out of bounds check
		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(tag) > header->tag_count && !injectedTag)
		{
			LOG_CRITICAL_FUNC("Bad tag datum - index out of bounds (idx: {}, bounds: {})", DATUM_INDEX_TO_ABSOLUTE_INDEX(tag), header->tag_count);
			return nullptr;
		}

		//tag_instance instance = header->tag_instances[tag.Index];
		tag_instance instance = get_tag_instances()[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag)];
		if (request_type != NONE && !is_tag_or_parent_tag(instance.type, (e_tag_group)request_type))
		{
			LOG_ERROR_FUNC("tag type doesn't match requested type - to disable check set requested type to 'none' in template");
			return nullptr;
		}

		return get_at_tag_data_offset<T>(instance.data_offset);
	}

	template <typename T = void>
	inline T* get_tag_fast(datum tag)
	{
		return reinterpret_cast<T*>(&get_tag_data()[get_tag_instances()[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag)].data_offset]);
	}

	/*
		Returns the tag datum or a null datum
	*/
	datum find_tag(e_tag_group type, const std::string& name);
	std::map<datum, std::string> find_tags(e_tag_group type);

	struct ilterator
	{
		ilterator() = default;
		ilterator(tag_group _type) : type(_type) {};

		tag_group type = { (e_tag_group)NONE }; // type we are searching for
		long current_index = 0; // current tag idx
		datum m_datum = NONE; // last tag datum we returned

		datum next()
		{
			while (current_index < get_tag_count())
			{
				auto tag_instance = &get_tag_instances()[current_index++];
				if (tag_instance && tag_instance->type.group != NONE && tag_instance->datum_index != NONE)
				{
					if (type.group == NONE|| is_tag_or_parent_tag(tag_instance->type, type.group))
					{
						m_datum = tag_instance->datum_index;
						return m_datum;
					}
				}
			}

			return NONE;
		}

	};
}
