#pragma once

namespace MapChecksumSync
{
	struct state
	{
		bool is_done = false;
		bool is_offical = true;
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

	enum startup_state {
		not_done,
		in_progress,
		failed,
		done
	};

	startup_state get_startup_info();
};