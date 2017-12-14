#pragma once

class Halo2FinalSettings
{
public:
	//Globals
	float hitscan_projectile_velocity = 5000.f;
	float magnetism_friction = 0.5f;
	float magnetism_adhesion = 0.7f;
	float melee_angles_y = 0.05f;
	float melee_angles_p = 0.02f;
	float melee_depth = 0.7f;
	bool player_turn_off_melee_lunge = false;
	bool player_turn_off_spawn_protection = true;
	bool player_turn_off_overshield_shader = true;

	//BR
	float br_autoaim_angle = 0.0095f;
	float br_autoaim_range = 30.f;
	float br_fire_recovery_time = 0.21f;
	float br_camo_ding = 0.1f;
	float br_camo_regrowth_rate = 0.25f;
	bool br_turn_off_crosshair_dot = false;
	bool br_turn_off_contrail = false;

	//Sniper
	float snipe_autoaim_angle = 0.f;
	float snipe_camo_ding = 1.f;
	float snipe_camo_regrowth_rate = 0.25f;

	//Needler
	float n_rounds_per_second_min = 1.f;
	float n_rounds_per_second_max = 14.5f;
	float n_initial_velocity = 16.f;
	float n_final_velocity = 16.f;
	float n_camo_ding = 0.1f;
	float n_camo_regrowth_rate = 0.25f;

	//Plasma Rifle
	float pr_stun = 0.3f;
	float pr_max_stun = 0.3f;
	float pr_stun_time = 0.2f;
	float pr_initial_velocity = 18.f;
	float pr_final_velocity = 18.f;
	float pr_max_error_angle = 0.05f;
	float pr_camo_ding = 0.1f;
	float pr_camo_regrowth_rate = 0.25f;

	//Rocket
	float r_initial_velocity = 10.f;
	float r_damage_upper_bound_min = 600.f;
	float r_damage_upper_bound_max = 600.f;
	float r_camo_ding = 1.f;
	float r_camo_regrowth_rate = 0.25f;

	//Smg
	float smg_rounds_per_second_min = 15.f;
	float smg_rounds_per_second_max = 15.f;
	float smg_minimum_error = 0.017f;
	float smg_damage_min = 4.75f;
	float smg_damage_max = 4.75f;
	float smg_camo_ding = 0.1f;
	float smg_camo_regrowth_rate = 0.25f;

	//Magnum
	float mag_fire_recovery_time = 0.1f;
	float mag_error_angle_min = 0.00436f;
	float mag_error_angle_max = 0.0262f;
	float mag_damage_lower_bound = 8.f;
	float mag_damage_upper_bound_min = 8.f;
	float mag_damage_upper_bound_max = 8.f;
	float mag_camo_ding = 0.1f;
	float mag_camo_regrowth_rate = 0.25f;

	//Plasma Pistol
	float pp_initial_velocity = 5.f;
	float pp_final_velocity = 5.f;
	float pp_guided_angular_velocity_lower = 1.39f;
	float pp_guided_angular_velocity_upper = 1.39f;
	float pp_heat_per_charged_shot = 0.25f;
	float pp_camo_ding = 0.1f;
	float pp_camo_regrowth_rate = 0.25f;

	//Carbine
	float car_autoaim_angle = 0.0095f;
	float car_autoaim_range = 30.f;
	float car_fire_recovery_time = 0.14f;
	float car_error_angle = 0.f;
	float car_damage = 11.f;
	float car_camo_ding = 0.2;
	float car_camo_regrowth_rate = 0.25f;

	//Shotty
	int sg_projectiles_per_shot = 15;
	float sg_error_angle_min = 0.35f;
	float sg_error_angle_max = 0.35f;
	float sg_damage_lower_bound = 16.5f;
	float sg_damage_upper_bound_min = 16.5f;
	float sg_damage_upper_bound_max = 16.5f;
	float sg_camo_ding = 1.f;
	float sg_camo_regrowth_rate = 0.25f;

	//Frag
	float f_arming_time = 2.f;
	float f_radius_to = 2.5f;

	//Plasma Grenade
	float pg_arming_time = 2.f;

	//Player Spawns
	float ps_max_random_spawn_bias = 0.005f;
};

class Halo2Final
{
public:
	void Initialize(bool isHost);
	void Dispose();
private:
	Halo2FinalSettings *settings;
};
