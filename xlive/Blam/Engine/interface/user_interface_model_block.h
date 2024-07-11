#pragma once
#include "user_interface_widget.h"
#include "user_interface_screen_widget_definition.h"
#include "game/players.h"


/* classes */

class c_model_widget : public c_user_interface_widget
{
protected:
	s_ui_model_scene_reference* m_tag_block;

public:
	void apply_appearance_and_character(s_player_profile_traits* appearance, e_character_type character);

	// c_model_widget virtual functions

	virtual ~c_model_widget();
	virtual int setup_children() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual int get_intro_delay() override;
	virtual c_user_interface_text* get_interface() override;
};
ASSERT_STRUCT_SIZE(c_model_widget, 0x74);