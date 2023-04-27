#pragma once
#include "animation_channel.h"
#include "animation_interpolation.h"
#include "Blam/Cache/DataTypes/BlamDataTypes.h"

class c_animation_manager
{
public:
	c_animation_channel channel1;
	c_animation_channel channel2;
	c_animation_channel channel3;
	c_interpolator_control interpolator_control_0;
	c_interpolator_control interpolator_control_1;
	datum jmad_tag_datum;
	WORD animation_manager_flags;
	WORD unk6E;
	string_id animation_string_ids[4];
	DWORD unk80;

	c_animation_manager();
	~c_animation_manager() = default;
	void initialize();
	bool reset_graph(datum jmad_tag_index, datum hlmt_tag_index, bool some_bool);
};
CHECK_STRUCT_SIZE(c_animation_manager, 0x84);