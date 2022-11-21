#include "stdafx.h"

#include "HudElements.h"

#include "Blam/Cache/TagGroups/bitmap_definition.hpp"
#include "Blam/Engine/Game/game/cheats.h"
#include "Blam/Engine/Game/game/game.h"
#include "Blam/Engine/Game/game/players.h"
#include "Blam/Engine/Game/networking/logic/network_life_cycle.h"
#include "Blam/Engine/Game/render/render_cameras.h"

#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"
#include "H2MOD/Tags/TagInterface.h"
#include "H2MOD/Utils/Utils.h"
#include "Util/Hooks/Hook.h"

#define _USE_MATH_DEFINES
#include <math.h>

static bool b_showHUD = true;
static bool b_showFirstPerson = true;
static bool RenderIngameChat() {
	if (H2Config_hide_ingame_chat) {
		datum local_player_datum_index = players::get_player_datum_index_from_controller_index(0);
		if (s_player::GetPlayer(DATUM_INDEX_TO_ABSOLUTE_INDEX(local_player_datum_index))->is_chatting == 2) {
			extern void hotkeyFuncToggleHideIngameChat();
			hotkeyFuncToggleHideIngameChat();
		}
		return true;
	}

	else if (!s_game_globals::game_is_mainmenu() && network_life_cycle::get_game_life_cycle() == _life_cycle_in_game) {
		//Enable chat in engine mode and game state mp.
		return false;
	}
	else {
		//original test - if is campaign
		return true;
	}
}

static bool __cdecl RenderFirstPersonCheck(unsigned int a1)
{
	return !b_showFirstPerson;
}

static bool __cdecl RenderHudCheck(unsigned int a1)
{
	// TODO: cleanup
	static bool hud_opacity_reset = true;
	DWORD new_hud_globals = *(DWORD*)(H2BaseAddr + 0x9770F4);
	float& hud_opacity = *(float*)(new_hud_globals + 0x228); // set the opacity

	if (!b_showHUD || cheats::ice_cream_flavor_available((unsigned int)e_skulls::_blind))
	{
		hud_opacity = 0.f;
		hud_opacity_reset = false;
	}
	else if (!hud_opacity_reset)
	{
		hud_opacity = 1.f;
		hud_opacity_reset = true;
	}

	return false;
}

void HudElements::setCrosshairSize(bool mapLoadContext)
{
	static bool crosshairInit = false;
	static point2d* defaultCrosshairSizes = nullptr;

	if (Memory::IsDedicatedServer())
		return;

	// if we are in a "mapLoadContext", save default crosshair size and delete if we previously saved something
	if (mapLoadContext)
	{
		if (defaultCrosshairSizes != nullptr)
		{
			delete[] defaultCrosshairSizes;
			defaultCrosshairSizes = nullptr;
		}
		crosshairInit = false;
	}

	if (!s_game_globals::game_is_mainmenu()) {
		auto hud_reticles_datum = tags::find_tag(blam_tag::tag_group_type::bitmap, "ui\\hud\\bitmaps\\new_hud\\crosshairs\\hud_reticles");
		if (hud_reticles_datum != DATUM_INDEX_NONE)
		{
			auto hud_reticles_data = tags::get_tag_fast<bitmap_definition>(hud_reticles_datum);

			for (auto i = 0; i < hud_reticles_data->bitmaps.size; i++)
			{
				if (!crosshairInit)
				{
					if (defaultCrosshairSizes == nullptr) defaultCrosshairSizes = new point2d[hud_reticles_data->bitmaps.size];

					defaultCrosshairSizes[i] = hud_reticles_data->bitmaps[i]->size;
				}

				hud_reticles_data->bitmaps[i]->size = point2d
				{
					(short)round(defaultCrosshairSizes[i].x * H2Config_crosshair_scale),
					(short)round(defaultCrosshairSizes[i].y * H2Config_crosshair_scale)
				};
			}
			crosshairInit = true;
		}
	}
}

void HudElements::setCrosshairPos() {

	if (Memory::IsDedicatedServer())
		return;

	if (!FloatIsNaN(H2Config_crosshair_offset)) {
		s_globals_group_definition* globals = tags::get_matg_globals_ptr();
		globals->player_control[0]->crosshair_location.y = H2Config_crosshair_offset;
	}
}

void HudElements::RadarPatch()
{
	WriteValue<BYTE>(Memory::GetAddress(0x2849C4), (BYTE)4);
	addDebugText("Motion sensor patched successfully.");
}

void HudElements::setFOV() {

	if (Memory::IsDedicatedServer())
		return;

	static bool fov_redirected = false;
	static float fov = 70.0f * M_PI / 180.0f;
	if (H2Config_field_of_view > 0 && H2Config_field_of_view <= 110)
	{
		if (!fov_redirected)
		{
			BYTE opcode[6] = { 0xD9, 0x05, 0x00, 0x00, 0x00, 0x00 };
			WritePointer((DWORD)&opcode[2], &fov);
			WriteBytes(Memory::GetAddress(0x907F3), opcode, sizeof(opcode)); // fld dword ptr[fov]

			fov_redirected = true;
		}

		//const double default_radians_field_of_view = 70.0f * M_PI / 180.0f;
		if (currentVariantSettings.forcedFOV == 0)
			fov = (float)H2Config_field_of_view * M_PI / 180.0f;
		else
			fov = (float)currentVariantSettings.forcedFOV * M_PI / 180.0f;
	}
}

void HudElements::setVehicleFOV() {

	if (Memory::IsDedicatedServer())
		return;

	if (H2Config_vehicle_field_of_view > 0 && H2Config_vehicle_field_of_view <= 110)
	{
		float calculated_radians_FOV;
		if (currentVariantSettings.forcedFOV == 0) {
			calculated_radians_FOV = (float)H2Config_vehicle_field_of_view * M_PI / 180.0f;
			//WriteValue(Memory::GetAddress(0x413780), (float)H2Config_vehicle_field_of_view * M_PI / 180.0f);
		}
			//
		else {
			calculated_radians_FOV = (float)currentVariantSettings.forcedFOV * M_PI / 180.0f;
			//WriteValue(Memory::GetAddress(0x413780), (float)CurrentVariantSettings.ForcedFOV * M_PI / 180.0f);
		}
			//
		WriteValue(Memory::GetAddress(0x413780), calculated_radians_FOV); // Third Person
	}
}


void HudElements::ToggleFirstPerson(bool state)
{
	if (state == NULL)
		b_showFirstPerson = !b_showFirstPerson;
	else
		b_showFirstPerson = state;
}

void HudElements::ToggleHUD(bool state)
{
	if (state == NULL)
		b_showHUD = !b_showHUD;
	else
		b_showHUD = state;
}

void HudElements::OnMapLoad()
{
	setCrosshairSize(true);
	setCrosshairPos();
}

void HudElements::ApplyHooks()
{
	// Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
	PatchCall(Memory::GetAddress(0x22667B), RenderIngameChat);
	PatchCall(Memory::GetAddress(0x226628), RenderIngameChat);

	PatchCall(Memory::GetAddress(0x228579), RenderFirstPersonCheck);

	// Redirect ice_cream_flavor_available call 
	PatchCall(Memory::GetAddress(0x223955), RenderHudCheck);

	PatchCall(Memory::GetAddress(0x191440), render_camera_build_projection_hook);
}

void HudElements::Init()
{
	if (H2IsDediServer)
		return;
	KeyboardInput::RegisterHotkey(&H2Config_hotkeyIdToggleHideIngameChat, 
		[]()	{
			H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat;
		}
	);
	ApplyHooks();
	setFOV();
	setVehicleFOV();
	RadarPatch();
}
