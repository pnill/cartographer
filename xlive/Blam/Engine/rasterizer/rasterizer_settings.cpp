#include "stdafx.h"
#include "rasterizer_settings.h"

#include "math/math.h"
#include "shell/shell.h"
#include "dx9/rasterizer_dx9_main.h"
#include "interface/user_interface_text.h"


void __cdecl rasterizer_discard_refresh_rate();

s_display_option g_display_options[k_max_display_option_count] = {};


bool* get_render_fog_enabled(void)
{
	return Memory::GetAddress<bool*>(0x41F6AA, 0x3C2B7E);
}

typedef void(__cdecl* update_screen_settings_t)(int, int, short, short, short, short, float, float);
update_screen_settings_t p_update_screen_settings;

uint32** g_video_mode_refresh_rates = NULL;

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
uint32 calculate_gcd(uint32 n, uint32 m)
{
	uint32 r;
	while (n != 0)
	{
		r = m % n;
		m = n;
		n = r;
	}
	return m;
}

s_aspect_ratio calculate_aspect_ratio(uint32 width, uint32 height)
{
	uint32 gcd = calculate_gcd(width, height);
	s_aspect_ratio aspect_ratio { uint8(width / gcd), uint8(height / gcd) };
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

		// discard anything bellow 640x480 (which is already very low res)
		if (screen.dmPelsWidth < 640
			|| screen.dmPelsHeight < 480)
		{
			populate_setting = false;
			continue;
		}

		for (size_t j = 0; j < count; j++)
		{
			if (screen.dmPelsWidth == g_display_options[j].width 
				&& screen.dmPelsHeight == g_display_options[j].height)
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
			count++;
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

s_video_mode* rasterizer_get_video_modes()
{
	return *Memory::GetAddress<s_video_mode**>(0xA3D9F0);
}

s_video_mode* rasterizer_get_video_mode(int32 video_mode)
{
	return &rasterizer_get_video_modes()[video_mode];
}

uint32 rasterizer_get_video_mode_count()
{
	return *Memory::GetAddress<uint32*>(0xA3D9F4);
}

int32 rasterizer_get_default_display_monitor()
{
	int32 monitor_index = 0;

	if (shell_startup_flag_is_set(_startup_flag_monitor_count))
	{
		if ((uint32)shell_startup_flag_get(_startup_flag_monitor_count) < rasterizer_dx9_main_globals_get()->global_d3d_interface->GetAdapterCount())
		{
			monitor_index = shell_startup_flag_get(_startup_flag_monitor_count);
		}
	}

	return monitor_index;
}

int __cdecl d3d_display_mode_compare(void* context, void const* a, void const* b)
{
	D3DDISPLAYMODE* mode_a = (D3DDISPLAYMODE*)a;
	D3DDISPLAYMODE* mode_b = (D3DDISPLAYMODE*)b;

	if (mode_a->Width != mode_b->Width)
	{
		return mode_a->Width - mode_b->Width;
	}
	else if (mode_a->Height != mode_b->Height)
	{
		return mode_a->Height - mode_b->Height;
	}

	return mode_a->RefreshRate - mode_b->RefreshRate;
}

bool rasterizer_display_refresh_rate_exists(D3DDISPLAYMODE* d3d_display_modes, int32 display_mode_count, UINT width, UINT height, UINT refresh_rate)
{
	for (int32 i = 0; i < display_mode_count; i++)
	{
		if (d3d_display_modes[i].Width == width && d3d_display_modes[i].Height == height)
		{
			if (d3d_display_modes[i].RefreshRate == refresh_rate)
				return true;
		}
	}

	return false;
}

typedef void(__cdecl* t_video_settings_get_available_monitor_display_modes)();
t_video_settings_get_available_monitor_display_modes p_video_settings_get_available_monitor_display_modes;

void __cdecl video_settings_get_available_monitor_display_modes_hook()
{
	p_video_settings_get_available_monitor_display_modes();

	static bool initialized = false;
	if (!initialized)
	{
		IDirect3D9* d3d9 = rasterizer_dx9_main_globals_get()->global_d3d_interface;

		int32 adapter_index = rasterizer_get_default_display_monitor();

		D3DDISPLAYMODE d3d_display_mode;
		d3d9->GetAdapterDisplayMode(adapter_index, &d3d_display_mode);

		uint32 d3d_adapter_mode_count = d3d9->GetAdapterModeCount(adapter_index, d3d_display_mode.Format);

		if (d3d_adapter_mode_count > 0)
		{
			D3DDISPLAYMODE* d3d_display_modes = new D3DDISPLAYMODE[d3d_adapter_mode_count];

			int32 d3d_mode_index = 0;

			for (UINT i = 0; i < d3d_adapter_mode_count; i++)
			{
				if (SUCCEEDED(d3d9->EnumAdapterModes(adapter_index, d3d_display_mode.Format, i, &d3d_display_modes[d3d_mode_index])))
				{
					d3d_mode_index++;
				}
			}

			int32 d3d_mode_count = d3d_mode_index;
			qsort_s(d3d_display_modes, d3d_mode_count, sizeof(D3DDISPLAYMODE), d3d_display_mode_compare, NULL);

			s_video_mode* video_modes = rasterizer_get_video_modes();
			uint32 video_mode_count = rasterizer_get_video_mode_count();

			// allocate the new refresh rate buffers
			g_video_mode_refresh_rates = new uint32*[video_mode_count];

			// reset the refresh rate data
			for (uint32 i = 0; i < video_mode_count; i++)
			{
				for (uint32 j = 0; j < video_modes[i].refresh_rate_count; j++)
				{
					video_modes[i].refresh_rate[j] = 0;
				}
				video_modes[i].refresh_rate_count = 0;
			}

			// a tad too complex, might need some cleanup
			// move to function that's using callbacks for each stage
			bool compute_refresh_rate_count = true;
			for (uint32 i = 0; i < video_mode_count; )
			{
				int32 refresh_rate_index = 0;
				for (int32 j = 0; j < d3d_mode_count; j++)
				{
					if (d3d_display_modes[j].Width == video_modes[i].width
						&& d3d_display_modes[j].Height == video_modes[i].height)
					{
						bool refresh_rate_valid = true;

						if (d3d_mode_count > 2)
						{
							if ((d3d_display_modes[j].RefreshRate > 30 && d3d_display_modes[j].RefreshRate < 60)
								&& rasterizer_display_refresh_rate_exists(d3d_display_modes, d3d_mode_count, d3d_display_modes[j].Width, d3d_display_modes[j].Height, 60))
							{
								refresh_rate_valid = false;
							}

							if (d3d_display_modes[j].RefreshRate < 30
								&& rasterizer_display_refresh_rate_exists(d3d_display_modes, d3d_mode_count, d3d_display_modes[j].Width, d3d_display_modes[j].Height, 30))
							{
								refresh_rate_valid = false;
							}
						}

						// stage 1
						// check if we're computing the refresh rate count
						if (compute_refresh_rate_count)
						{
							if (refresh_rate_valid)
							{
								video_modes[i].refresh_rate_count++;
							}
						}
						else
						{
							// stage 2
							// copy, then move to next
							if (refresh_rate_valid)
							{
								// ### FIXME needs some more code re-written
								g_video_mode_refresh_rates[i][refresh_rate_index++] = d3d_display_modes[j].RefreshRate;
							}
						}
					}
				}

				// if we computed the valid refresh rate count, add them to the list
				if (compute_refresh_rate_count && video_modes[i].refresh_rate_count > 0)
				{
					g_video_mode_refresh_rates[i] = new uint32[video_modes[i].refresh_rate_count];
					compute_refresh_rate_count = false;
				}
				else
				{
					// skip entirely if no refresh rates are available
					// and calculate the refresh rate count again
					i++;
					compute_refresh_rate_count = true;
				}
			}

			// ### FIXME replace
			// copy the refresh rates 
			for (uint32 i = 0; i < video_mode_count; i++)
			{
				int32 refresh_rate_start_index = MIN(video_modes[i].refresh_rate_count, k_max_default_display_refresh_rate_count) - 1;
				for (int32 j = (int32)(video_modes[i].refresh_rate_count - 1); j >= 0; j--)
				{
					if (refresh_rate_start_index >= 0)
					{
						video_modes[i].refresh_rate[refresh_rate_start_index--] = g_video_mode_refresh_rates[i][j];
					}
					else
					{
						break;
					}
				}

				video_modes[i].refresh_rate_count = MIN(video_modes[i].refresh_rate_count, k_max_default_display_refresh_rate_count);
			}

			// ### FIXME remove once extended refresh list is ready
			rasterizer_discard_refresh_rate();

			delete[] d3d_display_modes;
		}

		initialized = true;
	}

	return;
}

uint32 rasterizer_get_video_mode_refresh_rate_count(uint32 video_mode_index)
{
	if (video_mode_index < rasterizer_get_video_mode_count())
	{
		return rasterizer_get_video_mode(video_mode_index)->refresh_rate_count;
	}

	return 0;
}

void __cdecl rasterizer_discard_refresh_rate()
{
	// atexit

	if (g_video_mode_refresh_rates != NULL)
	{
		for (uint32 i = 0; i < rasterizer_get_video_mode_count(); i++)
		{
			if (g_video_mode_refresh_rates[i])
				delete[] g_video_mode_refresh_rates[i];
		}
		
		delete[] g_video_mode_refresh_rates;
	}

	return;
}

typedef int32(__cdecl* t_rasterizer_get_video_mode_refresh_rate)(uint32, uint32);
t_rasterizer_get_video_mode_refresh_rate p_rasterizer_get_video_mode_refresh_rate;

int32 __cdecl rasterizer_get_video_mode_refresh_rate_hook(uint32 video_mode_index, uint32 refresh_rate_index)
{
	if (video_mode_index < rasterizer_get_video_mode_count())
	{
		if (refresh_rate_index < rasterizer_get_video_mode_refresh_rate_count(video_mode_index))
			return g_video_mode_refresh_rates[video_mode_index][refresh_rate_index];
	}

	return g_video_mode_refresh_rates[0][0];
}

void rasterizer_settings_apply_hooks()
{
	DETOUR_ATTACH(p_update_screen_settings, Memory::GetAddress<update_screen_settings_t>(0x264979), update_screen_settings);
	create_new_display_setting_array();

	DETOUR_ATTACH(p_video_settings_get_available_monitor_display_modes, Memory::GetAddress<t_video_settings_get_available_monitor_display_modes>(0x2638BB), video_settings_get_available_monitor_display_modes_hook);

	//DETOUR_ATTACH(p_rasterizer_get_video_mode_refresh_rate, Memory::GetAddress<t_rasterizer_get_video_mode_refresh_rate>(0x263CA8), rasterizer_get_video_mode_refresh_rate_hook);

	atexit(rasterizer_discard_refresh_rate);

	// fix (or workaround) the refresh rate being unset
	// by some hacky looking code in the game
	NopFill(Memory::GetAddress(0x26475D), 26);
}