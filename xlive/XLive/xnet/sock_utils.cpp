
#include "sock_utils.h"

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
