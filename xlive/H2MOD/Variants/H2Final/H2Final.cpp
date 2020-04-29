#include "stdafx.h"
#include "Globals.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"
#include "..\H2MOD\Tags\TagInterface.h"

/*
Todo:
- Does the QuickCamo stuff need to be ran on the server?
- 2v2:
	- Start with 4 Frags
		- On Hold
	- Carbine (On hold until shot leading is consistent)
		- Should be 3 shot kill
		- Have the same ttk as H1 Magnum
*/

bool isEnabled = false;
bool detoursHavePreviouslyBeenApplied = false;

DWORD dwBack;
DWORD base_address;
DWORD address_offset;
DWORD game_time_globals;


#pragma region Static Weapon Respawns

int equpiment_offset;
int game_clock_initial_value;

int GetCurrentGameTime()
{
	return *(int*)(game_time_globals + 0x2d70);
}

void SetAllWeaponsToStartSpawnTimerOnPickup()
{
	int numItems = *(int*)((BYTE*)address_offset + 0x143C120);

	for (int equipment_index = 0; equipment_index < numItems; equipment_index++)
	{
		DWORD itemStart = equpiment_offset + (equipment_index * 0x90);
		BYTE respawnTimerStartsOn = *(BYTE*)(address_offset + itemStart + 0x12);

		if (respawnTimerStartsOn == 0x1) // If item was set to start respawn timer on body depletion
		{
			*(BYTE*)((char*)address_offset + itemStart + 0x12) = 0x0; //Set item respawn timer to start on pickup
			*(int16_t*)((char*)address_offset + itemStart + 0xE) = 90; //Set the Spawn time to 90 seconds
		}
	}
}

typedef signed int(__stdcall *get_spawn_time)(int equipment_index);
get_spawn_time pget_spawn_time;

signed int __stdcall GetSecondsUntilEquipmentRespawn(int equipment_index)
{
	if (isEnabled)
	{
		LOG_TRACE_GAME("[H2MOD-H2F] : start GetSecondsUntilEquipmentRespawn({})", equipment_index);

		int currentGameTime = GetCurrentGameTime();

		LOG_TRACE_GAME("[H2MOD-H2F] currentGameTime: {}", currentGameTime);

		DWORD itemStart = equpiment_offset + (equipment_index * 0x90);
		int16_t respawnInterval = *(int16_t*)((char*)address_offset + itemStart + 0xE);

		//If spawnInterval is set to 0, use a default spawn interval of 90 seconds
		if (respawnInterval < 1)
			respawnInterval = 90;

		LOG_TRACE_GAME("[H2MOD-H2F] respawnInterval: {}", respawnInterval);

		int secondsUntilRespawn;
		if (game_clock_initial_value)
			//Calculate secondsUntilRespawn based off of currentGameTime starting at the time limit, and counting downwards to 0
			secondsUntilRespawn = (currentGameTime % respawnInterval) + (game_clock_initial_value % respawnInterval);
		else
			//Calculate secondsUntilRespawn based off of currentGameTime starting at 0 and counting upwards indefinitely
			secondsUntilRespawn = respawnInterval - (currentGameTime % respawnInterval);

		LOG_TRACE_GAME("[H2MOD-H2F] Spawning item {0} in {1} seconds!", equipment_index, secondsUntilRespawn);

		return secondsUntilRespawn + 1;
	}
	return pget_spawn_time(equipment_index);
}

int64_t originalGetSecondsUntilEquipmentRespawnFunctionData;
void EnableStaticWeaponSpawns()
{
	if (NetworkSession::localPeerIsSessionHost() || h2mod->Server)
	{
		originalGetSecondsUntilEquipmentRespawnFunctionData = *(int64_t*)(base_address + 0x6A8C4);
		pget_spawn_time = (get_spawn_time)DetourFunc((BYTE*)base_address + 0x6A8C4, (BYTE*)GetSecondsUntilEquipmentRespawn, 5);
	}
}

void DisableStaticWeaponSpawns()
{
	if (NetworkSession::localPeerIsSessionHost() || h2mod->Server)
	{
		VirtualProtect((LPVOID)(base_address + 0x6A8C4), 8, PAGE_EXECUTE_READWRITE, &dwBack);
		*(int64_t*)(base_address + 0x6A8C4) = originalGetSecondsUntilEquipmentRespawnFunctionData;
	}
}

#pragma endregion

#pragma region QuickCamo

typedef int(__cdecl *player_weapon_switched)(int, int, int);
player_weapon_switched pplayer_weapon_switched;

int PlayerWeaponSwitched(int a1, int a2, int a3)
{
	if (isEnabled)
	{
		int v3 = *(DWORD*)(*(DWORD*)((char*)game_state_objects_header + 0x44) + 12 * (a1 & 0xFFFF) + 8);
		char* weap_tag_offset = &tags::get_tag_data()[tags::get_tag_instances()[*(DWORD*)v3 & 0xFFFF].data_offset];

		int player_datum = *(DWORD*)(*(DWORD*)((char*)game_state_objects_header + 0x44) + 12 * (a2 & 0xFFFF) + 8);
		*(float*)(player_datum + 0x2CC) = *(float*)(weap_tag_offset + 0x280);
	}
	return pplayer_weapon_switched(a1, a2, a3);
}

int32_t originalSetDefaultRegrowthRateData;
int16_t originalSetRegrowthRateToMaxOf25;
void DisableCamoRegenerationDefaults()
{
	//Disable setting default regrowth rate
	VirtualProtect((LPVOID)(base_address + 0x13FA13), 4, PAGE_EXECUTE_READWRITE, &dwBack);
	originalSetDefaultRegrowthRateData = *(int32_t*)(base_address + 0x13FA13);
	*(BYTE*)(base_address + 0x13FA13) = 0x90;
	*(BYTE*)(base_address + 0x13FA14) = 0x90;
	*(BYTE*)(base_address + 0x13FA15) = 0x90;

	//Always set camo regrowth rate to the value passed in
	VirtualProtect((LPVOID)(base_address + 0x13FA26), 2, PAGE_EXECUTE_READWRITE, &dwBack);
	originalSetRegrowthRateToMaxOf25 = *(int16_t*)(base_address + 0x13FA26);
	*(BYTE*)(base_address + 0x13FA26) = 0x90;
	*(BYTE*)(base_address + 0x13FA27) = 0x90;
}

void EnableCamoRegenerationDefaults()
{
	//Disable setting default regrowth rate
	*(int32_t*)(base_address + 0x13FA13) = originalSetDefaultRegrowthRateData;

	//Only set regrowth rate to value passed in if it is less than 0.25f
	*(int16_t*)(base_address + 0x13FA26) = originalSetRegrowthRateToMaxOf25;
}

#pragma endregion

#pragma region Melee Lunge

int32_t originalMeleeLungeFunctionData;
void DisableMeleeLunge()
{
	VirtualProtect((LPVOID)(base_address + 0x53ED5), 4, PAGE_EXECUTE_READWRITE, &dwBack);
	originalMeleeLungeFunctionData = *(int32_t*)(base_address + 0x53ED5);
	*(BYTE*)(base_address + 0x53ED5) = 0x90;
	*(BYTE*)(base_address + 0x53ED6) = 0x90;
	*(BYTE*)(base_address + 0x53ED7) = 0x90;
}

void EnableMeleeLunge()
{
	VirtualProtect((LPVOID)(base_address + 0x53ED5), 4, PAGE_EXECUTE_READWRITE, &dwBack);
	*(int32_t*)(base_address + 0x53ED5) = originalMeleeLungeFunctionData;
}

#pragma endregion

void Halo2Final::Initialize()
{
	LOG_TRACE_GAME("[H2MOD-H2F] : start Initialize()");
	settings = new Halo2FinalSettings();

#pragma region Load Configuration
	FILE *h2f;
	h2f = fopen("halo2final.ini", "r");

	if (h2f)
	{
		LOG_TRACE_GAME("[H2MOD-H2F] : Opened halo2final.ini");

		while (!feof(h2f))
		{
			char str[256];
			fgets(str, 256, h2f);

			CHECK_ARG_FLOAT("hitscan_projectile_velocity = ", settings->hitscan_projectile_velocity);
			CHECK_ARG_FLOAT("magnetism_friction = ", settings->magnetism_friction);
			CHECK_ARG_FLOAT("magnetism_adhesion = ", settings->magnetism_adhesion);
			CHECK_ARG_FLOAT("melee_angles_y = ", settings->melee_angles_y);
			CHECK_ARG_FLOAT("melee_angles_p = ", settings->melee_angles_p);
			CHECK_ARG_FLOAT("melee_depth = ", settings->melee_depth);
			CHECK_ARG_BOOL("player_turn_off_melee_lunge = ", settings->player_turn_off_melee_lunge);
			CHECK_ARG_BOOL("player_turn_off_spawn_protection = ", settings->player_turn_off_spawn_protection);
			CHECK_ARG_BOOL("player_turn_off_overshield_shader = ", settings->player_turn_off_overshield_shader);
			CHECK_ARG_FLOAT("br_autoaim_angle = ", settings->br_autoaim_angle);
			CHECK_ARG_FLOAT("br_autoaim_range = ", settings->br_autoaim_range);
			CHECK_ARG_FLOAT("br_fire_recovery_time = ", settings->br_fire_recovery_time);
			CHECK_ARG_FLOAT("br_camo_ding = ", settings->br_camo_ding);
			CHECK_ARG_FLOAT("br_camo_regrowth_rate = ", settings->br_camo_regrowth_rate);
			CHECK_ARG_BOOL("br_turn_off_crosshair_dot = ", settings->br_turn_off_crosshair_dot);
			CHECK_ARG_BOOL("br_turn_off_contrail = ", settings->br_turn_off_contrail);
			CHECK_ARG_FLOAT("snipe_autoaim_angle = ", settings->snipe_autoaim_angle);
			CHECK_ARG_FLOAT("snipe_camo_ding =", settings->snipe_camo_ding);
			CHECK_ARG_FLOAT("snipe_camo_regrowth_rate =", settings->snipe_camo_regrowth_rate);
			CHECK_ARG_FLOAT("n_rounds_per_second_min = ", settings->n_rounds_per_second_min);
			CHECK_ARG_FLOAT("n_rounds_per_second_max = ", settings->n_rounds_per_second_max);
			CHECK_ARG_FLOAT("n_initial_velocity = ", settings->n_initial_velocity);
			CHECK_ARG_FLOAT("n_final_velocity = ", settings->n_final_velocity);
			CHECK_ARG_FLOAT("n_camo_ding = ", settings->n_camo_ding);
			CHECK_ARG_FLOAT("n_camo_regrowth_rate = ", settings->n_camo_regrowth_rate);
			CHECK_ARG_FLOAT("pr_stun = ", settings->pr_stun);
			CHECK_ARG_FLOAT("pr_max_stun = ", settings->pr_max_stun);
			CHECK_ARG_FLOAT("pr_stun_time = ", settings->pr_stun_time);
			CHECK_ARG_FLOAT("pr_initial_velocity = ", settings->pr_initial_velocity);
			CHECK_ARG_FLOAT("pr_final_velocity = ", settings->pr_final_velocity);
			CHECK_ARG_FLOAT("pr_max_error_angle = ", settings->pr_max_error_angle);
			CHECK_ARG_FLOAT("pr_camo_ding = ", settings->pr_camo_ding);
			CHECK_ARG_FLOAT("pr_camo_regrowth_rate = ", settings->pr_camo_regrowth_rate);
			CHECK_ARG_FLOAT("r_initial_velocity = ", settings->r_initial_velocity);
			CHECK_ARG_FLOAT("r_damage_upper_bound_min = ", settings->r_damage_upper_bound_min);
			CHECK_ARG_FLOAT("r_damage_upper_bound_max = ", settings->r_damage_upper_bound_max);
			CHECK_ARG_FLOAT("r_camo_ding = ", settings->r_camo_ding);
			CHECK_ARG_FLOAT("r_camo_regrowth_rate = ", settings->r_camo_regrowth_rate);
			CHECK_ARG_FLOAT("smg_rounds_per_second_min = ", settings->smg_rounds_per_second_min);
			CHECK_ARG_FLOAT("smg_rounds_per_second_max = ", settings->smg_rounds_per_second_max);
			CHECK_ARG_FLOAT("smg_minimum_error = ", settings->smg_minimum_error);
			CHECK_ARG_FLOAT("smg_damage_min = ", settings->smg_damage_min);
			CHECK_ARG_FLOAT("smg_damage_max = ", settings->smg_damage_max);
			CHECK_ARG_FLOAT("smg_camo_ding = ", settings->smg_camo_ding);
			CHECK_ARG_FLOAT("smg_camo_regrowth_rate = ", settings->smg_camo_regrowth_rate);
			CHECK_ARG_FLOAT("mag_fire_recovery_time = ", settings->mag_fire_recovery_time);
			CHECK_ARG_FLOAT("mag_error_angle_min = ", settings->mag_error_angle_min);
			CHECK_ARG_FLOAT("mag_error_angle_max = ", settings->mag_error_angle_max);
			CHECK_ARG_FLOAT("mag_damage_lower_bound = ", settings->mag_damage_lower_bound);
			CHECK_ARG_FLOAT("mag_damage_upper_bound_min = ", settings->mag_damage_upper_bound_min);
			CHECK_ARG_FLOAT("mag_damage_upper_bound_max = ", settings->mag_damage_upper_bound_max);
			CHECK_ARG_FLOAT("mag_camo_ding = ", settings->mag_camo_ding);
			CHECK_ARG_FLOAT("mag_camo_regrowth_rate = ", settings->mag_camo_regrowth_rate);
			CHECK_ARG_FLOAT("pp_initial_velocity = ", settings->pp_initial_velocity);
			CHECK_ARG_FLOAT("pp_final_velocity = ", settings->pp_final_velocity);
			CHECK_ARG_FLOAT("pp_guided_angular_velocity_lower = ", settings->pp_guided_angular_velocity_lower);
			CHECK_ARG_FLOAT("pp_guided_angular_velocity_upper = ", settings->pp_guided_angular_velocity_upper);
			CHECK_ARG_FLOAT("pp_heat_per_charged_shot = ", settings->pp_heat_per_charged_shot);
			CHECK_ARG_FLOAT("pp_camo_ding = ", settings->pp_camo_ding);
			CHECK_ARG_FLOAT("pp_camo_regrowth_rate = ", settings->pp_camo_regrowth_rate);
			CHECK_ARG_FLOAT("car_autoaim_angle = ", settings->car_autoaim_angle);
			CHECK_ARG_FLOAT("car_autoaim_range = ", settings->car_autoaim_range);
			CHECK_ARG_FLOAT("car_fire_recovery_time = ", settings->car_fire_recovery_time);
			CHECK_ARG_FLOAT("car_error_angle = ", settings->car_error_angle);
			CHECK_ARG_FLOAT("car_damage = ", settings->car_damage);
			CHECK_ARG_FLOAT("car_camo_ding = ", settings->car_camo_ding);
			CHECK_ARG_FLOAT("car_camo_regrowth_rate = ", settings->car_camo_regrowth_rate);
			CHECK_ARG_INT("sg_projectiles_per_shot = ", settings->sg_projectiles_per_shot);
			CHECK_ARG_FLOAT("sg_error_angle_min = ", settings->sg_error_angle_min);
			CHECK_ARG_FLOAT("sg_error_angle_max = ", settings->sg_error_angle_max);
			CHECK_ARG_FLOAT("sg_damage_lower_bound = ", settings->sg_damage_lower_bound);
			CHECK_ARG_FLOAT("sg_damage_upper_bound_min = ", settings->sg_damage_upper_bound_min);
			CHECK_ARG_FLOAT("sg_damage_upper_bound_max = ", settings->sg_damage_upper_bound_max);
			CHECK_ARG_FLOAT("sg_camo_ding = ", settings->sg_camo_ding);
			CHECK_ARG_FLOAT("sg_camo_regrowth_rate = ", settings->sg_camo_regrowth_rate);
			CHECK_ARG_FLOAT("f_arming_time = ", settings->f_arming_time);
			CHECK_ARG_FLOAT("f_radius_to = ", settings->f_radius_to);
			CHECK_ARG_FLOAT("pg_arming_time = ", settings->pg_arming_time);
			CHECK_ARG_FLOAT("ps_max_random_spawn_bias = ", settings->ps_max_random_spawn_bias);
		}
	}
	else
	{
		LOG_TRACE_GAME("[H2MOD-H2F] : Could not open halo2final.ini");
	}

#pragma endregion

	game_time_globals = *(DWORD*)(&tags::get_tag_data()[0x4c06e4]);
	equpiment_offset = *(int*)(&tags::get_tag_data()[0x143C124]);
	game_clock_initial_value = *(DWORD*)(&tags::get_tag_data()[0x50A5B0]);

	LOG_TRACE_GAME("[H2MOD-H2F] game_clock_initial_value: {} (0 for no time limit)", game_clock_initial_value);

	EnableStaticWeaponSpawns();

	if (!detoursHavePreviouslyBeenApplied)
	{
		pplayer_weapon_switched = (player_weapon_switched)DetourFunc((BYTE*)h2mod->GetAddress(0x13AA9A), (BYTE*)PlayerWeaponSwitched, 8);
	}

#pragma region Apply Weapon Balances
	//globals.matg
	*(float*)(address_offset + 0x3DBE4) = settings->magnetism_friction;
	*(float*)(address_offset + 0x3DBE8) = settings->magnetism_adhesion;

	//Battle Rifle
	*(float*)(address_offset + 0xA49A7C) = settings->br_fire_recovery_time;
	*(float*)(address_offset + 0xA49750) = settings->br_autoaim_angle;
	*(float*)(address_offset + 0xA49754) = settings->br_autoaim_range;
	//*(float*)(address_offset + 0xA49758) = 0.05f; //Magnetism Angle
	*(float*)(address_offset + 0xA49AD0) = 0.0f; // Error Angle Min
	*(float*)(address_offset + 0xA49AD4) = 0.0f; // Error Angle Max
	*(float*)(address_offset + 0xA4F288) = 8.0f; // Damage Lower Bound
	*(float*)(address_offset + 0xA4F28C) = 8.0f; // Damage Upper Bound Min
	*(float*)(address_offset + 0xA4F290) = 8.0f; // Damage Upper Bound Max
	*(float*)(address_offset + 0xA496F8) = settings->melee_angles_y;
	*(float*)(address_offset + 0xA496FC) = settings->melee_angles_p;
	*(float*)(address_offset + 0xA49700) = settings->melee_depth;
	*(float*)(address_offset + 0xA497C4) = settings->br_camo_ding;
	*(float*)(address_offset + 0xA497C8) = settings->br_camo_regrowth_rate;

	if (settings->br_turn_off_crosshair_dot)
		*(BYTE*)(address_offset + 0xA4DE32) = 0;

	if (settings->br_turn_off_contrail)
		for (int i = 0; i < 8; i++)
			*(BYTE*)(address_offset + 0xA4ECB0 + i) = 255;

	//Sniper
	*(float*)(address_offset + 0xB7A08C) = settings->snipe_autoaim_angle;
	*(float*)(address_offset + 0xB7A034) = settings->melee_angles_y;
	*(float*)(address_offset + 0xB7A038) = settings->melee_angles_p;
	*(float*)(address_offset + 0xB7A03C) = settings->melee_depth;
	*(float*)(address_offset + 0xB7A100) = settings->snipe_camo_ding;
	*(float*)(address_offset + 0xB7A104) = settings->snipe_camo_regrowth_rate;

	//Needler
	*(float*)(address_offset + 0x9C0E88) = settings->n_rounds_per_second_min;
	*(float*)(address_offset + 0x9C0E8C) = settings->n_rounds_per_second_max;
	*(float*)(address_offset + 0x9C91A8) = settings->n_initial_velocity;
	*(float*)(address_offset + 0x9C91AC) = settings->n_final_velocity;
	*(float*)(address_offset + 0x9C0BB0) = settings->melee_angles_y;
	*(float*)(address_offset + 0x9C0BB4) = settings->melee_angles_p;
	*(float*)(address_offset + 0x9C0BB8) = settings->melee_depth;
	*(float*)(address_offset + 0x9C0C7C) = settings->n_camo_ding;
	*(float*)(address_offset + 0x9C0C80) = settings->n_camo_regrowth_rate;
	*(BYTE*)(address_offset + 0x9C0B2E) = 0; // No dual Wielding

											 //PR
	*(float*)(address_offset + 0xAB3D98) = settings->pr_stun;
	*(float*)(address_offset + 0xAB3D9C) = settings->pr_max_stun;
	*(float*)(address_offset + 0xAB3DA0) = settings->pr_stun_time;
	*(float*)(address_offset + 0xAAE244) = settings->melee_angles_y;
	*(float*)(address_offset + 0xAAE248) = settings->melee_angles_p;
	*(float*)(address_offset + 0xAAE24C) = settings->melee_depth;
	*(float*)(address_offset + 0xAB377C) = settings->pr_initial_velocity;
	*(float*)(address_offset + 0xAB3780) = settings->pr_final_velocity;
	*(float*)(address_offset + 0xAAE5B4) = settings->pr_max_error_angle;
	*(float*)(address_offset + 0xAAE310) = settings->pr_camo_ding;
	*(float*)(address_offset + 0xAAE314) = settings->pr_camo_regrowth_rate;
	*(BYTE*)(address_offset + 0xAAE1C2) = 2; // No dual Wielding

											 //Rocket
	*(float*)(address_offset + 0xBE0CF4) = settings->r_initial_velocity;
	*(float*)(address_offset + 0xBE236C) = settings->r_damage_upper_bound_min;
	*(float*)(address_offset + 0xBE2370) = settings->r_damage_upper_bound_max;
	*(float*)(address_offset + 0xBDBBFC) = settings->melee_angles_y;
	*(float*)(address_offset + 0xBDBC00) = settings->melee_angles_p;
	*(float*)(address_offset + 0xBDBC04) = settings->melee_depth;
	*(float*)(address_offset + 0xBDBCC8) = settings->r_camo_ding;
	*(float*)(address_offset + 0xBDBCCC) = settings->r_camo_regrowth_rate;
	*(float*)(address_offset + 0xBDBC54) = 0.0f; // Auto Aim Angle Radians

												 //Smg
	*(float*)(address_offset + 0xB1E5EC) = settings->smg_rounds_per_second_min;
	*(float*)(address_offset + 0xB1E5F0) = settings->smg_rounds_per_second_max;
	*(float*)(address_offset + 0xB1E658) = settings->smg_minimum_error;
	*(float*)(address_offset + 0xB22C68) = settings->smg_damage_min;
	*(float*)(address_offset + 0xB22C6C) = settings->smg_damage_max;
	*(float*)(address_offset + 0xB1E33C) = settings->melee_angles_y;
	*(float*)(address_offset + 0xB1E340) = settings->melee_angles_p;
	*(float*)(address_offset + 0xB1E344) = settings->melee_depth;
	*(float*)(address_offset + 0xB228B0) = settings->hitscan_projectile_velocity;
	*(float*)(address_offset + 0xB228B4) = settings->hitscan_projectile_velocity;
	*(float*)(address_offset + 0xB1E408) = settings->smg_camo_ding;
	*(float*)(address_offset + 0xB1E40C) = settings->smg_camo_regrowth_rate;
	*(BYTE*)(address_offset + 0xB1E2BA) = 2; // No dual Wielding

											 //Magnum
	*(float*)(address_offset + 0x96EC88) = settings->mag_error_angle_min;
	*(float*)(address_offset + 0x96EC8C) = settings->mag_error_angle_max;
	*(float*)(address_offset + 0x97A29C) = settings->mag_damage_lower_bound;
	*(float*)(address_offset + 0x97A2A0) = settings->mag_damage_upper_bound_min;
	*(float*)(address_offset + 0x97A2A4) = settings->mag_damage_upper_bound_max;
	*(float*)(address_offset + 0x96E928) = settings->melee_angles_y;
	*(float*)(address_offset + 0x96E92C) = settings->melee_angles_p;
	*(float*)(address_offset + 0x96E930) = settings->melee_depth;
	*(float*)(address_offset + 0x96E9F4) = settings->mag_camo_ding;
	*(float*)(address_offset + 0x96E9F8) = settings->mag_camo_regrowth_rate;
	*(BYTE*)(address_offset + 0x96E8A6) = 0; // No dual Wielding

											 //PP
	*(float*)(address_offset + 0xA0D27C) = settings->pp_initial_velocity;
	*(float*)(address_offset + 0xA0D280) = settings->pp_final_velocity;
	*(float*)(address_offset + 0xA0D284) = settings->pp_guided_angular_velocity_lower;
	*(float*)(address_offset + 0xA0D288) = settings->pp_guided_angular_velocity_upper;
	*(float*)(address_offset + 0xA02EE4) = settings->melee_angles_y;
	*(float*)(address_offset + 0xA02EE8) = settings->melee_angles_p;
	*(float*)(address_offset + 0xA02EEC) = settings->melee_depth;
	*(float*)(address_offset + 0xA033C4) = settings->pp_heat_per_charged_shot;
	*(float*)(address_offset + 0xA02FB0) = settings->pp_camo_ding;
	*(float*)(address_offset + 0xA02FB4) = settings->pp_camo_regrowth_rate;
	*(BYTE*)(address_offset + 0xA02E62) = 0; // No dual Wielding

											 //Carbine
	*(float*)(address_offset + 0xA7C428) = settings->car_error_angle;
	*(float*)(address_offset + 0xA7C42C) = settings->car_error_angle;
	*(float*)(address_offset + 0xA84DD4) = settings->hitscan_projectile_velocity;
	*(float*)(address_offset + 0xA84DD8) = settings->hitscan_projectile_velocity;
	*(float*)(address_offset + 0xA85260) = settings->car_damage; // Damage Lower Bound
	*(float*)(address_offset + 0xA85264) = settings->car_damage; // Damage Upper Bound Min
	*(float*)(address_offset + 0xA85268) = settings->car_damage; // Damage Upper Bound Max
	*(float*)(address_offset + 0xA7BFC8) = settings->melee_angles_y;
	*(float*)(address_offset + 0xA7BFCC) = settings->melee_angles_p;
	*(float*)(address_offset + 0xA7BFD0) = settings->melee_depth;
	*(float*)(address_offset + 0xA7C094) = settings->car_camo_ding;
	*(float*)(address_offset + 0xA7C098) = settings->car_camo_regrowth_rate;

	// Shotty (Hitfix?)
	*(float*)(address_offset + 0xAFE014) = settings->hitscan_projectile_velocity;
	*(float*)(address_offset + 0xAFE018) = settings->hitscan_projectile_velocity;
	*(int*)(address_offset + 0xAF1DEE) = settings->sg_projectiles_per_shot;
	*(float*)(address_offset + 0xAF1DF8) = settings->sg_error_angle_min;
	*(float*)(address_offset + 0xAF1DFC) = settings->sg_error_angle_max;
	*(float*)(address_offset + 0xAFE224) = settings->sg_damage_lower_bound;
	*(float*)(address_offset + 0xAFE228) = settings->sg_damage_upper_bound_min;
	*(float*)(address_offset + 0xAFE22C) = settings->sg_damage_upper_bound_max;
	*(float*)(address_offset + 0xAF1A78) = settings->melee_angles_y;
	*(float*)(address_offset + 0xAF1A7C) = settings->melee_angles_p;
	*(float*)(address_offset + 0xAF1A80) = settings->melee_depth;
	*(float*)(address_offset + 0xAF1B44) = settings->sg_camo_ding;
	*(float*)(address_offset + 0xAF1B48) = settings->sg_camo_regrowth_rate;

	//Overshield shader
	if (settings->player_turn_off_overshield_shader)
	{
		*(int64_t*)(address_offset + 0xE61B34) = -1;
		*(int64_t*)(address_offset + 0xE61B3C) = -1;
	}

	//Frag
	*(float*)(address_offset + 0xD82A28) = settings->f_arming_time;
	*(float*)(address_offset + 0xD83120) = settings->f_radius_to;
	*(BYTE*)(address_offset + 0xD82A1C) = 2; // When at rest
	*(BYTE*)(address_offset + 0xD83038 + 2) = 0; // Force Unattached Effects

												 //Plasma Grenade
	*(float*)(address_offset + 0xD834E8) = settings->pg_arming_time;

	//Player Spawns
	//Should be applied differently if it is a 2v2, 4v4, or FFA
	*(float*)(address_offset + 0xD80FFC) = settings->ps_max_random_spawn_bias;
	*(float*)(address_offset + 0xD81004) = 0.f; //Grenade Danger Weight

#pragma endregion

	//Spawn Protection
	if (settings->player_turn_off_spawn_protection)
	{
		VirtualProtect((LPVOID)(base_address + 0x55D01), 1, PAGE_EXECUTE_READWRITE, &dwBack);
		*(BYTE*)(base_address + 0x55D01) = 0;
	}

	//Disable Melee Lunge
	if (settings->player_turn_off_melee_lunge)
	{
		DisableMeleeLunge();
	}

	SetAllWeaponsToStartSpawnTimerOnPickup();
	DisableCamoRegenerationDefaults();

	isEnabled = true;
	detoursHavePreviouslyBeenApplied = true;

	LOG_TRACE_GAME("[H2MOD-H2F] : end Initialize()");
}

void Halo2Final::Dispose()
{
	isEnabled = false;

	EnableCamoRegenerationDefaults();
	DisableStaticWeaponSpawns();

	//Turn Spawn Protection back on
	if (settings->player_turn_off_spawn_protection)
	{
		VirtualProtect((LPVOID)(base_address + 0x55D01), 1, PAGE_EXECUTE_READWRITE, &dwBack);
		*(BYTE*)(base_address + 0x55D01) = 1;
	}

	//Enable Melee Lunges
	if (settings->player_turn_off_melee_lunge)
	{
		EnableMeleeLunge();
	}

	delete settings;
	settings = NULL;
}
