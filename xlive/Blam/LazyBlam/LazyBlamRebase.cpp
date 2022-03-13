#include "stdafx.h"

#include "LazyBlam.hpp"
#include "Blam\Cache\TagGroups\object_definition.hpp"
#include "Blam\Cache\TagGroups\unit_definition.hpp"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\vehicle_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#define L_BLAM_REBASE_BLOCK(tag_block) \
	if(tag_block.data > 0 && tag_block.size > 0)	\
	{						\
		auto r = base + (tag_block.data - instance->data_offset); \
		if(tag_block.data >= instance->data_offset && tag_block.data <= instance->data_offset + instance->size){\
			tag_block.data = r;\
		} else { \
			LOG_ERROR_GAME("[{}] offset outside of bounds {} Range: {:x}-{:x} Offset: {:x} {}", __FUNCTION__, #tag_block, instance->data_offset, (instance->data_offset + instance->size), tag_block.data, instance->name); \
		} \
	}

#define L_BLAM_REBASE_DATA(data_block) \
	if(data_block.block_offset > 0 && data_block.block_size > 0)\
	{\
		auto r = base + (data_block.block_offset - instance->data_offset);\
		if(!(instance->data_offset <= data_block.block_offset && data_block.block_offset <= (instance->data_offset + instance->size)))\
		{\
			data_block.block_offset = r; \
		} else {\
			LOG_ERROR_GAME("[{}] offset outside of bounds {} Range: {:x}-{:x} Offset: {:x} {}", __FUNCTION__, #data_block, instance->data_offset, (instance->data_offset + instance->size), data_block.block_offset, instance->name); \
		}\
	}

#define L1_BLOCK(t1, p1)\
	L_BLAM_REBASE_BLOCK(instance->data.as<t1>()->p1);
#define L1_DATA(t, p1, t1, d1)\
	for(auto i = 0; i < instance->data.as<t>()->p1.size; i++)\
	{\
		L_BLAM_REBASE_DATA(instance->data.as<t::t1>((instance->data.as<t>()->p1.data - instance->data_offset) + (sizeof(t::t1) * i))->d1);\
	}
#define L2_BLOCK(t, p1, t1, p2)\
	for(auto i = 0; i < instance->data.as<t>()->p1.size; i++)\
	{\
		L_BLAM_REBASE_BLOCK(instance->data.as<t::t1>((instance->data.as<t>()->p1.data - instance->data_offset) + (sizeof(t::t1) * i))->p2);\
	}
namespace lazy_blam
{
	namespace rebase
	{
		void object(lazy_blam_tag_instance* instance, unsigned int base)
		{
			//auto o = (s_object_group_definition*)instance->data.buffer;
			L1_BLOCK(s_object_group_definition, ai_properties);
			L1_BLOCK(s_object_group_definition, functions);
			L1_DATA(s_object_group_definition, functions, s_functions_block, data);
			L1_BLOCK(s_object_group_definition, attachments)
			L1_BLOCK(s_object_group_definition, widgets);
			L1_BLOCK(s_object_group_definition, old_functions);
			L1_BLOCK(s_object_group_definition, change_colors);
			L2_BLOCK(s_object_group_definition, change_colors, s_change_colors_block, initial_permutations);
			L2_BLOCK(s_object_group_definition, change_colors, s_change_colors_block, functions);
			/*for(auto &color : o->change_colors)
			{
				L_BLAM_REBASE_BLOCK(color.initial_permutations);
				L_BLAM_REBASE_BLOCK(color.functions);
			}*/
			L1_BLOCK(s_object_group_definition, predicted_resources);
		}
		void unit(lazy_blam_tag_instance* instance, unsigned int base)
		{
			object(instance, base);
			//auto unit = (s_unit_group_definition*)instance->data.buffer;
			//L_BLAM_LOADER_REBASE(unit->camera_tracks);
			//L_BLAM_LOADER_REBASE(unit->postures);
			//L_BLAM_LOADER_REBASE(unit->new_hud_interfaces);
			//L_BLAM_LOADER_REBASE(unit->dialogue_variants);
			//L_BLAM_LOADER_REBASE(unit->powered_seats);
			//L_BLAM_LOADER_REBASE(unit->weapons);
			//L_BLAM_LOADER_REBASE(unit->seats);
			//for(auto &seat : unit->seats)
			//{
			//	L_BLAM_LOADER_REBASE(seat.camera_tracks);
			//	L_BLAM_LOADER_REBASE(seat.unit_hud_interface);
			//}
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
		void biped(lazy_blam_tag_instance* instance, unsigned int base)
		{
			unit(instance, base);
			/*auto biped = (s_biped_group_definition*)instance->data.buffer;
			L_BLAM_LOADER_REBASE(biped->dead_sphere_shapes);
			L_BLAM_LOADER_REBASE(biped->pill_shapes);
			L_BLAM_LOADER_REBASE(biped->sphere_shapes);
			L_BLAM_LOADER_REBASE(biped->contact_points);*/
			L1_BLOCK(s_biped_group_definition, dead_sphere_shapes);
			L1_BLOCK(s_biped_group_definition, pill_shapes);
			L1_BLOCK(s_biped_group_definition, sphere_shapes);
			L1_BLOCK(s_biped_group_definition, contact_points);
		}
		void vehicle(lazy_blam_tag_instance* instance, unsigned int base)
		{
			unit(instance, base);
			//auto vehicle = (s_vehicle_group_definition*)instance->data.buffer;
			//L_BLAM_LOADER_REBASE(vehicle->gears);
			//L_BLAM_LOADER_REBASE(vehicle->anti_gravity_points);
			//L_BLAM_LOADER_REBASE(vehicle->friction_points);
			//L_BLAM_LOADER_REBASE(vehicle->shape_phantom_shape);
			L1_BLOCK(s_vehicle_group_definition, gears);
			L1_BLOCK(s_vehicle_group_definition, anti_gravity_points);
			L1_BLOCK(s_vehicle_group_definition, friction_points);
			L1_BLOCK(s_vehicle_group_definition, shape_phantom_shape);
		}
		void weapon(lazy_blam_tag_instance* instance, unsigned int base)
		{
			object(instance, base);
			//auto w = (s_weapon_group_definition*)instance->data.buffer;
			//L_BLAM_LOADER_REBASE(w->predicted_bitmaps);
			//L_BLAM_LOADER_REBASE(w->first_person);
			//L_BLAM_LOADER_REBASE(w->predicted_resources_1);
			//L_BLAM_LOADER_REBASE(w->magazines);
			//for (auto &magazines : w->magazines)
			//	L_BLAM_LOADER_REBASE(magazines.magazines_equipment);
			//L_BLAM_LOADER_REBASE(w->new_triggers);
			//L_BLAM_LOADER_REBASE(w->barrels);
			////			L_BLAM_LOADER_FOR_TAG_BLOCK(barrel, w->barrels)
			//for(auto i = 0; i < w->barrels.size; i++)
			//	L_BLAM_LOADER_REBASE(w->barrels[i]->firing_effects);
			L1_BLOCK(s_weapon_group_definition, predicted_bitmaps);
			L1_BLOCK(s_weapon_group_definition, first_person);
			L1_BLOCK(s_weapon_group_definition, predicted_resources_1);
			L1_BLOCK(s_weapon_group_definition, magazines);
			L2_BLOCK(s_weapon_group_definition, magazines, s_magazines_block, magazines_equipment);
			L1_BLOCK(s_weapon_group_definition, new_triggers);
			L1_BLOCK(s_weapon_group_definition, barrels);
			L2_BLOCK(s_weapon_group_definition, barrels, s_barrels_block, firing_effects);
		}
		void globals(lazy_blam_tag_instance* instance, unsigned int base)
		{
			/*auto g = (s_globals_group_definition*)instance->data.buffer;
			L_BLAM_LOADER_REBASE(g->havok_cleanup_resources);
			L_BLAM_LOADER_REBASE(g->collision_damage);
			L_BLAM_LOADER_REBASE(g->sound_globals);
			L_BLAM_LOADER_REBASE(g->ai_globals);

			for (auto &ai_g : g->ai_globals)
				L_BLAM_LOADER_REBASE(ai_g.gravemind_properties);

			L_BLAM_LOADER_REBASE(g->damage_table);
			for(auto &damage_table : g->damage_table)
			{
				L_BLAM_LOADER_REBASE(damage_table.damage_groups);
				for (auto &damage_group : damage_table.damage_groups)
					L_BLAM_LOADER_REBASE(damage_group.armor_modifiers);
			}

			L_BLAM_LOADER_REBASE(g->sounds);
			L_BLAM_LOADER_REBASE(g->camera);
			L_BLAM_LOADER_REBASE(g->player_control);
			for (auto &player_control : g->player_control)
				L_BLAM_LOADER_REBASE(player_control.look_function);

			L_BLAM_LOADER_REBASE(g->difficulty);
			L_BLAM_LOADER_REBASE(g->grenades);
			L_BLAM_LOADER_REBASE(g->rasterizer_data);
			for (auto &rasterizer_data : g->rasterizer_data)
				L_BLAM_LOADER_REBASE(rasterizer_data.global_vertex_shaders);

			L_BLAM_LOADER_REBASE(g->interface_tags);
			L_BLAM_LOADER_REBASE(g->weapon_list_update__weapon_list_enum_in_game_globalsh);
			L_BLAM_LOADER_REBASE(g->cheat_powerups);

			L_BLAM_LOADER_REBASE(g->multiplayer_information);
			for(auto &info : g->multiplayer_information)
			{
				L_BLAM_LOADER_REBASE(info.vehicles);
				L_BLAM_LOADER_REBASE(info.sounds);
				L_BLAM_LOADER_REBASE(info.general_events);
				for (auto &events : info.general_events)
					L_BLAM_LOADER_REBASE(events.sound_permutations);
				for (auto &events : info.slayer_events)
					L_BLAM_LOADER_REBASE(events.sound_permutations);
				for (auto &events : info.ctf_events)
					L_BLAM_LOADER_REBASE(events.sound_permutations);
				for (auto &events : info.oddball_events)
					L_BLAM_LOADER_REBASE(events.sound_permutations);
				for (auto &events : info.king_events)
					L_BLAM_LOADER_REBASE(events.sound_permutations);
			}

			L_BLAM_LOADER_REBASE(g->player_information);
			L_BLAM_LOADER_REBASE(g->player_representation);
			L_BLAM_LOADER_REBASE(g->falling_damage);
			L_BLAM_LOADER_REBASE(g->old_materials);
			L_BLAM_LOADER_REBASE(g->materials);

			L_BLAM_LOADER_REBASE(g->multiplayer_ui);
			for(auto &mui : g->multiplayer_ui)
			{
				L_BLAM_LOADER_REBASE(mui.obsolete_profile_colors);
				L_BLAM_LOADER_REBASE(mui.team_colors);
			}
			L_BLAM_LOADER_REBASE(g->profile_colors);
			L_BLAM_LOADER_REBASE(g->runtime_level_data);
			for (auto &levels : g->runtime_level_data)
				L_BLAM_LOADER_REBASE(levels.campaign_levels);

			L_BLAM_LOADER_REBASE(g->ui_level_data);
			for(auto &levels : g->ui_level_data)
			{
				L_BLAM_LOADER_REBASE(levels.campaigns);
				L_BLAM_LOADER_REBASE(levels.campaign_levels);
				L_BLAM_LOADER_REBASE(levels.multiplayer_levels);
			}*/
		}
	}
}