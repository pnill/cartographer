#include "stdafx.h"
#include "CustomResolution.h"

#include "H2MOD.h"
#include "..\Util\Hooks\Hook.h"


s_diplay_config video_options[TOTAL_RESOLUTIONS];

int compare(void* context, const void* a1, const void* a2)
{
	const s_diplay_config* res1 = reinterpret_cast<const s_diplay_config*>(a1);
	const s_diplay_config* res2 = reinterpret_cast<const s_diplay_config*>(a2);

	int result = res1->res_x - res2->res_x;
	if (res1->res_x == res2->res_x)
		result = res1->res_y - res2->res_y;
	
	return result;
}

// TODO: cleanup
void CustomResolution::Initialize()
{
	// copy default settings
	memcpy(video_options, h2mod->GetAddress<s_diplay_config*>(0x4680C0, 0x0), sizeof(s_diplay_config) * DEFAULT_TOTAL_RESOLUTIONS); // 11 by default

	WritePointer(h2mod->GetAddress(0x263A53, 0x0), video_options);
	WritePointer(h2mod->GetAddress(0x263A5E, 0x0), (char*)video_options + 4);

	WritePointer(h2mod->GetAddress(0x263C7C, 0x0), video_options);
	WritePointer(h2mod->GetAddress(0x263C84, 0x0), &video_options[0].res_y);
	WritePointer(h2mod->GetAddress(0x263CA1, 0x0), &video_options[0].unk);


	WritePointer(h2mod->GetAddress(0x26443B, 0x0), video_options);
	WritePointer(h2mod->GetAddress(0x264414, 0x0), &video_options[0].res_y);

	WritePointer(h2mod->GetAddress(0x263C25, 0x0), video_options);
	WritePointer(h2mod->GetAddress(0x263C55, 0x0), &video_options[0].res_y);

	WritePointer(h2mod->GetAddress(0x264375, 0x0), video_options);
	WritePointer(h2mod->GetAddress(0x26439F, 0x0), &video_options[0].res_y);


	WriteValue(h2mod->GetAddress(0x264333, 0x0), TOTAL_RESOLUTIONS - 1); // last index of the array
	WritePointer(h2mod->GetAddress(0x26434F, 0x0), &video_options[0].res_y);
	WritePointer(h2mod->GetAddress(0x264338, 0x0), &video_options[TOTAL_RESOLUTIONS - 1].res_y);

	WriteValue(h2mod->GetAddress(0x263A69, 0x0), sizeof(s_diplay_config) * TOTAL_RESOLUTIONS);
	WriteValue(h2mod->GetAddress(0x263C92, 0x0), sizeof(s_diplay_config) * TOTAL_RESOLUTIONS);

	video_options[11].res_x = 3840;
	video_options[11].res_y = 2160;
	video_options[11].unk = 2;

	video_options[12].res_x = 2560;
	video_options[12].res_y = 1440;
	video_options[12].unk = 2;

	video_options[13].res_x = 640;
	video_options[13].res_y = 480;
	video_options[13].unk = 0;

	qsort_s(video_options, TOTAL_RESOLUTIONS, sizeof(s_diplay_config), compare, NULL);
}