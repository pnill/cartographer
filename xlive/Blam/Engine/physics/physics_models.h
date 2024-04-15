#pragma once

#define k_maximum_shapes_per_physics_model 1024
#include "havok/physics/collide/shape/capsule/hkCapsuleShape.h"
#include "havok/physics/collide/shape/misc/transform/hkTransformShape.h"
#include "havok/physics/collide/shape/shpere/hkShpere.h"

enum e_physics_constraint : int16
{
	_physics_constraint_hinge = 0,
	_physics_constraint_limited_hinge = 1,
	_physics_constraint_ragdoll = 2,
	_physics_constraint_stiff_spring = 3,
	_physics_constraint_ball_and_socket = 4,
	_physics_constraint_prismatic = 5
};

enum e_physics_model_shape_flags : int16
{
	_physics_model_shape_unused = FLAG(0)
};

struct s_physics_model_shape
{
    string_id name;
    int16 material;		// Block Index: materials_block
    e_physics_model_shape_flags flags;
    real32 relative_mass_scale;
    real32 friction;
    real32 restitution;
    real32 volume;
    real32 mass;
    int16 mass_distribution_index;
    int8 phantom_type_index;
    int8 collision_group;
};
CHECK_STRUCT_SIZE(s_physics_model_shape, 32);

struct s_physics_model_capsule
{
    hkCapsuleShape shape;
};
CHECK_STRUCT_SIZE(s_physics_model_capsule, 48);

struct s_physics_model_translation
{
    hkTransformShape transform;
};
CHECK_STRUCT_SIZE(s_physics_model_translation, 80);

// max count: k_maximum_shapes_per_physics_model 1024
struct s_physics_model_sphere
{
    s_physics_model_shape shape;

    hkSphereShape sphere;

    s_physics_model_translation translate;
};
TAG_BLOCK_SIZE_ASSERT(s_physics_model_sphere, 128);


// max count: k_maximum_shapes_per_physics_model 1024
struct s_physics_model_pill
{
    s_physics_model_shape shape;
    s_physics_model_capsule capsule;
};
TAG_BLOCK_SIZE_ASSERT(s_physics_model_pill, 80);
