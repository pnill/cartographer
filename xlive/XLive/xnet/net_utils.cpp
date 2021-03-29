
#include "net_utils.h"

std::string IOCTLSocket_cmd_string(long cmd)
{
	switch (cmd)
	{
	case FIONBIO:
		return "FIONBIO";
	case FIONREAD:
		return "FIONREAD";

	default:
		std::stringstream strstream; 
		strstream << "UNKNOWN: " << std::hex << cmd;
		return strstream.str();
		break;
	}
}

bool Ipv4AddressIsReservedOrLocalhost(const IN_ADDR ipv4Addr)
{
	struct IPv4AddressMaskPair
	{
		IPv4AddressMaskPair(unsigned long _address, unsigned long _addressMask) {
			address.s_addr = _address;
			addressMask.s_addr = _addressMask;
		}

		IN_ADDR address;
		IN_ADDR addressMask;
	};

	const IPv4AddressMaskPair privateIPv4Entries[] = 
	{
		// reserved for private usage
		{ inet_addr("192.168.0.0"), inet_addr("255.255.0.0") },
		{ inet_addr("10.0.0.0"),    inet_addr("255.0.0.0")   },
		{ inet_addr("172.16.0.0"),  inet_addr("255.240.0.0") },
		
		// local host
		{ inet_addr("127.0.0.0"),  inet_addr("255.0.0.0") },
	};

	for (auto& entry : privateIPv4Entries)
	{
		//LOG_TRACE_NETWORK("{} - 0x{:X} == 0x{:X}, 0x{:X} - 0x{:X}", __FUNCTION__, (ipv4Addr.s_addr & entry.addressMask.s_addr), (entry.address.s_addr & entry.addressMask.s_addr), ipv4Addr.s_addr, entry.addressMask.s_addr);
		if ((ipv4Addr.s_addr & entry.addressMask.s_addr) == (entry.address.s_addr & entry.addressMask.s_addr))
			return true;
	}
	
	return false;
}
