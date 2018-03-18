#pragma once
#include "stdafx.h"
namespace Blam
{
	namespace EngineDefinitions
	{
		namespace Game
		{			//Broken atm
			struct GameManager
			{
				BOOL MapReset;
				BOOL RevertCheckPoint;
				BOOL SaveCheckpoint;
				BOOL LoadMainmenu;
				BOOL unk_0;
				BOOL LoadingDone;
				BOOL unk_1;
				BOOL unk_2;
				BOOL UpdateProfile;
				BOOL unk_3;
				INT16 structure_bsp_index;			
				DWORD unk_4[8];
			};
		}
	}
}
