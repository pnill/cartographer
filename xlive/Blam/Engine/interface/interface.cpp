#include "stdafx.h"
#include "interface.h"

#include "cutscene/cinematics.h"

#define k_splitscreen_line_colour D3DCOLOR_ARGB(255, 0, 0, 0);
int get_player_window_count()
{
	return *Memory::GetAddress<int*>(0x4E6974);
}

e_split_type get_splitscreen_split_type()
{
	return *Memory::GetAddress<e_split_type*>(0x4E6970);
}


void set_display_type(e_display_type display_type)
{
	WriteValue<e_display_type>(Memory::GetAddress(0xA3E460), display_type);
}

// Renders the lines inbetween windows in splitscreen
// Updated function so it works when the resolution isnt 640x480
void render_splitscreen_line(void)
{
	int32 player_window_count = get_player_window_count();

	ASSERT(IN_RANGE_INCLUSIVE(player_window_count, 1, 4));

	int g_resolution_x = *Memory::GetAddress<int*>(0xA3E408);
	int g_resolution_y = *Memory::GetAddress<int*>(0xA3E40C);

	const int32 line_width = g_resolution_y / 480;
	
	rectangle2d line;
	pixel32 color;
	color.color = k_splitscreen_line_colour;
	if (player_window_count > 1)
	{
		if (get_splitscreen_split_type() == _split_type_vertical)
		{
			line.top = (g_resolution_y / 2) - line_width;
			line.left = 0;
			line.bottom = (g_resolution_y / 2) + line_width;
			line.right = g_resolution_x;
			cinematics_draw_line(&line, color);

			// Draw horizontal line between players
			if (player_window_count > 2)
			{
				line.left = (g_resolution_x / 2) - line_width;
				line.bottom = g_resolution_y;
				line.right = (g_resolution_x / 2) + line_width;
				line.top = (player_window_count == 3 ? g_resolution_y / 2 : 0);
			}
		}
		else
		{
			line.top = 0;
			line.left = (g_resolution_x / 2) - line_width;
			line.bottom = g_resolution_y;
			line.right = (g_resolution_x / 2) + line_width;
			cinematics_draw_line(&line, color);

			// Draw vertical line between players
			if (player_window_count > 2)
			{
				line.top = (g_resolution_y / 2) - line_width;
				line.bottom = (g_resolution_y / 2) + line_width;
				line.right = g_resolution_x;
				line.left = (player_window_count == 3 ? g_resolution_x / 2 : 0);
			}
		}
		cinematics_draw_line(&line, color);
	}
	return;
}

void apply_interface_hooks(void)
{
	PatchCall(Memory::GetAddress(0x227A17), render_splitscreen_line);
	return;
}