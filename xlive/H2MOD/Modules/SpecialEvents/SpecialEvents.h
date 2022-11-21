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
	void ChristmasOnMapLoad();
	void PaddysOnMapLoad();
	void BirthdayOnMapLoad();
	void AddNewMarkers();
	void ApplyHooks();
	void Initialize();

	struct s_event_scenery_placement
	{
		short type;
		short variant_id;
		real_point3d position;
		real_vector3d rotation;
		float scale;
	};
}