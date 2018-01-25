#pragma once
#include "..\stdafx.h"
namespace Blam
{
	namespace Enums
	{
		namespace Game
		{
			enum class CameraState : BOOL
			{
				Frozen = FALSE,
				Normal = TRUE
			};
			enum class GameState : BOOL
			{
				Running = FALSE,
				Paused = TRUE			    
			};
		}
	}
}