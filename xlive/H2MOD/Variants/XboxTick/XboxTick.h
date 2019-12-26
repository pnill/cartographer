#pragma once
#include "Globals.h"

class XboxTickHandler : public GameClientServerHandler {
public:
private:
};

class XboxTickPreSpawnHandler : public XboxTickHandler {
	// Inherited via GameClientServerHandler
	virtual void onPeerHost() override;
	virtual void onDedi() override;
	virtual void onClient() override;
};

class XboxTick : public GameType<XboxTickHandler>
{
public:
	XboxTick();
	static void tickRate();
	void applyHooks();
};