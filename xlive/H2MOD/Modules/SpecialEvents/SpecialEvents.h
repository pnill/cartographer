#pragma once

#include "Blam/Engine/Game/math/real_math.h"

namespace SpecialEvents
{
	enum e_event_type
	{
		_no_event,
		_christmas,
		_st_paddys,
		_mook_maddness,
		_halloween,
		_birthday
	};
	e_event_type getCurrentEvent();
	void Initialize();
}