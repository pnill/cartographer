#include "stdafx.h"

#include "H2MOD.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Tags\TagInterface.h"

#include "Util\Hooks\Hook.h"

void UIRankPatch() {

	if (h2mod->GetEngineType() != MainMenu || Memory::isDedicatedServer())
		return;

	//BYTE PlayerLevel = -1;								//eventually this will pull level from webserver
	//DWORD PlayerLevelPCR = -1;						//eventually this will pull level from webserver
	//WriteValue(H2BaseAddr + 0x1B2C2F, PlayerLevel);			//sets player level in pregame lobby
	//WriteValue(H2BaseAddr + 0xCC72, PlayerLevelPCR);		//sets player level in postgame carnage report
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
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPreGameLobby + (pSkinChunkIndex * pSkinChunkSize) + yDefOffset], yValuePGL);
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPreGameLobby + (pSkinChunkIndex * pSkinChunkSize) + bitmOffset], RankIconSM);
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPreGameLobby + (pSkinChunkIndex * pSkinChunkSize) + xDefOffset], xValuePGL);

	//Sets Postgame Carnage Report 1
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPostGameCarnage1 + (pSkinChunkIndex * pSkinChunkSize) + xDefOffset], xValuePCR);
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPostGameCarnage1 + (pSkinChunkIndex * pSkinChunkSize) + yDefOffset], yValuePCR);
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPostGameCarnage1 + (pSkinChunkIndex * pSkinChunkSize) + bitmOffset], RankIconSM);

	//Sets Postgame Carnage Report 2
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPostGameCarnage2 + (pSkinChunkIndex * pSkinChunkSize) + xDefOffset], xValuePCR);
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPostGameCarnage2 + (pSkinChunkIndex * pSkinChunkSize) + yDefOffset], yValuePCR);
	WriteValue((DWORD)&tags::get_tag_data()[TagOffsetPostGameCarnage2 + (pSkinChunkIndex * pSkinChunkSize) + bitmOffset], RankIconSM);

	//Sets Ranks 
	for (RankChunkIndex = 0; RankChunkIndex < 50; RankChunkIndex++)
	{
		//Setting Rank bitmap size
		WriteValue((DWORD)&tags::get_tag_data()[RankIconOff + (RankChunkIndex * RankChunkSize) + WidthDefOff], WidthValue);
		WriteValue((DWORD)&tags::get_tag_data()[RankIconOff + (RankChunkIndex * RankChunkSize) + HeightDefOff], HeightValue);

		//Setting Small Rank bitmap size
		WriteValue((DWORD)&tags::get_tag_data()[RankIconSMOff + (RankChunkIndex * RankChunkSize) + WidthDefOff], WidthValue);
		WriteValue((DWORD)&tags::get_tag_data()[RankIconSMOff + (RankChunkIndex * RankChunkSize) + HeightDefOff], HeightValue);
	}		

	addDebugText("Rank bitmaps patched.");
}