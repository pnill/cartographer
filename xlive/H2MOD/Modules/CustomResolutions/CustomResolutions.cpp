#include "stdafx.h"
#include "CustomResolutions.h"

#include "H2MOD.h"
#include "..\Util\Hooks\Hook.h"


s_display_res video_options[TOTAL_RESOLUTIONS];

int compare(void* context, const void* a1, const void* a2)
{
	const s_display_res* res1 = reinterpret_cast<const s_display_res*>(a1);
	const s_display_res* res2 = reinterpret_cast<const s_display_res*>(a2);

	int result = res1->res_x - res2->res_x;
	if (res1->res_x == res2->res_x)
		result = res1->res_y - res2->res_y;
	
	return result;
}

void CustomResolution::Initialize()
{
	// copy default settings
	memcpy(video_options, h2mod->GetAddress<s_display_res*>(0x4680C0), sizeof(s_display_res) * DEFAULT_TOTAL_RESOLUTIONS); // 11 by default

	WritePointer(h2mod->GetAddress(0x263A53), video_options);
	WritePointer(h2mod->GetAddress(0x263A5E), &video_options[0].res_y);

	WritePointer(h2mod->GetAddress(0x263C7C), video_options);
	WritePointer(h2mod->GetAddress(0x263C84), &video_options[0].res_y);
	WritePointer(h2mod->GetAddress(0x263CA1), &video_options[0].unk);


	WritePointer(h2mod->GetAddress(0x26443B), video_options);
	WritePointer(h2mod->GetAddress(0x264414), &video_options[0].res_y);

	WritePointer(h2mod->GetAddress(0x263C25), video_options);
	WritePointer(h2mod->GetAddress(0x263C55), &video_options[0].res_y);

	WritePointer(h2mod->GetAddress(0x264375), video_options);
	WritePointer(h2mod->GetAddress(0x26439F), &video_options[0].res_y);


	WriteValue(h2mod->GetAddress(0x264333), TOTAL_RESOLUTIONS - 1); // last index of the array
	WritePointer(h2mod->GetAddress(0x26434F), &video_options[0].res_y);
	WritePointer(h2mod->GetAddress(0x264338), &video_options[TOTAL_RESOLUTIONS - 1].res_y);

	WriteValue(h2mod->GetAddress(0x263A69), sizeof(s_display_res) * TOTAL_RESOLUTIONS);
	WriteValue(h2mod->GetAddress(0x263C92), sizeof(s_display_res) * TOTAL_RESOLUTIONS);

	video_options[11].res_x = 3840;
	video_options[11].res_y = 2160;
	video_options[11].unk = 2;

	video_options[12].res_x = 2560;
	video_options[12].res_y = 1440;
	video_options[12].unk = 2;

	video_options[13].res_x = 640;
	video_options[13].res_y = 480;
	video_options[13].unk = 0;

	qsort_s(video_options, TOTAL_RESOLUTIONS, sizeof(s_display_res), compare, NULL);
}