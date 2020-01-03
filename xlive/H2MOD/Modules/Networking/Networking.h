#pragma once

#include "CustomPackets/CustomPackets.h"

class CustomNetwork {
public:
	void applyNetworkHooks();
private:
};

extern CustomNetwork *network;
