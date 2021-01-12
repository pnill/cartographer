#include "ObserverMode.h"
#include "Blam/Maths/real_math.h"
#include "H2MOD.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Utils/Utils.h"
#include "Blam/Engine/Game/GameTimeGlobals.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"

namespace ObserverMode
{
	real_vector3d* CameraAngles;
	


	typedef float*(__cdecl *h_editor_camera_update)(int *a1, int a2, int a3);
	h_editor_camera_update p_editor_camera_update;
	float DegToRad(float degree)
	{
		return degree * (M_PI / 180.0f);
	}
	float RadToDeg(float rad)
	{
		return rad * (180.0f / M_PI);
	}
	float GetDistance(float x1, float y1, float x2, float y2)
	{
		//Distance between two points 
		auto d1 = x2 - x1;
		auto d2 = y2 - y1;
		return (float)(
			sqrt(
				pow(d1, 2) + pow(d2, 2)
			)
		);
	}
	//Gets the small angle theta
	float GetAngle(float x1, float y1, float x2, float y2)
	{
		float oppositeDistance = y2 - y1;
		float adjacentDistance = GetDistance(x1, y1, x2, y2);
		float d = oppositeDistance / adjacentDistance;
		float n2 = (float)(-asin(d) * RadToDeg(1));

		if (y2 > y1)
			n2 = (float)(360.0f - asin(d) * RadToDeg(1));
		if (x1 > x2)
			n2 = (float)(180.0f + asin(d) * RadToDeg(1));
		
		return n2;
	}

	float* __cdecl editor_camera_update(int *a1, int a2, int a3)
	{
		//LOG_INFO_GAME("[ObserverMode] : {} {} {}", *a1, a2, a3);
		float* result = p_editor_camera_update(a1, a2, a3);
		real_vector3d* CameraPosition = *h2mod->GetAddress<real_vector3d**>(0x4d84ec);
		//These are fucked and need to be fixed.... everything is offset by 4, have to go back and fix everything that uses it.
		auto control = PlayerControl::GetControls(0);

		CameraAngles->i = control->Actions.pitch;
		CameraAngles->j = control->Actions.throttle_x;

		imgui_handler::DebugOverlay::UpdateWatchItem("oyaw", IntToString<float>(CameraAngles->i));
		imgui_handler::DebugOverlay::UpdateWatchItem("opitch", IntToString<float>(CameraAngles->j));

		//x2,y2 = [0, 1] * MaxShort
		float movementAngle = GetAngle(0, 0, control->Actions.throttle_y * MAXSHORT, control->Actions.trigger* MAXSHORT) + 90.0f;
		float movementDistance = GetDistance(0, 0, control->Actions.throttle_y, control->Actions.trigger);
		if (isnan(movementAngle)) movementAngle = 0;
		if (isnan(movementDistance)) movementDistance = 0;
		if (movementDistance > 1) movementDistance = 1;
		if (abs(movementAngle - 0) < 10) movementAngle = 0;
		if (abs(movementAngle - 360) < 10) movementAngle = 0;
		if (abs(movementAngle - 90) < 10) movementAngle = 90;
		if (abs(movementAngle - 180) < 10) movementAngle = 180;
		if (abs(movementAngle - 270) < 10) movementAngle = 270;

		LOG_INFO_GAME("[ObserverMode] : Movement Angle {}", IntToString<float>(movementAngle));
		
		float PitchUnk = (((CameraAngles->j + ((float)1.492256522)) * 180.0f) / 2.984513f) - 90.0f;
		float YawUnk = (CameraAngles->i * 360.0f) / 6.283185f;

		float Num1 = (float)(sin((movementAngle * M_PI) / 180.0f) * (movementDistance * 0.05f));
		float Num2 = (float)-(cos((movementAngle * M_PI) / 180.0f) * (movementDistance * 0.05f));
		float num14 = (float)((sin((PitchUnk * M_PI) / 180.0) * Num1) * movementDistance);
		float num15 = (float)(((cos((PitchUnk * M_PI) / 180.0) * Num1) * movementDistance) * sin((YawUnk * M_PI) / 180.0));
		float num16 = (float)(((cos((PitchUnk * M_PI) / 180.0) * Num1) * movementDistance) * cos((YawUnk * M_PI) / 180.0));
		float num17 = (float)((Num2 * movementDistance) * sin(((YawUnk - 90.0f) * M_PI) / 180.0));
		float num18 = (float)((Num2 * movementDistance) * cos(((YawUnk - 90.0f) * M_PI) / 180.0));

		CameraPosition->i += (num18 + num16);
		CameraPosition->j += (num15 + num17);
		CameraPosition->k += num14;

		imgui_handler::DebugOverlay::UpdateWatchItem("oyaw", IntToString<float>(CameraAngles->i));
		imgui_handler::DebugOverlay::UpdateWatchItem("opitch", IntToString<float>(CameraAngles->j));
		imgui_handler::DebugOverlay::UpdateWatchItem("oyawu", IntToString<float>(YawUnk));
		imgui_handler::DebugOverlay::UpdateWatchItem("opitchu", IntToString<float>(PitchUnk));
		imgui_handler::DebugOverlay::UpdateWatchItem("o1", IntToString<float>(Num1));
		imgui_handler::DebugOverlay::UpdateWatchItem("o2", IntToString<float>(Num2));
		imgui_handler::DebugOverlay::UpdateWatchItem("o3", IntToString<float>(num14));
		imgui_handler::DebugOverlay::UpdateWatchItem("o4", IntToString<float>(num15));
		imgui_handler::DebugOverlay::UpdateWatchItem("o5", IntToString<float>(num16));
		imgui_handler::DebugOverlay::UpdateWatchItem("o6", IntToString<float>(num17));
		imgui_handler::DebugOverlay::UpdateWatchItem("o7", IntToString<float>(num18));
		imgui_handler::DebugOverlay::UpdateWatchItem("ox", IntToString<float>((num18 + num16)));
		imgui_handler::DebugOverlay::UpdateWatchItem("oy", IntToString<float>((num15 + num17)));
		imgui_handler::DebugOverlay::UpdateWatchItem("oz", IntToString<float>(num14));
		/*float n1 = (float)(sin(DegToRad(movementAngle) * movementDistance));
		float n2 = (float)(cos(DegToRad(movementAngle) * movementDistance));

		float n3 = (float)(((cos(CameraAngles->j * n1) * sin(CameraAngles->i))));
		float n4 = (float)(((cos(CameraAngles->j * n1) * cos(CameraAngles->i))));
		float n5 = (float)(n2 * sin(DegToRad(RadToDeg(CameraAngles->i) - 90.0f)));
		float n6 = (float)(n2 * cos(DegToRad(RadToDeg(CameraAngles->i) - 90.0f)));

		CameraPosition->i += (n3 + n5) * control->Actions.throttle_y * 0.0005f;
		CameraPosition->j += (n4 + n6) * control->Actions.trigger * 0.0005f;*/

		//LOG_INFO_GAME("[ObserverMode] : {} {} {} {} {} {}", IntToString<float>(n1), IntToString<float>(n2), IntToString<float>(n3), IntToString<float>(n4), IntToString<float>(n5), IntToString<float>(n6), IntToString<float>((n3 + n5) * control->Actions.throttle_y), IntToString<float>((n4 + n6) * control->Actions.trigger));
		//LOG_INFO_GAME("[ObserverMode] : {} {}", IntToString<float>(n3), IntToString<float>(n4));
		//LOG_INFO_GAME("[ObserverMode] : {} {}", IntToString<float>(n5), IntToString<float>(n6));
		//LOG_INFO_GAME("[ObserverMode] : {} {}", IntToString<float>((n3 + n5) * control->Actions.throttle_y), IntToString<float>((n4 + n6) * control->Actions.trigger));

		/*CameraPosition->i += control->Actions.throttle_y * 0.0005;
		CameraPosition->j += control->Actions.trigger * 0.0005;*/

		//CameraPosition->j += (idk1 + idk3) * control->Actions.throttle_y;//PlayerControl::GetControls(0)->Actions.throttle_y * 0.5 * time_globals::get_seconds_per_tick();
		//CameraPosition->i -= (idk2 + idk4) * control->Actions.trigger;//PlayerControl::GetControls(0)->Actions.trigger * 0.5 * time_globals::get_seconds_per_tick();
		
		
		//LOG_INFO_GAME("[ObserverMode] : {} {} - {} {}", IntToString<float>(CameraAngles->i), IntToString<float>(CameraAngles->j),
		//	IntToString<float>(PlayerControl::GetControls(0)->Actions.pitch), IntToString<float>(PlayerControl::GetControls(0)->Actions.yaw));
		return result;
	}

	void ApplyHooks()
	{
		p_editor_camera_update = (h_editor_camera_update)DetourFunc(h2mod->GetAddress<BYTE*>(0xCC41E), (BYTE*)editor_camera_update, 7);
	}

	void Initialize()
	{
		CameraAngles = h2mod->GetAddress<real_vector3d*>(0x4A84C0);
		imgui_handler::DebugOverlay::AddWatchItem("oyaw", "Observer Yaw");
		imgui_handler::DebugOverlay::AddWatchItem("opitch", "Observer Pitch");
		imgui_handler::DebugOverlay::AddWatchItem("oyawu", "Yaw Calc");
		imgui_handler::DebugOverlay::AddWatchItem("opitchu", "Pitch Calc");
		imgui_handler::DebugOverlay::AddWatchItem("o1", "num1");
		imgui_handler::DebugOverlay::AddWatchItem("o2", "num2");
		imgui_handler::DebugOverlay::AddWatchItem("o3", "num14");
		imgui_handler::DebugOverlay::AddWatchItem("o4", "num15");
		imgui_handler::DebugOverlay::AddWatchItem("o5", "num16");
		imgui_handler::DebugOverlay::AddWatchItem("o6", "num17");
		imgui_handler::DebugOverlay::AddWatchItem("o7", "num18");
		imgui_handler::DebugOverlay::AddWatchItem("ox", "XMod");
		imgui_handler::DebugOverlay::AddWatchItem("oy", "YMod");
		imgui_handler::DebugOverlay::AddWatchItem("oz", "ZMod");
		//CameraPosition = *h2mod->GetAddress<real_vector3d**>(0x4d84ec);
		ApplyHooks();
	}
}
