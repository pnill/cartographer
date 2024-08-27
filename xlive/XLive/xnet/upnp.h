#pragma once
#include <miniupnpc/upnpdev.h>

namespace Utils
{
	enum class UPnPErrorType
	{
		None,
		DiscoveryError,
		IGDError,
		PortMapError
	};

	enum UPnPIGDErrors
	{
		UPNP_IGD_NONE = 0,
		UPNP_IGD_VALID_CONNECTED = 1,
		UPNP_IGD_VALID_NOT_CONNECTED = 2,
		UPNP_IGD_INVALID = 3
	};

	enum UPnPPortMapErrors
	{
		UPNPCOMMAND_SUCCESS = 0,
		UPNPCOMMAND_UNKNOWN_ERROR = -1,
		UPNPCOMMAND_INVALID_ARGS = -2,
		UPNPCOMMAND_HTTP_ERROR = -3,
		UPNPCOMMAND_INVALID_RESPONSE = -4,
	};

	struct UPnPResult
	{
		UPnPErrorType ErrorType;
		int ErrorCode;

		UPnPResult(UPnPErrorType type, int code)
		{
			ErrorType = type;
			ErrorCode = code;
		}
	};
}

class ModuleUPnP
{
public:
	ModuleUPnP();
	Utils::UPnPResult UPnPForwardPort(bool tcp, int externalport, int internalport, const std::string & ruleName);
private:
	int upnpDiscoverError;
	UPNPDev* upnpDevice = nullptr;
};

void ForwardPorts();
