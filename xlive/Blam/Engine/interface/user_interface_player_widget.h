#pragma once
#include "user_interface_group_widget.h"
#include "user_interface_screen_widget_definition.h"
#include "game/game_allegiance.h"

/* classes */

class c_player_widget : protected c_group_widget
{
protected:
	int32 m_screen_player_index;
	s_player_block_reference* m_tag_block;

public:
	// c_player_widget virtual functions

	virtual ~c_player_widget()
	{
	}
	virtual int32 setup_children() override;
};
ASSERT_STRUCT_SIZE(c_player_widget, 0x78);


class c_player_widget_representation
{
private:
	uint32 m_flags;
	wchar_t* m_player_configuration_name;
	c_static_wchar_string32 m_player_custom_name;
	s_player_profile_traits m_appearance;
	string_id m_team_name;
	e_game_team m_player_team;
	bool m_fake_player;
	bool m_player_is_observing;
	uint16 m_player_rank;
	uint8 gap_62[2];
	uint32 m_bungie_role;
	real_rgb_color m_change_colors;

public:
	c_player_widget_representation();
	void set_player_name_from_configuration(s_player_properties* configuration);
	void set_player_rank(int32 rank);
	void set_player_team(e_game_team team);
	void set_player_team_name(string_id team_name);
	void set_player_is_observer(bool observer);
	void set_fake_player(bool fake);
	void set_user_role(int32 role);
	void set_appearance(s_player_profile_traits* appearance);
};
ASSERT_STRUCT_SIZE(c_player_widget_representation, 0x74);