
#include "VariantSystem.h"

#include "H2MOD/Modules/Networking/Networking.h"

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
