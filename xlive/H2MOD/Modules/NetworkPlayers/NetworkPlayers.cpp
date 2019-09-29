#include "stdafx.h"
#include "Globals.h"

bool NetworkPlayers::IsActive(int playerIndex)
{
	return (1 << playerIndex) & NetworkSession::getCurrentNetworkSession()->membership.players_active_mask;
}

Membership* NetworkPlayers::getMembershipData()
{
	return &NetworkSession::getCurrentNetworkSession()->membership;
}

int NetworkPlayers::getPlayerCount()
{
	return getMembershipData()->total_players;
}

PlayerInformation* NetworkPlayers::getPlayerInformation(int playerIndex) 
{
	return &this->getMembershipData()->player_info[playerIndex];
}

int NetworkPlayers::getPeerIndexFromPlayerIndex(int playerIndex) 
{
	return this->getPlayerInformation(playerIndex)->peer_index;
}

wchar_t* NetworkPlayers::getPlayerName(int playerIndex)
{
	return this->getPlayerInformation(playerIndex)->properties.player_name;
}

long long NetworkPlayers::getPlayerXuidFromPlayerIndex(int playerIndex)
{
	return this->getPlayerInformation(playerIndex)->identifier;
}

int NetworkPlayers::getPlayerTeamFromPlayerIndex(int playerIndex)
{
	return this->getPlayerInformation(playerIndex)->properties.player_team;
}

int NetworkPlayers::getPeerIndexFromPlayerXuid(long long xuid) {
	if (getPlayerCount() > 0) {
		int playerIndex = 0;
		do {
			if (IsActive(playerIndex) && (this->getPlayerXuidFromPlayerIndex(playerIndex) == xuid))
				return this->getPeerIndexFromPlayerIndex(playerIndex);
			playerIndex++;
		} while (playerIndex < 16);
	}
	return -1;
}

int NetworkPlayers::getPlayerTeamFromXuid(long long xuid) {
	if (getPlayerCount() > 0) {
		int playerIndex = 0;
		do {
			if (IsActive(playerIndex) && (this->getPlayerXuidFromPlayerIndex(playerIndex) == xuid))
				return this->getPlayerTeamFromPlayerIndex(playerIndex);
			playerIndex++;
		} while (playerIndex < 16);
	}
	return -1;
}

void NetworkPlayers::logAllPlayersToConsole() {
	int playerIndex = 0;
	do {
		if (this->IsActive(playerIndex)) {
			std::wstring outStr = L"Player index=" + std::to_wstring(playerIndex);
			outStr += L", Peer index=" + std::to_wstring(this->getPeerIndexFromPlayerIndex(playerIndex));
			outStr += L", PlayerName=";
			outStr += this->getPlayerName(playerIndex);
			outStr += L", Name from game player state=";
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

void NetworkPlayers::logAllPeersToConsole() {
	if (NetworkSession::getCurrentNetworkSession()->membership.total_peers > 0) {
		int peerIndex = 0;
		do {
			int playerIndex = NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].player_index[0];

			std::wstring outStr = L"Peer index=" + std::to_wstring(peerIndex);
			outStr += L", Player index=" + std::to_wstring(playerIndex);
			outStr += L", Peer Name=";
			outStr += NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].peer_name;
			outStr += L", Player name=";
			outStr += this->getPlayerName(playerIndex);
			outStr += L", Name from game player state=";
			outStr += h2mod->get_player_name_from_player_index(playerIndex);
			outStr += L", Identifier=" + std::to_wstring(this->getPlayerXuidFromPlayerIndex(playerIndex));

			commands->output(outStr);
			peerIndex++;
		} while (peerIndex < NetworkSession::getCurrentNetworkSession()->membership.total_peers);
	}

	std::wstring total_players = L"Total peers: " + std::to_wstring(NetworkSession::getCurrentNetworkSession()->membership.total_peers);
	commands->output(total_players);
}