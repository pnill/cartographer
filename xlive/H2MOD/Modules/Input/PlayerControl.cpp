#include "PlayerControl.h"
#include "H2MOD/Modules/Config/Config.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "Blam/Engine/Players/PlayerControls.h"


typedef void __cdecl c_UpdatePlayerControl(float yaw, float pitch);
c_UpdatePlayerControl* p_UpdatePlayerControl;

void __cdecl UpdatePlayerControl(float yawChange, float pitchChange)
{
	p_UpdatePlayerControl(yawChange, pitchChange);
	EventHandler::executePlayerControlCallback(yawChange, pitchChange);
}

typedef void __cdecl p_network_player_actions_to_player_actions(s_network_player_actions *nActions, s_player_actions *pActions);
p_network_player_actions_to_player_actions* network_player_actions_to_player_actions;



void PlayerControl::ApplyHooks()
{
	p_UpdatePlayerControl = h2mod->GetAddress<c_UpdatePlayerControl*>(0x90D62);
	network_player_actions_to_player_actions = h2mod->GetAddress<p_network_player_actions_to_player_actions*>(0x1DB569);
	PatchCall(h2mod->GetAddress(0x93596), UpdatePlayerControl);
	PatchCall(h2mod->GetAddress(0x9390D), UpdatePlayerControl);
}

s_player_actions PlayerControl::GetPlayerActions(int player_index)
{
	s_player_actions newActions;
	s_network_player_actions nActions = *h2mod->GetAddress<s_network_player_actions*>(0x514EE8 + player_index * sizeof(s_network_player_actions));
	network_player_actions_to_player_actions(&nActions, &newActions);
	return newActions;
}

Blam::EngineDefinitions::Players::s_player_control* PlayerControl::GetControls(int local_player_index)
{
	return *h2mod->GetAddress<s_player_control**>(0x4ca37c + (local_player_index * sizeof(s_player_control)));
}
