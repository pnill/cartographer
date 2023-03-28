#include "stdafx.h"
#include "interface.h"
#include "Blam/Math/integer_math.h"
#include "Util/Memory.h"
#include "Util/Hooks/Hook.h"

//0xFF000000 is a D3DCOLOR where the alpha is 255 and the rgb is 0
#define D3DCOLOR_BLACK 0xFF000000

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

// Not the exact same function but updated so it works when the resolution isnt 640x480
void draw_splitscreen_line()
{
	typedef void(__cdecl* tdrawRectangle)(rect2d* points, int rect_color);
	tdrawRectangle drawRectangle = Memory::GetAddress<tdrawRectangle>(0x3B101);

	int player_window_count = get_player_window_count();

	if (player_window_count <= 1) { return; }
	
	int g_resolution_x = *Memory::GetAddress<int*>(0xA3E408);
	int g_resolution_y = *Memory::GetAddress<int*>(0xA3E40C);

	const int line_width = g_resolution_y / 480;
	rect2d line;
	if (get_splitscreen_split_type() == _split_type_vertical)
	{
		line.top = (g_resolution_y / 2) - line_width;
		line.left = 0;
		line.bottom = (g_resolution_y / 2) + line_width;
		line.right = g_resolution_x;
		drawRectangle(&line, D3DCOLOR_BLACK);

		if (player_window_count <= 2) { return; }
			
		line.left = (g_resolution_x / 2) - line_width;
		line.bottom = g_resolution_y;
		line.right = (g_resolution_x / 2) + line_width;
		if (player_window_count == 3)
		{
			line.top = g_resolution_y / 2;
			drawRectangle(&line, D3DCOLOR_BLACK);
			return;
		}
		line.top = 0;
	}
	else
	{
		line.top = 0;
		line.left = (g_resolution_x / 2) - line_width;
		line.bottom = g_resolution_y;
		line.right = (g_resolution_x / 2) + line_width;
		drawRectangle(&line, D3DCOLOR_BLACK);
		if (player_window_count <= 2)
			return;
		line.top = (g_resolution_y / 2) - line_width;
		line.bottom = (g_resolution_y / 2) + line_width;
		line.right = g_resolution_x;
		if (player_window_count == 3)
		{
			line.left = g_resolution_x / 2;
			drawRectangle(&line, D3DCOLOR_BLACK);
			return;
		}
		line.left = 0;
	}
	drawRectangle(&line, D3DCOLOR_BLACK);
	
}

void apply_interface_hooks()
{
	if (!Memory::IsDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0x227A17), draw_splitscreen_line);
	}
}