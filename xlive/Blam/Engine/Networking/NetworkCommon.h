#pragma once



struct network_address
{
	union
	{
		int ipv4;
		char ipv6[16];
	} address;
	short port;
	short address_type;
};
CHECK_STRUCT_SIZE(network_address, 20);