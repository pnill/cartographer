#pragma once

enum e_motion_sensor_blip_size : int16
{
    _motion_sensor_blip_medium = 0,
    _motion_sensor_blip_small = 1,
    _motion_sensor_blip_large = 2
};

void radar_patch();
void motion_sensor_apply_patches();
void motion_sensor_fix_size();