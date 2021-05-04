#include "PhysicsConstants.h"

physics_constants* physics_constants::get()
{
	return *Memory::GetAddress<physics_constants**>(0x4D2AB4, 0x4f696C);
}
