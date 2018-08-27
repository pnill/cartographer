#pragma once
#include <map>

namespace MapChecksumSync
{
	struct state
	{
		bool is_offical;
		std::map<std::string, std::string> map_checksum_names;
	};
	void StartupError(const std::string &error);
	void Calculate();
	void Init();
};