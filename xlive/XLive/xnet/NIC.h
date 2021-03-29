#pragma once

#include <iphlpapi.h>

class CNic
{
public:
	static IP_ADAPTER_ADDRESSES* GetNetworkAdapterWithGateway();
	static IN_ADDR GetBestInterfaceRouteAddressFromIp(const char* ipToTest);
};