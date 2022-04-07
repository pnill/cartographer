#include "stdafx.h"

#include "HudElements.h"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"
#include "H2MOD\Modules\CustomVariantSettings\CustomVariantSettings.h"
#include "H2MOD\Modules\Input\KeyboardInput.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"
#include "H2MOD\Utils\Utils.h"
#include "H2MOD\Tags\TagInterface.h"

#include "Util\Hooks\Hook.h"

#include "Blam\Engine\IceCreamFlavour\IceCreamFlavor.h"

#define _USE_MATH_DEFINES
#include <math.h>

static bool b_showHUD = true;
static bool b_showFirstPerson = true;
static bool RenderIngameChat() {
	int GameGlobals = *Memory::GetAddress<int*>(0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);

	if (H2Config_hide_ingame_chat) {
		datum local_player_datum_index = h2mod->get_player_datum_index_from_controller_index(0);
		if (s_player::GetPlayer(DATUM_INDEX_TO_ABSOLUTE_INDEX(local_player_datum_index))->is_chatting == 2) {
			extern void hotkeyFuncToggleHideIngameChat();
			hotkeyFuncToggleHideIngameChat();
		}
		return true;
	}

	else if (*GameEngine != 3 && Engine::get_game_life_cycle() == _life_cycle_in_game) {
		//Enable chat in engine mode and game state mp.
		return false;
	}
	else {
		//original test - if is campaign
		return *GameEngine == 1;
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

	if (!b_showHUD || ice_cream_flavor_available(_blind))
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

typedef void(__cdecl render_camera_build_projection_t)(char*, int, int);
render_camera_build_projection_t* p_render_camera_build_projection;

void __cdecl render_camera_build_projection_hook(char* camera, int frustum_bounds, int out_projection)
{
	float old_camera_field_of_view = *(float*)(camera + 0x28);
	
	if (H2Config_static_first_person) 
	{
		*(float*)(camera + 0x28) = ((64.f * M_PI) / 180.0f) * 0.78500003f;
		//*(float*)(a1 + 0x28) = 0.86558843f;
	}
	
	p_render_camera_build_projection(camera, frustum_bounds, out_projection);
	
	*(float*)(camera + 0x28) = old_camera_field_of_view;
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

	if (h2mod->GetEngineType() == e_engine_type::_multiplayer) {

		auto hud_reticles = tags::find_tag(blam_tag::tag_group_type::bitmap, "ui\\hud\\bitmaps\\new_hud\\crosshairs\\hud_reticles");
		char* hud_reticles_data = tags::get_tag<blam_tag::tag_group_type::bitmap, char>(hud_reticles);
		tags::tag_data_block* hud_reticles_bitmaps = reinterpret_cast<tags::tag_data_block*>(hud_reticles_data + 0x44);
		if (hud_reticles_bitmaps->block_data_offset != -1)
		{
			char* reticle_bitmap = tags::get_tag_data() + hud_reticles_bitmaps->block_data_offset;
			for (auto i = 0; i < hud_reticles_bitmaps->block_count; i++)
			{
				point2d* ui_bitmap_size = reinterpret_cast<point2d*>(reticle_bitmap + (i * 0x74) + 0x4);
				if (!crosshairInit) {
					if (defaultCrosshairSizes == nullptr)
						defaultCrosshairSizes = new point2d[hud_reticles_bitmaps->block_count];

					defaultCrosshairSizes[i].x = ui_bitmap_size->x;
					defaultCrosshairSizes[i].y = ui_bitmap_size->y;
				}

				*ui_bitmap_size = point2d{ (short)round(defaultCrosshairSizes[i].x * H2Config_crosshair_scale), (short)round(defaultCrosshairSizes[i].y * H2Config_crosshair_scale) };
			}
		}
		crosshairInit = true;
	}
}
void HudElements::setCrosshairPos() {

	if (Memory::IsDedicatedServer())
		return;

	if (!FloatIsNaN(H2Config_crosshair_offset)) {
		tags::tag_data_block* player_controls_block = reinterpret_cast<tags::tag_data_block*>(tags::get_matg_globals_ptr() + 240);
		if (player_controls_block->block_count > 0)
		{
			for (int i = 0; i < player_controls_block->block_count; i++) {
				*(float*)(tags::get_tag_data() + player_controls_block->block_data_offset + 128 * i + 28) = H2Config_crosshair_offset;
			}
		}
	}
}

void HudElements::RadarPatch()
{
	WriteValue<BYTE>(H2BaseAddr + 0x2849C4, (BYTE)4);
	addDebugText("Motion sensor patched successfully.");
}

void HudElements::setFOV() {

	if (Memory::IsDedicatedServer())
		return;

	static float fov = 70.0f * M_PI / 180.0f;
	static bool fov_redirected = false;
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
	//Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
	PatchCall(Memory::GetAddress(0x22667B), RenderIngameChat);
	PatchCall(Memory::GetAddress(0x226628), RenderIngameChat);
	PatchCall(Memory::GetAddress(0x228579), RenderFirstPersonCheck);
	PatchCall(Memory::GetAddress(0x223955), RenderHudCheck);
	PatchCall(Memory::GetAddress(0x191440), render_camera_build_projection_hook);
	p_render_camera_build_projection = Memory::GetAddress<render_camera_build_projection_t*>(0x1953f5);
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
