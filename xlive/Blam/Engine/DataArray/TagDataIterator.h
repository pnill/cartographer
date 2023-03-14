#pragma once
#include "Blam/Cache/DataTypes/BlamTag.h"

struct tag_iterator
{
	DWORD dword0;
	DWORD dword4;
	datum current_tag_datum;
	datum next_tag_datum;
	blam_tag tag_type;
};
static_assert(sizeof(tag_iterator) == 20, "Size incorrect");

datum tag_iterator_next(tag_iterator*);