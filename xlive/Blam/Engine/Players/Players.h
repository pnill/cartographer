#pragma once
#include "..\stdafx.h"
#include "..\Blam\Enums\Enums.h"
#include "..\Blam\Shared\SharedDefinitions.h"


namespace Blam 
{
	namespace EngineDefinitions
	{
		namespace Players
		{
			//size:0x204
			struct GameStatePlayers
			{
				DWORD unk_0;
				DWORD GamertagID;
				char unk_block_0[0x20];//unknown,so skipping them
				Blam::SharedDefinitions::c_object_index ObjectDatum;
				Blam::SharedDefinitions::c_object_index DeadObjectDatum;//Holds the object datum for spectate period
				DWORD unk_1[4];//4*4 bytes related to action or something
				WCHAR PlayerName[32];				
				Blam::Enums::Player::PrimaryArmorColor PrimaryArmorColor;
				Blam::Enums::Player::PrimaryArmorColor SecondaryArmorColor;
				Blam::Enums::Player::PrimaryEmblemColor PrimaryEmblemColor;
				Blam::Enums::Player::SecondaryEmblemColor SecondaryEmblemColor;
				Blam::Enums::Player::Biped Biped;
				Blam::Enums::Player::EmblemForeground EmblemForeground;
				Blam::Enums::Player::EmblemBackground EmblemBackground;
				Blam::Enums::Player::EmblemToggle EmblemToggle;
				DWORD unk_3[13];//13*4 bytes still unknown
				Blam::Enums::Player::Team Team;
				Blam::Enums::Player::Handicap Handicap;
				INT16 unk_4;//2 Unused bytes
				INT32 unk_5;//4 unused bytes
				char unk_block_1[0x80];//20*4 bytes unknown,so skipping them
				INT32 unk_6;//4 unused bytes
				INT32 MillisecondsToSpawn;
				DWORD unk_7[2];//unused 8 bytes
				DWORD RespawnCountDown;
				char unk_block_2[0x28];
				FLOAT PlayerSpeed;
				char unk_block_3[0x80];			
				
			};
		}
	}
}
