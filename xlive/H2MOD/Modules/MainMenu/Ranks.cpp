#include "stdafx.h"
#include <ShellAPI.h>
#include <string>
#include <unordered_set>
#include <codecvt>

#include "H2MOD.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "Util\Hooks\Hook.h"

void UIRankPatch() {

	if (H2IsDediServer)
		return;

	DWORD& MapHeaderType = *(DWORD*)(H2BaseAddr + 0x47CD68 + 0x14C);

	DWORD dwBack;

	BYTE PlayerLevel = 0x00;
	WORD PlayerLevelPCR = 0x0000;
	WriteValue(H2BaseAddr + 0x1B2C2F, PlayerLevel); //Pregame Lobby
	//WriteValue(H2BaseAddr + 0xCC72, PlayerLevelPCR);//Postgame Carnage Report
	//WriteValue(H2BaseAddr + 0xCC74, 0x00); //Postgame Carnage Report enable

	if (MapHeaderType != 2) //If not on mainmenu, returns
		return;

	DWORD SharedMapMetaDataPointer = *(DWORD*)(H2BaseAddr + 0x47CD64);

	//Tag : ui\player_skins\player_skin_lobby.skin
	const DWORD TagOffsetPreGameLobby = 0x0049DE90;	//Property : Bitmap Buttons

	//Tag : ui\player_skins\pcr_1.skin
	const DWORD TagOffsetPostGameCarnage1 = 0x00485C70;	//Property : Bitmap Buttons

	//Tag : ui\player_skins\pcr_2.skin
	const DWORD TagOffsetPostGameCarnage2 = 0x00485F50;	//Property : Bitmap Buttons


	const BYTE pSkinChunkSize = 0x38;	//Property size per chunk
	const int pSkinChunkIndex = 2;		//Property chunk index number (2 represents rank field)
	const BYTE xDefOffset = 0x0C;		//Definition : Horizontal position on mainmenu
	const WORD xValuePGL = 0x01C4;		//Value : 452 (decimal)
	const WORD xValuePCR = 0x0131;		//Value : 305 (decimal)
	const BYTE yDefOffset = 0x0E;		//Definition : Vertical position on mainmenu
	const WORD yValuePGL = 0x001A;		//Value : 26 (decimal)
	const WORD yValuePCR = 0x0017;		//Value : 23 (decimal)
	const BYTE bitmOffset = 0x18;		//Definition : Bitm (bitmap loaded based on datum index)

	//Tag : ui\global_bitmaps\rank_icons.bitm
	const DWORD RankIcon = 0xE50802E6;			//Bitmap Datum Index
	const DWORD RankIconOff = 0x0047E9A8;		//Property : Bitmap

	//Tag : ui\global_bitmaps\rank_icons_sm.bitm
	const DWORD RankIconSM = 0xE4EC02CA;		//Bitmap Datum Index
	const DWORD RankIconSMOff = 0x00476D3C;		//Property : Bitmap


	const BYTE RankChunkSize = 0x74;	//Property size per chunk
	int RankChunkIndex;					//Property chunk index number (0-49)
	const BYTE WidthDefOff = 0x04;		//Definition : Width
	const WORD WidthValue = 0x0020;		//Value : 32 (decimal)
	const BYTE HeightDefOff = 0x06;		//Definition : Height
	const WORD HeightValue = 0x0020;	//Value : 32 (decimal)


	//Sets Pregame Lobby 
	WriteValue(SharedMapMetaDataPointer + TagOffsetPreGameLobby + (pSkinChunkIndex * pSkinChunkSize) + xDefOffset, xValuePGL);
	WriteValue(SharedMapMetaDataPointer + TagOffsetPreGameLobby + (pSkinChunkIndex * pSkinChunkSize) + yDefOffset, yValuePGL);
	WriteValue(SharedMapMetaDataPointer + TagOffsetPreGameLobby + (pSkinChunkIndex * pSkinChunkSize) + bitmOffset, RankIconSM);

	//Sets Postgame Carnage Report 1
	WriteValue(SharedMapMetaDataPointer + TagOffsetPostGameCarnage1 + (pSkinChunkIndex * pSkinChunkSize) + xDefOffset, xValuePCR);
	WriteValue(SharedMapMetaDataPointer + TagOffsetPostGameCarnage1 + (pSkinChunkIndex * pSkinChunkSize) + yDefOffset, yValuePCR);
	WriteValue(SharedMapMetaDataPointer + TagOffsetPostGameCarnage1 + (pSkinChunkIndex * pSkinChunkSize) + bitmOffset, RankIconSM);

	//Sets Postgame Carnage Report 2
	WriteValue(SharedMapMetaDataPointer + TagOffsetPostGameCarnage2 + (pSkinChunkIndex * pSkinChunkSize) + xDefOffset, xValuePCR);
	WriteValue(SharedMapMetaDataPointer + TagOffsetPostGameCarnage2 + (pSkinChunkIndex * pSkinChunkSize) + yDefOffset, yValuePCR);
	WriteValue(SharedMapMetaDataPointer + TagOffsetPostGameCarnage2 + (pSkinChunkIndex * pSkinChunkSize) + bitmOffset, RankIconSM);

	//Sets Ranks 
	for (RankChunkIndex = 0; RankChunkIndex < 49; RankChunkIndex++)
	{
		//Setting Rank bitmap size
		WriteValue(SharedMapMetaDataPointer + RankIconOff + (RankChunkIndex * RankChunkSize) + WidthDefOff, WidthValue);
		WriteValue(SharedMapMetaDataPointer + RankIconOff + (RankChunkIndex * RankChunkSize) + HeightDefOff, HeightValue);

		//Setting Small Rank bitmap size
		WriteValue(SharedMapMetaDataPointer + RankIconSMOff + (RankChunkIndex * RankChunkSize) + WidthDefOff, WidthValue);
		WriteValue(SharedMapMetaDataPointer + RankIconSMOff + (RankChunkIndex * RankChunkSize) + HeightDefOff, HeightValue);
	}

	addDebugText("Rank bitmaps patched.");
}