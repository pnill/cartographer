#pragma once

struct s_location
{
	DWORD leaf_index;
	WORD cluster;
	WORD bsp_index;
};

__int16 get_global_structure_bsp_index();
