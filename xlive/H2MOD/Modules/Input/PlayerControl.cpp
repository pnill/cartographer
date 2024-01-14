#include "stdafx.h"

#include "PlayerControl.h"

typedef void __cdecl p_network_player_actions_to_player_actions(s_player_motion* nActions, player_action* pActions);
p_network_player_actions_to_player_actions* network_player_actions_to_player_actions;

player_action PlayerControl::GetPlayerActions(int player_index)
{
	player_action newActions;
	s_player_motion nActions = Memory::GetAddress<s_player_motion*>(0x514EE8)[player_index];
	network_player_actions_to_player_actions(&nActions, &newActions);
	return newActions;
}