#pragma once

#include "Blam\Common\Common.h"

#define NONE -1

// floating point definitions
typedef float real;

typedef int datum;

#define DATUM_NONE NONE

#define DATUM_NEW(_absolute_index, _salt) (datum)((_absolute_index) & ((_salt) << 16))

#define DATUM_IS_NONE(_absolute_index) ((int)(_absolute_index) == DATUM_NONE)
#define DATUM_ABSOLUTE_INDEX(_absolute_index) (unsigned short)((_absolute_index) & 0xFFFF)
#define DATUM_SALT(_absolute_index) ((unsigned short)(((_absolute_index) >> 16) & 0xFFFF))
