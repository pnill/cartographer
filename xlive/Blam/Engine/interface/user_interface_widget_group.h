#pragma once

/* constants */

enum
{
	k_user_interface_widget_shared_globals_tag = 'wigl',
	k_user_interface_widget_shared_globals_version = 1,	// TODO: confirm
	k_user_interface_widget_globals_tag = 'wgtz',
	k_user_interface_widget_globals_version = 3,		// TODO: confirm
	k_maximum_number_of_screen_widgets = 256
};

/* macros */

#define user_interface_tag_globals_get(index)	((struct s_user_interface_tag_globals *)tag_get(k_user_interface_widget_globals_tag, (index)))

/* structures */

struct s_user_interface_tag_globals
{
	// Explaination("Shared Globals", "This is a reference to the ui shared globals tag")
	tag_reference shared_globals;	// wigl

	// Explaination("Screen Widgets", "These are the screen widgets")
	s_tag_block screen_widgets;	// s_user_interface_widget_reference

	// Explaination("Multiplayer Variant Settings Interface", "This blob defines the ui for setting multiplayer game variant parameters")
	tag_reference mp_variant_settings_ui;	// goof

	// Explaination("Game Hopper Localization Strings", "This is for the loc game hopper strings")
	tag_reference game_hopper_descriptions;	// unic
};
ASSERT_STRUCT_SIZE(s_user_interface_tag_globals, 32);
