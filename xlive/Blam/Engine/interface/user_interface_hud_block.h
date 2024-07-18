#pragma once
#include "user_interface_widget.h"
#include "user_interface_screen_widget_definition.h"
#include "game/players.h"


/* classes */

class c_hud_widget : protected c_user_interface_widget
{
protected:
	s_hud_block_reference* m_tag_block;
	s_player_profile_traits m_appearance;
	int16 m_team_index;
	int16 m_rank_index;

public:
	// c_hud_widget virtual functions

	virtual ~c_hud_widget();
	virtual void update() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual int32 get_intro_delay() override;
	virtual c_user_interface_text* get_interface() override;
};
ASSERT_STRUCT_SIZE(c_hud_widget, 0x88);