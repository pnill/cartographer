#pragma once

namespace H2X
{
	void ApplyMapLoadPatches(bool toggle);

	void ApplyPatches();

	struct h2x_mod_info
	{
		std::string tag_string;
		float h2x_rate_of_fire; // on 60 tick
		float original_rate_of_fire;
		int barrel_data_block_index;
		bool rounds_per_second_based;
	};
};