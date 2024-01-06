#include "stdafx.h"
#include "rasterizer_settings.h"

#include "Blam/Engine/interface/user_interface_text.h"
#include "Util/Hooks/Hook.h"

s_display_option g_display_options[k_max_display_option_count] = { 0 };

typedef void(__cdecl* update_screen_settings_t)(int, int, short, short, short, short, float, float);
update_screen_settings_t p_update_screen_settings;

void __cdecl update_screen_settings(
	int width, 
	int height, 
	short left,
	short top,
	short a5, 
	short a6,
	float ui_scale,
	float window_scale)
{
	p_update_screen_settings(width, height, left, top, a5, a6, ui_scale, window_scale);

	// Change the text label scale if the global ui scale goes above 1 
	// (text fix for resolutions above 1080 pixels vertically)
	if (ui_scale > 1.0f)
		set_ui_text_label_scale(1.f / ui_scale);
	else
		set_ui_text_label_scale(ui_scale);
}


int compare_display_options(void* context, const void* a1, const void* a2)
{
	const s_display_option* res1 = (s_display_option*)a1;
	const s_display_option* res2 = (s_display_option*)a2;

	int result;
	if (res1->width == res2->width)
	{
		result = res1->height - res2->height;
	}
	else
	{
		result = res1->width - res2->width;
	}

	return result;
}

// Calculates the greatest common denominator of the 2 numbers
size_t calculate_gcd(size_t n, size_t m)
{
	size_t r;
	while (n != 0)
	{
		r = m % n;
		m = n;
		n = r;
	}
	return m;
}

s_aspect_ratio calculate_aspect_ratio(size_t width, size_t height)
{
	size_t gcd = calculate_gcd(width, height);

	s_aspect_ratio aspect_ratio;
	aspect_ratio.x = byte(width / gcd);
	aspect_ratio.y = byte(height / gcd);
	return aspect_ratio;
}

// Create new resolution array to replace the hardcoded one in-game
// Includes all supported video modes by the current monitor
void create_new_display_setting_array()
{
	size_t count = 0;
	DEVMODE screen;
	screen.dmSize = sizeof(DEVMODE);

	// Loop through every supported display setting 
	for (size_t i = 0; EnumDisplaySettings(NULL, i, &screen) && count < k_max_display_option_count; i++)
	{
		// See if we already have a duplicate setting
		// Don't populate it if we do
		bool populate_setting = true;
		for (size_t j = 0; j < count; j++)
		{
			if (screen.dmPelsWidth == g_display_options[j].width && screen.dmPelsHeight == g_display_options[j].height)
			{
				populate_setting = false;
				break;
			}
		}

		if (populate_setting)
		{
			g_display_options[count].width = screen.dmPelsWidth;
			g_display_options[count].height = screen.dmPelsHeight;
			s_aspect_ratio aspect_ratio = calculate_aspect_ratio(screen.dmPelsWidth, screen.dmPelsHeight);

			if (aspect_ratio.x == 4 && aspect_ratio.y == 3)
			{
				g_display_options[count].aspect_ratio = _aspect_ratio_4x3;
			}
			else if (aspect_ratio.x == 16 && aspect_ratio.y == 9)
			{
				g_display_options[count].aspect_ratio = _aspect_ratio_16x9;
			}
			// we check for 8 by 5 instead of 16 by 10 since you can still divide 16 over 10 by 2 and have a proper fraction
			else if (aspect_ratio.x == 8 && aspect_ratio.y == 5)
			{
				g_display_options[count].aspect_ratio = _aspect_ratio_16x10;
			}
			else
			{
				g_display_options[count].aspect_ratio = _aspect_ratio_4x3;
			}
			++count;
		}
	}

	if (count == 0)
	{
		LOG_CRITICAL_FUNCW("Display Option count is 0, something is definitely wrong here");
		exit(EXIT_FAILURE);
	}
	else
	{
		// Change address with references to beginning of display option struct and end of the struct 
		WritePointer(Memory::GetAddress(0x263A53), g_display_options);
		WritePointer(Memory::GetAddress(0x263A5E), &g_display_options[0].height);

		WritePointer(Memory::GetAddress(0x263C7C), g_display_options);
		WritePointer(Memory::GetAddress(0x263C84), &g_display_options[0].height);
		WritePointer(Memory::GetAddress(0x263CA1), &g_display_options[0].aspect_ratio);


		WritePointer(Memory::GetAddress(0x26443B), g_display_options);
		WritePointer(Memory::GetAddress(0x264414), &g_display_options[0].height);

		WritePointer(Memory::GetAddress(0x263C25), g_display_options);
		WritePointer(Memory::GetAddress(0x263C55), &g_display_options[0].height);

		WritePointer(Memory::GetAddress(0x264375), g_display_options);
		WritePointer(Memory::GetAddress(0x26439F), &g_display_options[0].height);


		WriteValue(Memory::GetAddress(0x264333), count - 1); // last index of the array
		WritePointer(Memory::GetAddress(0x26434F), &g_display_options[0].height);
		WritePointer(Memory::GetAddress(0x264338), &g_display_options[count - 1].height);

		WriteValue(Memory::GetAddress(0x263A69), sizeof(s_display_option) * count);
		WriteValue(Memory::GetAddress(0x263C92), sizeof(s_display_option) * count);

		WriteValue<DWORD>(Memory::GetAddress(0x263BEA) + 1, count);

		// Sort display resolution count from lowest resolution to greatest
		// We can't sort from highest to lowest since it'll break the ui for the display settings
		// Maybe rewrite the functions for this in the future to fix this?
		qsort_s(g_display_options, count, sizeof(s_display_option), compare_display_options, NULL);
	}
}

void rasterizer_settings_apply_hooks()
{
	DETOUR_ATTACH(p_update_screen_settings, Memory::GetAddress<update_screen_settings_t>(0x264979), update_screen_settings);
	create_new_display_setting_array();

	// fix (or workaround) the refresh rate being unset
	// by some hacky looking code in the game
	NopFill(Memory::GetAddress(0x26475D), 26);
}