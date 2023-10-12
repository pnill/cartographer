#pragma once

enum e_predicted_resource_type : int16
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

// max count: 2048
struct predicted_resource
{
    e_predicted_resource_type type;
    int16 resource_index;
    int32 tag_index;
};
TAG_GROUP_SIZE_ASSERT(predicted_resource, 8);
