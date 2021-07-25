#pragma once
#include "LazyBlam.hpp"
#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "Blam/Cache/TagGroups/unit_definition.hpp"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/vehicle_definition.hpp"
#define L_BLAM_LOADER_REBASE(tag_block) if(tag_block.data > 0) (tag_block.data = base + (tag_block.data - instance->data_offset))
#define L_BLAM_LOADER_CAST_BLOCK(tag_block, type) reinterpret_cast<type*>(&data[tag_block.data])
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
			//auto colors = L_BLAM_LOADER_CAST_BLOCK(o->change_colors, s_object_group_definition::s_change_colors_block);
			LOG_INFO_GAME("[{}] {:x}", __FUNCTION__, o->change_colors.data);
			for (auto i = 0; i < o->change_colors.size; i++)
			{
				//L_BLAM_LOADER_REBASE(colors[i].functions);
				//L_BLAM_LOADER_REBASE(colors[i].initial_permutations);
				L_BLAM_LOADER_REBASE(o->change_colors[i]->initial_permutations);
				L_BLAM_LOADER_REBASE(o->change_colors[i]->functions);
			}
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
			//auto seats = L_BLAM_LOADER_CAST_BLOCK(unit->seats, s_unit_group_definition::s_seats_block);
			for (auto i = 0; i < unit->seats.size; i++)
			{
				//L_BLAM_LOADER_REBASE(seats[i].camera_tracks);
				//L_BLAM_LOADER_REBASE(seats[i].unit_hud_interface);
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
	}
}