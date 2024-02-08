#pragma once
#include "machine_id.h"
#include "Networking/Session/NetworkSession.h"

struct simulation_machine_update
{
	uint32 machine_valid_mask;
	s_machine_identifier identifiers[NETWORK_SESSION_PEERS_MAX];
};
