#pragma once

struct pixel32
{
	byte alpha;
	byte red;
	byte green;
	byte blue;
};
CHECK_STRUCT_SIZE(pixel32, sizeof(BYTE) * 4);