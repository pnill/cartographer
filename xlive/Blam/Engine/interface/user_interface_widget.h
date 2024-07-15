#pragma once
#include "user_interface.h"
#include "math/color_math.h"

/* enums */

enum e_user_interface_widget_type : int32
{
	_widget_type_screen = 0x0,
	_widget_type_list = 0x1,
	_widget_type_list_item = 0x2,
	_widget_type_button = 0x3,
	_widget_type_4 = 0x4,
	_widget_type_table_view = 0x5,
	_widget_type_text = 0x6,
	_widget_type_model = 0x7,
	_widget_type_bitmap = 0x8,
	_widget_type_hud = 0x9,
	_widget_type_player = 0xA,
	k_number_of_user_interface_widget_types
};


/* forward declarations */

struct s_event_record;

class c_user_interface_text;
class c_bitmap_widget;
class c_hud_widget;
class c_text_widget;
class c_player_widget;
class c_model_widget;
class c_screen_widget;

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
	int32 looping_stlye;
	int32 field_18;
	int32 last_frame_time;
	int32 period_milliseconds;
	real32 field_24;
	real32 field_28;
	real32 field_2C;
	real32 current_alpha;
};
ASSERT_STRUCT_SIZE(s_animation_transform, 0x34);

/* classes */

class c_user_interface_widget
{
protected:
	//void* __vtable;
	e_user_interface_widget_type m_widget_type;
	int16 m_controllers_mask;
	int16 m_block_index;
	int32 m_hierarchy_order;
	c_user_interface_widget* parent_widget;
	c_user_interface_widget* child_widget;
	c_user_interface_widget* next_widget;
	c_user_interface_widget* previous_widget;
	rectangle2d m_bounds;
	real_rgb_color m_widget_color;
	s_animation_transform m_current_animation;
	int16 m_animation_index;
	int16 m_render_dept_bias;
	bool m_allocated;
	bool field_6D;
	bool m_visible;
	bool m_can_handle_events;


	void destroy_recursive();
	void initialize_animation(s_animation_transform* animation);

public:
	c_user_interface_widget(e_user_interface_widget_type widget_type, int16 user_flags);

	int16 get_animation_type();
	e_user_interface_widget_type get_type();
	c_user_interface_widget* get_next();
	c_user_interface_widget* get_previous();
	c_user_interface_widget* get_parent();
	c_user_interface_widget* get_children();
	c_user_interface_widget* try_find_child(e_user_interface_widget_type type, uint32 idx, bool recursive_search);
	c_text_widget* try_find_text_widget(uint32 idx);
	c_hud_widget* try_find_hud_widget(uint32 idx);
	c_bitmap_widget* try_find_bitmap_widget(uint32 idx);
	c_player_widget* try_find_player_widget(uint32 idx);
	c_model_widget* try_find_model_widget(uint32 idx);
	c_screen_widget* get_parent_screen();

	void set_visible(bool visible);
	void set_child_visible(e_user_interface_widget_type type, uint32 idx,  bool visible);
	void set_allocated(bool allocated);
	void add_new_child(c_user_interface_widget* child);
	void set_bounds(rectangle2d* bounds);
	void set_controller_mask(uint32 user_mask);
	void start_widget_animation(int32 type);

	
	// c_user_interface_widget virtual functions

	virtual ~c_user_interface_widget();
	virtual int32 setup_children();
	virtual void on_screen_leave();
	virtual void update();
	virtual void render_widget(rectangle2d* viewport_bounds);
	virtual void* get_mouse_region(rectangle2d* mouse_region_out);
	virtual int32 initialize_child_animations(s_animation_transform* a2);
	virtual int32 get_intro_delay();
	virtual void* get_unprojected_bounds(rectangle2d* unprojected_bounds);
	virtual void sub_612A7C(c_user_interface_widget* a2);
	virtual c_user_interface_widget* sub_612ABC();
	virtual c_user_interface_widget* sub_612BCA();
	virtual bool handle_event(s_event_record* event);
	virtual e_user_interface_channel_type get_parent_channel();
	virtual e_user_interface_render_window get_parent_render_window();
	virtual void construct_animation_on_region_enter(int32 a1);
	virtual void construct_animation_on_region_leave(int32 a1);
	virtual c_user_interface_widget* sub_6121F6(rectangle2d* point);
	virtual bool can_interact();
	virtual c_user_interface_text* get_interface() = 0;
	virtual bool sub_6114B9();
};
ASSERT_STRUCT_SIZE(c_user_interface_widget, 0x70);

