#pragma once
#include "input/controllers.h"

/* enums */

enum e_user_interface_widget_type 
{
	_widget_type_screen = 0,
	_widget_type_list,
	_widget_type_list_item,
	_widget_type_button,
	_widget_type_4,
	_widget_type_table_view,
	_widget_type_text,
	_widget_type_model,
	_widget_type_bitmap,
	_widget_type_hud,
	_widget_type_player,
	k_number_of_user_interface_widget_types
};

/* structures */

struct s_animation_transform
{
	int32 field_0;
	int32 keyframes_tag_blocks;
	int16 number_of_keyframes;
	int16 last_keyframe_index;
	int16 direction;
	uint16 m_flags;
	int32 intro_delay_ms;
	int32 looping_style;
	int32 field_18;
	int32 last_frame_time;
	int32 period_milliseconds;
	real32 field_24;
	real32 field_28;
	real32 field_2C;
	real32 current_alpha;
};
ASSERT_STRUCT_SIZE(s_animation_transform, 0x34);

// max count: k_maximum_number_of_screen_widgets
struct s_user_interface_widget_reference
{
	tag_reference widget;	// wgit
};
ASSERT_STRUCT_SIZE(s_user_interface_widget_reference, 8);

/* classes */

class c_user_interface_widget
{
protected:
	//void* __vtable;
	e_user_interface_widget_type m_widget_type;
	int16 m_controllers_mask;
	int16 m_block_index;
	int32 m_hierarchy_order;
	c_user_interface_widget* m_parent_widget;
	c_user_interface_widget* m_child_widget;
	c_user_interface_widget* m_next_widget;
	c_user_interface_widget* m_previous_widget;
	rectangle2d m_bounds;
	real_rgb_color m_widget_color;
	s_animation_transform m_current_animation;
	int16 m_animation_index;
	int16 m_render_dept_bias;
	bool m_allocated;
	bool field_6D;
	bool m_visible;
	bool m_can_handle_events;

	void destroy_recursive(void);
	void initialize_animation(s_animation_transform* animation);

public:
	c_user_interface_widget(e_user_interface_widget_type widget_type, uint16 user_flags);

	e_controller_index get_any_responding_controller(void) const;
	int16 get_animation_type(void) const;
	e_user_interface_widget_type get_type(void) const;
	c_user_interface_widget* get_next(void) const;
	c_user_interface_widget* get_previous(void) const;
	c_user_interface_widget* get_parent(void) const;
	c_user_interface_widget* get_children(void) const;
	c_user_interface_widget* try_find_child(e_user_interface_widget_type type, uint32 idx, bool recursive_search);
	class c_text_widget* try_find_text_widget(uint32 idx);
	class c_hud_widget* try_find_hud_widget(uint32 idx);
	class c_bitmap_widget* try_find_bitmap_widget(uint32 idx);
	class c_player_widget* try_find_player_widget(uint32 idx);
	class c_model_widget* try_find_model_widget(uint32 idx);
	class c_screen_widget* get_parent_screen(void);

	void set_visible(bool visible);
	void set_child_visible(e_user_interface_widget_type type, uint32 idx,  bool visible);
	void set_allocated(bool allocated);
	void add_new_child(c_user_interface_widget* child);
	void get_bounds(rectangle2d* bounds) const;
	void set_bounds(rectangle2d const* bounds);
	void set_controller_mask(uint32 user_mask);
	void set_controller_mask_recursive(uint32 user_mask);
	void start_widget_animation(int32 type);
	void set_change_color(real_rgb_color const* new_color);

	
	// c_user_interface_widget virtual functions

	virtual ~c_user_interface_widget(void);
	virtual void setup_children(void);
	virtual void pre_destroy(void);
	virtual void update(void);
	virtual void render_widget(rectangle2d* viewport_bounds);
	virtual void* get_mouse_region(rectangle2d* mouse_region_out);
	virtual int32 initialize_child_animations(s_animation_transform* a2);
	virtual int32 get_intro_delay(void);
	virtual void* get_unprojected_bounds(rectangle2d* unprojected_bounds);
	virtual void sub_612A7C(c_user_interface_widget* child);
	virtual c_user_interface_widget* sub_612ABC(void);
	virtual c_user_interface_widget* sub_612BCA(void);
	virtual bool handle_event(struct s_event_record* event);
	virtual enum e_user_interface_channel_type get_parent_channel(void);
	virtual enum e_user_interface_render_window get_parent_render_window(void);
	virtual void construct_animation_on_region_enter(int32 a1);
	virtual void construct_animation_on_region_leave(int32 a1);
	virtual c_user_interface_widget* sub_6121F6(rectangle2d* point);
	virtual bool can_interact(void);
	virtual class c_user_interface_text* get_interface(void) = 0;
	virtual bool sub_6114B9(void);
};
ASSERT_STRUCT_SIZE(c_user_interface_widget, 0x70);

