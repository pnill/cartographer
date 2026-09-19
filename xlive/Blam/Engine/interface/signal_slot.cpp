#include "stdafx.h"
#include "signal_slot.h"

/* public code */

void _slot_linker::link(
	_slot* slot)
{
	//INVOKE_TYPE(0x2113D3, 0x0, void(__thiscall*)(_slot_linker*, _slot*), this, slot);

	_slot* old;

	ASSERT(slot);
	vassert(slot->m_signal == NULL, "cannot connect more than one signal to a slot", NULL);
	ASSERT(slot->m_next == NULL);
	ASSERT(slot->m_previous == NULL);

	slot->m_signal = this;
	old = m_current;

	if (m_current)
	{
		while (old->m_next)
		{
			old = old->m_next;
		}

		old->m_next = slot;
		slot->m_previous = old;
	}
	else
	{
		m_current = slot;
	}

	return;
}
