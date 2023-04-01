#pragma once

DWORD get_scenario_volume_count();
void scenario_kill_trigger_volume_disable(int volume_id);
void scenario_kill_trigger_volume_enable(int volume_id);
void toggle_kill_trigger_volumes(bool enable_volumes);
