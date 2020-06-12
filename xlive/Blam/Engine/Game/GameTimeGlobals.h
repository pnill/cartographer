#pragma once
#include "../Blam/Enums/Enums.h"

//size: 0x24
struct game_time_globals
{
	enum CameraState : BOOL
	{
		Frozen = FALSE,
		Normal = TRUE
	};

	enum GameState : BOOL
	{
		Running = FALSE,
		Paused = TRUE
	};

	CameraState CameraState;//0
	GameState GameState;//1
	WORD GameEngineClockSpeed;//2
	FLOAT GameEngineTickRate;//4
	DWORD GameTimeElapsed;//8
	FLOAT GameSpeedMultiplier;//0xC
	FLOAT unk_0;//0x10
	FLOAT unk_1;//0x14
	DWORD unk_2;//0x18
	FLOAT unk_3;//0x20
	DWORD unk_4;//0x24
};
