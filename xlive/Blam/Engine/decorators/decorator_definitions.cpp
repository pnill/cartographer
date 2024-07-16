#include "stdafx.h"
#include "decorator_definitions.h"

const geometry_block_info* c_decorator_cache_block::get_geometry_block() const
{
    return &this->m_geometry_block;
}

const s_decorator_cache_block_data* c_decorator_cache_block::get_cache_block_data() const
{
    return this->m_cache_block_data[0];
}


short c_decorator_cell_collection::get_child_indice(DWORD indice_index) const
{
    return this->m_child_indices[indice_index];
}

short c_decorator_cell_collection::get_cache_block_index() const
{
    return this->m_cache_block_index;
}

short c_decorator_cell_collection::get_group_count() const
{
    return this->m_group_count;
}

int c_decorator_cell_collection::get_group_start_index() const
{
    return this->m_group_start_index;
}


const real_point3d* c_decorator_placement_definition::get_grid_origin() const
{
    return &this->m_grid_origin;
}

int c_decorator_placement_definition::get_cell_count_per_dimension() const
{
    return this->m_cell_count_per_dimension;
}

const c_decorator_cache_block* c_decorator_placement_definition::get_cache_block(DWORD cache_block_index) const
{
    return this->m_cache_blocks[cache_block_index];
}

const s_decorator_group* c_decorator_placement_definition::get_group(DWORD group_index) const
{
    return this->m_groups[group_index];
}

const c_decorator_cell_collection* c_decorator_placement_definition::get_cell(DWORD cell_index) const
{
    return this->m_cells[cell_index];
}

const s_decorator_projected_decal* c_decorator_placement_definition::get_decal(DWORD decal_index) const
{
    return this->m_decals[decal_index];
}
