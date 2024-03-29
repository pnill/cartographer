#pragma once
#include "hkRotation.h"
#include "havok/common/base/math/vector/hkVector4.h"


struct hkTransform
{
	hkRotation m_rotation;
	hkVector4 m_translation;
};
