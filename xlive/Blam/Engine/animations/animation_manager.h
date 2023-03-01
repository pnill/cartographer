#pragma once
#include "animation_channel.h"
#include "interpolator_control.h"
#include "Blam/Cache/DataTypes/BlamDataTypes.h"

class c_animation_manager
{
	c_animation_channel channel1;
	c_animation_channel channel2;
	c_animation_channel channel3;
	c_interpolator_control interpolator_control_0;
	c_interpolator_control interpolator_control_1;
	DWORD animation_tag_index;
	WORD animation_manager_flags;
	WORD unk6E;
	int animation_string_ids[4];
	DWORD unk80;

public:
	c_animation_manager();
	~c_animation_manager();
	void initialize();
	bool reset_graph(datum jmad_tag_index, datum hlmt_tag_index, bool some_bool);
};
