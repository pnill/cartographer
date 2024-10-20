#include "stdafx.h"

#include "render_objects.h"
#include "interface/first_person_weapons.h"
#include "objects/objects.h"
#include "objects/object_definition.h"
#include "models/models.h"

#include "main/interpolator.h"

void object_get_model_node_data(
    datum object_index,
    bool first_person,
    int16 render_model_index_index,
    uint32* flags,
    datum* out_object_index,
    datum* render_model_index,
    real_matrix4x3** object_node_matrices,
    int32* object_node_matrix_count)
{
    ASSERT(flags && object_index && render_model_index && object_node_matrices && object_node_matrix_count);
    ASSERT(render_model_index_index >= 0);
    ASSERT(render_model_index_index < MAXIMUM_NUMBER_OF_FIRST_PERSON_MODELS);
    // ASSERT(render_model_index_index < render_object_globals.first_person_model_count || !first_person); ### TODO map out render_object_globals structure

    int32 block_count;
    if (first_person)
    {
        s_first_person_model_data* model_data = first_person_model_data_get(render_model_index_index);
        render_model_definition* render_model = (render_model_definition*)tag_get_fast(model_data->render_model_index);
        ASSERT(render_model);

        *render_model_index = model_data->render_model_index;
        *object_node_matrices = model_data->nodes;
        block_count = MIN(render_model->nodes.count, MAXIMUM_NODES_PER_FIRST_PERSON_MODEL);
        *object_node_matrix_count = block_count;
        *flags = model_data->flags;
        *out_object_index = model_data->object_index;
    }
    else
    {
        datum tag_index = object_get_fast_unsafe(object_index)->tag_definition_index;
        object_definition* object_def = (object_definition*)tag_get_fast(tag_index);
        ASSERT(object_def);
        s_model_definition* model = (s_model_definition*)tag_get_fast(object_def->model.index);
        ASSERT(model);
        render_model_definition* render_model = (render_model_definition*)tag_get_fast(model->render_model.index);
        ASSERT(render_model);

        ASSERT(render_model_index_index == 0);
        *render_model_index = model->render_model.index;

        if (!halo_interpolator_interpolate_object_node_matrices(object_index, object_node_matrices, object_node_matrix_count))
        {
            *object_node_matrices = object_get_node_matrices(object_index, object_node_matrix_count);
            *object_node_matrix_count = render_model->nodes.count;
        }
        *flags = 0;
        *out_object_index = object_index;
    }
    return;
}