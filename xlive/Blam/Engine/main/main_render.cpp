#include "stdafx.h"
#include "main_render.h"

#include "Blam/Engine/render/render_cartographer_ingame_ui.h"

#include "Util/Hooks/Hook.h"

void main_render_hook()
{
	render_cartographer_status_text();
	render_cartographer_achievements();
	render_cartographer_update();
}

void main_render_apply_patches()
{
	PatchCall(Memory::GetAddress(0x19228E), main_render_hook);
}
