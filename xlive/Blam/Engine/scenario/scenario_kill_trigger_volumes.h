#pragma once

/* structures */

// max count: MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
struct s_scenario_kill_trigger_volume
{
	int16 trigger_volume;       // Block Index: scenario_trigger_volume
};
ASSERT_STRUCT_SIZE(s_scenario_kill_trigger_volume, 2);

// max count: MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
struct scenario_trigger_volume
{
	string_id name;
	int16 object_name;                      // Block index: scenario_light_palette
	int16 pad;
	string_id node_name;
	real_vector3d forward;
	real_vector3d up;
	real_point3d position;
	real_point3d extents;
	real32 runtime_radius;
	int16 kill_triggger_volume_index;       // Block index: scenario_trigger_volume
};
ASSERT_STRUCT_SIZE(scenario_trigger_volume, 68);

/* prototypes */

int32 get_scenario_volume_count(void);

void __cdecl scenario_kill_trigger_volume_disable(int32 trigger_volume_index);

void __cdecl scenario_kill_trigger_volume_enable(int32 trigger_volume_index);
