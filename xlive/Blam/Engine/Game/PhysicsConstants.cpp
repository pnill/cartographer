#include "stdafx.h"

#include "PhysicsConstants.h"

physics_constants* physics_constants::get()
{
	return *Memory::GetAddress<physics_constants**>(0x4D2AB4, 0x4f696C);
}
float physics_constants::get_default_gravity()
{
	return *Memory::GetAddress<float*>(0x3BC9DC, 0x378E50);
}