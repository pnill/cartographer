#pragma once

// Gets the FOV value for the observer camera
float observer_suggested_field_of_view();

// Sets the FOV value for the observer camera
void observer_set_suggested_field_of_view(const float fov);

// Sets the overridden FOV value for the player
void player_control_set_field_of_view(const float fov);

// Gets the FOV from the unit tag of the player with the passed controller index
// We modified it so it overriddes the FOV depending on if the user has a custom FOV set or the forced FOV value is set for custom variants
float __cdecl player_control_get_field_of_view(int controller_index);

void first_person_camera_apply_patches();