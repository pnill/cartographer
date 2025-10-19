#include "stdafx.h"
#include "transport_security.h"

#include "transport_address.h"

#include "shell/shell_windows.h"

#include <Xlive/xnet/Sockets/XSocket.h>

/* globals */

static char g_transport_security_identifier_string[32];
static char g_transport_security_managed_session_id_string[64];
static char g_transport_security_unique_identifier_string[13];

/* public code */

bool transport_secure_address_compare(const s_transport_secure_address* a, const s_transport_secure_address* b)
{
	return csmemcmp(a, b, sizeof(s_transport_secure_address)) == 0;
}

char* transport_secure_identifier_get_string(const s_transport_secure_identifier* identifier)
{
	csprintf(
		g_transport_security_identifier_string,
		NUMBEROF(g_transport_security_identifier_string),
		"%02X%02X%02X%02X:%02X%02X%02X%02X",
		identifier->id.ab[0],
		identifier->id.ab[1],
		identifier->id.ab[2],
		identifier->id.ab[3],
		identifier->id.ab[4],
		identifier->id.ab[5],
		identifier->id.ab[6],
		identifier->id.ab[7]
	);
	return g_transport_security_identifier_string;
}

const char* managed_session_get_id_string(const s_transport_secure_identifier* session_id)
{
    ASSERT(session_id != NULL);

	return csprintf(
		g_transport_security_managed_session_id_string,
		NUMBEROF(g_transport_security_managed_session_id_string),
        "%02X %02X %02X %02X %02X %02X %02X %02X",
		session_id->id.ab[0],
		session_id->id.ab[1],
		session_id->id.ab[2],
		session_id->id.ab[3],
		session_id->id.ab[4],
		session_id->id.ab[5],
		session_id->id.ab[6],
		session_id->id.ab[7]
	);
}

const char* transport_unique_identifier_get_string(const s_transport_unique_identifier* unique_identifier)
{
	ASSERT(unique_identifier != NULL);
	
	return csprintf(
		g_transport_security_unique_identifier_string,
		NUMBEROF(g_transport_security_unique_identifier_string),
		"%02X%02X%02X%02X%02X%02X",
		unique_identifier->ab[0],
		unique_identifier->ab[1],
		unique_identifier->ab[2],
		unique_identifier->ab[3],
		unique_identifier->ab[4],
		unique_identifier->ab[5]);
}

char* transport_secure_address_to_string(const s_transport_secure_address* in_secure_address, char* string, int32 size, bool include_online_address, bool include_mac_address)
{
	if (*xlive_initilized_get())
	{
		s_transport_secure_address secure_address = *in_secure_address;

		const u_short port = XSocketHTONS(secure_address.addr.wPortOnline);
		transport_address address;
		address.address.raw_ipv4 = XSocketNTOHL(secure_address.addr.ina.S_un.S_addr);

		csprintf(
			string,
			size,
			"%hd.%hd.%hd.%hd", 
			address.address.ipv4[3],
			address.address.ipv4[2],
			address.address.ipv4[1], 
			address.address.ipv4[0]);

		if (include_mac_address)
		{
			csnappendf(
				string,
				size,
				"!MAC=%02X:%02X:%02X:%02X:%02X:%02X",
				secure_address.addr.abEnet[0],
				secure_address.addr.abEnet[1],
				secure_address.addr.abEnet[2],
				secure_address.addr.abEnet[3],
				secure_address.addr.abEnet[4],
				secure_address.addr.abEnet[5]);
		}

		if (include_online_address)
		{
			transport_address online_address;
			online_address.address.raw_ipv4 = XSocketNTOHL(secure_address.addr.inaOnline.S_un.S_addr);
			csnappendf(
				string,
				size,
				"!online=%hd.%hd.%hd.%hd:%hd",
				online_address.address.ipv4[3],
				online_address.address.ipv4[2],
				online_address.address.ipv4[1],
				online_address.address.ipv4[0],
				port);

			if (include_mac_address)
			{
				csnappendf(
					string,
					size,
					"!online-ident=%08X:%08X:%08X:%08X:%08X",
					secure_address.addr.abOnline[3] | ((secure_address.addr.abOnline[2] | ((secure_address.addr.abOnline[1] | (secure_address.addr.abOnline[0] << 8)) << 8)) << 8),
					secure_address.addr.abOnline[7] | ((secure_address.addr.abOnline[6] | ((secure_address.addr.abOnline[5] | (secure_address.addr.abOnline[4] << 8)) << 8)) << 8),
					secure_address.addr.abOnline[11]| ((secure_address.addr.abOnline[10]| ((secure_address.addr.abOnline[9] | (secure_address.addr.abOnline[8] << 8)) << 8)) << 8),
					secure_address.addr.abOnline[15]| ((secure_address.addr.abOnline[14]| ((secure_address.addr.abOnline[13] | (secure_address.addr.abOnline[12] << 8)) << 8)) << 8),
					secure_address.addr.abOnline[19]| ((secure_address.addr.abOnline[18]| ((secure_address.addr.abOnline[17] | (secure_address.addr.abOnline[16] << 8)) << 8)) << 8));
			}
		}
	}
	return string;
}

const char* transport_secure_address_get_mac_string(const s_transport_secure_address* address)
{
	s_transport_unique_identifier unique_id;
	csmemcpy(&unique_id, address->addr.abEnet, sizeof(unique_id));
	return transport_unique_identifier_get_string(&unique_id);
}

bool __cdecl transport_secure_identifier_retrieve(
	const transport_address* usable_address,
	e_transport_platform platform, 
	int32* remote_identifier,
	s_transport_secure_identifier* secure_identifier,
	s_transport_secure_key* secure_key,
	s_transport_secure_address* secure_address)
{
	return INVOKE(0x1B5F84, 0x1963BE, transport_secure_identifier_retrieve, usable_address, platform, remote_identifier, secure_identifier, secure_key, secure_address);
}

bool __cdecl transport_secure_address_get(s_transport_secure_address* secure_address, transport_address* address)
{
	return INVOKE(0x1B5836, 0x0, transport_secure_address_get, secure_address, address);
}
