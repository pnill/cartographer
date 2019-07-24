#include "stdafx.h"
#include "CustomResolution.h"

#include "H2MOD.h"


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

void CustomResolution::Initialize()
{
	// copy default settings
	memcpy(video_options, h2mod->GetAddress(0x4680C0, 0x0), sizeof(s_diplay_config) * DEFAULT_TOTAL_RESOLUTIONS); // 11 by default

	WritePointer((DWORD)h2mod->GetAddress(0x263A53, 0x0), video_options);
	WritePointer((DWORD)h2mod->GetAddress(0x263A5E, 0x0), (char*)video_options + 4);

	WritePointer((DWORD)h2mod->GetAddress(0x263C7C, 0x0), video_options);
	WritePointer((DWORD)h2mod->GetAddress(0x263C84, 0x0), (char*)video_options + 4);
	WritePointer((DWORD)h2mod->GetAddress(0x263CA1, 0x0), (char*)video_options + 8);


	WritePointer((DWORD)h2mod->GetAddress(0x26443B, 0x0), (char*)video_options);
	WritePointer((DWORD)h2mod->GetAddress(0x264414, 0x0), (char*)video_options + 4);

	WritePointer((DWORD)h2mod->GetAddress(0x263C25, 0x0), (char*)video_options);
	WritePointer((DWORD)h2mod->GetAddress(0x263C55, 0x0), (char*)video_options + 4);

	WritePointer((DWORD)h2mod->GetAddress(0x264375, 0x0), (char*)video_options);
	WritePointer((DWORD)h2mod->GetAddress(0x26439F, 0x0), (char*)video_options + 4);


	WriteValue((DWORD)h2mod->GetAddress(0x264333, 0x0), TOTAL_RESOLUTIONS - 1); // last index of the array
	WritePointer((DWORD)h2mod->GetAddress(0x26434F, 0x0), &video_options[0].res_y);
	WritePointer((DWORD)h2mod->GetAddress(0x264338, 0x0), &video_options[TOTAL_RESOLUTIONS - 1].res_y);

	WriteValue((DWORD)h2mod->GetAddress(0x263A69, 0x0), sizeof(s_diplay_config) * TOTAL_RESOLUTIONS);
	WriteValue((DWORD)h2mod->GetAddress(0x263C92, 0x0), sizeof(s_diplay_config) * TOTAL_RESOLUTIONS);


	video_options[11].res_x = 320;
	video_options[11].res_y = 240;
	video_options[11].unk = 0;

	qsort_s(video_options, TOTAL_RESOLUTIONS, sizeof(s_diplay_config), compare, NULL);
}