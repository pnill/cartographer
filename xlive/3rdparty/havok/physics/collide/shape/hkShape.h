#pragma once
#include "havok/serialize/hkSerializable.h"

struct hkShape : hkSerializable
{
	hkInt32 m_userData;
};
