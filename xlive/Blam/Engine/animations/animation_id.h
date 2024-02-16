#pragma once

class c_animation_id
{
	int16 graph_index;
	int16 animation_index;

public:
	c_animation_id(void);
	~c_animation_id(void) = default;
	void clear(void);
	int16 index(void) const;
	void set_index(int16 index);
	void set_subgraph(int16 subgraph);
	int16 subgraph(void) const;
	bool valid(void) const;

	bool operator==(c_animation_id* animation_id);
};
CHECK_STRUCT_SIZE(c_animation_id, 4);
