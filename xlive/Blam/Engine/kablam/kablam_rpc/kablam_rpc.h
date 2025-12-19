#pragma once

struct kablam_rpc_service_version_info
{
	unsigned char unknown_version[4];
	unsigned char build_string[32];
	int32 build_version;
};
