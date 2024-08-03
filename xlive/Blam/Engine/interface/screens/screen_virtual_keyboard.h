#pragma once

#include "interface/user_interface_headers.h"

#include "H2MOD/Modules/CustomMenu/CustomMenuGlobals.h"

#define VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX 17

// we just re-use the first seventeen keyboard id's in default code for now
#define VIRTUAL_KEYBOARD_MENU_TYPE_MAX_NEW (VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX * 2)

#define VIRTUAL_KEYBOARD_BUTTON_COUNT 47

void ui_globals_set_keyboard_input_state(bool state);

class c_virtual_keyboard_button : public c_button_widget
{
protected:
	// ### TODO cleanup this
	bool handle_virtual_keyboard_event(s_event_record* a2);

public:

	typedef c_virtual_keyboard_button class_type;

	c_virtual_keyboard_button() :
		c_button_widget(NONE, 0)
	{
	}

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_virtual_keyboard_button();
		if (TEST_BIT(flags, 0))
		{
		}

		return this;
	}

	virtual int32 setup_children() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 1, int32(class_type::**)());
	}

	virtual void on_screen_leave() override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 2, void(class_type::**)());
	}

	virtual void update() override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 3, void(class_type::**)());
	}

	virtual void render_widget(rectangle2d* viewport_bounds) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 4, void(class_type::**)(rectangle2d*), viewport_bounds);
	}

	virtual void* get_mouse_region(rectangle2d* mouse_region_out) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 5, void* (class_type::**)(rectangle2d*), mouse_region_out);
	}

	virtual int32 initialize_child_animations(s_animation_transform* a2) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 6, int32(class_type::**)(s_animation_transform*), a2);
	}

	virtual int32 get_intro_delay() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 7, int32(class_type::**)());
	}

	virtual void* get_unprojected_bounds(rectangle2d* unprojected_bounds) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 8, void* (class_type::**)(rectangle2d*), unprojected_bounds);
	}

	virtual void sub_612A7C(c_user_interface_widget* a2) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 9, void(class_type::**)(c_user_interface_widget*), a2);
	}

	virtual c_user_interface_widget* sub_612ABC() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 10, c_user_interface_widget * (class_type::**)());
	}

	virtual c_user_interface_widget* sub_612BCA() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 11, c_user_interface_widget * (class_type::**)());
	}

	virtual bool handle_event(s_event_record* event) override
	{
		return this->handle_virtual_keyboard_event(event);

		//return INVOKE_VFPTR_FN(_get_vfptr_table, 12, bool(class_type::**)(s_event_record*), event);
	}

	virtual e_user_interface_channel_type get_parent_channel() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 13, e_user_interface_channel_type(class_type::**)());
	}

	virtual e_user_interface_render_window get_parent_render_window() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 14, e_user_interface_render_window(class_type::**)());
	}

	virtual void construct_animation_on_region_enter(int32 a1) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 15, void(class_type::**)(int32), a1);
	}

	virtual void construct_animation_on_region_leave(int32 a1) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 16, void(class_type::**)(int32), a1);
	}

	virtual c_user_interface_widget* sub_6121F6(rectangle2d* point) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 17, c_user_interface_widget * (class_type::**)(rectangle2d*), point);
	}

	virtual bool can_interact() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 18, bool(class_type::**)());
	}

	virtual c_user_interface_text* get_interface() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 19, c_user_interface_text * (class_type::**)());
	}

	virtual bool sub_6114B9() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 20, bool(class_type::**)());
	}

	// c_button_widget additions

	virtual void setup_texts() override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 21, void(class_type::**)());
	}

	virtual int32 get_bitmap_active_index() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 22, int32(class_type::**)());
	}

private:

	// reserved name
	template<typename T>
	static T _get_vfptr_table(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x3D30D4)[idx]);
	}
};

class c_virtual_keyboard_menu : protected c_screen_widget
{
protected:
	int field_A5C;
	int field_A60;
	uint8 field_A64[512];
	int field_C64;
	int field_C68;
	int16 field_C6C;
	int16 field_C6E;
	int16 field_C70;
	int16 field_C72;
	char field_C74;
	c_virtual_keyboard_button m_virtual_keyboard_buttons[VIRTUAL_KEYBOARD_BUTTON_COUNT];
	int field_3B78;
	char field_3B7C;
	char field_3B7D;

public:
	static void* __cdecl open(s_screen_parameters* parameters);

	// TODO list initializer once the ctors are re-implemented
	c_virtual_keyboard_menu(e_user_interface_channel_type _channel_type, e_user_interface_render_window _window_index, uint16 _user_flags) :
		c_screen_widget(_screen_virtual_keyboard, _channel_type, _window_index, _user_flags)
	{
		field_A5C = NONE;
		field_A60 = NONE;
		csmemset(field_A64, 0, sizeof(field_A64));
		field_C64 = 0;
		field_C68 = 0;
		field_C6C = 0;
		field_C6E = 4;
		field_C70 = 10;
		field_C72 = 0;
		field_C74 = 0;

		field_3B78 = NONE;
		field_3B7C = 1;
		field_3B7D = 0;

		for (int32 i = 0; i < NUMBEROF(m_virtual_keyboard_buttons); i++)
		{
			m_virtual_keyboard_buttons[i].m_button_index = i;
			m_virtual_keyboard_buttons[i].m_controllers_mask = _user_flags;
		}

		ui_globals_set_keyboard_input_state(true);
	}

	~c_virtual_keyboard_menu()
	{
		ui_globals_set_keyboard_input_state(false);
	}

	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_virtual_keyboard_menu();
		if (TEST_BIT(flags, 0))
		{
		}

		return this;
	}

	virtual void render_widget(rectangle2d* viewport_bounds)
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 4, void(class_type::**)(rectangle2d * viewport_bounds), viewport_bounds);
	}

	virtual bool handle_event(s_event_record* event) override;

	virtual void initialize(s_screen_parameters* parameters);

	virtual void* load_proc() override
	{
		return c_virtual_keyboard_menu::open;
	}

	void __thiscall set_input_string_buffer(wchar_t* buffer, size_t buffer_size);

private:
	typedef c_screen_widget class_type;

	template<typename T>
	static T _get_vfptr_table(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x3D302C)[idx]);
	}
};
ASSERT_STRUCT_SIZE(c_virtual_keyboard_menu, 15232);

void CustomMenuCall_VKeyboard_Inner(wchar_t* textBuffer, __int16 textBufferLen, int menuType);