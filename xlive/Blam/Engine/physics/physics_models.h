#pragma once

enum e_physics_constraint : int16
{
	_physics_constraint_hinge = 0,
	_physics_constraint_limited_hinge = 1,
	_physics_constraint_ragdoll = 2,
	_physics_constraint_stiff_spring = 3,
	_physics_constraint_ball_and_socket = 4,
	_physics_constraint_prismatic = 5
};
