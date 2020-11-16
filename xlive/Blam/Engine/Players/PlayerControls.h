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
			struct s_player_actions
			{
				int control_flag0;
				int control_flag1;
				float yaw;
				float pitch;
				float throttle_x;
				float throttle_y;
				float trigger;
				float secondary_trigger;
				int action_flags;
				__int16 weapon_set_identifier;
				char primary_weapon_index;
				char secondary_weapon_index;
				WORD grenade_index;
				__int16 zoom_level;
				__int16 interaction_type;
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
			//size  : 0xB0
			struct s_gamestate_player_controls
			{
				bool Initialized;
				BYTE Pad_1[3];
				int Flags;
				int Field_10;
				int Field_14;
				bool DisableCamera;
				BYTE Pad_2[7];
				datum ControllingDatum;
				int InputFlags_1;
				int InputFlags_2;
				BYTE Pad_3[sizeof(DWORD) * 3];
				s_player_actions Actions;
				BYTE Pad_4[12];
				int melee_target_exists;
				BYTE Pad_5[16];
			};
		}
	}
}
