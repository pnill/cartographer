#include "stdafx.h"

#include "LazyBlam.hpp"
#include "Blam\Cache\TagGroups\object_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Cache\TagGroups\unit_definition.hpp"
#include "Blam\Cache\TagGroups\biped_definition.hpp"

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
#define L_BLAM_LOADER_LOAD_DATA(block) \
	if(block.block_offset > 0 && block.block_size > 0)\
	{\
		if (!(instance->data_offset <= block.block_offset && block.block_offset <= (instance->data_offset + instance->size)))\
		{\
			blam->map_stream->seekg(blam->resolve_data_offset(block.block_offset));\
			block.block_offset = instance->data_offset + instance->size;\
			blam->map_stream->read(instance->data.next(block.block_size), block.block_size);\
			instance->size += block.block_size;\
			LOG_ERROR_GAME(\
				"[{}] {} Range:{:x}-{:x} New Offset:{:x} Data Size:{:x} Instance Size:{:x} data_block:{:x} tag: {}",\
				__FUNCTION__,\
				#block,\
				instance->data_offset,\
				instance->data_offset + instance->size,\
				block.block_offset,\
				block.block_size,\
				instance->size,\
				(unsigned long)&block,\
				instance->name\
			);\
		}\
	}
/**
 * \brief Loads a block at a level 1 depth
 * \param t1 tag type
 * \param t2 tag_block type
 */
#define L1_BLOCK(t1, p1)\
	L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<t1>()->p1);

/**
 * \brief Loads a data ref at a level 1 depth
 * \param t tag type
 * \param p1 tag_block member name
 * \param t1 tag block type
 * \param d1 data_ref member name
 */
#define L1_DATA(t, p1, t1, d1)\
	for(auto i = 0; i < instance->data.as<t>()->p1.size; i++)\
	{\
		L_BLAM_LOADER_LOAD_DATA(instance->data.as<t::t1>((instance->data.as<t>()->p1.data - instance->data_offset) + (sizeof(t::t1) * i))->d1);\
	}
/**
 * \brief Loads a block at a level 2 depth
 * \param t tag type
 * \param p1 tag_block member name
 * \param t1 tag_block type
 * \param p2 tag_block child member name
 */
#define L2_BLOCK(t, p1, t1, p2)\
	for(auto i = 0; i < instance->data.as<t>()->p1.size; i++)\
	{\
		L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<t::t1>((instance->data.as<t>()->p1.data - instance->data_offset) + (sizeof(t::t1) * i))->p2);\
	}

//#define L3(t1, t2, t3 ,t4)\
//	for(auto i = 0; i < instance->data.as<t1>()->t2.size; i++)\
//	{\
//		for(auto j = 0; j < instance->data.as<t2>((instance->data.as<t1>()->t2.data - instance->data_offset) + (sizeof(t2) * i))->t3.size; j++)\
//		{\
//			L_BLAM_LOADER_LOAD_BLOCK(instance->data.as<t3>((instance->data.as<t2>()->t3.data - instance->data_offset) + (sizeof(t3) * i))->t4);\
//		}\
//	}
namespace lazy_blam
{
	void lazy_blam::loader::object(lazy_blam* blam, lazy_blam_tag_instance* instance)
	{
		L1_BLOCK(s_object_group_definition, ai_properties);
		L1_BLOCK(s_object_group_definition, functions);
		L1_DATA(s_object_group_definition, functions, s_functions_block, data);
		L1_BLOCK(s_object_group_definition, attachments);
		L1_BLOCK(s_object_group_definition, widgets);
		L1_BLOCK(s_object_group_definition, old_functions);
		L1_BLOCK(s_object_group_definition, change_colors);
		L2_BLOCK(s_object_group_definition, change_colors, s_change_colors_block, initial_permutations);
		L2_BLOCK(s_object_group_definition, change_colors, s_change_colors_block, functions);
		L1_BLOCK(s_object_group_definition, predicted_resources);
	}
	void lazy_blam::loader::unit(lazy_blam* blam, lazy_blam_tag_instance* instance)
	{
		object(blam, instance);
		L1_BLOCK(s_unit_group_definition, camera_tracks);
		L1_BLOCK(s_unit_group_definition, postures);
		L1_BLOCK(s_unit_group_definition, new_hud_interfaces);
		L1_BLOCK(s_unit_group_definition, dialogue_variants);
		L1_BLOCK(s_unit_group_definition, powered_seats);
		L1_BLOCK(s_unit_group_definition, weapons);
		L1_BLOCK(s_unit_group_definition, seats);
		L2_BLOCK(s_unit_group_definition, seats, s_seats_block, camera_tracks);
		L2_BLOCK(s_unit_group_definition, seats, s_seats_block, unit_hud_interface);
	}

	void lazy_blam::loader::biped(lazy_blam* blam, lazy_blam_tag_instance* instance)
	{
		unit(blam, instance);
		L1_BLOCK(s_biped_group_definition, dead_sphere_shapes);
		L1_BLOCK(s_biped_group_definition, pill_shapes);
		L1_BLOCK(s_biped_group_definition, sphere_shapes);
		L1_BLOCK(s_biped_group_definition, contact_points);
	}

	void lazy_blam::loader::weapon(lazy_blam* blam, lazy_blam_tag_instance* instance)
	{
		object(blam, instance);
		L1_BLOCK(s_weapon_group_definition, predicted_bitmaps);
		L1_BLOCK(s_weapon_group_definition, first_person);
		L1_BLOCK(s_weapon_group_definition, predicted_resources_1);
		L1_BLOCK(s_weapon_group_definition, magazines);
		L2_BLOCK(s_weapon_group_definition, magazines, s_magazines_block, magazines_equipment);
		L1_BLOCK(s_weapon_group_definition, new_triggers);
		L1_BLOCK(s_weapon_group_definition, barrels);
		L2_BLOCK(s_weapon_group_definition, barrels, s_barrels_block, firing_effects);
	}
}
#undef L_BLAM_LOADER_LOAD_BLOCK
#undef L1_BLOCK
#undef L2_BLOCK
#undef L1_DATA