#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Math\BlamMath.h"

struct s_object_globals
{
	bool initialized;
	bool do_garbage_collection;
	bool do_garbage_collection_unsafe;
	bool objects_updating;
	int active_objects_maybe;
	int field_8;
	int field_C;
	int field_10;
	DWORD unk_game_time_store;
	DWORD unique_id;
	real_color_rgb cinematic_ambient_color;
	real_vector3d cimenatic_primary_light_vector_1;
	float cimenatic_primary_light_unk1;
	float cimenatic_primary_light_unk2;
	real_color_rgb cimenatic_primary_light_color;
	real_vector3d cimenatic_primary_light_vector;
	real_color_rgb cinematic_secondary_light_color;
	real_vector3d cinematic_secondary_light_vector;
	DWORD field_6C;
	float global_function_values[4];
	char object_custom_animations_hold_on_last_frame;
	bool object_custom_animations_prevent_lipsync_head_movement;
	bool enable_warthog_turret_light;
	bool field_83;

	static s_object_globals* get();
	static bool object_is_connected_to_map(datum header_datum);
	static void object_connect_lights_recursive(datum header_datum, int a2, int a3, char a4, char a5);
	static void object_update_collision_culling(datum header_datum);
	static void object_wake(datum object_datum);
	static bool object_globals_initialized();
};
