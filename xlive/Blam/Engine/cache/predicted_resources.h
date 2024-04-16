#pragma once

enum e_predicted_resource_type : int16
{
    _predicted_resource_type_bitmap = 0,
    _predicted_resource_type_sound = 1,
    _predicted_resource_type_render_model_geometry = 2,
    _predicted_resource_type_cluster_geometry = 3,
    _predicted_resource_type_cluster_instanced_geometry = 4,
    _predicted_resource_type_lightmap_geometry_object_buckets = 5,
    _predicted_resource_type_lightmap_geometry_instance_buckets = 6,
    _predicted_resource_type_lightmap_cluster_bitmaps = 7,
    _predicted_resource_type_lightmap_instance_bitmaps = 8
};

// max count: 2048
struct predicted_resource
{
    e_predicted_resource_type type;
    int16 resource_index;
    int32 tag_index;
};
ASSERT_STRUCT_SIZE(predicted_resource, 8);
