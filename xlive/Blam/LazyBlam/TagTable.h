#pragma once

#include "H2MOD/Tags/TagInterface.h"

struct s_tag_table_data
{
	unsigned int tag_table_start_unpadded;
	unsigned int tag_table_start;
	unsigned int tag_table_end;
	unsigned int tag_count;
	unsigned int scenario_address;
	unsigned int tag_data_start;
	std::vector<std::tuple<tags::tag_instance, std::string, char*>> table;
};
