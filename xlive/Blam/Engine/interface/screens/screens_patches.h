#pragma once

/* prototypes */

// Applies patches to ui screens (wgit tags)
void screens_apply_patches_on_map_load(void);

// Applies patches to ui screens ingame (wgit tags)
void screens_apply_patches_on_mp_map_load(void);

// Applies screen related hooks
void screens_apply_patches(void);