#include "stdafx.h"

#include "TagDataIterator.h"

datum tag_iterator_next(tag_iterator* iterator)
{
	typedef datum(__cdecl* tag_iterator_next_t)(tag_iterator* it);
	tag_iterator_next_t p_tag_iterator_next = Memory::GetAddress<tag_iterator_next_t>(0x3172C, 0x255DC);
	return  p_tag_iterator_next(iterator);
}
