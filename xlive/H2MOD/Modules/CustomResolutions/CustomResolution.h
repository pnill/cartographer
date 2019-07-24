#pragma once

#define TOTAL_RESOLUTIONS 12
#define DEFAULT_TOTAL_RESOLUTIONS 12

struct s_diplay_config
{
	int res_x;
	int res_y;
	int unk; // not sure what this is, maybe it defines what type of res it is, i.e widescreen, 4:3 etc.
};

namespace CustomResolution
{
	void Initialize();
}