#pragma once
#include "tag_files/string_id.h"

#define k_max_ik_points 8

class ik_point_iterator
{
public:
	ik_point_iterator();
	~ik_point_iterator() = default;
	void clear(void);

	string_id marker;
	string_id attach_to_marker;
	int16 weapon_ik_index;
	int16 field_A;
	string_id field_C;
	string_id field_10;
};
