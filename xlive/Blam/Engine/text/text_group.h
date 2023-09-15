#pragma once
#include "Blam/Engine/tag_files/string_id.h"

struct s_string_reference
{
	string_id string_id;
	int32 index;
};

// TODO add member functions
class c_language_pack
{
private:
	s_string_reference* m_string_references;
	char* m_string_data;						// this is utf8 string data
	int32 string_references_size;
	int32 m_string_data_size;
	int32 m_gap_10;
	int32 m_offset;
	bool m_loaded;
	byte m_pad[3];
};
