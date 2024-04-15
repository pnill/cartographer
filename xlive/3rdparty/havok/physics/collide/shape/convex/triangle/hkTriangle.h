#pragma once
#include "havok/common/base/math/vector/hkVector4.h"
#include "havok/physics/collide/shape/convex/hkConvexShape.h"

struct hkTriangleShape : hkConvexShape
{
	hkVector4 m_vertexA;
	hkVector4 m_vertexB;
	hkVector4 m_vertexC;
};
