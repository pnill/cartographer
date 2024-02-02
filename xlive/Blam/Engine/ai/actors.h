#pragma once
#include "actor_firing_position.h"
#include "actor_moving.h"
#include "pathfinding_utilities.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/objects/objects.h"
#include "Blam/Engine/objects/object_placement.h"


/*
   TODO: Reverse engineer more of the actor struct and fill this data in appropriately.
   For now, we only know where the character datum which was used to create the actor is.
*/

// Unsure about the size for this struct...
struct actor_datum_struct_41C
{
	byte gap_0[52];
	int field_34;
	__int16 field_38;
	byte gap_34[42];
	bool unk_bool_480;
	byte gap_481[43];
};

// Unsure about the size for this struct...
struct actor_control_data
{
	int ignore_target_object_index;
	bool unk_bool_4E8;
	byte gap_4F0[3];
	c_ai_point3d ai_point_4EC;
	int field_4FC;
	float field_500;
	__int16 field_504;
	bool unk_bool_506;
	byte gap_507;
	__int16 field_508;
	byte gap_50A[2];
	bool unk_bool_50C;
	byte gap_50D[159];
	int field_5AC;
	__int16 field_5B0;
	bool unk_bool_5B2;
	byte pad_5B3;
	__int16 field_5B4;
	__int16 field_5B6;
	byte gap_5B8[24];
	bool unk_bool_5D0;
	byte gap_5D1;
	byte unk_bool_5D2;
	byte gap_5D3[77];
	__int16 suppressed_shooting_tick_count;
	byte gap_622[52];
	__int16 field_656;
	byte gap_658[44];
	__int16 field_684;
	__int16 tick_count_686;
	__int16 field_688;
	__int16 field_68A;
	DWORD field_68C;
	byte gap_690[16];
	__int16 field_6A0;
	byte gap_6A2[2];
	datum field_6A4;
	byte gap_6A8[8];
	__int16 field_6B0;
	byte gap_6B2[2];
	datum field_6B4;
	byte gap_6B8[26];
	real_vector3d desired_facing_vector;
	real_vector3d desired_aiming_vector;
	real_vector3d desired_looking_vector;
	byte gap_6F8[6];
	__int16 fire_state;
	byte gap_700[34];
	__int16 current_fire_target_type;
	datum current_fire_target_prop_index;
	byte gap_728[136];
	real_vector3d burst_aim_vector;
};

// Unsure about the size for this struct...
struct actor_meta
{
	BYTE gap_0[4];
	__int16 type;
	bool unk_bool_6;
	bool swarm;
	bool unk_bool_8;
	bool active;
	bool squadless;
	bool force_active;
	BYTE gap_B[4];
	DWORD flee_firing_position_index;
	int pathfinding_timeslice;
	datum unit_index;
	datum swarm_index;
	DWORD field_20;
	e_game_team unit_team;
	BYTE gap_1C[2];
	int field_28;
	__int16 tick_count_2C;
	BYTE gap_2E[2];
	datum squad_index;
	datum squad_to_migrate_to;
	BYTE gap_38[6];
	__int16 sbsp_index;
	bool unk_bool_34;
	BYTE gap_41;
	WORD field_42;
	bool unk_bool_44;
	BYTE gap_45;
	WORD field_46;
	BYTE gap_42[12];
	datum character_tag_datum;
	int field_58;
	datum tracks[8];
	datum clump_index;
};

// Unsure about the size for this struct...
struct actor_datum_struct_90
{
	WORD behavior_info_index;
	byte gap_0[62];
};

// Unsure about the size for this struct...
struct actor_target
{
	datum target_prop_index;
	byte gap_33C[8];
	datum retreat_target_prop_index;
};

// Unsure about the size for this struct...
struct actor_danger_zone
{
	__int16 danger_type;
	__int16 field_35A;
	bool unk_bool_35C;
	byte gap_35D;
	bool unk_bool_35E;
	byte gap_35F;
	datum object_index;
};

struct actor_datum
{
	actor_meta meta;
	BYTE gap_80[4];
	__int16 actor_status;
	__int16 field_86;
	BYTE gap_88[8];
	actor_datum_struct_90 array_90[2];
	BYTE gap_90[32];
	datum actor_datum;
	BYTE gap_134[92];
	__int16 gap_90_array_size;
	BYTE gap_192[150];
	bool blind;
	bool unk_bool_229;
	bool unk_bool_22A;
	bool unk_bool_22B;
	real_point3d position_22C;
	real_point3d current_position;
	BYTE gap_244[12];
	s_location location;
	BYTE gap_258[12];
	actor_input input;
	float body_current_vitality;
	float shield_current_vitality;
	float field_2D8;
	float field_2DC;
	bool unk_bool_2E0;
	BYTE gap_2E1[7];
	int field_2E8;
	__int16 field_2EC;
	__int16 field_2EE;
	bool unk_bool_2F0;
	byte gap_2F1;
	__int16 field_2F2;
	__int16 field_2F4;
	BYTE gap_2F6[10];
	int field_300;
	__int16 field_304;
	WORD tick_count_306;
	BYTE gap_308[32];
	__int16 field_328;
	BYTE gap_32A[2];
	datum field_32C;
	BYTE gap_330[7];
	actor_target target;
	int field_348;
	byte gap_34C[12];
	actor_danger_zone danger_zone;
	byte gap_364[4];
	datum field_368;
	byte gap_36C[72];
	datum field_3B4;
	byte gap_3B8[60];
	firing_position_ref firing_positions;
	actor_datum_struct_41C some_struct_41C;
	__int16 field_4AC;
	bool unk_bool_4AE;
	bool unk_bool_4AF;
	DWORD field_4B0;
	DWORD field_4B4;
	c_ai_point3d ai_point_4B8;
	int field_4C8;
	DWORD field_4CC;
	DWORD field_4D0;
	bool unk_bool_4D4;
	byte gap_4D5;
	byte gap_4D6[14];
	actor_control_data control;
	float damage_modifier;
	byte gap_7C0[32];
	datum field_7E0;
	int field_7E4;
	byte gap_7E8[24];
	string_id movement_mode;
	datum field_804;
	byte gap_808[16];
	int field_818;
	int field_81C;
	real_vector3d vector_820;
	int field_82C;
	int field_830;
	real_vector3d vector_834;
	real_vector3d vector_840;
	real_vector3d vector_84C;
	byte gap_858[16];
	datum cs_script_datum;
	BYTE gap_86C[44];
};
CHECK_STRUCT_SIZE(actor_datum, 0x898);

s_data_array* get_actor_table();
