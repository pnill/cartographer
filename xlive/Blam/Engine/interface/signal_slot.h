#pragma once

/* classes */

class _slot
{
	friend class _slot_linker;

protected:
	class _slot* m_previous;
	class _slot* m_next;
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

template <class X = struct s_event_record*, typename type = short>
class _slot2 : public _slot
{
public:
};

// generally used by c_screen_widget
template <class X = class c_user_interface_widget, typename type = short>
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
template <class X = class c_list_widget, typename Y = struct s_event_record * const, typename type = int16>
class c_slot2 : public _slot2<Y, type>
{
	typedef void(X::* handler_t)(Y const &, type*);

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
	virtual void event_handler(Y const & event, type* id)
	{
		return INVOKE_CLASS_FN(m_class_ptr, m_handler)(event, id);
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

	void link(_slot* slot);
};
