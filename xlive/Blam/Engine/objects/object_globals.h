#pragma once

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Math/BlamMath.h"

struct s_object_globals
{
	bool initialized;
	bool do_garbage_collection;
	bool do_garbage_collection_unsafe;
	bool objects_updating;
	int32 active_garbage_object_count;
	int32 field_8;
	int32 field_C;
	int32 field_10;
	uint32 total_game_time_ticks;
	int32 unique_id;
	real_color_rgb cinematic_ambient_color;
	real_vector3d cinematic_primary_light_vector_1;
	real32 cinematic_primary_light_unk1;
	real32 cinematic_primary_light_unk2;
	real_color_rgb cinematic_primary_light_color;
	real_vector3d cinematic_primary_light_vector;
	real_color_rgb cinematic_secondary_light_color;
	real_vector3d cinematic_secondary_light_vector;
	int32 field_6C;
	real32 global_function_values[4];
	bool object_custom_animations_hold_on_last_frame;
	bool object_custom_animations_prevent_lipsync_head_movement;
	bool enable_warthog_turret_light;
	bool pad83;
};
CHECK_STRUCT_SIZE(s_object_globals, 132);

s_object_globals* object_globals_get(void);
bool objects_can_connect_to_map(void);