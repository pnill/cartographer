#pragma once

enum e_predicted_resource_type : short
{
    predicted_resource_type_bitmap = 0,
    predicted_resource_type_sound = 1,
    predicted_resource_type_render_model_geometry = 2,
    predicted_resource_type_cluster_geometry = 3,
    predicted_resource_type_cluster_instanced_geometry = 4,
    predicted_resource_type_lightmap_geometry_object_buckets = 5,
    predicted_resource_type_lightmap_geometry_instance_buckets = 6,
    predicted_resource_type_lightmap_cluster_bitmaps = 7,
    predicted_resource_type_lightmap_instance_bitmaps = 8
};

struct predicted_resource
{
    e_predicted_resource_type type;
    short resource_index;
    int tag_index;
};
