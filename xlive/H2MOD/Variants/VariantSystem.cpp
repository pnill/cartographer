#include "stdafx.h"
#include "Globals.h"

void GameClientServerHandler::execute()
{
	if (!NetworkSession::localPeerIsSessionHost()) {
		this->onClient();
	}
	else {
		if (Memory::isDedicatedServer()) {
			this->onDedi();
		}
		else {
			this->onPeerHost();
		}
	}
}
