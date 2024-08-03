#pragma once

#include "interface/user_interface_headers.h"

// base class of many menus
class c_screen_with_menu : protected c_screen_widget
{
protected:
	void* m_button_list;

public:

	c_screen_with_menu(e_user_interface_screen_id _menu_id, e_user_interface_channel_type _channel_type, e_user_interface_render_window _window_index, uint16 _user_flags, void* _item_edit_list) :
		c_screen_widget(_menu_id, _channel_type, _window_index, _user_flags)
	{
		m_button_list = _item_edit_list;
	}

	// base interface overrides
	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_screen_with_menu();

		return this;
	}

	virtual void initialize(s_screen_parameters* parameters) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 22, void(class_type::**)(s_screen_parameters*), parameters);
	}

	// member functions
	void __thiscall build_player_list(void* a1, int player_count);

	// static member functions
	static void apply_patches();

private:
	typedef c_screen_with_menu class_type;

	template<typename T>
	static T _get_vfptr_table(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x3CF3A4)[idx]);
	}
};
static_assert(sizeof(c_screen_with_menu) == 2656);
