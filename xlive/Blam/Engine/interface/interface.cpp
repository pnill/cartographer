#include "stdafx.h"
#include "interface.h"

#include "cutscene/cinematics.h"
#include "rasterizer/rasterizer_globals.h"

#define k_splitscreen_line_colour D3DCOLOR_ARGB(255, 0, 0, 0);
int get_player_window_count()
{
	return *Memory::GetAddress<int*>(0x4E6974);
}

e_split_type get_splitscreen_split_type()
{
	return *Memory::GetAddress<e_split_type*>(0x4E6970);
}

// Renders the lines inbetween windows in splitscreen
// Updated function so it works when the resolution isnt 640x480
void render_splitscreen_line(void)
{
	const int32 player_window_count = get_player_window_count();
	ASSERT(IN_RANGE_INCLUSIVE(player_window_count, 1, 4));

	const s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
	const int32 resolution_x = rasterizer_globals->resolution_x;
	const int32 resolution_y = rasterizer_globals->resolution_y;

	// We calculate the size of the line by dividing our resolution by the height the original game ran at
	const int32 line_size = resolution_y / 480;		
	
	rectangle2d line;
	pixel32 color;
	color.color = k_splitscreen_line_colour;
	if (player_window_count > 1)
	{
		if (get_splitscreen_split_type() == _split_type_vertical)
		{
			line.top = (resolution_y / 2) - line_size;
			line.left = 0;
			line.bottom = (resolution_y / 2) + line_size;
			line.right = resolution_x;
			cinematics_draw_line(&line, color);

			// Draw horizontal line between players
			if (player_window_count > 2)
			{
				line.left = (resolution_x / 2) - line_size;
				line.bottom = resolution_y;
				line.right = (resolution_x / 2) + line_size;
				line.top = (player_window_count == 3 ? resolution_y / 2 : 0);
			}
		}
		else
		{
			line.top = 0;
			line.left = (resolution_x / 2) - line_size;
			line.bottom = resolution_y;
			line.right = (resolution_x / 2) + line_size;
			cinematics_draw_line(&line, color);

			// Draw vertical line between players
			if (player_window_count > 2)
			{
				line.top = (resolution_y / 2) - line_size;
				line.bottom = (resolution_y / 2) + line_size;
				line.right = resolution_x;
				line.left = (player_window_count == 3 ? resolution_x / 2 : 0);
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
