#pragma once

enum e_language : uint32
{
	_language_english = 0,
	_language_japanese = 1,
	_language_german = 2,
	_language_french = 3,
	_language_spanish = 4,
	_language_italian = 5,
	_language_korean = 6,
	_language_chinese = 7,
	_language_portuguese = 8,
	k_language_count
};

e_language get_current_language(void);
