#include "PlayerControl.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"


typedef void __cdecl c_UpdatePlayerControl(float yaw, float pitch);
c_UpdatePlayerControl* p_UpdatePlayerControl;

void __cdecl UpdatePlayerControl(float yawChange, float pitchChange)
{
	p_UpdatePlayerControl(yawChange, pitchChange);
	EventHandler::executePlayerControlCallback(yawChange, pitchChange);
}

typedef void __cdecl p_network_player_actions_to_player_actions(s_player_motion *nActions, s_player_actions *pActions);
p_network_player_actions_to_player_actions* network_player_actions_to_player_actions;



void PlayerControl::ApplyHooks()
{
	p_UpdatePlayerControl = Memory::GetAddress<c_UpdatePlayerControl*>(0x90D62);
	network_player_actions_to_player_actions = Memory::GetAddress<p_network_player_actions_to_player_actions*>(0x1DB569);
	PatchCall(Memory::GetAddress(0x93596), UpdatePlayerControl);
	PatchCall(Memory::GetAddress(0x9390D), UpdatePlayerControl);
}

s_player_actions PlayerControl::GetPlayerActions(int player_index)
{
	s_player_actions newActions;
	s_player_motion nActions = *Memory::GetAddress<s_player_motion*>(0x514EE8 + player_index * sizeof(s_player_motion));
	network_player_actions_to_player_actions(&nActions, &newActions);
	return newActions;
}

s_player_control* PlayerControl::GetControls(int local_player_index)
{
	return *Memory::GetAddress<s_player_control**>(0x4ca37c + (local_player_index * sizeof(s_player_control)));
}

s_player_motion* PlayerControl::GetPlayerMotion(int player_index)
{
	return Memory::GetAddress<s_player_motion*>(0x514EE8 + player_index * sizeof(s_player_motion));
}
