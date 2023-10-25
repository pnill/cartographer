#pragma once

enum e_special_event_type
{
	_special_event_none = 0,
	_special_event_christmas = 1,
	_special_event_st_paddys = 2,
	_special_event_mook_maddness = 3,
	_special_event_halloween = 4,
	_special_event_birthday = 5,
	k_special_event_count
};
e_special_event_type get_current_special_event();
void load_special_event();
