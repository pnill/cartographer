#pragma once
#include "havok/common/base/math/vector/hkVector4.h"
#include "havok/physics/collide/shape/heightfeild/hkpSphereRepShape.h"

struct hkMultiSphereShape : hkSphereRepShape
{
	hkInt32 m_numSpheres;
	hkVector4 m_spheres[8];
};
