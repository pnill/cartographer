#pragma once
#include "havok/common/base/container/array/hkArray.h"
#include "havok/common/base/types/hkBaseTypes.h"
#include "havok/physics/collide/shape/hkShape.h"
#include "havok/physics/collide/shape/compund/hkShapeCollection.h"

template<unsigned N>
struct hkListShape : hkShapeCollection
{
	struct ChildInfo
	{
		hkShape* m_shape;
		hkUint32 m_collisionFilterInfo;
	};
	hkInplaceArray<hkListShape::ChildInfo, N> m_childInfo;
};