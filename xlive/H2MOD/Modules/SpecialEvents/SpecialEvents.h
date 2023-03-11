#pragma once

enum e_special_event_type
{
	_no_event,
	_christmas,
	_st_paddys,
	_mook_maddness,
	_halloween,
	_birthday
};
e_special_event_type get_current_special_event();
void load_special_event();
