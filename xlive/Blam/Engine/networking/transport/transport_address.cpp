#include "stdafx.h"
#include "transport_address.h"

#include "transport.h"
#include "transport_security.h"

/* globals */

static char g_transport_address_string[256];

/* public code */

const char* transport_address_get_string(const transport_address* address)
{
	return transport_address_to_string(address, 0, g_transport_address_string, NUMBEROF(g_transport_address_string), false, true);
}

char* transport_address_to_string(const transport_address* address, const s_transport_secure_address* secure_address, char* string, int16 maximum_string_length, bool include_port, bool secure)
{
	ASSERT(address);
	ASSERT(string);
	ASSERT(maximum_string_length > 0);

	csstrncpy(string, "", maximum_string_length);

	switch (address->address_type)
	{
	// IPV4
	case 4:
		csprintf(
			string,
			maximum_string_length,
			"%hd.%hd.%hd.%hd",
			address->address.ipv4[3],
			address->address.ipv4[2],
			address->address.ipv4[1],
			address->address.ipv4[0]);
		
		if (include_port)
		{
			csnappendf(string, maximum_string_length, ":%hd", address->port);
		}

		if (secure)
		{
			csnappendf(string, maximum_string_length, ":");

			s_transport_secure_address retrieved_address;
			char secure_address_string[256];

			if (secure_address)
			{
				transport_secure_address_to_string(secure_address, secure_address_string, NUMBEROF(secure_address_string), true, true);
				csstrncat(string, secure_address_string, maximum_string_length);
			}
			else if (transport_secure_identifier_retrieve(address, _transport_platform_windows, 0, 0, 0, &retrieved_address))
			{
				transport_secure_address_to_string(&retrieved_address, secure_address_string, NUMBEROF(secure_address_string), true, true);
				csstrncat(string, secure_address_string, maximum_string_length);
			}
			else
			{
				csnappendf(string, maximum_string_length, " (insecure)");
			}			
		}
		break;
	// IPV6
	case 16:
		csprintf(
			string,
			maximum_string_length,
			"%04X.%04X.%04X.%04X.%04X.%04X.%04X.%04X",
			address->address.ipv6[0],
			address->address.ipv6[1],
			address->address.ipv6[2],
			address->address.ipv6[3],
			address->address.ipv6[4],
			address->address.ipv6[5],
			address->address.ipv6[6],
			address->address.ipv6[7]);
		if (include_port)
		{
			csnappendf(string, maximum_string_length, ":%hd", address->port);
		}
		break;
	case NONE:
		csstrncpy(string, address->address.string, maximum_string_length);
		break;
	default:
		DISPLAY_ASSERT("invalid transport address");
	}

	return string;
}