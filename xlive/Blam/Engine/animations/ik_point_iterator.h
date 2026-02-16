#pragma once

/* constants */

enum
{
	k_max_ik_points = 8
};

/* classes */

class ik_point_iterator
{
public:
	ik_point_iterator(void);
	~ik_point_iterator(void) = default;
	void clear(void);

	string_id marker;
	string_id attach_to_marker;
	int16 weapon_ik_index;
	int16 pass;
	string_id mode_name;
	string_id weapon_class_name;
};
