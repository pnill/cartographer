#include "stdafx.h"
#include "first_person_weapons.h"

#include "animations/animation_definitions.h"
#include "cache/cache_files.h"
#include "game/cheats.h"
#include "game/game.h"
#include "game/game_globals.h"
#include "items/weapons.h"
#include "items/weapon_definitions.h"
#include "main/interpolator.h"
#include "math/matrix_math.h"
#include "models/models.h"
#include "models/model_definitions.h"
#include "models/render_model_definitions.h"
#include "render/render_cameras.h"
#include "render/render.h"
#include "saved_games/game_variant.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"
#include "units/units.h"

/* constants */

enum
{
	k_fp_needler_animation_max_rounds_loaded = 44
};

/* prototypes */

static first_person_weapon* first_person_weapon_get_global(void);

static first_person_weapon* first_person_weapons_get(uint32 user_index);

static first_person_weapon_data* first_person_weapon_data_get(uint32 weapon_slot, first_person_weapon* first_person_data);

static int32 first_person_weapon_slot_by_weapon_datum_index(int32 user_index, datum weapon_index);

static s_first_person_orientations* first_person_orientations_get_global(void);

static s_first_person_orientations* first_person_orientations_get(uint32 user_index, uint32 weapon_slot);

static s_first_person_model_data* first_person_model_data_get_global(void);

static void first_person_apply_interpolation_patches(void);

static void __cdecl first_person_weapon_update(int32 user_index, int32 weapon_slot);

static void __cdecl first_person_weapon_build_node_matrices(int32 user_index, int32 weapon_slot);

static void first_person_weapon_build_model_nodes_from_remapping_table(datum render_model_tag_index,
	int32 max_node_count,
	real_matrix4x3* camera_matrix,
	real_matrix4x3* fp_model_nodes,
	real_matrix4x3* node_matrices,
	int32* node_remapping_table);

static void first_person_weapon_build_model_nodes(int32 node_matrices_count,
	datum unit_index,
	datum render_model_tag_index,
	uint32 fp_flags,
	real_matrix4x3* camera_matrix,
	real_matrix4x3* node_matrices,
	int32* node_remapping_table,
	s_first_person_model_data* fp_model_data);

static void first_person_weapon_apply_ik(int32 user_index, s_first_person_model_data* model_data_1, s_first_person_model_data* model_data_2);

// Grab the weapon offset from the cartographer config from the current weapon index
// If the weapon doesn't exist then use the default weapon offset from the weapon tag
static const real_point3d* first_person_weapons_get_weapon_offset(datum user_index, struct weapon_definition const* weapon_definition, weapon_datum const* weapon);

/* globals */

static bool g_show_first_person = true;

/* public code */

void first_person_weapons_apply_patches(void)
{
	first_person_apply_interpolation_patches();
	return;
}

s_first_person_model_data* first_person_model_data_get(uint32 user_index)
{
	return &first_person_model_data_get_global()[user_index];
}

void first_person_weapons_toggle(bool state)
{
	g_show_first_person = state;
	return;
}

void __cdecl first_person_weapons_update(void)
{
	INVOKE(0x22B371, 0x20FA21, first_person_weapons_update);
	return;
}

int32 __cdecl first_person_weapon_build_models(
	int32 user_index,
	datum unit_index,
	int32 maximum_model_count,
	s_first_person_model_data* fp_model_data)
{
	int32 model_count = 0;
	
	// Only show the first person model if the following are true:
	// 1. global bool to show first person is enabled
	// 2. Blind skull isn't enabled
	// 3. User index is not NONE
	if (g_show_first_person
		&& !ice_cream_flavor_available(_skull_type_blind) 
		&& user_index != NONE)
	{
		bool weapon_flag_0 = false;
		bool weapon_flag_1 = false;

		first_person_weapon* first_person_data = first_person_weapons_get(user_index);
		for (uint32 weapon_index = 0; weapon_index < k_first_person_max_weapons; weapon_index++)
		{
			first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_index, first_person_data);
			weapon_flag_0 |= TEST_BIT(weapon_data->flags, 0);
			weapon_flag_1 |= weapon_data->weapon_index != NONE;
		}
		if (weapon_flag_0 && weapon_flag_1)
		{
			if (first_person_data->unit_index == unit_index && first_person_data->unit_index != NONE)
			{
				s_game_globals* globals = scenario_get_game_globals();
				if (IN_RANGE(first_person_data->character_type, _character_type_masterchief, globals->player_representation.count - 1))
				{
					s_game_globals_player_representation const* player_rep = TAG_BLOCK_GET_ELEMENT(&globals->player_representation, first_person_data->character_type, s_game_globals_player_representation);
					const datum fp_hands_index = player_rep->first_person_hands.index;
					const datum fp_body_index = player_rep->first_person_body.index;
					datum unit_model_index = NONE;
					int32 node_count = 0;
					const real_matrix4x3* node_matrices = NULL;
					const object_datum* object = object_get(unit_index);
					const object_definition* object_def = object_definition_get(object->definition_index);

					const datum object_model_index = object_def->object.model.index;
					if (object_model_index != NONE)
					{
						const s_model_definition* model = model_definition_get(object_model_index);
						if (model->animation_graph.index != NONE)
						{
							unit_model_index = model->render_model.index;
							if (!halo_interpolator_interpolate_object_node_matrices(unit_index, &node_matrices, &node_count))
							{
								node_matrices = object_get_node_matrices(unit_index, &node_count);
							}
						}
					}
					real_matrix4x3 camera_matrix;
					render_camera* global_camera = &render_get()->camera;
					matrix4x3_from_point_and_vectors(&camera_matrix, &global_camera->point, &global_camera->forward, &global_camera->up);
					if (TEST_BIT(first_person_data->flags, _first_person_attachment_suspended_bit))
					{
						// Get the interpolated matrix and use it if we can interpolate
						real_matrix4x3 interpolated_adjustment_matrix;
						bool interpolated_matrix = halo_interpolator_get_interpolated_matrix_from_user_index(user_index, 1, &interpolated_adjustment_matrix);
						real_matrix4x3* adjustment_matrix = (interpolated_matrix ? &interpolated_adjustment_matrix : &first_person_data->adjustment_matrix);

						real_matrix4x3 inverse_adjustment_matrix;
						matrix4x3_inverse(adjustment_matrix, &inverse_adjustment_matrix);
						matrix4x3_multiply(&camera_matrix, &inverse_adjustment_matrix, &camera_matrix);
					}
					first_person_data->estimated_root_matrix = camera_matrix;

					for (uint32 weapon_slot = 0; weapon_slot < k_first_person_max_weapons; weapon_slot++)
					{
						first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_slot, first_person_data);
						if (TEST_BIT(weapon_data->flags, 0) 
							&& weapon_data->weapon_index != NONE 
							&& maximum_model_count > 1 
							&& TEST_BIT(weapon_data->flags, 2)
							&& fp_hands_index != NONE)
						{
							weapon_datum const* weapon = weapon_get(weapon_data->weapon_index);
							weapon_definition const* weapon_def = weapon_definition_get(weapon->definition_index);
							weapon_first_person_interface_definition const* interface_def = first_person_interface_definition_get(
								weapon_def, 
								first_person_data->character_type
							);
							datum weapon_animations_index = (interface_def ? interface_def->animations.index : NONE);

							real_matrix4x3* model_nodes;
							bool interpolated = halo_interpolator_interpolate_weapon(user_index, weapon_animations_index, weapon_slot, &model_nodes, &weapon_data->node_matrices_count);
							if (!interpolated)
							{
								model_nodes = weapon_data->nodes;
							}

							first_person_weapon_build_model_nodes(
								weapon_data->node_matrices_count,
								unit_index,
								fp_hands_index,
								1,
								&camera_matrix,
								model_nodes,
								weapon_data->hands_node_remapping_table,
								fp_model_data);
						}
					}

					model_count++;
					s_first_person_model_data* current_fp_model_data = &fp_model_data[model_count];

					for (uint32 weapon_slot = 0; weapon_slot < k_first_person_max_weapons; weapon_slot++)
					{
						first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_slot, first_person_data);
						if (TEST_BIT(weapon_data->flags, 0) && weapon_data->weapon_index != NONE)
						{
							weapon_datum const* weapon = weapon_get(weapon_data->weapon_index);
							weapon_definition const* weapon_def = weapon_definition_get(weapon->definition_index);
							weapon_first_person_interface_definition const* interface_def = first_person_interface_definition_get(
								weapon_def,
								first_person_data->character_type
							);

							datum weapon_model_index;
							datum weapon_animations_index;

							if (interface_def)
							{
								weapon_model_index = interface_def->model.index;
								weapon_animations_index = interface_def->animations.index;
							}
							else
							{
								weapon_model_index = NONE;
								weapon_animations_index = NONE;
							}
							
							real_matrix4x3* model_nodes = NULL;
							bool interpolated = halo_interpolator_interpolate_weapon(user_index, weapon_animations_index, weapon_slot, &model_nodes, &weapon_data->node_matrices_count);
							if (model_count < maximum_model_count && TEST_BIT(weapon_data->flags, _weapon_node_table_valid_bit) && weapon_model_index != NONE)
							{
								if (!interpolated)
								{
									model_nodes = weapon_data->nodes;
								}

								first_person_weapon_build_model_nodes(
									NONE,
									weapon_data->weapon_index,
									weapon_model_index,
									1,
									&camera_matrix,
									model_nodes,
									weapon_data->weapon_node_remapping_table,
									current_fp_model_data);
								++model_count;
								current_fp_model_data = &fp_model_data[model_count];
							}
						}
					}

					if (model_count < maximum_model_count && fp_body_index != NONE && unit_model_index != NONE && node_matrices)
					{
						render_model_definition const* body_render_model = render_model_definition_get(fp_body_index);
						render_model_definition const* unit_render_model = render_model_definition_get(unit_model_index);
						if (body_render_model->nodes.count == unit_render_model->nodes.count
							&& body_render_model->node_list_checksum == unit_render_model->node_list_checksum)
						{
							const real32 angle = body_render_model->dont_draw_over_camera_cosine_angle;
							if (angle == 0.f || angle > global_camera->forward.k)
							{
								int32 node_count_copied = node_count;
								s_first_person_model_data* current_data = &fp_model_data[model_count];
								current_data->object_index = unit_index;
								current_data->render_model_index = fp_body_index;
								current_data->flags = 0;
								if (node_count > MAXIMUM_NODES_PER_FIRST_PERSON_MODEL)
								{
									node_count_copied = MAXIMUM_NODES_PER_FIRST_PERSON_MODEL;
								}

								csmemcpy(current_data->nodes, node_matrices, sizeof(real_matrix4x3) * node_count_copied);
								++model_count;
							}
						}
					}
					first_person_weapon_apply_ik(user_index, fp_model_data, &fp_model_data[1]);
				}
			}
		}
	}

	ASSERT(model_count <= maximum_model_count);
	return model_count;
}

void first_person_weapon_apply_camera_effect(int32 user_index, real_matrix4x3* effect_matrix)
{
	first_person_weapon* fp = first_person_weapons_get(user_index);
	if (fp->adjustment_matrix_index != NONE && TEST_BIT(fp->flags, 2))
	{
		real_matrix4x3 interpolated_adjustment_matrix;
		bool can_interpolate = halo_interpolator_get_interpolated_matrix_from_user_index(user_index, 1, &interpolated_adjustment_matrix);

		real_matrix4x3* matrix_to_multiply = (can_interpolate ? &interpolated_adjustment_matrix : &fp->adjustment_matrix);
		matrix4x3_multiply(effect_matrix, matrix_to_multiply, effect_matrix);
	}

	return;
}

void __cdecl first_person_weapon_get_worldspace_node_matrix(int32 user_index, datum weapon_index, int16 node_index, real_matrix4x3* out_matrix)
{
	INVOKE(0x228CC6, 0, first_person_weapon_get_worldspace_node_matrix, user_index, weapon_index, node_index, out_matrix);
	return;
}

void __cdecl first_person_weapon_get_worldspace_node_matrix_interpolated(int32 user_index, datum weapon_index, int16 node_index, real_matrix4x3* out_matrix)
{
	real_matrix4x3 result = *first_person_weapon_get_relative_node_matrix_interpolated(user_index, weapon_index, node_index);

	first_person_weapon* first_person_data = first_person_weapons_get(user_index);
	int32 weapon_slot = first_person_weapon_slot_by_weapon_datum_index(user_index, weapon_index);
	if (weapon_slot != NONE)
	{
		first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_slot, first_person_data);
		if (TEST_BIT(weapon_data->flags, 0)
			&& weapon_data->weapon_index != NONE)
		{
			matrix4x3_multiply(&first_person_data->estimated_root_matrix, &result, out_matrix);
		}
	}
	return;
}

real_matrix4x3* __cdecl first_person_weapon_get_relative_node_matrix(int32 user_index, datum weapon_index, int16 node_index)
{
	return INVOKE(0x228C6F, 0, first_person_weapon_get_relative_node_matrix, user_index, weapon_index, node_index);
}

real_matrix4x3* first_person_weapon_get_relative_node_matrix_interpolated(int32 user_index, datum weapon_index, int16 node_index)
{
	real_matrix4x3* result = NULL;
	first_person_weapon* first_person_data = first_person_weapons_get(user_index);
	const int32 weapon_slot = first_person_weapon_slot_by_weapon_datum_index(user_index, weapon_index);
	if (weapon_slot != NONE)
	{
		first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_slot, first_person_data);
		if (TEST_BIT(weapon_data->flags, 0)
			&& weapon_data->weapon_index != NONE)
		{
			weapon_datum const* weapon = weapon_get(weapon_data->weapon_index);
			weapon_definition const* weapon_def = weapon_definition_get(weapon->definition_index);
			weapon_first_person_interface_definition const* interface_def = first_person_interface_definition_get(
				weapon_def,
				first_person_data->character_type
			);
			datum weapon_animations_index = (interface_def ? interface_def->animations.index : NONE);

			// ### FIXME either make use across the entire code of the intermediate buffer
			// or remove it entirely

			result = &g_frame_data_intermediate->weapon_data[user_index][weapon_slot].nodes[node_index];
			if (!halo_interpolator_interpolate_weapon_node(user_index, weapon_animations_index, node_index, weapon_slot, result))
			{
				result = first_person_weapon_get_relative_node_matrix(user_index, weapon_index, node_index);
			}
		}
		else
		{
			result = first_person_weapon_get_relative_node_matrix(user_index, weapon_index, node_index);
		}
	}
	
	return result;
}

/* private code */

static void first_person_apply_interpolation_patches(void)
{
	PatchCall(Memory::GetAddress(0x22B2D4), first_person_weapon_build_node_matrices);
	PatchCall(Memory::GetAddress(0x195EDA), first_person_weapon_build_models);
	return;
}

static first_person_weapon* first_person_weapon_get_global(void)
{
	return *Memory::GetAddress<first_person_weapon**>(0x977104, 0x99FBC4);
}

static first_person_weapon* first_person_weapons_get(uint32 user_index)
{
	ASSERT(VALID_INDEX(user_index, k_number_of_users));
	return &first_person_weapon_get_global()[user_index];
}

static first_person_weapon_data* first_person_weapon_data_get(uint32 weapon_slot, first_person_weapon* first_person_data)
{
	ASSERT(VALID_INDEX(weapon_slot, k_first_person_max_weapons));
	return &first_person_data->weapon[weapon_slot];
}

static int32 first_person_weapon_slot_by_weapon_datum_index(int32 user_index, datum weapon_index)
{
	for (int32 i = 0; i < k_first_person_max_weapons; i++)
	{
		first_person_weapon_data* weapon_data = first_person_weapon_data_get(i, first_person_weapons_get(user_index));
		if (weapon_data->weapon_index == weapon_index)
		{
			return i;
		}
	}

	return NONE;
}

static s_first_person_orientations* first_person_orientations_get_global(void)
{
	return *Memory::GetAddress<s_first_person_orientations**>(0x977100, 0x99FBC0);
}

static s_first_person_orientations* first_person_orientations_get(uint32 user_index, uint32 weapon_slot)
{
	return &first_person_orientations_get_global()[(k_first_person_max_weapons * user_index) + weapon_slot];
}

static s_first_person_model_data* first_person_model_data_get_global(void)
{
	return Memory::GetAddress<s_first_person_model_data*>(0x4E8F48, 0x50EDF0);
}

static void __cdecl first_person_weapon_update(int32 user_index, int32 weapon_slot)
{
	INVOKE(0x229068, 0x20D720, first_person_weapon_update, user_index, weapon_slot);
	return;
}

static void __cdecl first_person_weapon_build_node_matrices(int32 user_index, int32 weapon_slot)
{
	first_person_weapon* fp_data = first_person_weapons_get(user_index);
	first_person_weapon_data* weapon_data = &fp_data->weapon[weapon_slot];
	const datum weapon_index = weapon_data->weapon_index;
	if (weapon_index != NONE)
	{
		s_game_globals const* game_globals = scenario_get_game_globals();
		s_game_globals_player_representation const* player_representation = TAG_BLOCK_GET_ELEMENT(&game_globals->player_representation, fp_data->character_type, s_game_globals_player_representation);
		weapon_datum const* weapon = weapon_get(weapon_index);
		struct weapon_definition const* weapon_definition = weapon_definition_get(weapon->definition_index);
		
		ASSERT(weapon_definition);

		weapon_first_person_interface_definition const* first_person_weapon_interface = first_person_interface_definition_get(
			weapon_definition,
			fp_data->character_type
		);
		ASSERT(first_person_weapon_interface);

		render_model_definition* weapon_model = render_model_definition_get(first_person_weapon_interface->model.index);

		ASSERT(player_representation);
		render_model_definition* hands_model = render_model_definition_get(player_representation->first_person_hands.index);

		// Validation asserts
		ASSERT(TEST_BIT(weapon_data->flags, _weapon_node_table_valid_bit));
		ASSERT(weapon_data->weapon_node_remapping_table_count == weapon_model->nodes.count);
		ASSERT(TEST_BIT(weapon_data->flags, _arm_node_table_valid_bit));
		ASSERT(weapon_data->hands_node_remapping_table_count == hands_model->nodes.count);

		c_animation_channel weapon_channel;
		bool fp_weapon_nodes_match = weapon_data->weapon_node_remapping_table_count == weapon_model->nodes.count;
		bool fp_hand_nodes_match = weapon_data->hands_node_remapping_table_count == hands_model->nodes.count;
		if (!fp_weapon_nodes_match || !fp_hand_nodes_match)
		{
			first_person_weapon_update(user_index, weapon_slot);
		}

		int16 node_count = weapon_data->animation_manager.get_node_count();
		s_first_person_orientations* fp_orientations = first_person_orientations_get(user_index, weapon_slot);
		weapon_data->node_orientations_count = node_count;

		// Clear weapon orientations
		if (node_count > 0)
		{
			for (int32 i = 0; i < weapon_data->node_orientations_count; i++)
			{
				fp_orientations->weapon_orientations[i] = *global_identity_orientation;
			}
		}

		// Set weapon orientations from render model
		if (weapon_model->nodes.count > 0)
		{
			for (int32 node_index = 0; node_index < weapon_model->nodes.count; node_index++)
			{
				int32 animation_graph_node_index = weapon_data->weapon_node_remapping_table[node_index];
				if (animation_graph_node_index != NONE)
				{
					ASSERT(animation_graph_node_index >= 0 && animation_graph_node_index < weapon_data->node_orientations_count);

					render_model_node* node = weapon_model->nodes[node_index];
					real_orientation* orientation = &fp_orientations->weapon_orientations[animation_graph_node_index];
					orientation->rotation = node->default_rotation;
					orientation->translation = node->default_translation;
					orientation->scale = 1.f;
				}
			}
		}

		// Set hand orientations from render model
		if (hands_model->nodes.count > 0)
		{
			for (int32 node_index = 0; node_index < hands_model->nodes.count; node_index++)
			{
				int32 animation_graph_node_index = weapon_data->hands_node_remapping_table[node_index];

				if (animation_graph_node_index != NONE)
				{
					ASSERT(animation_graph_node_index >= 0 && animation_graph_node_index < weapon_data->node_orientations_count);

					render_model_node* node = hands_model->nodes[node_index];
					real_orientation* orientation = &fp_orientations->weapon_orientations[animation_graph_node_index];
					orientation->rotation = node->default_rotation;
					orientation->translation = node->default_translation;
					orientation->scale = 1.f;
				}
			}
		}

		if (weapon_data->animation_manager.valid())
		{
			bool weapon_aim_enabled = true;
			weapon_data->animation_manager.get_node_orientations(0.f, 1.f, NULL, weapon_data->node_orientations_count, fp_orientations->weapon_orientations, NULL, 0);
			object_apply_function_overlay_node_orientations(weapon_index,
				weapon_model,
				&weapon_data->animation_manager,
				0,
				weapon_data->node_orientations_count,
				fp_orientations->weapon_orientations);

			const c_model_animation* state_animation = weapon_data->animation_manager.channels[0].get_state_animation();
			real32 duration = 0.4f;
			if (state_animation && TEST_FLAG(state_animation->get_playback_flags(), playback_flag_disable_weapon_aim_1st_person))
			{
				duration = 0.2f;
				weapon_aim_enabled = false;
			}

			fp_data->rate_interpolator_control.set_status(weapon_aim_enabled, duration);
			if (fp_data->rate_interpolator_control.enabled())
			{
				real32 ratio = fp_data->rate_interpolator_control.get_ratio();
				if (ratio > k_real_epsilon)
				{
					if (weapon_data->move_animation_channel.valid())
					{
						weapon_data->move_animation_channel.apply_weighted_node_orientations(0.f, ratio, 0.f, weapon_data->node_orientations_count, fp_orientations->weapon_orientations, NULL, 0);
					}
					if (weapon_data->jitter_animation_channel.valid())
					{
						weapon_data->jitter_animation_channel.apply_weighted_node_orientations(0.f,
							(weapon->weapon.field_1C + 0.5f) * ratio,
							0.f,
							weapon_data->node_orientations_count,
							fp_orientations->weapon_orientations,
							NULL,
							0);
					}
					bool pitch_turn_valid = false;
					if (weapon_data->pitch_turn_animation_id.valid() &&
						weapon_data->animation_manager.setup_animation_channel_by_index(&weapon_channel, weapon_data->pitch_turn_animation_id, 0))
					{
						weapon_channel.apply_weighted_blend_screen_node_orientations(
							fp_data->turn.i,
							fp_data->facing_angles.pitch,
							ratio,
							NULL,
							weapon_data->node_orientations_count,
							fp_orientations->weapon_orientations,
							0,
							0);
						pitch_turn_valid = true;
					}

					if (weapon_data->overlay_animation_id.valid() &&
						weapon_data->animation_manager.setup_animation_channel_by_index(&weapon_channel, weapon_data->overlay_animation_id, 0))
					{
						const c_model_animation* weapon_state_animation = weapon_channel.get_state_animation();
						int16 frame_count = weapon_state_animation->get_frame_count();
						if (frame_count < 9)
						{
							error(_error_delayed, "### first-person overlays animation has %d frames (needs to be %d)", frame_count, 9);
							weapon_data->overlay_animation_id.clear();
						}
						else
						{
							real32 pos_yaw = fp_data->position.i;
							if (pos_yaw <= 0.f)
							{
								if (pos_yaw < 0.f)
								{
									weapon_channel.set_frame_position(1.f);
									weapon_channel.apply_weighted_node_orientations(
										0.f,
										-(ratio * pos_yaw),
										0.f,
										weapon_data->node_orientations_count,
										fp_orientations->weapon_orientations,
										NULL,
										0);
								}
							}
							else
							{
								weapon_channel.set_frame_position(0.f);
								weapon_channel.apply_weighted_node_orientations(
									0.f,
									ratio * pos_yaw,
									0.f,
									weapon_data->node_orientations_count,
									fp_orientations->weapon_orientations,
									NULL,
									0);
							}

							real32 pos_pitch = fp_data->position.j;
							if (pos_pitch <= 0.f)
							{
								if (pos_pitch < 0.f)
								{
									weapon_channel.set_frame_position(2.f);
									weapon_channel.apply_weighted_node_orientations(
										0.f,
										-(ratio * pos_pitch),
										0.f,
										weapon_data->node_orientations_count,
										fp_orientations->weapon_orientations,
										NULL,
										0);
								}
							}
							else
							{
								weapon_channel.set_frame_position(3.f);
								weapon_channel.apply_weighted_node_orientations(
									0.f,
									ratio * pos_pitch,
									0.f,
									weapon_data->node_orientations_count,
									fp_orientations->weapon_orientations,
									NULL,
									0);
							}

							if (!pitch_turn_valid)
							{
								real32 turn_yaw = fp_data->turn.j;
								if (turn_yaw <= 0.f)
								{
									if (turn_yaw < 0.f)
									{
										weapon_channel.set_frame_position(5.f);
										weapon_channel.apply_weighted_node_orientations(
											0.f,
											-(ratio * turn_yaw),
											0.f,
											weapon_data->node_orientations_count,
											fp_orientations->weapon_orientations,
											NULL,
											0);
									}
								}
								else
								{
									weapon_channel.set_frame_position(4.f);
									weapon_channel.apply_weighted_node_orientations(
										0.f,
										ratio * turn_yaw,
										0.f,
										weapon_data->node_orientations_count,
										fp_orientations->weapon_orientations,
										NULL,
										0);
								}

								real32 turn_pitch = fp_data->turn.j;
								if (turn_pitch <= 0.f)
								{
									if (turn_pitch < 0.f)
									{
										weapon_channel.set_frame_position(6.f);
										weapon_channel.apply_weighted_node_orientations(
											0.f,
											-(ratio * turn_pitch),
											0.f,
											weapon_data->node_orientations_count,
											fp_orientations->weapon_orientations,
											NULL,
											0);
									}
								}
								else
								{
									weapon_channel.set_frame_position(7.f);
									weapon_channel.apply_weighted_node_orientations(
										0.f,
										ratio * turn_pitch,
										0.f,
										weapon_data->node_orientations_count,
										fp_orientations->weapon_orientations,
										NULL,
										0);
								}
							}

							if (weapon_data->field_E4 > 0.f)
							{
								weapon_channel.set_frame_position(8.f);
								weapon_channel.apply_weighted_node_orientations(
									0.f,
									weapon_data->field_E4 * ratio,
									0.f,
									weapon_data->node_orientations_count,
									fp_orientations->weapon_orientations,
									NULL,
									0);
							}
						}
					}
				}
			}

			// Special code for the animation for the needles that come out of the needler
			if (weapon_data->ammo_animation_id.valid() &&
				weapon_data->animation_manager.setup_animation_channel_by_index(
					&weapon_channel,
					weapon_data->ammo_animation_id,
					0)
				)
			{
				const c_model_animation* weapon_state_animation = weapon_channel.get_state_animation();
				const string_id state_name = weapon_data->animation_manager.get_state_name();
				
				real32 position;
				if (weapon_definition->weapon.weapon_type == _weapon_type_needler && 
					(
						state_name == _string_id_reload_empty || 
						state_name == _string_id_reload_full
					)
				)
				{
					int16 ammunition_frame_position;

					int16 rounds_loaded_maximum = weapon->weapon.magazines[0].rounds_loaded;
					int16 ammunition_result = weapon->weapon.magazines[0].original_time - weapon->weapon.magazines[0].state_timer;


					if (ammunition_result < k_fp_needler_animation_max_rounds_loaded)
					{
						ammunition_frame_position = weapon->weapon.magazines[0].rounds_loaded;
					}
					else
					{
						weapon_magazine_definition const* magazine = TAG_BLOCK_GET_ELEMENT(&weapon_definition->weapon.magazines, 0, weapon_magazine_definition);

						ammunition_result -= k_fp_needler_animation_max_rounds_loaded;

						real32 v1 = ((ammunition_result * 0.2f) <= 1.f ? ammunition_result * 0.2f : 1.f);

						int32 rounds_total = weapon_get_rounds_total(weapon_index, 0, 0);
						if (rounds_total > magazine->rounds_loaded_maximum)
						{
							rounds_total = magazine->rounds_loaded_maximum;
						}

						ammunition_frame_position = (int16)(((rounds_total - rounds_loaded_maximum) * v1) + rounds_loaded_maximum);
					}

					position = (real32)ammunition_frame_position;
				}
				else
				{
					const int16 frame_count = weapon_state_animation->get_frame_count();
					position = (real32)PIN(weapon->weapon.magazines[0].rounds_loaded, 0, frame_count - 1);
				}
				weapon_channel.set_frame_position(position);
				weapon_channel.apply_node_orientations(0.f, 0.f, weapon_data->node_orientations_count, fp_orientations->weapon_orientations, 0, 0);
			}

			if (TEST_BIT(fp_data->flags, _first_person_node_orientations_valid_bit) && weapon_data->animation_manager.interpolator_controls[1].enabled())
			{
				ASSERT(weapon_data->node_orientations_count == weapon_data->animation_manager.get_node_count());
				weapon_data->animation_manager.interpolate_node_orientations(
					(int16)weapon_data->node_orientations_count,
					NULL,
					fp_orientations->hand_orientations,
					fp_orientations->weapon_orientations);
			}
			SET_BIT(fp_data->flags, _first_person_attachment_suspended_bit, true);


			const real_point3d* weapon_offset = first_person_weapons_get_weapon_offset(user_index, weapon_definition, weapon);

			real_matrix4x3 matrix;
			matrix4x3_translation(&matrix, weapon_offset);
			weapon_data->node_matrices_count = weapon_data->node_orientations_count;
			if (weapon_data->child_node_index == NONE || weapon_data->alternate_parent_node_index == NONE)
			{
				weapon_data->animation_manager.node_matrices_from_orientations(weapon_data->nodes, fp_orientations->weapon_orientations, &matrix);
			}
			else
			{
				weapon_data->animation_manager.node_matrices_from_orientations_with_gun_hand_swap(
					weapon_data->nodes,
					fp_orientations->weapon_orientations,
					&matrix,
					weapon_data->child_node_index,
					weapon_data->alternate_parent_node_index);
			}

			halo_interpolator_setup_weapon_data(user_index, first_person_weapon_interface->animations.index, weapon_slot, weapon_data->nodes, weapon_data->node_matrices_count);
			if (!weapon_slot)
			{
				const int32 adjustment_matrix_index = fp_data->adjustment_matrix_index;
				const bool valid_adjustment_matrix_index = adjustment_matrix_index != NONE;

				SET_BIT(fp_data->flags, _first_person_attachment_suspended_bit, valid_adjustment_matrix_index);
				if (valid_adjustment_matrix_index)
				{
					fp_data->adjustment_matrix = weapon_data->nodes[adjustment_matrix_index];
					fp_data->adjustment_matrix.position.x = fp_data->adjustment_matrix.position.x - weapon_offset->x;
					fp_data->adjustment_matrix.position.y = fp_data->adjustment_matrix.position.y - weapon_offset->y;
					fp_data->adjustment_matrix.position.z = fp_data->adjustment_matrix.position.z - weapon_offset->z;
					halo_interpolator_set_target_position_data(user_index, 1, &fp_data->adjustment_matrix);
				}
			}
		}
	}

	return;
}

static void first_person_weapon_build_model_nodes_from_remapping_table(
	datum render_model_tag_index,
	int32 max_node_count,
	real_matrix4x3* camera_matrix,
	real_matrix4x3* fp_model_nodes,
	real_matrix4x3* node_matrices,
	int32* node_remapping_table)
{
	render_model_definition const* model = render_model_definition_get(render_model_tag_index);
	int32 model_node_count = model->nodes.count;
	int32 node_count = model_node_count;
	
	if (model_node_count > max_node_count)
	{
		node_count = max_node_count;
		model_node_count = max_node_count;
	}

	for (int32 node_index = 0; node_index < model_node_count; node_index++)
	{
		int32 node_remapping_index = node_remapping_table[node_index];
		if (node_remapping_index != NONE)
		{
			if (camera_matrix)
			{
				matrix4x3_multiply(camera_matrix, &node_matrices[node_remapping_index], &fp_model_nodes[node_index]);
			}
			else
			{
				fp_model_nodes[node_index] = node_matrices[node_remapping_index];
				model_node_count = node_count;
			}
		}
	}
	return;
}

static void __cdecl first_person_weapon_build_model_nodes(int32 node_matrices_count,
	datum unit_index,
	datum render_model_tag_index,
	uint32 fp_flags,
	real_matrix4x3* camera_matrix,
	real_matrix4x3* node_matrices,
	int32* node_remapping_table,
	s_first_person_model_data* fp_model_data)
{
	fp_model_data->object_index = unit_index;
	fp_model_data->render_model_index = render_model_tag_index;
	fp_model_data->flags = fp_flags;
	if (node_remapping_table)
	{
		first_person_weapon_build_model_nodes_from_remapping_table(render_model_tag_index, MAXIMUM_NODES_PER_FIRST_PERSON_MODEL, camera_matrix, fp_model_data->nodes, node_matrices, node_remapping_table);
	}
	else
	{
		if (node_matrices_count > MAXIMUM_NODES_PER_FIRST_PERSON_MODEL)
		{
			node_matrices_count = MAXIMUM_NODES_PER_FIRST_PERSON_MODEL;
		}

		if (camera_matrix)
		{
			if (node_matrices_count > 0)
			{
				for (int32 node_index = 0; node_index < node_matrices_count; node_index++)
				{
					matrix4x3_multiply(camera_matrix, &node_matrices[node_index], &fp_model_data->nodes[node_index]);
				}
			}
		}
		else
		{
			csmemcpy(fp_model_data->nodes, node_matrices, sizeof(*node_matrices) * node_matrices_count);
		}
	}
}

static void first_person_weapon_apply_ik(int32 user_index, s_first_person_model_data* fp_hands_model_data, s_first_person_model_data* fp_weapon_model_data)
{
	first_person_weapon* fp_data = first_person_weapons_get(user_index);
	datum unit_index = fp_data->unit_index;
	if (unit_index != NONE
		&& TEST_BIT(fp_data->weapon[k_first_person_primary_weapon].flags, 0)
		&& !TEST_BIT(fp_data->weapon[k_first_person_secondary_weapon].flags, 0)
		&& fp_data->weapon[k_first_person_primary_weapon].weapon_index != NONE
		&& !unit_is_dual_wielding(unit_index))
	{
		const unit_datum* unit = unit_get(unit_index);
		ASSERT(unit);

		c_interpolator_control* interpolator_controls = (c_interpolator_control*)((uint8*)object_header_block_get(unit_index, &unit->unit.animation_storage) + 128);
		if (fp_data->weapon[k_first_person_primary_weapon].animation_manager.valid())
		{
			if (interpolator_controls[0].enabled())
			{
				real32 ratio = interpolator_controls[0].get_ratio();
				if (::abs(ratio) >= k_real_epsilon)
				{
					s_game_globals* globals = scenario_get_game_globals();
					ik_point_iterator iterator;
					while (fp_data->weapon[k_first_person_primary_weapon].animation_manager.find_next_weapon_ik_point(&iterator))
					{
						if (iterator.attach_to_marker != NONE
							&& iterator.attach_to_marker
							&& IN_RANGE(fp_data->character_type, _character_type_masterchief, globals->player_representation.count - 1))
						{
							s_game_globals_player_representation const* player_rep = TAG_BLOCK_GET_ELEMENT(&globals->player_representation, fp_data->character_type, s_game_globals_player_representation);
							datum fp_hands_model_index = player_rep->first_person_hands.index;
							weapon_datum const* weapon = weapon_get(fp_data->weapon[k_first_person_primary_weapon].weapon_index);
							weapon_definition const* weapon_def = weapon_definition_get(weapon->definition_index);
							weapon_first_person_interface_definition const* interface_def = first_person_interface_definition_get(
								weapon_def,
								fp_data->character_type
							);

							datum fp_weapon_model_index = (interface_def ? interface_def->model.index : NONE);

							if (fp_hands_model_index != NONE && fp_weapon_model_index != NONE)
							{
								int32 fp_hands_marker_group_index = render_model_find_marker_group_by_name(fp_hands_model_index, iterator.marker);
								int32 fp_weapon_marker_group_index = render_model_find_marker_group_by_name(fp_weapon_model_index, iterator.attach_to_marker);
								if (fp_hands_marker_group_index != NONE && fp_weapon_marker_group_index != NONE)
								{
									const render_model_definition* fp_hands_model = render_model_definition_get(fp_hands_model_index);
									const render_model_definition* fp_weapon_model = render_model_definition_get(fp_weapon_model_index);

									const render_model_marker_group* fp_hands_marker_group = fp_hands_model->marker_groups[fp_hands_marker_group_index];
									const render_model_marker_group* fp_weapon_marker_group = fp_weapon_model->marker_groups[fp_weapon_marker_group_index];

									if (fp_hands_marker_group->markers.count > 0 && fp_weapon_marker_group->markers.count > 0)
									{
										const render_model_marker* fp_hands_marker = fp_hands_marker_group->markers[0];
										const render_model_marker* fp_weapon_marker = fp_weapon_marker_group->markers[0];

										real_matrix4x3 fp_hands_marker_matrix;
										real_matrix4x3 fp_weapon_marker_matrix;
										matrix4x3_from_point_and_quaternion(&fp_hands_marker_matrix, &fp_hands_marker->position, &fp_hands_marker->rotation);
										matrix4x3_from_point_and_quaternion(&fp_weapon_marker_matrix, &fp_weapon_marker->position, &fp_weapon_marker->rotation);
										matrix4x3_multiply(&fp_weapon_model_data->nodes[fp_weapon_marker->node_index], &fp_weapon_marker_matrix, &fp_weapon_marker_matrix);
										render_model_apply_two_bone_ik(fp_hands_model_index,
											fp_hands_marker->node_index,
											&fp_hands_marker_matrix,
											&fp_weapon_marker_matrix,
											ratio,
											fp_hands_model->nodes.count,
											fp_hands_model_data->nodes);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return;
}

static const real_point3d* first_person_weapons_get_weapon_offset(
	datum user_index,
	struct weapon_definition const* weapon_definition,
	weapon_datum const* weapon)
{
	ASSERT(weapon_definition);
	ASSERT(weapon);

	const s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(user_index);
	const e_weapon_offset_weapon weapon_offset_index = cartographer_player_profile_weapon_offsets_get_weapon_index_from_tag_index(weapon->definition_index);

	const real_point3d* weapon_offset;
	s_game_variant* variant = get_game_variant();

	if (game_is_multiplayer() && variant && variant->cartographer_settings.flags.test(_cartographer_variant_force_default_weapon_offsets))
	{
		weapon_offset = &weapon_definition->weapon.first_person_weapon_offset;
	}
	else if (weapon_offset_index != NONE)
	{
		weapon_offset = &player_profile->weapon_offsets[weapon_offset_index];
	}
	else
	{
		weapon_offset = &weapon_definition->weapon.first_person_weapon_offset;
	}

	return weapon_offset;
}
