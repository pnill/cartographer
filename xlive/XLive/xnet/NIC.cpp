#include "stdafx.h"

#include "NIC.h"

// TODO:
IP_ADAPTER_ADDRESSES* CNic::GetNetworkAdapterWithGateway()
{
	return nullptr;

#define MAX_ITERATIONS 3

	IP_ADAPTER_ADDRESSES* result = nullptr;
	DWORD dwRetVal = 0;
	// Set the flags to pass to GetAdaptersAddresses.
	ULONG flags = 0;
	ULONG family = AF_INET;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;

	// get the function to tell us how much memory we need
	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	ULONG Iterations = 0;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;

	do {
		pAddresses = (IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, (outBufLen));
		if (pAddresses == NULL) {
			LOG_TRACE_NETWORK("{} memory allocation failed for IP_ADAPTER_ADDRESSES struct.", __FUNCTION__);
			break;
		}

		dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

		if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
			HeapFree(GetProcessHeap(), 0, pAddresses);
			pAddresses = NULL;
		}
		else {
			break;
		}

		Iterations++;
		// 3 attempts max.
	} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_ITERATIONS));

	if (dwRetVal == NO_ERROR) {
		for (auto adapters = pAddresses; adapters != nullptr; adapters = adapters->Next) {

			// check if this adapter is ready to send packets
			if (adapters->OperStatus == IfOperStatusUp) {
				for (auto address = adapters->FirstUnicastAddress; address != nullptr; address = address->Next)
				{
					if (address->Address.lpSockaddr->sa_family == AF_INET)
					{
						SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);

						LOG_TRACE_NETWORK("{} - found local ip address: {}", __FUNCTION__, inet_ntoa(ipv4->sin_addr));
					}
				}
			}
		}
	}
	

	if (pAddresses) {
		HeapFree(GetProcessHeap(), 0, pAddresses);
	}

#undef MAX_ITERATIONS
}

IN_ADDR CNic::GetBestInterfaceRouteAddressFromIp(const char* ipToTest)
{
	DWORD dwIfIndex;
	IN_ADDR address;
	address.S_un.S_addr = 0;

	if (GetBestInterface(inet_addr(ipToTest), &dwIfIndex) == NO_ERROR)
	{
		auto ipAddrTable = (MIB_IPADDRTABLE *)HeapAlloc(GetProcessHeap(), 0, sizeof(MIB_IPADDRTABLE));

		DWORD size = 0;
		if (GetIpAddrTable(ipAddrTable, &size, 0) == ERROR_INSUFFICIENT_BUFFER)
		{
			HeapFree(GetProcessHeap(), 0, ipAddrTable);
			ipAddrTable = (MIB_IPADDRTABLE *)HeapAlloc(GetProcessHeap(), 0, size);
		}

		if (ipAddrTable == 0
			|| GetIpAddrTable(ipAddrTable, &size, 0) != NO_ERROR)
		{
			HeapFree(GetProcessHeap(), 0, ipAddrTable);
			return address;
		}

		for (DWORD i = 0; i < ipAddrTable->dwNumEntries; i++)
		{
			if (dwIfIndex == ipAddrTable->table[i].dwIndex)
			{
				address.S_un.S_addr = (ULONG)ipAddrTable->table[i].dwAddr;
				LOG_TRACE_NETWORK("{} - found interface with IPv4 address: {} to route from: {} (0 = INADDR_ANY)", __FUNCTION__, inet_ntoa(address), ipToTest);

				HeapFree(GetProcessHeap(), 0, ipAddrTable);
				return address;
			}
		}

		HeapFree(GetProcessHeap(), 0, ipAddrTable);
		return address;
	}

	return address;
}