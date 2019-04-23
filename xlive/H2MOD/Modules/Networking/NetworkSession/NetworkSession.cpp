#include "NetworkSession.h"
#include "H2MOD.h"

network_session* NetworkSession::getNetworkSession()
{
	network_session* netsession = reinterpret_cast<network_session*>(*(DWORD*)h2mod->GetAddress(0x51C474, 0x520B94));
	return netsession;
}

network_session* NetworkSession::getCurrentNetworkSession()
{
	network_session* netsession = reinterpret_cast<network_session*>(*(DWORD*)h2mod->GetAddress(0x420FE8, 0x520B94));
	return netsession;
}

bool NetworkSession::localPeerIsSessionHost()
{
	network_session_state state = getCurrentNetworkSession()->session_state;

	return state == _network_session_state_session_host
		|| state == _network_session_state_host_disband
		|| state == _network_session_state_host_handoff
		|| state == _network_session_state_host_reestablish;
}