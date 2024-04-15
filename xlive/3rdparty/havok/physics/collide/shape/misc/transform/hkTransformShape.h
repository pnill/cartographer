#pragma once
#include "havok/common/base/math/matrix/hkTransform.h"
#include "havok/physics/collide/shape/hkShape.h"

struct hkTransformShape : hkShape
{
	const hkShape* m_childShape;
	hkTransform m_transform;
};
