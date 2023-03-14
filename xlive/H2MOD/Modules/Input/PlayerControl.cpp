#include "stdafx.h"

#include "PlayerControl.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\Modules\EventHandler\EventHandler.h"


typedef void __cdecl c_UpdatePlayerControl(float* yaw, float* pitch);
c_UpdatePlayerControl* p_UpdatePlayerControl;

void __cdecl UpdatePlayerControl(float* yawChange, float* pitchChange)
{
	p_UpdatePlayerControl(yawChange, pitchChange);
	//EventHandler::execute_callback<EventHandler::PlayerControlEvent>(execute_after, &yawChange, &pitchChange);
	//EventHandler::executePlayerControlCallback(yawChange, pitchChange);
}

typedef void __cdecl p_network_player_actions_to_player_actions(s_player_motion* nActions, s_player_actions* pActions);
p_network_player_actions_to_player_actions* network_player_actions_to_player_actions;


void PlayerControl::ApplyHooks()
{
	return; // TODO FIXME usercall convention functions, bad hooks
	p_UpdatePlayerControl = Memory::GetAddress<c_UpdatePlayerControl*>(0x90D62);
	network_player_actions_to_player_actions = Memory::GetAddress<p_network_player_actions_to_player_actions*>(0x1DB569);
	PatchCall(Memory::GetAddress(0x93596), UpdatePlayerControl);
	PatchCall(Memory::GetAddress(0x9390D), UpdatePlayerControl);
}

s_player_actions PlayerControl::GetPlayerActions(int player_index)
{
	s_player_actions newActions;
	s_player_motion nActions = Memory::GetAddress<s_player_motion*>(0x514EE8)[player_index];
	network_player_actions_to_player_actions(&nActions, &newActions);
	return newActions;
}

s_player_control* PlayerControl::GetControls(int local_player_index)
{
	//What the hell is even this
	//auto player_controls_globals = (s_player_control_globals*)(*(DWORD*)(0x4ca37c));
	auto player_controls_globals = *Memory::GetAddress<s_player_control_globals**>(0x4ca37c);
	return &player_controls_globals->local_players[local_player_index];
}

s_player_motion* PlayerControl::GetPlayerMotion(int player_index)
{
	return Memory::GetAddress<s_player_motion*>(0x514EE8 + player_index * sizeof(s_player_motion));
}
void PlayerControl::DisableLocalCamera(bool state)
{
	auto player_controls_globals = *Memory::GetAddress<s_player_control_globals**>(0x4ca37c);
	player_controls_globals->disableCamera = state;
}
