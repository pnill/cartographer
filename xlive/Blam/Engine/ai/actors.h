#pragma once
#include "pathfinding_utilities.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Blam/Math/real_math.h"

/*
   TODO: Reverse engineer more of the actor struct and fill this data in appropriately.
   For now, we only know where the character datum which was used to create the actor is.
*/


// Unsure about the size for this struct...
struct s_actor_meta
{
	BYTE gap_0[4];
	__int16 type;
	byte gap_6;
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
	BYTE gap_35[19];
	datum character_tag_datum;
	int field_58;
	datum tracks[8];
	datum clump_index;
};

struct s_actor
{
	s_actor_meta meta;
	BYTE gap_80[4];
	__int16 field_84;
	__int16 field_86;
	BYTE gap_88[8];
	BYTE gap_90[160];
	datum actor_datum;
	BYTE gap_134[244];
	bool blind;
	bool unk_bool_229;
	bool unk_bool_22A;
	bool unk_bool_22B;
	real_point3d position_22C;
	real_point3d position_238;
	BYTE gap_244[12];
	s_location location;
	BYTE gap_258[13];
	bool underwater;
	bool unk_bool_266;
	bool unk_bool_267;
	bool unk_bool_268;
	bool unk_bool_269;
	BYTE gap_26A[2];
	datum root_object_datum;
	__int16 field_270;
	BYTE gap_272[2];
	int field_274;
	BYTE gap_270[192];
	datum target_prop_index;
	byte gap_33C[220];
	int firing_position_ref;
	byte gap_41C[100];
	bool unk_bool_480;
	byte gap_481[43];
	__int16 field_4AC;
	bool unk_bool_4AD;
	bool unk_bool_4AE;
	DWORD field_4B0;
	DWORD field_4B4;
	c_ai_point3d ai_point_4B8;
	int field_4C8;
	DWORD field_4CC;
	DWORD field_4D0;
	bool unk_bool_4D4;
	byte gap_4D5;
	byte gap_4D6[14];
	int field_4E4;
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
	__int16 field_686;
	__int16 field_688;
	__int16 field_68A;
	DWORD field_68C;
	byte gap_690[68];
	real_vector3d field_6D4;
	real_vector3d field_6E0;
	real_vector3d field_6EC;
	byte gap_6F8[194];
	float damage_modifier;
	byte gap_7C0[168];
	datum cs_script_datum;
	BYTE gap_86C[44];
};
CHECK_STRUCT_SIZE(s_actor, 0x898);

s_data_array* get_actor_table();
