#pragma once
namespace SpecialEvents
{
	enum e_event_type
	{
		_no_event,
		_christmas,
		_st_paddys,
		_mook_maddness,
		_halloween
	};
	e_event_type getCurrentEvent();
	void ChristmasOnMapLoad();
	void PaddysOnMapLoad();
	void AddNewMarkers();
	void ApplyHooks();
	void Initialize();
}