#pragma once
#include "animation_definitions.h"


#include "Blam/Engine/memory/static_arrays.h"
#include "Blam/Engine/tag_files/string_id.h"

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
	c_animation_channel(void);
	~c_animation_channel(void);

	void apply_node_orientations(real32 a2, real32 a3, int32 orientation_count, real_orientation* orientation_list, void* func, datum object_index);
	void apply_weighted_blend_screen_node_orientations(real32 yaw,
		real32 pitch,
		real32 ratio,
		const c_static_flags<255>* flags,
		int32 orientation_count,
		real_orientation* orientation_list,
		void* func,
		datum object_index);
	void apply_weighted_node_orientations(real32 a2, real32 ratio, real32 a3, int32 orientation_count, real_orientation* orientation_list, void* func, datum object_index);
	const c_model_animation* get_state_animation(void);
	void initialize(void);
	void reset(void);
	void set_frame_position(real32 position);
	bool valid(void) const;
};
CHECK_STRUCT_SIZE(c_animation_channel, 0x20);