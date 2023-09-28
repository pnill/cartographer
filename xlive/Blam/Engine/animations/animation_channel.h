#pragma once
#include "Blam/Engine/tag_files/string_id.h"

class c_animation_id
{
	int16 graph_index;
	int16 animation_index;

public:
	c_animation_id();
	~c_animation_id() = default;
	void clear();
	int16 index();
	void set_index(int16 index);
	void set_subgraph(int16 subgraph);
	int16 subgraph();

	bool operator==(c_animation_id* animation_id);
};
CHECK_STRUCT_SIZE(c_animation_id, 4);

// TODO populate flags
enum e_animation_state_flag_bits : int8
{
	animation_state_none = 0,
	animation_state_paused = FLAG(1),
};

// TODO populate flags
enum e_animation_frame_event_types : int16
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
	int8 animation_mode;
	int8 unkD;
	int16 unkE;
	int8 unk10;
	e_animation_state_flag_bits animation_state_flags;
	e_animation_frame_event_types frame_event_types_1;
	e_animation_frame_event_types frame_event_types_2;
	e_animation_frame_event_types frame_event_types_3;
	real32 unk18;
	real32 authored_time;

public:
	c_animation_channel();
	~c_animation_channel();
	void reset();
	void initialize();
};
CHECK_STRUCT_SIZE(c_animation_channel, 0x20);