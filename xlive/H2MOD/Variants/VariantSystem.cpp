#include "stdafx.h"
#include "Globals.h"
#include "H2MOD/Modules/Networking/NetworkSession\NetworkSession.h"

void GameClientServerHandler::execute()
{
	if (!NetworkSession::localPeerIsSessionHost()) {
		this->onClient();
	}
	else {
		if (h2mod->Server) {
			this->onDedi();
		}
		else {
			this->onPeerHost();
		}
	}
}
