#pragma once
#include "havok/common/base/types/geometry/sphere/hkSphere.h"
#include "havok/physics/collide/shape/convex/hkConvexShape.h"

class hkCapsuleShape : hkConvexShape
{
	hkSphere m_vertexA;
	hkSphere m_vertexB;
};
