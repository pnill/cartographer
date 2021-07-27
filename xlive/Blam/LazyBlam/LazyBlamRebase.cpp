#pragma once
#include "LazyBlam.hpp"
#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "Blam/Cache/TagGroups/unit_definition.hpp"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/vehicle_definition.hpp"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"
#include "Blam/Cache/TagGroups/globals_definition.hpp"
#define L_BLAM_LOADER_REBASE(tag_block) if(tag_block.data > 0) (tag_block.data = base + (tag_block.data - instance->data_offset))
namespace lazy_blam
{
	namespace rebase
	{
		void object(char* data, tags::tag_instance* instance, int base)
		{
			auto o = (s_object_group_definition*)data;
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
			//for (auto i = 0; i < o->change_colors.size; i++)
			//{
			//	//L_BLAM_LOADER_REBASE(colors[i].functions);
			//	//L_BLAM_LOADER_REBASE(colors[i].initial_permutations);
			//	L_BLAM_LOADER_REBASE(o->change_colors[i]->initial_permutations);
			//	L_BLAM_LOADER_REBASE(o->change_colors[i]->functions);
			//}
			L_BLAM_LOADER_REBASE(o->predicted_resources);
		}
		void unit(char* data, tags::tag_instance* instance, int base)
		{
			object(data, instance, base);
			auto unit = (s_unit_group_definition*)data;
			L_BLAM_LOADER_REBASE(unit->camera_tracks);
			L_BLAM_LOADER_REBASE(unit->postures);
			L_BLAM_LOADER_REBASE(unit->new_hud_interfaces);
			L_BLAM_LOADER_REBASE(unit->dialogue_variants);
			L_BLAM_LOADER_REBASE(unit->powered_seats);
			L_BLAM_LOADER_REBASE(unit->weapons);
			L_BLAM_LOADER_REBASE(unit->seats);
			for (auto i = 0; i < unit->seats.size; i++)
			{
				L_BLAM_LOADER_REBASE(unit->seats[i]->camera_tracks);
				L_BLAM_LOADER_REBASE(unit->seats[i]->unit_hud_interface);
			}
		}
		void biped(char* data, tags::tag_instance* instance, int base)
		{
			unit(data, instance, base);
			auto biped = (s_biped_group_definition*)data;
			L_BLAM_LOADER_REBASE(biped->dead_sphere_shapes);
			L_BLAM_LOADER_REBASE(biped->pill_shapes);
			L_BLAM_LOADER_REBASE(biped->sphere_shapes);
			L_BLAM_LOADER_REBASE(biped->contact_points);
		}
		void vehicle(char* data, tags::tag_instance* instance, int base)
		{
			unit(data, instance, base);
			auto vehicle = (s_vehicle_group_definition*)data;
			L_BLAM_LOADER_REBASE(vehicle->gears);
			L_BLAM_LOADER_REBASE(vehicle->anti_gravity_points);
			L_BLAM_LOADER_REBASE(vehicle->friction_points);
			L_BLAM_LOADER_REBASE(vehicle->shape_phantom_shape);
		}
		void weapon(char* data, tags::tag_instance* instance, int base)
		{
			object(data, instance, base);
			auto w = (s_weapon_group_definition*)data;
			L_BLAM_LOADER_REBASE(w->predicted_bitmaps);
			L_BLAM_LOADER_REBASE(w->first_person);
			L_BLAM_LOADER_REBASE(w->predicted_resources_1);
			L_BLAM_LOADER_REBASE(w->magazines);
			for(auto i = 0; i < w->magazines.size; i++)
				L_BLAM_LOADER_REBASE(w->magazines[i]->magazines_equipment);
			L_BLAM_LOADER_REBASE(w->new_triggers);
			L_BLAM_LOADER_REBASE(w->barrels);
			for (auto i = 0; i < w->barrels.size; i++)
				L_BLAM_LOADER_REBASE(w->barrels[i]->firing_effects);
		}
		void globals(char* data, tags::tag_instance* instance, int base)
		{
			auto g = (s_globals_group_definition*)data;
			L_BLAM_LOADER_REBASE(g->havok_cleanup_resources);
			L_BLAM_LOADER_REBASE(g->collision_damage);
			L_BLAM_LOADER_REBASE(g->sound_globals);
			L_BLAM_LOADER_REBASE(g->ai_globals);
			for (auto i = 0; i < g->ai_globals.size; i++)
				L_BLAM_LOADER_REBASE(g->ai_globals[i]->gravemind_properties);

			L_BLAM_LOADER_REBASE(g->damage_table);
			for (auto i = 0; i < g->damage_table.size; i++)
			{
				L_BLAM_LOADER_REBASE(g->damage_table[i]->damage_groups);
				for (auto j = 0; j < g->damage_table[i]->damage_groups.size; j++)
				{
					L_BLAM_LOADER_REBASE(g->damage_table[i]->damage_groups[j]->armor_modifiers);
				}
			}
			L_BLAM_LOADER_REBASE(g->sounds);
			L_BLAM_LOADER_REBASE(g->camera);
			L_BLAM_LOADER_REBASE(g->player_control);
			for (auto i = 0; i < g->player_control.size; i++)
				L_BLAM_LOADER_REBASE(g->player_control[i]->look_function);

			L_BLAM_LOADER_REBASE(g->difficulty);
			L_BLAM_LOADER_REBASE(g->grenades);
			L_BLAM_LOADER_REBASE(g->rasterizer_data);
			for (auto i = 0; i < g->rasterizer_data.size; i++)
				L_BLAM_LOADER_REBASE(g->rasterizer_data[i]->global_vertex_shaders);

			L_BLAM_LOADER_REBASE(g->interface_tags);
			L_BLAM_LOADER_REBASE(g->weapon_list_update__weapon_list_enum_in_game_globalsh);
			L_BLAM_LOADER_REBASE(g->cheat_powerups);

			L_BLAM_LOADER_REBASE(g->multiplayer_information);
			for (auto i = 0; i < g->multiplayer_information.size; i++)
			{
				L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->vehicles);
				L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->sounds);
				L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->general_events);
				for (auto j = 0; j < g->multiplayer_information[i]->general_events.size; j++)
					L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->general_events[j]->sound_permutations);

				L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->slayer_events);
				for (auto j = 0; j < g->multiplayer_information[i]->slayer_events.size; j++)
					L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->slayer_events[j]->sound_permutations);

				L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->ctf_events);
				for (auto j = 0; j < g->multiplayer_information[i]->ctf_events.size; j++)
					L_BLAM_LOADER_REBASE(g->multiplayer_information[i]->slayer_events[j]->sound_permutations);

				//TODO Continue here...
			}
		}
	}
}