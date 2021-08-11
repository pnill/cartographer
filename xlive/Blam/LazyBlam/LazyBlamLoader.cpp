#include "LazyBlam.hpp"
#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"

#define L_BLAM_LOADER_LOAD_BLOCK(tag_block) \
		if (tag_block.size > 0 && tag_block.data > 0) \
		{ \
			if (!(instance->data_offset <= tag_block.data && tag_block.data <= (instance->data_offset + instance->size)))\
			{\
				blam->map_stream->seekg(blam->resolve_data_offset(tag_block.data));\
				tag_block.data = instance->data_offset + instance->size;\
				blam->map_stream->read(instance->data.next(tag_block.data_size()), tag_block.data_size());\
				instance->size += tag_block.data_size();\
				LOG_ERROR_GAME(\
					"[{}] {} Range:{:x}-{:x} New Offset:{:x} Data Size:{:x} Instance Size: {:x} tag_block: {:x} Tag: {}",\
					__FUNCTION__,\
					#tag_block,\
					instance->data_offset,\
					instance->data_offset + instance->size,\
					tag_block.data,\
					tag_block.data_size(),\
					instance->size,\
					(unsigned long)&tag_block,\
					instance->name\
				);\
			}\
		}


namespace lazy_blam
{
	void lazy_blam::loader::object(lazy_blam* blam, lazy_blam_tag_instance* instance)
	{
		using object = s_object_group_definition;
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->ai_properties);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->functions);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->attachments);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->widgets);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->functions);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->change_colors);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->ai_properties);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<object>()->predicted_resources);
	}
	void lazy_blam::loader::weapon(lazy_blam* blam, lazy_blam_tag_instance* instance)
	{
		using weapon = s_weapon_group_definition;
		using magazine = weapon::s_magazines_block;
		using barrel = weapon::s_barrels_block;
		object(blam, instance);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<weapon>()->predicted_bitmaps);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<weapon>()->first_person);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<weapon>()->predicted_resources_1);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<weapon>()->magazines);		
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<weapon>()->new_triggers);
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<weapon>()->barrels);
		for (auto i = 0; i < instance->data.as<weapon>()->barrels.size; i++)
		{
			L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<barrel>((instance->data.as<weapon>()->barrels.data - instance->data_offset) + (i * sizeof(magazine)))->firing_effects);
		}
		auto test = instance->data.as<weapon>()->barrels.data - instance->data_offset;
		LOG_INFO_GAME("[{}] {}", __FUNCTION__, instance->data.as<weapon::s_barrels_block>(test)->acceleration_time);
	}
}
#undef L_BLAM_LOADER_LOAD_BLOCK