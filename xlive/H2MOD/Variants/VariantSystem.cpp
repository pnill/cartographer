#include "Globals.h"

void GameClientServerHandler::execute()
{
	if (!gameManager->isHost()) {
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
