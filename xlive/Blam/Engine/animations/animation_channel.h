#pragma once
#include "Blam/Engine/tag_files/string_id.h"

class c_animation_id
{
	short graph_index;
	short animation_index;

public:
	c_animation_id();
	~c_animation_id() = default;
	void clear();
	short index();
	void set_index(short index);
	void set_subgraph(short subgraph);
	short subgraph();

	bool operator==(c_animation_id* animation_id);
};

class c_animation_channel
{
	c_animation_id animation_id_0;
	c_animation_id animation_id_1;
	string_id animation_string;
	char unkC;
	char unkD;
	short unkE;
	char unk10;
	char unk11;
	short flags;
	float unk14;
	float unk18;
	float unk1C;

public:
	c_animation_channel();
	~c_animation_channel();
	void reset();
	void initialize();
};
