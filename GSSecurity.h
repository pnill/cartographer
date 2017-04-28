#pragma once
#include "stdafx.h"
#include "H2OnscreenDebugLog.h"
#include "GSUtils.h"

extern DWORD H2BaseAddr;
extern bool H2IsDediServer;

void GSSecSweetLeetHaxA2(int type);
void GSSecSweetLeetHaxA(int type) {
	addDebugText("Leet Hax A");
	//GSFIXME
	//GSSecSweetLeetHaxA2(type);
}

void GSSecStartLoop2();
void GSSecStartLoop() {
	//GSFIXME
	//GSSecStartLoop2();
}
