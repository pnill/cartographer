#pragma once
#include "cseries/cseries.h"

/* macro defines*/

#define INVOKE_CLASS_FN(classobj,functionPtr)  ((classobj)->*(functionPtr))


/* forward declarations */

struct s_event_record;

class c_screen_widget;
class c_list_widget;


/* structures */

class _slot
{

protected:
	_slot* m_previous;
	_slot* m_next;
	void* m_signal;

public:
	_slot()
	{
		m_previous = nullptr;
		m_next = nullptr;
		m_signal = nullptr;
	}

	void link(_slot* slot)
	{
		//INVOKE_TYPE(0x2113D3, 0x0, void(__thiscall*)(_slot*, _slot*), this, slot);
		slot->m_signal = this;
		_slot* old = this->m_previous;
		if (this->m_previous)
		{
			while (old->m_next)
				old = old->m_next;
			old->m_next = slot;
			slot->m_previous = old;
		}
		else
		{
			this->m_previous = slot;
		}
	}
};
ASSERT_STRUCT_SIZE(_slot, 0xC);




template <typename type = short>
class _slot1 :_slot
{
public:
	template <class T>
	static void link_signal_to_slot(_slot* signal, T* slot)
	{
		signal->link((_slot*)slot);
	}
};

template <class X = s_event_record*, typename type = short>
class _slot2 :_slot
{
public:
	template <class T>
	static void link_signal_to_slot(_slot* signal,T* slot)
	{
		signal->link((_slot*)slot);
	}
};

// generally used by c_screen_widget
template <class X = c_user_interface_widget, typename type = short>
class c_slot1 : public _slot1<type>
{
	typedef bool(X::* handler_t)(type*);

	X* m_class_ptr;
	handler_t m_handler;

public:
	c_slot1()
	{
		m_class_ptr = 0;
		m_handler = 0;
	}
	c_slot1(X* _class, handler_t handler)
	{

		m_class_ptr = _class;
		m_handler = handler;
	}
	virtual char event_handler(type* id)
	{
		return INVOKE_CLASS_FN(m_class_ptr, m_handler) (id);
	}
};
//ASSERT_STRUCT_SIZE(class c_slot1<c_screen_widget,long>, 0x18);


// generally used by c_list_widget
template <class X = c_list_widget, typename Y = s_event_record*, typename type = short>
class c_slot2 : public _slot2<Y, type>
{
	typedef bool(X::* handler_t)(Y*, type*);

	X* m_class_ptr;
	handler_t m_handler;

public:
	c_slot2()
	{
		m_class_ptr = 0;
		m_handler = 0;
	}
	c_slot2(X* _class, handler_t handler)
	{

		m_class_ptr = _class;
		m_handler = handler;
	}
	virtual char event_handler(Y* event, type* id)
	{
		return INVOKE_CLASS_FN(m_class_ptr, m_handler) (event, id);
	}
};
//ASSERT_STRUCT_SIZE(class c_slot2<class c_search_option_max_players_edit_list, struct s_event_record *, long>, 0x18);