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

namespace ObserverMode
{
	real_vector3d* CameraAngles;
	


	typedef float*(__cdecl *h_editor_camera_update)(int *a1, int a2, int a3);
	h_editor_camera_update p_editor_camera_update;

	float GetDistance(float x1, float y1, float x2, float y2)
	{
		auto d1 = x2 = x1;
		auto d2 = y2 - y2;
		return (float)(sqrt(pow(d1, 2.0f) + pow(d2, 2.0f)));
	}
	float GetAngle(float x1, float y1, float x2, float y2)
	{
		float n1 = y2 - y1;
		float distance = GetDistance(x1, y1, x2, y2);
		float d = n1 / distance;
		float n2 = (float)(-asin(d) * 57.295779513082323);
		if (y2 > y1)
			n2 = (float)(360.0 - asin(d) * 57.295779513082323);
		if (x1 > x2)
			n2 = (float)(180.0 + asin(d) * 57.295779513082323);
		
		return n2;
	}
	float DegToRad(float a1)
	{
		return (a1 * M_PI) / 180;
	}
	float* __cdecl editor_camera_update(int *a1, int a2, int a3)
	{
		LOG_INFO_GAME("[ObserverMode] : {} {} {}", *a1, a2, a3);
		float* result = p_editor_camera_update(a1, a2, a3);
		real_vector3d* CameraPosition = *h2mod->GetAddress<real_vector3d**>(0x4d84ec);
		//These are fucked and need to be fixed.... everything is offset by 4, have to go back and fix everything that uses it.
		auto control = PlayerControl::GetControls(0);

		CameraAngles->i = control->Actions.pitch;
		CameraAngles->j = control->Actions.throttle_x;

		
		//Temp
		//I don't know what the fuck I am doing.
		float movementAngle = GetAngle(0, 0, control->Actions.throttle_y, control->Actions.trigger);
		float movementDistance = GetDistance(0, 0, control->Actions.throttle_y, control->Actions.trigger);
		if (FloatIsNaN(movementAngle)) movementAngle = 0;
		if (FloatIsNaN(movementDistance)) movementDistance = 0;
		if (abs(movementAngle - 0) < 10) movementAngle = 0;
		if (abs(movementAngle - 360) < 10) movementAngle = 0;
		if (abs(movementAngle - 90) < 10) movementAngle = 90;
		if (abs(movementAngle - 180) < 10) movementAngle = 180;
		if (abs(movementAngle - 270) < 10) movementAngle = 270;

		float n1 = (float)(sin(DegToRad(movementAngle) * movementDistance));
		float n2 = (float)(sin(DegToRad(movementAngle) * movementDistance));

		float n3 = (float)(((cos(CameraAngles->j * n1) * sin(CameraAngles->i))));
		float n4 = (float)(((cos(CameraAngles->j * n1) * cos(CameraAngles->i))));
		float n5 = (float)(n2 * sin(CameraAngles->i));
		float n6 = (float)(n2 * cos(CameraAngles->i));

		CameraPosition->i -= (n3 + n5) * control->Actions.throttle_y;
		CameraPosition->j -= (n4 + n6) * control->Actions.trigger;

		

		//CameraPosition->j += (idk1 + idk3) * control->Actions.throttle_y;//PlayerControl::GetControls(0)->Actions.throttle_y * 0.5 * time_globals::get_seconds_per_tick();
		//CameraPosition->i -= (idk2 + idk4) * control->Actions.trigger;//PlayerControl::GetControls(0)->Actions.trigger * 0.5 * time_globals::get_seconds_per_tick();
		
		
		LOG_INFO_GAME("[ObserverMode] : {} {} - {} {}", IntToString<float>(CameraAngles->i), IntToString<float>(CameraAngles->j),
			IntToString<float>(PlayerControl::GetControls(0)->Actions.pitch), IntToString<float>(PlayerControl::GetControls(0)->Actions.yaw));
		return result;
	}

	void ApplyHooks()
	{
		p_editor_camera_update = (h_editor_camera_update)DetourFunc(h2mod->GetAddress<BYTE*>(0xCC41E), (BYTE*)editor_camera_update, 7);
	}

	void Initialize()
	{
		CameraAngles = h2mod->GetAddress<real_vector3d*>(0x4A84C0);
		//CameraPosition = *h2mod->GetAddress<real_vector3d**>(0x4d84ec);
		ApplyHooks();
	}
}
