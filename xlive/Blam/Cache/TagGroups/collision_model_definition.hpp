#pragma once
#include "stdafx.h"

#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\DataTypes\Strings.h"
#include "Blam\Cache\TagGroups.hpp"
#include "Blam\Cache\TagGroups\scenario_structure_bsp_definition.hpp"
#include "Blam\Math\BlamMath.h"

#pragma pack(push,1)
struct s_collision_model_block
{
    tag_block<s_scenario_structure_bsp_group_definition::s_import_info_block> importInfo;

    tag_block<s_scenario_structure_bsp_group_definition::s_errors_block> errors;

    enum Flags : int
    {
        ContainsOpenEdges = 0x1,
    };
    Flags flags;

    struct s_collision_model_material_block
    {
        string_id name;
    };
    TAG_BLOCK_SIZE_ASSERT(s_collision_model_material_block, 4);
    tag_block<s_collision_model_material_block> materials;

    struct s_collision_model_region_block
    {
        string_id name;
        struct s_collision_model_permutation_block
        {
            string_id name;
            struct s_collision_model_bsp_block
            {
                __int16 nodeIndex;
                PAD(0x2);
                s_scenario_structure_bsp_group_definition::s_collision_bsp_block bsp;

            };
            tag_block<s_collision_model_bsp_block> bsps;

            tag_block<s_scenario_structure_bsp_group_definition::s_instanced_geometries_definitions_block::s_bsp_physics_block> bspphysics;
        };
        tag_block<s_collision_model_permutation_block> permutations;
    };
    tag_block<s_collision_model_region_block> regions;

    struct s_collision_model_pathfinding_sphere_block
    {
        // BlockIndex1("collision_model_node_block")
        __int16 node;

        enum Flags : short
        {
            RemainsWhenOpen = 0x1,
            VehicleOnly = 0x2,
            WithSectors = 0x4,
        };
        Flags flags;
        real_point3d center;
        float radius;
    };
    TAG_BLOCK_SIZE_ASSERT(s_collision_model_pathfinding_sphere_block, 20);
    tag_block<s_collision_model_pathfinding_sphere_block> pathfindingSpheres;

    struct s_collision_model_node_block
    {
        string_id name;
        PAD(2);

        __int16 parentNode;
        __int16 nextSiblingNode;
        __int16 firstChildNode;
    };
    TAG_BLOCK_SIZE_ASSERT(s_collision_model_node_block, 12);
    tag_block<s_collision_model_node_block> nodes;
};
#pragma pack(pop)
