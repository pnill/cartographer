#pragma once

#include "H2MOD/Modules/CustomMenu/CustomMenuGlobals.h"

#include "interface/user_interface_headers.h"

class c_brightness_level_edit_list : public c_list_widget
{
public:
	c_list_item_widget m_edit_list[4];
	uint8 edit_list_info[28];

	// todo maybe implement the actual constructor

	c_brightness_level_edit_list(uint16 user_flags) :
		c_list_widget(user_flags)
	{
	}

	// overrides, only needed ones
	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 0, c_user_interface_widget * (class_type::**)(uint32), flags);
	}

	virtual int32 setup_children() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 1, int32(class_type::**)());
	}

	virtual int32 link_item_widgets() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 21, int32(class_type::**)());
	}

	virtual c_list_item_widget* get_list_items() override
	{
		return m_edit_list;
	}

	virtual int32 get_list_items_count() override
	{
		return NUMBEROF(m_edit_list);
	}

	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 24, void(class_type::**)(c_list_item_widget*, int32), item, skin_index);
	}

	virtual bool verify_item_in_focus(c_list_item_widget* item) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 25, bool(class_type::**)(c_list_item_widget*), item);
	}

private:
	typedef c_brightness_level_edit_list class_type;

	template<typename T>
	static T _get_vfptr_table(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x4D972C)[idx]);
	}
};

static_assert(offsetof(c_brightness_level_edit_list, m_edit_list) == 176);

class c_brightness_menu : protected c_screen_with_menu
{
public:
	c_brightness_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);

	~c_brightness_menu() = default;

	typedef c_brightness_menu class_type;

	// interface
	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_brightness_menu();
		if (TEST_BIT(flags, 0))
		{

		}

		return this;
	};

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
		return INVOKE_VFPTR_FN(_get_vfptr_table, 5, void*(class_type::**)(rectangle2d*), mouse_region_out);
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
		return INVOKE_VFPTR_FN(_get_vfptr_table, 8, void*(class_type::**)(rectangle2d*), unprojected_bounds);
	}

	virtual void sub_612A7C(c_user_interface_widget* a2) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 9, void(class_type::**)(c_user_interface_widget*), a2);
	}

	virtual c_user_interface_widget* sub_612ABC() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 10, c_user_interface_widget*(class_type::**)());
	}

	virtual c_user_interface_widget* sub_612BCA() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 11, c_user_interface_widget*(class_type::**)());
	}

	virtual bool handle_event(s_event_record* event) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 12, bool(class_type::**)(s_event_record*), event);
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
		return INVOKE_VFPTR_FN(_get_vfptr_table, 17, c_user_interface_widget*(class_type::**)(rectangle2d*), point);
	}

	virtual bool can_interact() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 18, bool(class_type::**)());
	}

	virtual c_user_interface_text* get_interface() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 19, c_user_interface_text*(class_type::**)());
	}

	virtual bool sub_6114B9() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 20, bool(class_type::**)());
	}

	virtual void sub_60E884() override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 21, void(class_type::**)());
	}

	virtual void initialize(s_screen_parameters* parameters) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 22, void(class_type::**)(s_screen_parameters*), parameters);
	}

	virtual void post_initialize() override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 23, void(class_type::**)());
	}

	virtual void setup_special_widgets() override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 24, void(class_type::**)());
	}

	virtual c_user_interface_widget* sub_6102C5() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 25, c_user_interface_widget*(class_type::**)());
	}

	virtual uint8 sub_6103D6() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 26, uint8(class_type::**)());
	}

	virtual int32 sub_60F1F4(s_event_record* a2) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 27, int32(class_type::**)(s_event_record*), a2);
	}

	virtual uint8 sub_60EFC1(s_event_record* event) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 28, uint8(class_type::**)(s_event_record*), event);
	}

	virtual int32 sub_60F081(s_event_record* a2) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 29, int32(class_type::**)(s_event_record*), a2);
	}

	virtual int32 sub_60F151(int32 a2) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 30, int32(class_type::**)(int32), a2);
	}

	virtual bool sub_40AD53(int32 a2) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 31, bool(class_type::**)(int32), a2);
	}

	virtual e_user_interface_channel_type get_channel() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 32, e_user_interface_channel_type(class_type::**)());
	}

	virtual e_user_interface_render_window get_render_window() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 33, e_user_interface_render_window(class_type::**)());
	}

	virtual int32 sub_60EB92(int32 a2) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 34, int32(class_type::**)(int32), a2);
	}

	virtual void sub_60EBC2(int32 a2) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 35, void(class_type::**)(int32), a2);
	}

	virtual void* sub_60EC5C(s_screen_state* state) override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 36, void*(class_type::**)(s_screen_state*), state);
	}

	virtual void sub_60ECC9(s_screen_state* state) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 37, void(class_type::**)(s_screen_state*), state);
	}

	virtual void* load_proc() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 38, void*(class_type::**)());
	}

	virtual bool overlay_effect_is_disabled() override
	{
		return INVOKE_VFPTR_FN(_get_vfptr_table, 39, bool(class_type::**)());
	}

	virtual void sub_60F2A4(uint8 bitmap_index) override
	{
		INVOKE_VFPTR_FN(_get_vfptr_table, 40, void(class_type::**)());
	}

	c_brightness_level_edit_list m_list_widgets;
	
	static void* open_brightness_menu(s_screen_parameters* a1);

private:
	// reserved name
	template<typename T>
	static T _get_vfptr_table(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x4D7ABC)[idx]);
	}
};

static_assert(sizeof(c_brightness_menu) == 3388);
static_assert(offsetof(c_brightness_menu, m_list_widgets) == 2656);

void replace_brightness_menu();
