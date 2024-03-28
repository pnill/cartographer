#pragma once
#include "decorator_projected_decals.h"

#include "tag_files/tag_block.h"

#include "geometry/geometry_block.h"
#include "math/color_math.h"


#define k_maximum_cache_block_count 4096
#define k_maximum_group_count = 131072
#define k_maximum_cell_collection_count 65535

// max count: 32*k_kilo 32768
struct s_decorator_placement
{
    int internal_data_1;
    int compressed_position;
    pixel32 tint_color;
    pixel32 lightmap_color;
    int compressed_light_direction;
    int compressed_light_2_direction;
};
TAG_BLOCK_SIZE_ASSERT(s_decorator_placement, 24);

// max count: 64*k_kilo 65536
struct rasterizer_vertex_decorator_sprite
{
    real_point3d position;
    real_vector3d offset;
    real_vector3d axis;
    real_point2d texcoord;
    pixel32 color;
};
TAG_BLOCK_SIZE_ASSERT(rasterizer_vertex_decorator_sprite, 48);

// max count: 1
struct s_decorator_cache_block_data
{
    tag_block<s_decorator_placement> placements;
    tag_block<rasterizer_vertex_decorator_decal> decal_vertices;
    tag_block<indices_block> decal_indices;
    void* decal_vertex_buffer;
    int pad[4];
    tag_block<rasterizer_vertex_decorator_sprite> sprite_vertices;
    tag_block<indices_block> sprite_indices;
    void* sprite_vertex_buffer;
    int pad1[4];
};
TAG_BLOCK_SIZE_ASSERT(s_decorator_cache_block_data, 80);
 
// max count: k_maximum_cache_block_count 4096
class c_decorator_cache_block
{
private:
    geometry_block_info m_geometry_block;
    tag_block<s_decorator_cache_block_data> m_cache_block_data;
    int pad[2];

public:
    const geometry_block_info* get_geometry_block() const;
    const s_decorator_cache_block_data* get_cache_block_data() const;
};
TAG_BLOCK_SIZE_ASSERT(c_decorator_cache_block, 52);

enum e_decorator_type : char
{
    decorator_type_model = 0,
    decorator_type_floating_decal = 1,
    decorator_type_projected_decal = 2,
    decorator_type_screen_facing_quad = 3,
    decorator_type_axis_rotating_quad = 4,
    decorator_type_cross_quad = 5
};

// max count: k_maximum_group_count 131072
struct s_decorator_group
{
    byte decorator_set;                 // Block Index: scenario_decorator_set_palette_entry
    e_decorator_type decorator_type;
    byte shader_index;
    byte compressed_radius;
    short cluster;
    short cache_block;                   // Block Index: c_decorator_cache_block
    short decorator_start_index;
    short decorator_count;
    short vertex_start_offset;
    short vertex_count;
    short index_start_offset;
    short index_count;
    int compressed_bounding_center;
};
TAG_BLOCK_SIZE_ASSERT(s_decorator_group, 24);

// max count: k_maximum_cell_collection_count 65535
class c_decorator_cell_collection
{
private:
    short m_child_indices[8];
    short m_cache_block_index;              // Block Index: c_decorator_cache_block
    short m_group_count;
    int m_group_start_index;

public:
    short get_child_indice(DWORD indice_index) const;
    short get_cache_block_index() const;
    short get_group_count() const;
    int get_group_start_index() const;
};
TAG_BLOCK_SIZE_ASSERT(c_decorator_cell_collection, 24);

// max count: 1
class c_decorator_placement_definition
{
private:
    real_point3d m_grid_origin;
    int m_cell_count_per_dimension;
    tag_block<c_decorator_cache_block> m_cache_blocks;
    tag_block<s_decorator_group> m_groups;
    tag_block<c_decorator_cell_collection> m_cells;
    tag_block<s_decorator_projected_decal> m_decals;

public: 
    const real_point3d* get_grid_origin() const;
    int get_cell_count_per_dimension()  const;
    const c_decorator_cache_block* get_cache_block(DWORD cache_block_index)  const;
    const s_decorator_group* get_group(DWORD group_index) const;
    const c_decorator_cell_collection* get_cell(DWORD cell_index) const;
    const s_decorator_projected_decal* get_decal(DWORD decal_index) const;
};
TAG_BLOCK_SIZE_ASSERT(c_decorator_placement_definition, 48);
