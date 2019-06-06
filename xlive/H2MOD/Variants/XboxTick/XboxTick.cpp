#include "Globals.h"

XboxTick::XboxTick()
{
	this->preSpawnPlayer = new XboxTickPreSpawnHandler();
}

void XboxTick::tickRate() {

	DWORD time_globals = *(DWORD*)(h2mod->GetAddress(0x4C06E4, 0x4CF0EC));
	*(BYTE*)((char*)time_globals + 0x2) = 30;
	*(float*)((char*)time_globals + 0x4) = 0.03333333507f;
	TRACE_GAME("[h2mod] Set tickrate to 30");
}

void XboxTickPreSpawnHandler::onClient() {
	XboxTick::tickRate();
}

void XboxTickPreSpawnHandler::onDedi() {
	XboxTick::tickRate();
}

void XboxTickPreSpawnHandler::onPeerHost() {
	XboxTick::tickRate();
}