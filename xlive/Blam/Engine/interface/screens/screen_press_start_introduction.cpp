#include "stdafx.h"
#include "screen_press_start_introduction.h"

#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"

// typedef

typedef void(__cdecl* t_screen_press_start_introduction_open)(c_screen_parameters* parameters);
t_screen_press_start_introduction_open p_screen_press_start_introduction_open;

c_screen_press_start_introduction::c_screen_press_start_introduction(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags) :
	c_screen_widget(_screen_press_start_intro, channel_type, window_index, user_flags),
	m_start_button(0, user_flags),
	m_slot(this, &c_screen_press_start_introduction::handle_item_pressed_event)
{
	this->m_has_input_saved = false;
}

void c_screen_press_start_introduction::update()
{
	INVOKE_TYPE(0x23F2FD, 0x0, void(__thiscall*)(c_screen_press_start_introduction*), this);
}

bool c_screen_press_start_introduction::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x23F0BA, 0x0, bool(__thiscall*)(c_screen_press_start_introduction*, s_event_record*), this, event);
}

CLASS_HOOK_DECLARE_LABEL(c_screen_press_start_introduction__initialize, c_screen_press_start_introduction::initialize);
void c_screen_press_start_introduction::initialize(
	c_screen_parameters const* parameters)
{
	if (XUserSignedIn(0))
	{
		XUserSignOut(0);
		UpdateMasterLoginStatus();
	}
	
	//orignal c_screen_press_start_introduction::initialize
	INVOKE_TYPE(0x23F180, 0x0, void(__thiscall*)(c_screen_press_start_introduction*, c_screen_parameters const*), this, parameters);
}

void c_screen_press_start_introduction::sub_60EBC2(int32 a1)
{
	INVOKE_TYPE(0x23F011, 0x0, void(__thiscall*)(c_screen_press_start_introduction*, int32), this, a1);
}

const void* c_screen_press_start_introduction::load_proc(void) const
{
	return &c_screen_press_start_introduction::load;
}

void c_screen_press_start_introduction::handle_item_pressed_event(
	s_event_record* const& event,
	datum* pitem_index)
{
	m_saved_input = *event;
	m_has_input_saved = true;
	
	return;
}

void* c_screen_press_start_introduction::load(
	c_screen_parameters* parameters)
{
	//return p_screen_press_start_introduction_open(parameters);

	c_screen_press_start_introduction* screen;

	parameters->set_user_flag(2);

	void* pool = ui_pool_allocate_space(sizeof(c_screen_press_start_introduction), 0);
	if (pool)
	{
		screen = new (pool) c_screen_press_start_introduction(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags()
		);

		screen->m_allocated = true;
		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		screen = 0;
	}

	return screen;
}


void c_screen_press_start_introduction::apply_instance_patches()
{
	DETOUR_ATTACH(p_screen_press_start_introduction_open, Memory::GetAddress<t_screen_press_start_introduction_open>(0x23f6B7), c_screen_press_start_introduction::load);
}
