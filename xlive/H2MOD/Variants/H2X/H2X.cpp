
#include "H2MOD\Variants\H2X\H2X.h"
#include "H2MOD\Tags\TagInterface.h"

#include "H2MOD.h"

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
__declspec(naked) void time_globals_seconds_to_ticks()
{
	__asm
	{
		push ecx
		mov ecx, 120
		mov[esp], ecx
		fild dword ptr[esp]
		fmul dword ptr[esp + 8]
		pop ecx
		ret
	}
}

void H2X::Initialize(bool enable)
{
	/*
	 * Hook the call that takes the fire recovery time and soft recovery time
	 * 
	 * * 
	 * X = (tickrate * fire recovery)							| 30 * 0.1				= 3 Ticks
	 * Y = tickrate * ((1 - soft recovery) * fire recovery)		| 30 * ((1 - 0.5) * 0.1)= 1.5 Ticks
	 * Z = floor(X) - floor(y)									| 3 - 1.5				= 1.5 Ticks
	 * 
	 * 
	 * X = (tickrate * fire recovery)							| 60 * 0.1				= 6 Ticks
	 * Y = tickrate * ((1 - soft recovery) * fire recovery)		| 60 * ((1 - 0.5) * 0.1)= 3 Ticks
	 * Z = floor(X) - floor(y)									| 6 - 3					= 3 Ticks
	 * 
	 * 
	 * X = (tickrate * fire recovery)							| 120 * 0.1				 = 12 Ticks
	 * Y = tickrate * ((1 - soft recovery) * fire recovery)		| 120 * ((1 - 0.5) * 0.1)= 6 Ticks
	 * Z = floor(X) - floor(y)									| 12 - 6				 = 6 Ticks
	 *
	 *	H2V does calculate the 2x ticks to wait as you would expect the issue is that the game also runs at 2x the speed.
	 *	
	 *	
	 */
	//if(!Memory::isDedicatedServer())
	//{
	//	if(enable)
	//	{
	//		PatchCall(Memory::GetAddress(0x15c5b3), time_globals_seconds_to_ticks);
	//		PatchCall(Memory::GetAddress(0x15c5e2), time_globals_seconds_to_ticks);
	//	}
	//	else
	//	{	
	//		typedef float(__cdecl time_globals_seconds_to_tick_percise)(float s);
	//		auto p_time_globals_seconds_to_tick_percise = (time_globals_seconds_to_tick_percise*)Memory::GetAddress(0x7c0c5);
	//		PatchCall(Memory::GetAddress(0x15c5b3), p_time_globals_seconds_to_tick_percise);
	//		PatchCall(Memory::GetAddress(0x15c5e2), p_time_globals_seconds_to_tick_percise);
	//	}
	//}

	for (auto& weapon : weapons)
	{
		for (auto& weapon : weapons)
		{
			auto required_datum = tags::find_tag(blam_tag::tag_group_type::weapon, weapon.tag_string);
			BYTE* weapon_tag = tags::get_tag<blam_tag::tag_group_type::weapon, BYTE>(required_datum);
			if (weapon_tag != nullptr)
			{
				int barrel_data_block_size = 236;
				tags::tag_data_block* barrel_data_block = reinterpret_cast<tags::tag_data_block*>(weapon_tag + 720);

				if (barrel_data_block->block_data_offset != -1)
				{
					*(float*)(tags::get_tag_data()
						+ barrel_data_block->block_data_offset
						+ barrel_data_block_size * weapon.barrel_data_block_index
						+ (weapon.rounds_per_second_based ? 8 : 32)) = (enable ? weapon.h2x_rate_of_fire : weapon.original_rate_of_fire);
				}
			}
		}
	}

	if (!Memory::isDedicatedServer() && enable && h2mod->GetEngineType() == e_engine_type::Multiplayer)
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

	