#include "stdafx.h"

#include "VariantSystem.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"

void GameClientServerHandler::execute()
{
	if (!NetworkSession::LocalPeerIsSessionHost()) {
		this->onClient();
	}
	else {
		if (Memory::IsDedicatedServer()) {
			this->onDedi();
		}
		else {
			this->onPeerHost();
		}
	}
}
