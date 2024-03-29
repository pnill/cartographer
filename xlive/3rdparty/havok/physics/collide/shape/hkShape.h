#pragma once
#include "havok/serialize/hkSerializable.h"

struct hkShape : hkSerializable
{
	int m_userData;
};
