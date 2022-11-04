#include "stdafx.h"

#include "ObserverMode.h"
#include "H2MOD.h"
#include "H2MOD\GUI\imgui_integration\imgui_handler.h"
#include "H2MOD\Modules\DirectorHooks\DirectorHooks.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "H2MOD\Utils\Utils.h"
#include "Util\Hooks\Hook.h"
#include <math.h>
#define _USE_MATH_DEFINES

namespace ObserverMode
{
	datum observer_current_index = 0;
	e_observer_mode ObserverMode = observer_freecam;
	real_vector3d* CameraAngles;
	byte* EditorCameraTargetFunction;
	s_player_motion** network_player_actions;



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
		real_vector3d* CameraPosition = *Memory::GetAddress<real_vector3d**>(0x4d84ec);
		//These are fucked and need to be fixed.... everything is offset by 4, have to go back and fix everything that uses it.
		auto control = PlayerControl::GetControls(0);

		CameraAngles->i = control->actions.facing.yaw.rad;
		CameraAngles->j = control->actions.facing.pitch.rad;
		//CameraAngles->k += (control->actions.trigger * 0.01f) + -(control->actions.secondary_trigger * 0.01f);

		

		if (ObserverMode == observer_freecam) 
		{
			//x2,y2 = [0, 1] * MaxShort
			float movementAngle = GetAngle(0, 0, control->actions.throttle.x * MAXSHORT, control->actions.throttle.y * MAXSHORT) + 90.0f;
			float movementDistance = GetDistance(0, 0, control->actions.throttle.x, control->actions.throttle.y);
			if (isnan(movementAngle)) movementAngle = 0;
			if (isnan(movementDistance)) movementDistance = 0;
			if (movementDistance > 1) movementDistance = 1;
			if (abs(movementAngle - 0) < 10) movementAngle = 0;
			if (abs(movementAngle - 360) < 10) movementAngle = 0;
			if (abs(movementAngle - 90) < 10) movementAngle = 90;
			if (abs(movementAngle - 180) < 10) movementAngle = 180;
			if (abs(movementAngle - 270) < 10) movementAngle = 270;

			//LOG_INFO_GAME("[ObserverMode] : Movement Angle {}", IntToString<float>(movementAngle));

			float PitchUnk = (((CameraAngles->j + ((float)1.492256522)) * 180.0f) / 2.984513f) - 90.0f;
			float YawUnk = (CameraAngles->i * 360.0f) / 6.283185f;

			float Num1 = (float)(sin((movementAngle * M_PI) / 180.0f) * (movementDistance * 0.05f));
			float Num2 = (float)-(cos((movementAngle * M_PI) / 180.0f) * (movementDistance * 0.05f));
			float num14 = (float)((sin((PitchUnk * M_PI) / 180.0) * Num1));
			float num15 = (float)(((cos((PitchUnk * M_PI) / 180.0) * Num1)) * sin((YawUnk * M_PI) / 180.0));
			float num16 = (float)(((cos((PitchUnk * M_PI) / 180.0) * Num1)) * cos((YawUnk * M_PI) / 180.0));
			float num17 = (float)((Num2)* sin(((YawUnk - 90.0f) * M_PI) / 180.0));
			float num18 = (float)((Num2)* cos(((YawUnk - 90.0f) * M_PI) / 180.0));

			CameraPosition->i += (num18 + num16);
			CameraPosition->j += (num15 + num17);
			CameraPosition->k += num14 + (control->actions.trigger * 0.03f) + -(control->actions.secondary_trigger * 0.03f);

			if (ImGuiHandler::IsWindowActive("debug_overlay")) 
			{
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("oyaw", IntToString<float>(CameraAngles->i));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("opitch", IntToString<float>(CameraAngles->j));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("oyaw", IntToString<float>(CameraAngles->i));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("opitch", IntToString<float>(CameraAngles->j));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("oyawu", IntToString<float>(YawUnk));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("opitchu", IntToString<float>(PitchUnk));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("o1", IntToString<float>(Num1));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("o2", IntToString<float>(Num2));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("o3", IntToString<float>(num14));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("o4", IntToString<float>(num15));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("o5", IntToString<float>(num16));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("o6", IntToString<float>(num17));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("o7", IntToString<float>(num18));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("ox", IntToString<float>((num18 + num16)));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("oy", IntToString<float>((num15 + num17)));
				ImGuiHandler::ImDebugOverlay::UpdateWatchItem("oz", IntToString<float>(num14));
			}
		}

		if(ObserverMode == observer_followcam)
		{
			float distance = control->actions.throttle.y * 0.01;
			CameraPosition->j -= distance;
		}


		return result;
	}
	int currentPlayerIndex = 0;
	void NextPlayer()
	{
		auto pArray = s_player::GetArray();
		if (currentPlayerIndex + 1 > pArray->total_elements_used - 1)
			currentPlayerIndex = 0;
		else
			currentPlayerIndex++;

		datum newIndex = ((datum)pArray->data[0x204 * currentPlayerIndex + 0x28]);
		if (DATUM_IS_NONE(newIndex))
			NextPlayer();
		else
			observer_current_index = newIndex;
	}

	void SwitchObserverMode(e_observer_mode mode)
	{
		if(mode != ObserverMode)
		{
			
			if (mode == observer_followcam) 
			{
				DirectorHooks::SetDirectorMode(DirectorHooks::e_editor);
				//real_vector3d* CameraPosition = *Memory::GetAddress<real_vector3d**>(0x4d84ec);
				*EditorCameraTargetFunction = 1;
				//PreviousPosY = CameraPosition->j;
				//CameraPosition->j = 3;
			}
			else if (mode == observer_freecam) 
			{
				DirectorHooks::SetDirectorMode(DirectorHooks::e_editor);
				//real_vector3d* CameraPosition = *Memory::GetAddress<real_vector3d**>(0x4d84ec);
				*EditorCameraTargetFunction = 0;
				//CameraPosition->j = PreviousPosY;
			}
			else if (mode == observer_firstperson)
			{
				DirectorHooks::SetDirectorMode(DirectorHooks::e_game);
			}
			else if (mode == observer_none)
			{
				
			}
			ObserverMode = mode;
		}
	}


	void SightJackObject()
	{
		if (ObserverMode == observer_firstperson) {
			PlayerIterator playerIt;

			datum unitIndexSameAsObserverIndex(DATUM_INDEX_NONE);

			while (playerIt.get_next_active_player())
			{
				s_player* player = playerIt.get_current_player_data();
				if (player->unit_index == observer_current_index)
				{
					unitIndexSameAsObserverIndex = observer_current_index;
					break;
				}
			}
				
			auto control = PlayerControl::GetControls(0);
			auto player_actions = PlayerControl::GetPlayerActions(currentPlayerIndex);
			s_player::GetPlayer(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)))->unit_index = observer_current_index;
			//control->ControllingDatum = observer_current_index;
			//control->actions.yaw = player_actions->facing.yaw.as_rad();
			//control->actions.pitch = player_actions->facing.pitch.as_rad();
			//control->actions = player_actions;
		}
	}

	typedef void(__cdecl p_sub_7BD2EC)(int a1, DWORD* a2);
	p_sub_7BD2EC* sub_7BD2EC;

	//This is called with the local player index and unk
	//local_player_index was used in the original function to grab the local player unit_datum for following
	//unk is believed to be the editor_camera setting
	void FollowObject(int local_player_index, int unk)
	{
		s_data_array* Objects = *Memory::GetAddress<s_data_array**>(0x4E461C);
		if (observer_current_index == -1) {
			NextPlayer();
		}
		if(unk != 65535){
			*(DWORD*)(unk + 8) = 0;
			*(DWORD*)(unk) = DATUM_INDEX_TO_ABSOLUTE_INDEX(observer_current_index);
			*(WORD*)(unk + 4) = -1;

			auto object = *(DWORD*)&Objects->data[12 * DATUM_INDEX_TO_ABSOLUTE_INDEX(observer_current_index) + 4];
			//LOG_INFO_GAME("[ObserverMode] Object {}, Address {}", observer_current_index, IntToString<int>(object, std::hex));
			sub_7BD2EC(DATUM_INDEX_TO_ABSOLUTE_INDEX(observer_current_index), (DWORD*)(unk + 12));
		}
	}

	void ApplyHooks()
	{
		p_editor_camera_update = (h_editor_camera_update)DetourFunc(Memory::GetAddress<BYTE*>(0xCC41E), (BYTE*)editor_camera_update, 7);


		PatchCall(Memory::GetAddress(0xCC59C), FollowObject);

		//Replace nullsub in game_tick
		PatchCall(Memory::GetAddress(0x4A4C7), SightJackObject);
	}

	void Initialize()
	{
		CameraAngles = Memory::GetAddress<real_vector3d*>(0x4A84C0);
		EditorCameraTargetFunction = Memory::GetAddress<byte*>(0x4D8500);
		network_player_actions = Memory::GetAddress<s_player_motion**>(0x514EE8);

		sub_7BD2EC = Memory::GetAddress<p_sub_7BD2EC*>(0x13D2EC);

		ImGuiHandler::ImDebugOverlay::AddWatchItem("oyaw", "Observer Yaw");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("opitch", "Observer Pitch");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("oyawu", "Yaw Calc");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("opitchu", "Pitch Calc");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("o1", "num1");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("o2", "num2");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("o3", "num14");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("o4", "num15");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("o5", "num16");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("o6", "num17");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("o7", "num18");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("ox", "XMod");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("oy", "YMod");
		ImGuiHandler::ImDebugOverlay::AddWatchItem("oz", "ZMod");


		ApplyHooks();
	}
}
