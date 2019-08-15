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
			std::wstring outStr = L"Name=";
			outStr += this->getPlayerName(playerIndex);

			outStr += L", Name from game player state= ";
			outStr += h2mod->get_player_name_from_player_index(playerIndex);

			outStr += L", Team=";
			std::wostringstream ws2;
			ws2 << this->getPlayerTeamFromPlayerIndex(playerIndex);
			outStr += ws2.str();

			outStr += L", Player index=";
			std::wostringstream ws3;
			ws3 << playerIndex;
			outStr += ws3.str();

			outStr += L", Peer index=";
			std::wostringstream ws;
			ws << this->getPeerIndexFromPlayerIndex(playerIndex);
			outStr += ws.str();

			outStr += L", Identifier=";
			std::wostringstream ws4;
			ws4 << this->getPlayerXuidFromPlayerIndex(playerIndex);
			outStr += ws4.str();

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
			std::wstring outStr = L"Peer Name = ";
			outStr += NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].peer_name;

			outStr += L", PeerIndex = ";
			std::wostringstream ws;
			ws << peerIndex;
			outStr += ws.str();

			outStr += L", Player index= ";
			std::wostringstream ws2;
			ws2 << NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].player_index;
			outStr += ws2.str();

			outStr += L", Player Name= ";
			outStr += this->getPlayerName(NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].player_index);

			outStr += L", Identifier= ";
			std::wostringstream ws3;
			ws3 << this->getPlayerXuidFromPlayerIndex(NetworkSession::getCurrentNetworkSession()->membership.peer_info[peerIndex].player_index);
			outStr += ws3.str();

			commands->output(outStr);
			peerIndex++;
		} while (peerIndex < NetworkSession::getCurrentNetworkSession()->membership.total_peers);
	}

	std::wstring total_players = L"Total peers: " + std::to_wstring(NetworkSession::getCurrentNetworkSession()->membership.total_peers);
	commands->output(total_players);
}