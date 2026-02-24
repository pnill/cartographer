#include "stdafx.h"
#include "patches_initialize.h"

#include "main/map_repository.h"
#include "rasterizer/rasterizer_main.h"

/* public code */

void sapien_apply_patches(void)
{
	map_repository_apply_sapien_patches();
	rasterizer_sapien_apply_patches();
	return;
}
