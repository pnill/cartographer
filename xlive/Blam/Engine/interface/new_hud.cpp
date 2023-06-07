#include "stdafx.h"
#include "new_hud.h"

#include "Blam/Engine/camera/camera.h"
#include "Blam/Engine/game/cheats.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/interface/hud.h"
#include "Blam/Engine/interface/new_hud_definitions.h"
#include "Blam/Engine/Networking/logic/life_cycle_manager.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "Util/Hooks/Hook.h"

s_new_hud_engine_globals* get_new_hud_engine_globals()
{
	return *Memory::GetAddress<s_new_hud_engine_globals**>(0x9770F4);
}

bool __cdecl render_ingame_chat_check() 
{
	if (H2Config_hide_ingame_chat) 
	{
		datum local_player_datum_index = h2mod->get_player_datum_index_from_controller_index(0);
		if (s_player::GetPlayer(DATUM_INDEX_TO_ABSOLUTE_INDEX(local_player_datum_index))->is_chatting == 2) 
		{
			hotkeyFuncToggleHideIngameChat();
		}
		return true;
	}

	else if (h2mod->GetEngineType() != _main_menu && get_game_life_cycle() == _life_cycle_in_game) 
	{
		//Enable chat in engine mode and game state mp.
		return false;
	}
	else {
		//original test - if is campaign
		return true;
	}
}

// Hook for ui_get_hud_elements for modifying the hud anchor for text
void __cdecl ui_get_hud_elemet_bounds_hook(e_hud_anchor type, real_bounds* bounds)
{
	float safe_area = *Memory::GetAddress<float*>(0x9770F0);
	s_camera* camera_data = get_global_camera();

	float scale_factor = *get_secondary_hud_scale();

	typedef void(__cdecl* ui_get_hud_elemets_anchor_t)(e_hud_anchor, real_bounds*);
	auto p_ui_get_hud_elemet_bounds = Memory::GetAddress<ui_get_hud_elemets_anchor_t>(0x223969);

	switch (type)
	{
	case hud_anchor_weapon_hud:
		bounds->lower = (float)camera_data->window_bounds.left + safe_area;
		bounds->upper= (float)camera_data->window_bounds.top + (safe_area / scale_factor); // (100.f * scale_factor) - 100.f;
		break;
	default:
		p_ui_get_hud_elemet_bounds(type, bounds);
		break;
	}
}

void new_hud_apply_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	KeyboardInput::RegisterHotkey(&H2Config_hotkeyIdToggleHideIngameChat,
		[]() {
			H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat;
		}
	);

	// Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
	PatchCall(Memory::GetAddress(0x22667B), render_ingame_chat_check);
	PatchCall(Memory::GetAddress(0x226628), render_ingame_chat_check);

	// Hook ui_get_hud_elements for modifying the hud anchor for text
	PatchCall(Memory::GetAddress(0x22D25A), ui_get_hud_elemet_bounds_hook);
}
