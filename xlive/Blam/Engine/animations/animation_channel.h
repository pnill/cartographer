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
CHECK_STRUCT_SIZE(c_animation_id, 4);

// TODO populate flags
enum e_animation_state_flag_bits : byte
{
	animation_state_none = 0,
	animation_state_paused = FLAG(1),
};

// TODO populate flags
enum e_animation_frame_event_types : WORD
{
	animation_frame_event_type_none = 0,
	animation_frame_event_type_autoplay = FLAG(0),
	animation_frame_event_type_looping = FLAG(1),
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
	e_animation_state_flag_bits animation_state_flags;
	e_animation_frame_event_types frame_event_types_1;
	e_animation_frame_event_types frame_event_types_2;
	e_animation_frame_event_types frame_event_types_3;
	float unk18;
	float unk1C;

public:
	c_animation_channel();
	~c_animation_channel();
	void reset();
	void initialize();
};
CHECK_STRUCT_SIZE(c_animation_channel, 0x20);