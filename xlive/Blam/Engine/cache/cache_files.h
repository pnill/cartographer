#pragma once
#include "Blam/Cache/DataTypes/BlamTag.h"

struct alignas(4) s_tag_data_iterator
{
	DWORD dword0;
	DWORD dword4;
	datum current_tag_datum;
	datum next_tag_datum;
	blam_tag tag_type;
	datum get_next_datum();
};
static_assert(sizeof(s_tag_data_iterator) == 20, "Size incorrect");

void cache_file_map_clear_all_failures(void);
