/*
	gbMichelle: 2020-04-14  -  2020-04-15
	 - Added universal firing rate and acceleration calculations for
	   Xbox behavior emulation.
*/

#include <math.h>

#include "stdafx.h"
#include "H2MOD\Variants\H2X\H2X.h"

#include "H2MOD\Tags\TagInterface.h"

// object string, H2X rate of fire, H2v rate of fire, active barrel index, rounds per second based
std::vector<H2X::h2x_mod_info> weapons =
{
	{ "objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle", 0.535f, 0.5f, 0, false },
	{ "objects\\weapons\\rifle\\battle_rifle\\battle_rifle", 0.295f, 0.26f, 0, false },
	{ "objects\\weapons\\pistol\\magnum\\magnum", 0.13f, 0.1f, 0, false },
	{ "objects\\weapons\\rifle\\shotgun\\shotgun", 1.035f, 1.0f, 0, false },
	{ "objects\\weapons\\rifle\\beam_rifle\\beam_rifle", 0.25875f, 0.25f, 0, false },
	{ "objects\\weapons\\support_low\\brute_shot\\brute_shot", 0.39f, 0.3f, 0, false },
	{ "objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol", 0.11f, 0.05f, 0, false },
	{ "objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine", 0.20f, 0.14f, 1, false },
	{ "objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher", 0.85f, 0.8f, 0, false },
	{ "objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle", 8.5f, 9.0f, 0, true },
	{ "objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle", 10.0f, 11.0f, 0, true }
};

float calculate_h2x_firerate(float h2v_firerate) {
	// Calculate the effective firerate per seconds.
	/* This works because every frame Halo 2 checks to see if enough time has
	   passed for the gun to be allowed to fire again.
	   These checks happen more often at higher framerates thus where at 30fps
	   it had to wait an extra frame, it now does not.

	   We simulate it using this formula */

	if (h2v_firerate < 0.0001) {
		// Do not divide by 0.
		return h2v_firerate;
	} else if (h2v_firerate > 30.0) {
		/* Firing rates above 30 are not possible at 30fps and below. */
		return 30.0;
	}

	/* Convert to double to avoid rounding errors that show up all too often
	   with floats. */
	double h2v_firerate_dbl = static_cast<double>(h2v_firerate);
	// -0.01 to avoid a potential rounding error.
	return static_cast<float>(30.0 / ceil(1 / h2v_firerate_dbl * 30.0 - 0.01));
}

float calculate_h2x_recovery_time(float h2v_recovery_time) {
	if (h2v_recovery_time < 0.0001) {
		// Our math always adds one tick. So, don't do anything if 0.
		return h2v_recovery_time;
	}

	/* The + hack is done because all of the brute forced values
	   I find here seem to add an extra tick. This should be wrong.
	   Please double check if that is actually right.
	   Potentially remove the + hack completely.

	   I am convinced that you over compensated and now your guns are too slow.*/

	float hack = 30.0/1000.0;
	return ceil(h2v_recovery_time * 30.0) / 30.0 + hack;
}

void H2X::Initialize(bool enable)
{
	// Why even execute any of this if we're not enabled?
	if (!enable) return;

	for (auto& weapon : weapons)
	{
		/* With the value calculation being made universal we should consider
		   applying this math to all weapon tags. */
		auto required_datum = tags::find_tag('weap', weapon.tag_string);
		BYTE* weapon_tag = tags::get_tag<'weap', BYTE>(required_datum);
		if (weapon_tag != nullptr)
		{
			int barrel_data_block_size = 236;
			tags::tag_data_block* barrel_data_block = reinterpret_cast<tags::tag_data_block*>(weapon_tag + 720);

			if (barrel_data_block->block_data_offset != -1)
			{
				// Get the address of where the barrel blocks start.
				auto barrel_blocks = tags::get_tag_data()
					+ barrel_data_block->block_data_offset;

				// Go through all barrel blocks and apply the fix.
				for (int i = 0; i < barrel_data_block->block_count; i++) {
					auto barrel_block = barrel_blocks + barrel_data_block_size * i;

					float* firerate_lower = (float*)(barrel_block + 4);
					float* firerate_upper = (float*)(barrel_block + 8);
					float* acceleration_time = (float*)(barrel_block + 12);
					float* deceleration_time = (float*)(barrel_block + 16);
					float* recovery_time = (float*)(barrel_block + 32);

					float new_upper = calculate_h2x_firerate(*firerate_upper);
					float new_lower = calculate_h2x_firerate(*firerate_lower);

					if (*firerate_lower != *firerate_upper) {
						/* To simulate the most correct between speeds we
						   might need to make the climb shorter.
						   Otherwise the rates will be too slow. */

						double old_difference = static_cast<double>(*firerate_upper) - static_cast<double>(*firerate_lower);
						double new_difference = static_cast<double>(new_upper) - static_cast<double>(new_lower);

						double factor = new_difference / old_difference;

						*acceleration_time = static_cast<float>(*acceleration_time * factor);
						*deceleration_time = static_cast<float>(*deceleration_time * factor);
					}

					*firerate_lower = new_lower;
					*firerate_upper = new_upper;

					*recovery_time = calculate_h2x_recovery_time(*recovery_time);
				}
			}
		}
	}

	if (!h2mod->Server && h2mod->GetMapType() == MULTIPLAYER_MAP)
	{
		// H2X Sound_Classes
		*(float*)(&tags::get_tag_data()[0x4821C]) = 0.0f; /*H2X projectile_impact Index 0 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48220]) = -4.0f; /*H2X projectile_impact Index 0 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48278]) = 0.0f; /*H2X projectile_detonation Index 1 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x4827C]) = -4.0f; /*H2X projectile_detonation Index 1 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x482D4]) = 0.0f; /*H2X projectile_flyby Index 2 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x482D8]) = -4.0f; /*H2X projectile_flyby Index 2 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x4838C]) = 0.0f; /*H2X weapon_fire Index 4 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48390]) = -4.0f; /*H2X weapon_fire Index 4 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x483E8]) = 0.0f; /*H2X weapon_ready Index 5 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x483EC]) = -4.0f; /*H2X weapon_ready Index 5 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48444]) = 0.0f; /*H2X weapon_reload Index 6 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48448]) = -4.0f; /*H2X weapon_reload Index 6 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x484A0]) = 0.0f; /*H2X weapon_empty Index 7 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x484A4]) = -4.0f; /*H2X weapon_empty Index 7 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x484FC]) = 0.0f; /*H2X weapon_charge Index 8 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48500]) = -4.0f; /*H2X weapon_charge Index 8 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48558]) = 0.0f; /*H2X weapon_overheat Index 9 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x4855C]) = -4.0f; /*H2X weapon_overheat Index 9 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x485B4]) = 0.0f; /*H2X weapon_idle Index 10 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x485B8]) = -4.0f; /*H2X weapon_idle Index 10 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48610]) = 0.0f; /*H2X weapon_melee Index 11 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48614]) = -4.0f; /*H2X weapon_melee Index 11 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x4866C]) = 0.0f; /*H2X weapon_animation Index 12 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48670]) = -4.0f; /*H2X weapon_animation Index 12 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x486C8]) = -64.0f; /*H2X object_impacts Index 13 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x486CC]) = -4.0f; /*H2X object_impacts Index 13 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48724]) = -12.0f; /*H2X particle_impacts Index 14 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48728]) = -4.0f; /*H2X particle_impacts Index 14 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48894]) = -32.0f; /*H2X unit_footsteps Index 18 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48898]) = -9.0f; /*H2X unit_footsteps Index 18 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x4894C]) = 0.0f; /*H2X unit_animation Index 20 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48950]) = -2.0f; /*H2X unit_animation Index 20 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48A04]) = 0.0f; /*H2X vehicle_collision Index 22 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48A08]) = -4.0f; /*H2X vehicle_collision Index 22 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48A60]) = 0.0f; /*H2X vehicle_engine Index 23 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48A64]) = -4.0f; /*H2X vehicle_engine Index 23 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48ABC]) = 0.0f; /*H2X vehicle_animation Index 24 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48AC0]) = -4.0f; /*H2X vehicle_animation Index 24 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48C2C]) = 0.0f; /*H2X device_machinery Index 28 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48C30]) = -4.0f; /*H2X device_machinery Index 28 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48C2C]) = 0.0f; /*H2X device_stationary Index 29 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48C30]) = -4.0f; /*H2X device_stationary Index 29 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48E54]) = 0.0f; /*H2X ambient_machinery Index 34 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48E58]) = -4.0f; /*H2X ambient_machinery Index 34 Gains Bounds upper*/
	}
}

	