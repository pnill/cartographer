#include "stdafx.h"
#include "cheats.h"

#include "cache/cache_files.h"
#include "camera/observer.h"
#include "effects/effects.h"
#include "effects/player_effects.h"
#include "game/game.h"
#include "game/game_globals.h"
#include "game/players.h"
#include "interface/hud_messaging.h"
#include "math/random_math.h"
#include "main/console.h"
#include "objects/object_definition.h"
#include "objects/objects.h"
#include "physics/collisions.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"

#include "units/bipeds.h"

/* typedefs */

typedef void(__cdecl* ice_cream_activation_t)(unsigned int skull);
typedef bool(__cdecl* ice_cream_flavor_available_t)(unsigned int skull);

/* constants */

static const char* k_shader_drop_object_name = "test\\shader_drop_object\\shader_drop_object";

static const string_id k_skull_string_ids[k_skull_count] =
{
	_string_id_unlocked_extra_1,	// Envy
	_string_id_unlocked_extra_2,	// Grunt Birthday Party
	_string_id_unlocked_extra_3,	// Assassins
	_string_id_unlocked_extra_4,	// Thunderstorm
	_string_id_unlocked_extra_5,	// Famine
	_string_id_unlocked_extra_6,	// I Would Have Been Your Daddy
	_string_id_unlocked_extra_7,	// Blind
	_string_id_unlocked_extra_8,	// Ghost
	_string_id_unlocked_extra_9,	// Black Eye
	_string_id_unlocked_extra_10,	// Catch
	_string_id_unlocked_extra_11,	// Sputnik
	_string_id_unlocked_extra_12,	// Iron
	_string_id_unlocked_extra_13,	// Mythic
	_string_id_unlocked_extra_14,	// Angry
	_string_id_unlocked_extra_16,	// That's Just... Wrong
	_string_id_unlocked_extra_15,	// Cow Bell
};

static const tag_group k_cheat_drop_tag_groups[] =
{
	_tag_group_vehicle,
	_tag_group_biped,
	_tag_group_creature,
	_tag_group_weapon,
	_tag_group_crate,
	_tag_group_equipment,
	_tag_group_device_machine,
	_tag_group_device_control,
	_tag_group_device_light_fixture,
	_tag_group_projectile,
	_tag_group_garbage,
	_tag_group_sound_scenery,
	_tag_group_scenery,
	_tag_group_shader,
	_tag_group_effect
};

/* globals */

ice_cream_flavor_available_t p_ice_cream_flavor_available;

ice_cream_activation_t p_ice_cream_activation;

bool g_cheats_skull_enabled[k_skull_count] = { };

/* prototypes */

static bool cheat_place_tag(const real_vector3d* forward, datum tag_index, const real_point3d* position, const char* tag_name, uint32 tag_group, datum shader_tag_index);

static bool cheat_drop_effect(datum tag_index, const real_vector3d* forward, const char* tag_name, const real_point3d* position);

/* public code */

void cheats_apply_patches(void)
{
	DETOUR_ATTACH(p_ice_cream_flavor_available, Memory::GetAddress<ice_cream_flavor_available_t>(0xBD114, 0xAC2A2), ice_cream_flavor_available);
	DETOUR_ATTACH(p_ice_cream_activation, Memory::GetAddress<ice_cream_activation_t>(0xBD137, 0xAC2C5), ice_cream_flavor_stock);
	return;
}

bool __cdecl ice_cream_flavor_available(const e_skull_type skull)
{
	return skull < k_skull_count && game_is_campaign() && g_cheats_skull_enabled[skull];
}

void __cdecl ice_cream_flavor_stock(const e_skull_type skull)
{
	if (skull < k_skull_count && !g_cheats_skull_enabled[skull])
	{
		g_cheats_skull_enabled[skull] = true;
		hud_messaging_clear();
		
		const int32 user_index = players_first_active_user();

		hud_messaging_post(user_index, k_skull_string_ids[skull]);
		scripted_player_effect_screen_fade_in(1.f, 1.f, 1.f, 20);
		
		const datum sound_datum = TAG_BLOCK_GET_ELEMENT(&scenario_get_game_globals()->player_information, 0, s_game_globals_player_information)->ice_cream.index;
		
		if (sound_datum != NONE)
		{
			unspatialized_impulse_sound_new(sound_datum, 1.f);
		}
	}

	return;
}

void cheat_drop_tag_name(const char* name)
{
	char tag_name[256];
	csstrncpy(tag_name, name, NUMBEROF(tag_name));
	
	const char* group_name= strrchr(tag_name, '.');
	if (group_name)
	{
		group_name = NULL;
	}

	for (size_t i = 0; !cheat_drop_tag(k_cheat_drop_tag_groups[i], tag_name, true); ++i)
	{
		if (i >= NUMBEROF(k_cheat_drop_tag_groups))
		{
			error(_error_delayed, "could not find any tags named '%s' to drop", tag_name);
			break;
		}
	}
	return;
}

bool cheat_drop_tag(
	tag_group group,
	char const* name,
	bool ignore_error)
{
	const datum tag_index = tag_loaded(group, name);

	if (tag_index == NONE)
	{
		if (!ignore_error)
		{
			error(_error_delayed, "### ERROR couldn't load tag '%s.%s' to place", name, "unknown");
		}
		return false;
	}

	const int32 active_user = players_first_active_user();
	if (active_user == NONE)
	{
		return false;
	}

	const s_observer_result* camera = observer_try_and_get_camera(active_user);
	
	tag_group base_group = group;
	
	switch (group)
	{
	case _tag_group_effect:
		base_group = _tag_group_effect;
		break;
	case _tag_group_shader:
		base_group = _tag_group_shader;
		break;
	case _tag_group_vehicle:
	case _tag_group_weapon:
	case _tag_group_sound_scenery:
	case _tag_group_device_machine:
	case _tag_group_projectile:
	case _tag_group_scenery:
	case _tag_group_device_light_fixture:
	case _tag_group_equipment:
	case _tag_group_garbage:
	case _tag_group_device_control:
	case _tag_group_biped:
	case _tag_group_crate:
	case _tag_group_creature:
		base_group = _tag_group_object;
		break;
	default:
		// Do nothing
		break;
	}

	random_seed_allow_use();

	bool result = false;

	if (camera)
	{
		if (base_group == _tag_group_effect)
		{
			result = cheat_drop_effect(tag_index, &camera->forward, name, &camera->position);
		}
		else if (base_group == _tag_group_shader)
		{
			const datum shader_drop_object_index = tag_loaded(_tag_group_scenery, k_shader_drop_object_name);
			result = cheat_place_tag(&camera->forward, shader_drop_object_index, &camera->position, name, _tag_group_shader, tag_index);
		}
		else if (base_group == _tag_group_object)
		{
			result = cheat_place_tag(&camera->forward, tag_index, &camera->position, name, _tag_group_object, NONE);
		}
		else
		{
			error(_error_delayed, "### ERROR don't know how to place tags of type '%s'", "unknown");
		}
	}

	random_seed_disallow_use();

	return result;
}

/* private code */

static bool cheat_place_tag(const real_vector3d* forward, datum tag_index, const real_point3d* position, const char* tag_name, uint32 tag_group, datum shader_tag_index)
{
	bool result = false;
	if (!game_is_predicted())
	{
		if (tag_index == NONE)
		{
			if (tag_group == _tag_group_shader)
			{
				error(_error_delayed, "### ERROR couldn't load drop-shader object '%s.scenery'", k_shader_drop_object_name);
				return 0;
			}
			if (tag_group == _tag_group_object)
			{
				error(_error_delayed, "### ERROR couldn't load object '%s.%s' to drop it", tag_name, "unknown");
			}
		}
		else
		{
			object_definition* object = object_definition_get(tag_index);

			object_placement_data placement_data;
			object_placement_data_new(&placement_data, tag_index, NONE, NULL);
			point_from_line3d(position, forward, object->object.bounding_radius + 1.f, &placement_data.position);

			const datum object_index = object_new(&placement_data);

			if (object_index == NONE)
			{
				error(_error_delayed, "### ERROR couldn't place '%s.%s'", tag_name, "unknown");
			}
			else
			{
				object_force_inside_bsp(object_index, position, NONE);
	#ifdef OBJECT_OVERRIDE_ENABLED
				if (shader_tag_index != NONE)
				{
					object_override_set_shader(object_index, shader_tag_index);
				}
	#endif

				if (object->object.object_type == _object_type_biped && biped_get(object_index)->unit.weapon_index == NONE)
				{
					tag_iterator weapon_iterator;
					tag_iterator_new(&weapon_iterator, _tag_group_weapon);
					for (datum i = tag_iterator_next(&weapon_iterator); i != NONE; i = tag_iterator_next(&weapon_iterator))
					{
						object_placement_data weapon_placement_data;
						object_placement_data_new(&weapon_placement_data, i, NONE, NULL);
						const datum weapon_index = object_new(&weapon_placement_data);
						if (weapon_index != NONE)
						{
							if (unit_add_weapon_to_inventory(object_index, weapon_index, _weapon_addition_method_one))
							{
								break;
							}
							object_delete(weapon_index);
						}
					}
				}
				simulation_action_object_create(object_index);
				console_printf("placed '%s.%s'", tag_name, "unknown");
				result = true;
			}
		}
	}

	return result;
}

static bool cheat_drop_effect(datum tag_index, const real_vector3d* forward, const char* tag_name, const real_point3d* position)
{
	bool result = false;

	assert_valid_real_normal3d(forward);

	if (tag_index == NONE)
	{
		error(_error_delayed, "### ERROR couldn't load effect '%s.effect' to drop it", tag_name);
	}
	else
	{
		collision_result collision_res;
		collision_res.global_material_index = NONE;

		real_vector3d collision_testing_vector;
		scale_vector3d(forward, 1000.f, &collision_testing_vector);

		const e_collision_test_flags flags = (e_collision_test_flags)(FLAG(_collision_test_structure_bit) | FLAG(_collision_test_instanced_geometry_bit) | FLAG(_collision_test_bit_23));
		if (collision_test_vector(flags, position, &collision_testing_vector, NONE, NONE, &collision_res))
		{
			real_point3d collision_position;
			collision_position.x = collision_res.point.x - (forward->i * 0.25f);
			collision_position.y = collision_res.point.y - (forward->j * 0.25f);
			collision_position.z = collision_res.point.z - (forward->k * 0.25f);
			effect_new_from_point_vector(tag_index, &collision_position, forward, &collision_res.fog_plane, _match_all_markers_true, _effect_deterministic_true);
		}
		else
		{
			error(_error_delayed, "### WARNING couldn't find location to drop effect '%s.effect'", tag_name);
		}
	}

	return result;
}