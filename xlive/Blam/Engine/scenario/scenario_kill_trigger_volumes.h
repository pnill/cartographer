#pragma once


// max count: MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
struct s_scenario_kill_trigger_volume
{
    short trigger_volume;       // Block Index: scenario_trigger_volume
};
ASSERT_STRUCT_SIZE(s_scenario_kill_trigger_volume, 2);

DWORD get_scenario_volume_count();
void scenario_kill_trigger_volume_disable(int volume_id);
void scenario_kill_trigger_volume_enable(int volume_id);
void toggle_kill_trigger_volumes(bool enable_volumes);
