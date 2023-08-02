#include "stdafx.h"
#include "CustomResolutions.h"

#include "Util/Hooks/Hook.h"

s_display_resolution g_display_options[k_max_display_option_count] = {0};

int compare(void* context, const void* a1, const void* a2)
{
	const s_display_resolution* res1 = (s_display_resolution*)a1;
	const s_display_resolution* res2 = (s_display_resolution*)a2;

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
	aspect_ratio.x = width / gcd;
	aspect_ratio.y = height / gcd;
	return aspect_ratio;
}

void CustomResolution::Initialize()
{	
	size_t count = 0;
	DEVMODE screen;
	screen.dmSize = sizeof(DEVMODE);
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

	// copy default settings
	//memcpy(video_options, Memory::GetAddress<s_display_res*>(0x4680C0), sizeof(s_display_res) * DEFAULT_TOTAL_RESOLUTIONS); // 11 by default

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

	WriteValue(Memory::GetAddress(0x263A69), sizeof(s_display_resolution) * count);
	WriteValue(Memory::GetAddress(0x263C92), sizeof(s_display_resolution) * count);

	WriteValue<DWORD>(Memory::GetAddress(0x263BEA) + 1, count);

	qsort_s(g_display_options, count, sizeof(s_display_resolution), compare, NULL);
}