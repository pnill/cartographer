#include "stdafx.h"
#include "upnp.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "Xlive/xnet/IpManagement/XnIp.h"

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>

/* constants */

enum
{
	UPNP_IGD_VALID_CONNECTED = 1,
	UPNP_PORT_MAPPING_SUCCESS = 0,

	UPNP_TTL = 2,
	UPNP_DEVICE_DISCOVERY_TIMEOUT = 1000,	// in miliseconds
};

static const WORD k_upnp_port_offsets[3] = { 0, 1, XNIP_QOS_PORT_OFFSET };

/* prototypes */

static void __cdecl xlive_upnp_forward_ports_proc(void* args);

static bool xlive_upnp_forward_port(UPNPDev* device, bool tcp, WORD externalport, WORD internalport, const char* rule_name);

/* globals */

static uintptr_t g_upnp_thread_handle;

/* public code */

void xlive_upnp_forward_ports(void)
{
	if (H2Config_upnp_enable)
	{
		g_upnp_thread_handle = _beginthread(&xlive_upnp_forward_ports_proc, 0, NULL);
	}
	else
	{
		LOG_INFO_NETWORK("UPNP disabled by config, skipping forwarding.");
	}
	return;
}

DWORD WINAPI XLiveGetUPnPState(DWORD a1)
{
	LOG_TRACE_XLIVE("XLiveGetUPnPState  (a1 = {:x})", a1);
	return 0;
}

/* private code */

static void __cdecl xlive_upnp_forward_ports_proc(void* args)
{
	int discover_error;
	UPNPDev* upnp_device = upnpDiscover(UPNP_DEVICE_DISCOVERY_TIMEOUT, NULL, NULL, UPNP_LOCAL_PORT_ANY, 0, UPNP_TTL, &discover_error);

	UPNPDev* seek_device = upnp_device;
	while (seek_device && seek_device->descURL)
	{
		addDebugText("found upnp_device: %s", seek_device->descURL);
		seek_device = seek_device->pNext;
	} 

	if (discover_error == UPNPDISCOVER_SUCCESS && upnp_device)
	{
		for (size_t i = 0; i < NUMBEROF(k_upnp_port_offsets); ++i)
		{
			const WORD port_offset = k_upnp_port_offsets[i];
			const bool is_qos_port = port_offset == XNIP_QOS_PORT_OFFSET;

			// Set rule name based on the port we're forwarding
			const char* rule_name = NULL;
			switch (port_offset)
			{
			case 0:
				rule_name = "halo2port";
				break;
			case 1:
				rule_name = "halo2port1";
				break;
			case XNIP_QOS_PORT_OFFSET:
				rule_name = "halo2portQoS";
				break;
			}


			const WORD port = H2Config_base_port + port_offset;
			const bool res = xlive_upnp_forward_port(upnp_device, is_qos_port, port, port, rule_name);
			
			// Stop forwarding if we didn't forward one of the ports
			if (!res)
			{
				break;
			}
		}
	}
	else
	{
		addDebugText("Failed to discover a UPNP compatible device.");
	}


	_endthread();
	return;
}

static bool xlive_upnp_forward_port(UPNPDev* device, bool tcp, WORD externalport, WORD internalport, const char* rule_name)
{
	bool continue_forwarding = false;

	struct UPNPUrls urls = {};
	struct IGDdatas data = {};
	char lanaddr[16];

	int result = UPNP_GetValidIGD(device, &urls, &data, lanaddr, sizeof(lanaddr), NULL, 0);
	if (result == UPNP_IGD_VALID_CONNECTED)
	{
		char iport_str[6];
		char eport_string[6];

		addDebugText("Attempting to open port %s, int %hu, ext %hu, tcp %s, device %s", rule_name, internalport, externalport, tcp, device->descURL);

		if (_itoa_s(internalport, iport_str, 10) == ERROR_SUCCESS && _itoa_s(externalport, eport_string, 10) == ERROR_SUCCESS)
		{
			const char* proto = tcp ? "TCP" : "UDP";
			result = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, eport_string, iport_str, lanaddr, rule_name, proto, NULL, NULL);
			if (result == UPNP_PORT_MAPPING_SUCCESS)
			{
				addDebugText("Successfully opened port %s %hu:%hu", rule_name, internalport, externalport);
				continue_forwarding = true;
			}
			else
			{
				addDebugText("Failed to open port %s %hu:%hu result code %d", rule_name, internalport, externalport, result);
			}
		}
	}
	else
	{
		addDebugText("UPNP IGD Error: %d on device %s", result, device->descURL);
	}

	FreeUPNPUrls(&urls);
	return continue_forwarding;
}
