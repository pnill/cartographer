#pragma once
#include "..\Blam\Enums\Enums.h"
#include "Blam/Common/Common.h"
#include "Blam/Cache/DataTypes/DatumIndex.h"

typedef DWORD c_object_entity;
namespace Blam 
{
	namespace EngineDefinitions
	{
		namespace Players
		{

			struct __unaligned __declspec(align(1)) s_network_player_actions
			{
				float yaw;
				float pitch;
				float throttle_x;
				float throttle_y;
				WORD control_flag;
				WORD weapon_set_identifier;
				BYTE primary_weapon_identifier;
				BYTE secondary_weapon_identifier;
				WORD zoom_level;
				BYTE pad1[26];
				BYTE byte32;
				BYTE pad2[85];
			};
			CHECK_STRUCT_SIZE(s_network_player_actions, 0x88);

			struct __declspec(align(4)) s_player_actions
			{
				int control_flag0;
				int control_flag1;
				float yaw;
				float pitch;
				float throttle_x;
				float throttle_y;
				float trigger;
				float secondary_trigger;
				//int action_flags;
				__int16 weapon_set_identifier;
				char primary_weapon_index;
				char secondary_weapon_index;
				WORD grenade_index;
				__int16 zoom_level;
				//__int16 interaction_type;
				byte auto_look_centering;
				byte auto_look_centering_scale;
				__int16 field_2E;
				int interaction_object;
				int melee_target_exists;
				int target_object;
				int model_target;
				int target_player;
				BYTE Pad_1[12];
				WORD auto_aim_flags;
				float primary_autoaim_level;
				float secondary_autoaim_level;
				BYTE field_5C;
			};
			struct __declspec(align(4))  s_player_control
			{
				BYTE Initialized;
				BYTE Pad1[3];
				DWORD Flags;
				DWORD Field_10;
				DWORD Field_14;
				BYTE DisableCamera;
				BYTE Pad2[7];
				datum ControllingDatum;
				DWORD InputFlags_1;
				DWORD InputFlags_2;
				DWORD Pad3[3];
				s_player_actions Actions;
				//BYTE Pad4[32];
				BYTE Pad_4[12];
				int pickup_object_target;
				int melee_autoaim_target;
				BYTE Pad_5[16];
			};
		}
	}
}
