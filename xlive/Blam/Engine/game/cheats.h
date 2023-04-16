#pragma once

#define k_skull_count 16

//	Explanation about the Cow Bell and Whuppopotamus skull incase anyone else gets confused by how it's originally setup
//	=============================================================================================================================================
//	Cow Bell was a planned skull for halo 2 and is using the string_id HS_UNLOCKED_EXTRA_15 (does not have a value set in the e_skull_type enum)
//	Whuppopotamus is using the string_id HS_UNLOCKED_EXTRA_16 and is used in the game (value of 14 in the e_skull_type enum)
//	In the original game the string id array for the skulls dosent actually include HS_UNLOCKED_EXTRA_16 in index 14 of the array 
//	It's set to 0 (even on halo 2 xbox) so nothing displays when you pickup the skull, we fix this in the project

enum e_skull_type : unsigned int
{
	skull_type_envy = 0,
	skull_type_grunt_birthday_party = 1,
	skull_type_assassians = 2,
	skull_type_thunderstorm = 3,
	skull_type_famine = 4,
	skull_type_IWHBYD = 5,
	skull_type_blind = 6,
	skull_type_ghost = 7,
	skull_type_black_eye = 8,
	skull_type_catch = 9,
	skull_type_sputnik = 10,
	skull_type_iron = 11,
	skull_type_mythic = 12,
	skull_type_anger = 13,
	skull_type_whuppopotamus = 14,
	skull_type_cow_bell = 15,
};

// Returns true if skull type paramater passed is activated, the skull type paramater passed is valid, and the game is campaign
bool __cdecl ice_cream_flavor_available(const e_skull_type skull);

// Activates the skull type passed to the function
void __cdecl ice_cream_flavor_stock(const e_skull_type skull);

// Gets the activation array for the skulls
bool* get_ice_cream_activation();

void apply_cheat_hooks();
