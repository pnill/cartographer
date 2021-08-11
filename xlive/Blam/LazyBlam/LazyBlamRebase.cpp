#pragma once
#include "LazyBlam.hpp"
#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "Blam/Cache/TagGroups/unit_definition.hpp"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/vehicle_definition.hpp"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"
#include "Blam/Cache/TagGroups/globals_definition.hpp"
#define L_BLAM_LOADER_REBASE(tag_block) \
	if(tag_block.data > 0 && tag_block.size > 0)	\
	{						\
		auto r = base + (tag_block.data - instance->data_offset); \
		if(tag_block.data >= instance->data_offset && tag_block.data <= instance->data_offset + instance->size){\
			/*LOG_TRACE_GAME("[{}] {} {:x} {:x} {:x} {:x}", __FUNCTION__, #tag_block, base, tag_block.data, instance->data_offset,  r);*/     \
			tag_block.data = r;\
		} else { \
			/*LOG_ERROR_GAME("[{}] Invalid offset {} {:x} {:x} {} {:x} {:x} {}", __FUNCTION__, #tag_block,  base, tag_block.data, instance->size, instance->data_offset,  r, instance->name);*/ \
			LOG_ERROR_GAME("[{}] offset outside of bounds {} Range: {:x}-{:x} Offset: {:x} {}", __FUNCTION__, #tag_block, instance->data_offset, (instance->data_offset + instance->size), tag_block.data, instance->name); \
		} \
	}
namespace lazy_blam
{
	namespace rebase
	{
		void object(lazy_blam_tag_instance* instance, unsigned int base)
		{
			auto o = (s_object_group_definition*)instance->data.buffer;
			L_BLAM_LOADER_REBASE(o->ai_properties);
			L_BLAM_LOADER_REBASE(o->functions);
			L_BLAM_LOADER_REBASE(o->attachments);
			L_BLAM_LOADER_REBASE(o->widgets);
			L_BLAM_LOADER_REBASE(o->old_functions);
			L_BLAM_LOADER_REBASE(o->change_colors);
			for(auto &color : o->change_colors)
			{
				L_BLAM_LOADER_REBASE(color.initial_permutations);
				L_BLAM_LOADER_REBASE(color.functions);
			}
			L_BLAM_LOADER_REBASE(o->predicted_resources);
		}
		void unit(lazy_blam_tag_instance* instance, unsigned int base)
		{
			object(instance, base);
			auto unit = (s_unit_group_definition*)instance->data.buffer;
			L_BLAM_LOADER_REBASE(unit->camera_tracks);
			L_BLAM_LOADER_REBASE(unit->postures);
			L_BLAM_LOADER_REBASE(unit->new_hud_interfaces);
			L_BLAM_LOADER_REBASE(unit->dialogue_variants);
			L_BLAM_LOADER_REBASE(unit->powered_seats);
			L_BLAM_LOADER_REBASE(unit->weapons);
			L_BLAM_LOADER_REBASE(unit->seats);
			for(auto &seat : unit->seats)
			{
				L_BLAM_LOADER_REBASE(seat.camera_tracks);
				L_BLAM_LOADER_REBASE(seat.unit_hud_interface);
			}
		}
		void biped(lazy_blam_tag_instance* instance, unsigned int base)
		{
			unit(instance, base);
			auto biped = (s_biped_group_definition*)instance->data.buffer;
			L_BLAM_LOADER_REBASE(biped->dead_sphere_shapes);
			L_BLAM_LOADER_REBASE(biped->pill_shapes);
			L_BLAM_LOADER_REBASE(biped->sphere_shapes);
			L_BLAM_LOADER_REBASE(biped->contact_points);
		}
		void vehicle(lazy_blam_tag_instance* instance, unsigned int base)
		{
			unit(instance, base);
			auto vehicle = (s_vehicle_group_definition*)instance->data.buffer;
			L_BLAM_LOADER_REBASE(vehicle->gears);
			L_BLAM_LOADER_REBASE(vehicle->anti_gravity_points);
			L_BLAM_LOADER_REBASE(vehicle->friction_points);
			L_BLAM_LOADER_REBASE(vehicle->shape_phantom_shape);
		}
		void weapon(lazy_blam_tag_instance* instance, unsigned int base)
		{
			object(instance, base);
			auto w = (s_weapon_group_definition*)instance->data.buffer;
			L_BLAM_LOADER_REBASE(w->predicted_bitmaps);
			L_BLAM_LOADER_REBASE(w->first_person);
			L_BLAM_LOADER_REBASE(w->predicted_resources_1);
			L_BLAM_LOADER_REBASE(w->magazines);
			for (auto &magazines : w->magazines)
				L_BLAM_LOADER_REBASE(magazines.magazines_equipment);
			L_BLAM_LOADER_REBASE(w->new_triggers);
			L_BLAM_LOADER_REBASE(w->barrels);
			//			L_BLAM_LOADER_FOR_TAG_BLOCK(barrel, w->barrels)
			for(auto i = 0; i < w->barrels.size; i++)
				L_BLAM_LOADER_REBASE(w->barrels[i]->firing_effects);
		}
		void globals(lazy_blam_tag_instance* instance, unsigned int base)
		{
			auto g = (s_globals_group_definition*)instance->data.buffer;
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
			}
		}
	}
}