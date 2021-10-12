#pragma once
namespace SpecialEvents
{
	enum e_event_type
	{
		e_none,
		e_christmas,
		e_st_paddys,
		e_mook_maddness,
		e_halloween
	};
	e_event_type getCurrentEvent();
	void ChristmasOnMapLoad();
	void PaddysOnMapLoad();
	void AddNewMarkers();
	void ApplyHooks();
	void Initialize();
}