#pragma once

/* enums */
enum e_rasterizer_target : uint32
{
	_rasterizer_target_0 = 0,
	_rasterizer_target_1 = 1,
	_rasterizer_target_2 = 2,
	_rasterizer_target_3 = 3,
	_rasterizer_target_4 = 4,
	_rasterizer_target_5 = 5,
	_rasterizer_target_6 = 6,
	_rasterizer_target_7 = 7,
	_rasterizer_target_8 = 8,
	_rasterizer_target_9 = 9,
	_rasterizer_target_10 = 10,
	_rasterizer_target_11 = 11,
	_rasterizer_target_12 = 12,
	_rasterizer_target_13 = 13,
	_rasterizer_target_14 = 14,
	_rasterizer_target_15 = 15,
	_rasterizer_target_16 = 16,
	_rasterizer_target_17 = 17,
	_rasterizer_target_18 = 18,
	_rasterizer_target_19 = 19,
	_rasterizer_target_20 = 20,
	_rasterizer_target_21 = 21,
	_rasterizer_target_22 = 22,
	_rasterizer_target_23 = 23,
	_rasterizer_target_24 = 24,
	_rasterizer_target_25 = 25,
	_rasterizer_target_26 = 26,
	_rasterizer_target_27 = 27,
	_rasterizer_target_28 = 28,
	_rasterizer_target_29 = 29,
	_rasterizer_target_30 = 30,
	_rasterizer_target_31 = 31,
	_rasterizer_target_32 = 32,
	_rasterizer_target_33 = 33,
	_rasterizer_target_34 = 34,
	_rasterizer_target_35 = 35,
	_rasterizer_target_36 = 36,
	_rasterizer_target_37 = 37,
	k_rasterizer_target_count
};

/* structures */

struct s_rasterizer_target
{
	IDirect3DTexture9* texture;
	IDirect3DSurface9* surfaces[2];
	int gap_C[2];
	int field_14;
	uint32 size_x;
	uint32 size_y;
	bool unk_bool_20;
	int8 pad[3];
};

/* public code */

e_rasterizer_target* rasterizer_dx9_present_target_get(void);

s_rasterizer_target* rasterizer_dx9_target_get(e_rasterizer_target rasterizer_target);

IDirect3DSurface9* __cdecl rasterizer_dx9_get_target_surface(uint16 rasterizer_target, uint16 mipmap_index);

IDirect3DSurface9* rasterizer_dx9_target_get_surface(e_rasterizer_target rasterizer_target);
