#include "Globals.h"

XboxTick::XboxTick()
{
	this->preSpawnPlayer = new XboxTickPreSpawnHandler();
}

void XboxTick::tickRate() {
	*(BYTE*)(0x300029C8 + 0x2) = 30;
	*(float*)(0x300029C8 + 0x4) = 0.03333333507f;
	TRACE_GAME("[h2mod] Tried to set values");
}

void XboxTickPreSpawnHandler::onClient() {
	XboxTick::tickRate();
}

void XboxTickPreSpawnHandler::onDedi() {
	XboxTick::tickRate();
}

void XboxTickPreSpawnHandler::onPeerHost() {
	XboxTick::tickRate();
