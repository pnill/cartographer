#pragma once
#include "animation_channel.h"
#include "animation_interpolation.h"
#include "ik_point_iterator.h"

#include "math/matrix_math.h"

class c_animation_manager
{
public:
	c_animation_channel channels[3];
	c_interpolator_control interpolator_controls[2];
	datum jmad_tag_datum;
	uint16 animation_manager_flags;
	int16 unk6E;
	string_id animation_string_ids[4];
	int32 field_80;

	c_animation_manager(void);
	~c_animation_manager(void) = default;
	
	void initialize(void);

	bool find_next_weapon_ik_point(ik_point_iterator* iterator);
	const c_model_animation_graph* get_graph(void) const;
	int16 get_node_count(void) const;
	void get_node_orientations(real32 a2, real32 a3, const c_static_flags<255>* flags, int32 node_orientations_count, real_orientation* orientation_list, void* func, int32 a8);
	string_id get_state_name(void) const;
	void interpolate_node_orientations(int16 orientation_count, const c_static_flags<255>* flags, real_orientation* orientation_in, real_orientation* orientation_out);
	void node_matrices_from_orientations(real_matrix4x3* node_matrices, const real_orientation* orientation, const real_matrix4x3* base_matrix);
	void node_matrices_from_orientations_with_gun_hand_swap(real_matrix4x3* node_matrices,
		real_orientation* orientation_list,
		real_matrix4x3* matrix,
		int16 child_node_index,
		int16 alternate_parent_node_index);
	bool reset_graph(datum jmad_tag_index, datum hlmt_tag_index, bool some_bool);
	bool set_goal(string_id a2, string_id a3, string_id a4, string_id a5, uint32 a6, int32 a7);
	bool setup_animation_channel_by_index(c_animation_channel* animation_channel, c_animation_id animation_id, uint32 a4);
	bool valid(void) const;
	bool validate_graph(datum jmad_tag_index, datum hlmt_tag_index);
};
ASSERT_STRUCT_SIZE(c_animation_manager, 0x84);