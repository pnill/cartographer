#pragma once
#include "cseries/cseries.h"

/* macro defines*/

/* forward declarations */

struct s_event_record;

class c_screen_widget;
class c_list_widget;

class _slot;

/* structures */

class _slot
{
	friend class _slot_linker;

protected:
	_slot* m_previous;
	_slot* m_next;
	_slot_linker* m_signal;

public:
	_slot()
	{
		m_previous = nullptr;
		m_next = nullptr;
		m_signal = nullptr;
	}
};
ASSERT_STRUCT_SIZE(_slot, 0xC);

template <typename type = short>
class _slot1 : public _slot
{
public:
};

template <class X = s_event_record*, typename type = short>
class _slot2 : public _slot
{
public:
};

// generally used by c_screen_widget
template <class X = c_user_interface_widget, typename type = short>
class c_slot1 : public _slot1<type>
{
	typedef void(X::* handler_t)(type*);

	X* m_class_ptr;
	handler_t m_handler;

public:
	c_slot1()
	{
		m_class_ptr = nullptr;
		m_handler = nullptr;
	}
	c_slot1(X* _class, handler_t handler)
	{

		m_class_ptr = _class;
		m_handler = handler;
	}
	virtual void event_handler(type* id)
	{
		return INVOKE_CLASS_FN(m_class_ptr, m_handler) (id);
	}
};
//ASSERT_STRUCT_SIZE(class c_slot1<c_screen_widget,long>, 0x18);


// generally used by c_list_widget
template <class X = c_list_widget, typename Y = s_event_record*, typename type = int16>
class c_slot2 : public _slot2<Y, type>
{
	typedef void(X::* handler_t)(Y, type*);

	X* m_class_ptr;
	handler_t m_handler;

public:
	c_slot2()
	{
		m_class_ptr = nullptr;
		m_handler = nullptr;
	}
	c_slot2(X* _class, handler_t handler)
	{
		m_class_ptr = _class;
		m_handler = handler;
	}
	virtual void event_handler(Y event, type* id)
	{
		return INVOKE_CLASS_FN(m_class_ptr, m_handler) (event, id);
	}
};
//ASSERT_STRUCT_SIZE(class c_slot2<class c_search_option_max_players_edit_list, struct s_event_record *, long>, 0x18);

class _slot_linker
{
	_slot* m_current;

public:
	_slot_linker()
	{
		m_current = nullptr;
	}

	void link(_slot* slot)
	{
		//INVOKE_TYPE(0x2113D3, 0x0, void(__thiscall*)(_slot*, _slot*), this, slot);
		slot->m_signal = this;
		_slot* old = m_current;
		if (m_current)
		{
			while (old->m_next)
				old = old->m_next;
			old->m_next = slot;
			slot->m_previous = old;
		}
		else
		{
			m_current = slot;
		}
	}
};
