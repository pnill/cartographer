#include "stdafx.h"
#include "Globals.h"

bool Players::IsActive(int playerIndex)
{
	return (1 << playerIndex) & NetworkSession::getCurrentNetworkSession()->membership.player_active_mask;
}

int Players::getPlayerCount()
{
	return NetworkSession::getCurrentNetworkSession()->membership.total_party_players;
}

Membership* Players::getMembershipData() 
{
	return &NetworkSession::getCurrentNetworkSession()->membership;
}

PlayerInformation* Players::getPlayerInformation(int peerIndex) 
{
	return &this->getMembershipData()->player_info[peerIndex];
}

int Players::getPeerIndexFromPlayerIndex(int playerIndex) 
{
	return this->getPlayerInformation(playerIndex)->peer_index;
}

wchar_t* Players::getPlayerName(int playerIndex)
{
	return this->getPlayerInformation(playerIndex)->player_properties.player_name;
}

int Players::getPeerIndexFromPlayerXuid(long long xuid) {
	if (NetworkSession::getCurrentNetworkSession()->membership.total_party_players > 0) {
		int playerIndex = 0;
		do {
			long long tempXuid = this->getPlayerXuidFromPlayerIndex(playerIndex);
			if (xuid == tempXuid)
				return this->getPeerIndexFromPlayerIndex(playerIndex);
	
			playerIndex++;
		} while (playerIndex < NetworkSession::getCurrentNetworkSession()->membership.total_party_players);
	}
	return -1;
}

long long Players::getPlayerXuidFromPlayerIndex(int playerIndex) 
{
	return this->getPlayerInformation(playerIndex)->identifier;
}

int Players::getPlayerTeamFromPlayerIndex(int playerIndex) 
{
	return this->getPlayerInformation(playerIndex)->player_properties.player_team;
}

int Players::getPlayerTeamFromXuid(long long xuid) {
	if (this->getPlayerCount() > 0) {
		int playerIndex = 0;
		do {
			long long tempXuid = this->getPlayerXuidFromPlayerIndex(playerIndex);
			if (xuid == tempXuid) 
				return this->getPlayerTeamFromPlayerIndex(playerIndex);
			
			playerIndex++;
		} while (playerIndex < this->getPlayerCount());
	}
	return -1;
}

void Players::logAllPlayersToConsole() {
	int playerIndex = 0;
	do {
		if (this->IsActive(playerIndex))
		{
			std::wstring outStr = L"Player index=" + std::to_wstring(playerIndex);
			outStr += L", Peer index=" + std::to_wstring(this->getPeerIndexFromPlayerIndex(playerIndex));
			outStr += L", PlayerName=";
			outStr += this->getPlayerName(playerIndex);
			outStr += L", Name from game player state= ";
			outStr += h2mod->get_player_name_from_player_index(playerIndex);
			outStr += L", Team=" + std::to_wstring(this->getPlayerTeamFromPlayerIndex(playerIndex));
			outStr += L", Identifier=" + std::to_wstring(this->getPlayerXuidFromPlayerIndex(playerIndex));
			
			commands->output(outStr);
		}
		playerIndex++;
	} while (playerIndex < 16);

	std::wstring total_players = L"Total players: " + std::to_wstring(this->getPlayerCount());
	commands->output(total_players);
}

void Players::logAllPeersToConsole() {
	if (NetworkSession::getCurrentNetworkSession()->membership.total_peers > 0) {
		int peerIndex = 0;
		do {
			int playerIndex = NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].player_index;

			std::wstring outStr = L"Peer index=" + std::to_wstring(peerIndex);
			outStr += L", Player index=" + std::to_wstring(playerIndex);
			outStr += L", Peer Name=";
			outStr += NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].peer_name;
			outStr += L", Player name=";
			outStr += this->getPlayerName(playerIndex);
			outStr += L", Name from game player state= ";
			outStr += h2mod->get_player_name_from_player_index(playerIndex);
			outStr += L", Identifier=" + std::to_wstring(this->getPlayerXuidFromPlayerIndex(playerIndex));

			commands->output(outStr);
			peerIndex++;
		} while (peerIndex < NetworkSession::getCurrentNetworkSession()->membership.total_peers);
	}

	std::wstring total_players = L"Total peers: " + std::to_wstring(NetworkSession::getCurrentNetworkSession()->membership.total_peers);
	commands->output(total_players);
}