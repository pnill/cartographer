#include "stdafx.h"
#include "first_person_weapons.h"

#include "Blam/Engine/cache/cache_files.h"
#include "Blam/Engine/camera/camera.h"
#include "Blam/Engine/game/cheats.h"
#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/items/weapons.h"
#include "Blam/Engine/main/interpolator.h"
#include "Blam/Engine/models/models.h"
#include "Blam/Engine/models/render_model_definitions.h"
#include "Blam/Engine/objects/object_definition.h"
#include "Blam/Engine/objects/objects.h"
#include "Blam/Engine/tag_files/global_string_ids.h"

#include "Util/Hooks/Hook.h"

bool show_first_person = true;

void __cdecl first_person_weapon_get_node_data(datum object_index,
    bool first_person,
    int16 render_model_index_index,
    uint32* flags,
    datum* out_object_index,
    datum* render_model_index,
    real_matrix4x3** object_node_matrices,
    int32* object_node_matrix_count);
void __cdecl first_person_weapon_update(int32 user_index, int32 weapon_slot);
void __cdecl first_person_weapons_update_nodes(int32 user_index, int32 weapon_slot);
void first_person_weapon_build_model_nodes_from_remapping_table(datum render_model_tag_index,
    int32 max_node_count,
    real_matrix4x3* camera_matrix,
    real_matrix4x3* fp_model_nodes,
    real_matrix4x3* node_matrices,
    int32* node_remapping_table);
void first_person_weapon_build_model_nodes(int32 node_matrices_count,
    datum unit_index,
    datum render_model_tag_index,
    uint32 fp_flags,
    real_matrix4x3* camera_matrix,
    real_matrix4x3* node_matrices,
    int32* node_remapping_table,
    s_first_person_model_data* fp_model_data);
void first_person_weapon_apply_ik(int32 user_index, s_first_person_model_data* model_data_1, s_first_person_model_data* model_data_2);

s_first_person_weapon* first_person_weapon_get_global(void)
{
	return *Memory::GetAddress<s_first_person_weapon**>(0x977104, 0x99FBC4);
}

s_first_person_weapon* first_person_weapons_get(uint32 user_index)
{
	return &first_person_weapon_get_global()[user_index];
}

s_first_person_weapon_data* first_person_weapon_data_get(uint32 weapon_slot, s_first_person_weapon* first_person_data)
{
    return &first_person_data->weapons[weapon_slot];
}

s_first_person_orientations* first_person_orientations_get_global(void)
{
	return *Memory::GetAddress<s_first_person_orientations**>(0x977100, 0x99FBC0);
}

s_first_person_orientations* first_person_orientations_get(uint32 user_index, uint32 weapon_slot)
{
	return &first_person_orientations_get_global()[(k_first_person_max_weapons * user_index) + weapon_slot];
}

s_first_person_model_data* first_person_model_data_get_global(void)
{
	return Memory::GetAddress<s_first_person_model_data*>(0x4E8F48, 0x50EDF0);
}

s_first_person_model_data* first_person_model_data_get(uint32 user_index)
{
	return &first_person_model_data_get_global()[user_index];
}

bool __cdecl render_first_person_check(e_skull_type skull_type)
{
	return ice_cream_flavor_available(skull_type) || !show_first_person;
}

void toggle_first_person(bool state)
{
	show_first_person = state;
	return;
}

void __cdecl first_person_weapon_get_node_data(datum object_index,
    bool first_person,
    int16 render_model_index_index,
    uint32* flags,
    datum* out_object_index,
    datum* render_model_index,
    real_matrix4x3** object_node_matrices,
    int32* object_node_matrix_count)
{
    int32 size;

    if (first_person)
    {
        s_first_person_model_data* model_data = first_person_model_data_get(render_model_index_index);
        render_model_definition* render_model = (render_model_definition*)tag_get_fast(model_data->render_model_index);
        *render_model_index = model_data->render_model_index;
        *object_node_matrices = model_data->nodes;
        size = render_model->nodes.size;
        if (size > MAXIMUM_NODES_PER_FIRST_PERSON_MODEL)
        {
            size = MAXIMUM_NODES_PER_FIRST_PERSON_MODEL;
        }
        *object_node_matrix_count = size;
        *flags = model_data->flags;
        *out_object_index = model_data->object_index;
    }
    else
    {
        datum tag_index = object_get_fast_unsafe(object_index)->tag_definition_index;
        object_definition* object_def = (object_definition*)tag_get_fast(tag_index);
        s_model_definition* model = (s_model_definition*)tag_get_fast(object_def->model.TagIndex);
        render_model_definition* render_model = (render_model_definition*)tag_get_fast(model->render_model.TagIndex);
        *render_model_index = model->render_model.TagIndex;
        
        real_matrix4x3* interpolated_matrices = NULL;
        if (halo_interpolator_interpolate_object_node_matrices(object_index, &interpolated_matrices, object_node_matrix_count))
        {
            *object_node_matrices = interpolated_matrices;
        }
        else
        {
            *object_node_matrices = object_get_node_matrices(object_index, object_node_matrix_count);
            *object_node_matrix_count = render_model->nodes.size;
        }
        *flags = 0;
        *out_object_index = object_index;
    }
    return;
}

void __cdecl first_person_weapon_update(int32 user_index, int32 weapon_slot)
{
    INVOKE(0x229068, 0x20D720, first_person_weapon_update, user_index, weapon_slot);
    return;
}

void __cdecl first_person_weapons_update_nodes(int32 user_index, int32 weapon_slot)
{
    s_first_person_weapon* fp_data = first_person_weapons_get(user_index);
    s_first_person_weapon_data* weapon_data = &fp_data->weapons[weapon_slot];
    datum weapon_index = weapon_data->weapon_index;
    if (weapon_index != NONE)
    {
        s_game_globals* game_globals = scenario_get_game_globals();
        s_game_globals_player_representation* player_representation = game_globals->player_representation[fp_data->character_type];
        weapon_datum* weapon = (weapon_datum*)object_get_fast_unsafe(weapon_index);
        _weapon_definition* weapon_definition = (_weapon_definition*)tag_get_fast(weapon->item.object.tag_definition_index);

        weapon_first_person_interface_definition* first_person_weapon_interface = first_person_interface_definition_get(weapon_definition, (e_character_type)fp_data->character_type);
        render_model_definition* fp_weapon_model = (render_model_definition*)tag_get_fast(first_person_weapon_interface->model.TagIndex);
        render_model_definition* fp_hands_model = (render_model_definition*)tag_get_fast(player_representation->first_person_hands.TagIndex);
        c_animation_channel weapon_channel;

        bool fp_weapon_nodes_match = weapon_data->weapon_node_remapping_table_count == fp_weapon_model->nodes.size;
        bool fp_hand_nodes_match = weapon_data->hands_node_remapping_table_count == fp_hands_model->nodes.size;
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
            for (uint32 i = 0; i < weapon_data->node_orientations_count; i++)
            {
                memcpy(fp_orientations->weapon_orientations, &global_identity_orientation, sizeof(real_orientation));
            }
        }

        // Set weapon orientations from render model
        if (fp_weapon_model->nodes.size > 0)
        {
            for (uint32 node_index = 0; node_index < fp_weapon_model->nodes.size; node_index++)
            {
                int32 orientation_index = weapon_data->weapon_node_remapping_table[node_index];
                render_model_node* node = fp_weapon_model->nodes[node_index];
                if (orientation_index != NONE)
                {
                    real_orientation* orientation = &fp_orientations->weapon_orientations[orientation_index];
                    orientation->quaternion = node->default_rotation;
                    orientation->position = node->default_translation;
                    orientation->scale = 1.0f;
                }
            }
        }

        // Set hand orientations from render model
        if (fp_hands_model->nodes.size > 0)
        {
            for (uint32 node_index = 0; node_index < fp_hands_model->nodes.size; node_index++)
            {
                int32 orientation_index = weapon_data->hands_node_remapping_table[node_index];
                render_model_node* node = fp_hands_model->nodes[node_index];
                if (orientation_index != NONE)
                {
                    real_orientation* orientation = &fp_orientations->weapon_orientations[orientation_index];
                    orientation->quaternion = node->default_rotation;
                    orientation->position = node->default_translation;
                    orientation->scale = 1.0f;
                }
            }
        }

        if (weapon_data->animation_manager.valid())
        {
            bool weapon_aim_enabled = true;
            weapon_data->animation_manager.get_node_orientations(0.0f, 1.0f, NULL, weapon_data->node_orientations_count, fp_orientations->weapon_orientations, NULL, 0);
            object_apply_function_overlay_node_orientations(weapon_index,
                fp_weapon_model,
                &weapon_data->animation_manager,
                0,
                weapon_data->node_orientations_count,
                fp_orientations->weapon_orientations);

            const c_model_animation* state_animation = weapon_data->animation_manager.channel1.get_state_animation();
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
                if (ratio > k_real_math_epsilon)
                {
                    if (weapon_data->move_animation_channel.valid())
                    {
                        weapon_data->move_animation_channel.apply_weighted_node_orientations(0.0f, ratio, 0.0f, weapon_data->node_orientations_count, fp_orientations->weapon_orientations, NULL, 0);
                    }
                    if (weapon_data->jitter_animation_channel.valid())
                    {
                        weapon_data->jitter_animation_channel.apply_weighted_node_orientations(0.0f,
                            (weapon->field_188 + 0.5f) * ratio,
                            0.0f,
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
                            fp_data->turn.yaw,
                            fp_data->player_facing.pitch,
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
                        if (weapon_state_animation->get_frame_count() < 9)
                        {
                            weapon_data->overlay_animation_id.clear();
                        }
                        else
                        {
                            real32 yaw = fp_data->pos.yaw;
                            if (yaw <= 0.0f)
                            {
                                if (yaw < 0.0f)
                                {
                                    weapon_channel.set_frame_position(1.0f);
                                    weapon_channel.apply_weighted_node_orientations(
                                        0.0f,
                                        -0.0f - (ratio * yaw),
                                        0.0f,
                                        weapon_data->node_orientations_count,
                                        fp_orientations->weapon_orientations,
                                        NULL,
                                        0);
                                }
                            }
                            else
                            {
                                weapon_channel.set_frame_position(0.0f);
                                weapon_channel.apply_weighted_node_orientations(
                                    0.0f,
                                    ratio * yaw,
                                    0.0f,
                                    weapon_data->node_orientations_count,
                                    fp_orientations->weapon_orientations,
                                    NULL,
                                    0);
                            }

                            real32 pitch = fp_data->pos.pitch;
                            if (pitch <= 0.0f)
                            {
                                if (pitch < 0.0f)
                                {
                                    weapon_channel.set_frame_position(2.0f);
                                    weapon_channel.apply_weighted_node_orientations(
                                        0.0f,
                                        -0.0f - (ratio * pitch),
                                        0.0f,
                                        weapon_data->node_orientations_count,
                                        fp_orientations->weapon_orientations,
                                        NULL,
                                        0);
                                }
                            }
                            else
                            {
                                weapon_channel.set_frame_position(3.0f);
                                weapon_channel.apply_weighted_node_orientations(
                                    0.0f,
                                    ratio * pitch,
                                    0.0f,
                                    weapon_data->node_orientations_count,
                                    fp_orientations->weapon_orientations,
                                    NULL,
                                    0);
                            }

                            if (!pitch_turn_valid)
                            {
                                real32 yaw = fp_data->turn.yaw;
                                if (yaw <= 0.0)
                                {
                                    if (yaw < 0.0)
                                    {
                                        weapon_channel.set_frame_position(5.0f);
                                        weapon_channel.apply_weighted_node_orientations(
                                            0.0f,
                                            -0.0f - (ratio * yaw),
                                            0.0f,
                                            weapon_data->node_orientations_count,
                                            fp_orientations->weapon_orientations,
                                            NULL,
                                            0);
                                    }
                                }
                                else
                                {
                                    weapon_channel.set_frame_position(4.0f);
                                    weapon_channel.apply_weighted_node_orientations(
                                        0.0f,
                                        ratio * yaw,
                                        0.0f,
                                        weapon_data->node_orientations_count,
                                        fp_orientations->weapon_orientations,
                                        NULL,
                                        0);
                                }

                                real32 pitch = fp_data->turn.pitch;
                                if (pitch <= 0.0f)
                                {
                                    if (pitch < 0.0f)
                                    {
                                        weapon_channel.set_frame_position(6.0f);
                                        weapon_channel.apply_weighted_node_orientations(
                                            0.0f,
                                            -0.0f - (ratio * pitch),
                                            0.0f,
                                            weapon_data->node_orientations_count,
                                            fp_orientations->weapon_orientations,
                                            NULL,
                                            0);
                                    }
                                }
                                else
                                {
                                    weapon_channel.set_frame_position(7.0f);
                                    weapon_channel.apply_weighted_node_orientations(
                                        0.0f,
                                        ratio * pitch,
                                        0.0f,
                                        weapon_data->node_orientations_count,
                                        fp_orientations->weapon_orientations,
                                        NULL,
                                        0);
                                }
                            }

                            if (weapon_data->field_E4 > 0.0f)
                            {
                                weapon_channel.set_frame_position(8.0f);
                                weapon_channel.apply_weighted_node_orientations(
                                    0.0f,
                                    weapon_data->field_E4 * ratio,
                                    0.0f,
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
                    0))
            {
                const c_model_animation* weapon_state_animation = weapon_channel.get_state_animation();
                string_id state_name = weapon_data->animation_manager.get_state_name();
                if (weapon_definition->weapon_type == _weapon_type_needler &&
                    (state_name == HS_RELOAD_EMPTY || state_name == HS_RELOAD_FULL))
                {
                    int16 rounds_loaded_maximum = weapon->magazines[0].rounds_loaded_maximum;
                    int16 ammunition_result = weapon->magazines[0].field_4 - weapon->magazines[0].field_2;

                    int16 ammunition_frame_position;
                    if (ammunition_result < 44)
                    {
                        ammunition_frame_position = weapon->magazines[0].rounds_loaded_maximum;
                    }
                    else
                    {
                        weapon_magazine_definition* magazine = weapon_definition->magazines[0];
                        ammunition_result -= 44;

                        real32 v1 = ((ammunition_result * 0.2f) <= 1.0f ? ammunition_result * 0.2f : 1.0f);

                        int32 rounds_total = weapon_get_rounds_total(weapon_index, 0, 0);
                        if (rounds_total > magazine->rounds_loaded_maximum)
                        {
                            rounds_total = magazine->rounds_loaded_maximum;
                        }

                        ammunition_frame_position = ((rounds_total - rounds_loaded_maximum) * v1) + rounds_loaded_maximum;
                    }
                    weapon_channel.set_frame_position((real32)ammunition_frame_position);
                    weapon_channel.apply_node_orientations(0.0f, 0.0f, weapon_data->node_orientations_count, fp_orientations->weapon_orientations, 0, 0);
                }
                else
                {
                    int16 frame_count = weapon_state_animation->get_frame_count();
                    int16 rounds_loaded_maximum = weapon->magazines[0].rounds_loaded_maximum;
                    int32 position = frame_count - 1;
                    if (rounds_loaded_maximum >= 0)
                    {
                        if (rounds_loaded_maximum <= position)
                        {
                            position = weapon->magazines[0].rounds_loaded_maximum;
                        }
                    }
                    else
                    {
                        position = 0;
                    }
                    weapon_channel.set_frame_position((real32)position);
                    weapon_channel.apply_node_orientations(0.0f, 0.0f, weapon_data->node_orientations_count, fp_orientations->weapon_orientations, 0, 0);
                }
            }
            if (TEST_BIT(fp_data->flags, 1) && weapon_data->animation_manager.interpolator_control_1.enabled())
            {
                weapon_data->animation_manager.interpolate_node_orientations(weapon_data->node_orientations_count, NULL, fp_orientations->hand_orientations, fp_orientations->weapon_orientations);
            }
            SET_FLAG(fp_data->flags, 1, true);
            
            real_matrix4x3 matrix;
            matrix4x3_translation(&matrix, &weapon_definition->first_person_weapon_offset);
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

            halo_interpolator_setup_weapon_data(user_index, first_person_weapon_interface->animations.TagIndex, weapon_slot, weapon_data->nodes, weapon_data->node_matrices_count);
            if (!weapon_slot)
            {
                int32 adjustment_matrix_index = fp_data->adjustment_matrix_index;
                if (adjustment_matrix_index == NONE)
                {
                    SET_FLAG(fp_data->flags, 2, false);
                }
                else
                {
                    memcpy(&fp_data->adjustment_matrix, &weapon_data->nodes[adjustment_matrix_index], sizeof(fp_data->adjustment_matrix));
                    fp_data->adjustment_matrix.position.x = fp_data->adjustment_matrix.position.x - weapon_definition->first_person_weapon_offset.i;
                    fp_data->adjustment_matrix.position.y = fp_data->adjustment_matrix.position.y - weapon_definition->first_person_weapon_offset.j;
                    fp_data->adjustment_matrix.position.z = fp_data->adjustment_matrix.position.z - weapon_definition->first_person_weapon_offset.k;
                    SET_FLAG(fp_data->flags, 2, true);
                    halo_interpolator_set_target_position_data(user_index, 1, &fp_data->adjustment_matrix);
                }
            }
        }
    }

    return;
}


void first_person_weapon_build_model_nodes_from_remapping_table(datum render_model_tag_index,
    int32 max_node_count,
    real_matrix4x3* camera_matrix,
    real_matrix4x3* fp_model_nodes,
    real_matrix4x3* node_matrices,
    int32* node_remapping_table)
{
    render_model_definition* model = (render_model_definition*)tag_get_fast(render_model_tag_index);
    int32 model_node_count = model->nodes.size;
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

void __cdecl first_person_weapon_build_model_nodes(int32 node_matrices_count,
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
                for (uint32 node_index = 0; node_index < node_matrices_count; node_index++)
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

int32 __cdecl first_person_weapon_build_models(int32 user_index, datum unit_index, int32 model_data_count, s_first_person_model_data* fp_model_data)
{
    int32 current_model_index = 0;
    if (!ice_cream_flavor_available(_skull_type_blind) && user_index != NONE)
    {
        bool weapon_flag_0 = false;
        bool weapon_flag_1 = false;

        s_first_person_weapon* first_person_data = first_person_weapons_get(user_index);
        for (uint32 weapon_index = 0; weapon_index < k_first_person_max_weapons; weapon_index++)
        {
            s_first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_index, first_person_data);
            weapon_flag_0 |= TEST_BIT(weapon_data->flags, 0);
            weapon_flag_1 |= weapon_data->weapon_index != NONE;
        }
        if (weapon_flag_0 && weapon_flag_1)
        {
            if (first_person_data->unit_index == unit_index && first_person_data->unit_index != NONE)
            {
                s_game_globals* globals = scenario_get_game_globals();
                if (IN_RANGE_INCLUSIVE(first_person_data->character_type, _character_type_masterchief, globals->player_representation.size - 1))
                {
                    s_game_globals_player_representation* player_rep = globals->player_representation[first_person_data->character_type];
                    datum fp_hands_index = player_rep->first_person_hands.TagIndex;
                    datum fp_body_index = player_rep->first_person_body.TagIndex;
                    datum unit_model_index = NONE;
                    int32 node_count = 0;
                    real_matrix4x3* node_matrices = NULL;
                    object_datum* object = object_get_fast_unsafe(unit_index);
                    object_definition* object_def = (object_definition*)tag_get_fast(object->tag_definition_index);
                    if (object_def->model.TagIndex != NONE)
                    {
                        s_model_definition* model = (s_model_definition*)tag_get_fast(object_def->model.TagIndex);
                        if (model->animation.TagIndex != NONE)
                        {
                            unit_model_index = model->render_model.TagIndex;
                            if (!halo_interpolator_interpolate_object_node_matrices(unit_index, &node_matrices, &node_count))
                            {
                                node_matrices = object_get_node_matrices(unit_index, &node_count);
                            }
                        }
                    }
                    real_matrix4x3 camera_matrix;
                    s_camera* global_camera = get_global_camera();
                    matrix4x3_from_point_and_vectors(&camera_matrix, &global_camera->point, &global_camera->forward, &global_camera->up);
                    if (TEST_BIT(first_person_data->flags, 2))
                    {
                        // Get the interpolated matrix and use it if we can interpolate
                        real_matrix4x3 interpolated_adjustment_matrix;
                        bool interpolated_matrix = halo_interpolator_get_interpolated_matrix_from_user_index(user_index, 1u, &interpolated_adjustment_matrix);
                        real_matrix4x3* adjustment_matrix = (interpolated_matrix ? &interpolated_adjustment_matrix : &first_person_data->adjustment_matrix);

                        real_matrix4x3 inverse_adjustment_matrix;
                        matrix4x3_inverse(adjustment_matrix, &inverse_adjustment_matrix);
                        matrix4x3_multiply(&camera_matrix, &inverse_adjustment_matrix, &camera_matrix);
                    }
                    first_person_data->identity_matrix = camera_matrix;

                    for (uint32 weapon_slot = 0; weapon_slot < k_first_person_max_weapons; weapon_slot++)
                    {
                        s_first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_slot, first_person_data);
                        if (TEST_BIT(weapon_data->flags, 0) 
                            && weapon_data->weapon_index != NONE 
                            && model_data_count > 1 
                            && TEST_BIT(weapon_data->flags, _arm_node_table_valid_bit)
                            && fp_hands_index != NONE)
                        {
                            weapon_datum* weapon = (weapon_datum*)object_get_fast_unsafe(weapon_data->weapon_index);
                            _weapon_definition* weapon_def = (_weapon_definition*)tag_get_fast(weapon->item.object.tag_definition_index);
                            weapon_first_person_interface_definition* interface_def = first_person_interface_definition_get(weapon_def, first_person_data->character_type);
                            datum weapon_animations_index = (interface_def ? interface_def->animations.TagIndex : NONE);

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

                    current_model_index++;
                    s_first_person_model_data* current_fp_model_data = &fp_model_data[current_model_index];

                    for (uint32 weapon_slot = 0; weapon_slot < k_first_person_max_weapons; weapon_slot++)
                    {
                        s_first_person_weapon_data* weapon_data = first_person_weapon_data_get(weapon_slot, first_person_data);
                        if (TEST_BIT(weapon_data->flags, 0) && weapon_data->weapon_index != NONE)
                        {
                            weapon_datum* weapon = (weapon_datum*)object_get_fast_unsafe(weapon_data->weapon_index);
                            _weapon_definition* weapon_def = (_weapon_definition*)tag_get_fast(weapon->item.object.tag_definition_index);
                            weapon_first_person_interface_definition* interface_def = first_person_interface_definition_get(weapon_def, first_person_data->character_type);

                            datum weapon_model_index;
                            datum weapon_animations_index;
                            if (interface_def)
                            {
                                weapon_model_index = interface_def->model.TagIndex;
                                weapon_animations_index = interface_def->animations.TagIndex;
                            }
                            else
                            {
                                weapon_model_index = NONE;
                                weapon_animations_index = NONE;
                            }
                            
                            real_matrix4x3* model_nodes = NULL;
                            bool interpolated = halo_interpolator_interpolate_weapon(user_index, weapon_animations_index, weapon_slot, &model_nodes, &weapon_data->node_matrices_count);
                            if (current_model_index < model_data_count && TEST_BIT(weapon_data->flags, _weapon_node_table_valid_bit) && weapon_model_index != NONE)
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
                                ++current_model_index;
                                current_fp_model_data = &fp_model_data[current_model_index];
                            }
                        }
                    }

                    if (current_model_index < model_data_count && fp_body_index != NONE && unit_model_index != NONE && node_matrices)
                    {
                        render_model_definition* body_render_model = (render_model_definition*)tag_get_fast(fp_body_index);
                        render_model_definition* unit_render_model = (render_model_definition*)tag_get_fast(unit_model_index);
                        if (body_render_model->nodes.size == unit_render_model->nodes.size
                            && body_render_model->node_list_checksum == unit_render_model->node_list_checksum)
                        {
                            real_angle angle = body_render_model->dont_draw_over_camera_cosine_angle;
                            if (angle == 0.0f || angle > global_camera->forward.k)
                            {
                                int32 node_count_copied = node_count;
                                s_first_person_model_data* current_data = &fp_model_data[current_model_index];
                                current_data->object_index = unit_index;
                                current_data->render_model_index = fp_body_index;
                                current_data->flags = 0;
                                if (node_count > MAXIMUM_NODES_PER_FIRST_PERSON_MODEL)
                                {
                                    node_count_copied = 64;
                                }
                                csmemcpy(current_data->nodes, node_matrices, 52 * node_count_copied);
                                ++current_model_index;
                            }
                        }
                    }
                    first_person_weapon_apply_ik(user_index, fp_model_data, &fp_model_data[1]);
                }
            }
        }
    }

    return current_model_index;
}

void first_person_weapon_apply_ik(int32 user_index, s_first_person_model_data* fp_hands_model_data, s_first_person_model_data* fp_weapon_model_data)
{
    s_first_person_weapon* fp_data = first_person_weapons_get(user_index);
    datum unit_index = fp_data->unit_index;
    if (unit_index != NONE
        && TEST_BIT(fp_data->weapons[0].flags, 0)
        && !TEST_BIT(fp_data->weapons[1].flags, 0)
        && fp_data->weapons[0].weapon_index != NONE
        && !unit_is_dual_wielding(unit_index))
    {
        const unit_datum* unit = (unit_datum*)object_get_fast_unsafe(unit_index);
        c_animation_manager* weapon_raised_manager = (c_animation_manager*)object_header_block_get(unit_index, &unit->weapon_raised_block);
        if (fp_data->weapons[0].animation_manager.valid())
        {
            if (weapon_raised_manager->interpolator_control_2.enabled())
            {
                real32 ratio = weapon_raised_manager->interpolator_control_2.get_ratio();
                if (fabs(ratio) >= k_real_math_epsilon)
                {
                    s_game_globals* globals = scenario_get_game_globals();
                    ik_point_iterator iterator;
                    while (fp_data->weapons[0].animation_manager.find_next_weapon_ik_point(&iterator))
                    {
                        if (iterator.attach_to_marker != NONE 
                            && iterator.attach_to_marker.get_packed() 
                            && IN_RANGE_INCLUSIVE(fp_data->character_type, _character_type_masterchief, globals->player_representation.size - 1) )
                        {
                            const s_game_globals_player_representation* player_rep = globals->player_representation[fp_data->character_type];
                            datum fp_hands_model_index = player_rep->first_person_hands.TagIndex;
                            const weapon_datum* weapon = (weapon_datum*)object_get_fast_unsafe(fp_data->weapons[0].weapon_index);
                            const _weapon_definition* weapon_def = (_weapon_definition*)tag_get_fast(weapon->item.object.tag_definition_index);
                            weapon_first_person_interface_definition* interface_def = first_person_interface_definition_get(weapon_def, fp_data->character_type);

                            datum fp_weapon_model_index = (interface_def ? interface_def->model.TagIndex : NONE);

                            if (fp_hands_model_index != NONE && fp_weapon_model_index != NONE)
                            {
                                int32 fp_hands_marker_group_index = render_model_find_marker_group_by_name(fp_hands_model_index, iterator.marker);
                                int32 fp_weapon_marker_group_index = render_model_find_marker_group_by_name(fp_weapon_model_index, iterator.attach_to_marker);
                                if (fp_hands_marker_group_index != NONE && fp_weapon_marker_group_index != NONE)
                                {
                                    const render_model_definition* fp_hands_model = (render_model_definition*)tag_get_fast(fp_hands_model_index);
                                    const render_model_definition* fp_weapon_model = (render_model_definition*)tag_get_fast(fp_weapon_model_index);

                                    const render_model_marker_group* fp_hands_marker_group = fp_hands_model->marker_groups[fp_hands_marker_group_index];
                                    const render_model_marker_group* fp_weapon_marker_group = fp_weapon_model->marker_groups[fp_weapon_marker_group_index];
                                    
                                    if (fp_hands_marker_group->markers.size > 0 && fp_weapon_marker_group->markers.size > 0)
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
                                            fp_hands_model->nodes.size,
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
}

void first_person_weapon_apply_camera_effect(int32 user_index, real_matrix4x3* effect_matrix)
{
    s_first_person_weapon* fp = first_person_weapons_get(user_index);
    if (fp->adjustment_matrix_index != NONE && TEST_BIT(fp->flags, 2))
    {
        real_matrix4x3 interpolated_adjustment_matrix;
        bool can_interpolate = halo_interpolator_get_interpolated_matrix_from_user_index(user_index, 1, &interpolated_adjustment_matrix);

        real_matrix4x3* matrix_to_multiply = (can_interpolate ? &interpolated_adjustment_matrix : &fp->adjustment_matrix);
        matrix4x3_multiply(effect_matrix, matrix_to_multiply, effect_matrix);
    }

    return;
}

void first_person_weapons_apply_patches(void)
{
	// Only patch if not a dedi
	if (!Memory::IsDedicatedServer()) 
	{ 
		PatchCall(Memory::GetAddress(0x228579), render_first_person_check);
        PatchCall(Memory::GetAddress(0x19D281), first_person_weapon_get_node_data);
        PatchCall(Memory::GetAddress(0x22B2D4), first_person_weapons_update_nodes);
        PatchCall(Memory::GetAddress(0x195EDA), first_person_weapon_build_models);
	}

	return;
}
