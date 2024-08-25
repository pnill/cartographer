#pragma once
#include "input/controllers.h"

// Gets the FOV value for the observer camera
float observer_suggested_field_of_view();

// Sets the FOV value for the observer camera
void observer_set_suggested_field_of_view(float fov);

// Sets the overridden FOV value for the player
void player_control_set_field_of_view(float fov);

// Gets the FOV from the unit tag of the player with the passed controller index
// We modified it so it overriddes the FOV depending on if the user has a custom FOV set or the forced FOV value is set for custom variants
real32 __cdecl player_control_get_field_of_view(uint32 user_index);

void first_person_camera_apply_patches(void);