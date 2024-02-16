#pragma once

void cloth_apply_patches(void);

struct s_cloth {
	char gap_0[12];
	real32 accum;
	char gap_10[4680];
};
CHECK_STRUCT_SIZE(s_cloth, 4696);

