#pragma once
#include <map>
#include <h2mod.pb.h>

namespace MapChecksumSync
{
	struct state
	{
		bool is_offical;
		std::map<std::string, std::string> map_checksum_list;
	};
	void StartupError(const std::string &error);
	void RuntimeError(const std::string &message);
	void SendState();
	void HandlePacket(const H2ModPacket &packet);
	void Calculate();
	void Init();
};