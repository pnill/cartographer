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
	enum error_id
	{
		official_needed,
		unofficial_needed,
		diff_checksums,
		internal
	};
	void RuntimeError(error_id type);

	void SendState();
	void HandlePacket(const H2ModPacket &packet);

	void Calculate();
	void Init();

	bool startup_failed();
};