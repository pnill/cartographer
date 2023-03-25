#include "stdafx.h"

#include "PhysicsConstants.h"

s_physics_constants* s_physics_constants::get()
{
	return *Memory::GetAddress<s_physics_constants**>(0x4D2AB4, 0x4f696C);
}
float s_physics_constants::get_default_gravity()
{
	return *Memory::GetAddress<float*>(0x3BC9DC, 0x378E50);
}