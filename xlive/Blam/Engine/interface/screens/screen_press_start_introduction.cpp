#include "stdafx.h"
#include "screen_press_start_introduction.h"
#include "interface/user_interface_memory.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"

// typedef

typedef void(__cdecl* t_screen_press_start_introduction_open)(s_screen_parameters* parameters);
t_screen_press_start_introduction_open p_screen_press_start_introduction_open;

c_screen_press_start_introduction::c_screen_press_start_introduction(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags) :
	c_screen_widget(_screen_press_start_intro, channel_type, window_index, user_flags),
	m_start_button(0, user_flags),
	m_slot(this, &c_screen_press_start_introduction::handle_item_pressed_event)
{
	this->m_has_input_saved = false;
}

c_screen_press_start_introduction::~c_screen_press_start_introduction()
{
}

void c_screen_press_start_introduction::update()
{
	INVOKE_TYPE(0x23F2FD, 0x0, void(__thiscall*)(c_screen_press_start_introduction*), this);
}

bool c_screen_press_start_introduction::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x23F0BA, 0x0, bool(__thiscall*)(c_screen_press_start_introduction*, s_event_record*), this, event);
}

void c_screen_press_start_introduction::initialize(s_screen_parameters* parameters)
{
	if (UserSignedIn(0)) {
		XUserSignOut(0);
		UpdateMasterLoginStatus();
	}
	
	//orignal c_screen_press_start_introduction::initialize
	INVOKE_TYPE(0x23F180, 0x0, void(__thiscall*)(c_screen_press_start_introduction*, s_screen_parameters*), this, parameters);
}

__declspec(naked) void press_start_introduction_init() { __asm jmp c_screen_press_start_introduction::initialize }

void c_screen_press_start_introduction::sub_60EBC2(int32 a1)
{
	INVOKE_TYPE(0x23F011, 0x0, void(__thiscall*)(c_screen_press_start_introduction*, int32), this, a1);
}

void* c_screen_press_start_introduction::load_proc()
{
	return &c_screen_press_start_introduction::load;
}

void c_screen_press_start_introduction::handle_item_pressed_event(s_event_record** pevent, datum* pitem_index)
{
	s_event_record* arg = *pevent;

	this->m_saved_input.type = arg->type;
	this->m_saved_input.controller = arg->controller;
	this->m_saved_input.component = arg->component;
	this->m_saved_input.value = arg->value;

	this->m_has_input_saved = true;

}

void* c_screen_press_start_introduction::load(s_screen_parameters* parameters)
{
	//return p_screen_press_start_introduction_open(parameters);

	c_screen_press_start_introduction* screen;

	parameters->m_flags |= 4u;
	void* pool = ui_pool_allocate_space(sizeof(c_screen_press_start_introduction), 0);
	if (pool)
	{
		screen = new (pool) c_screen_press_start_introduction(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->user_flags);

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
