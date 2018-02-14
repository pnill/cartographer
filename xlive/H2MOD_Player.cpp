#include "Globals.h"

Player::Player(std::string playerIp, std::wstring playerName, int peerIndex, int playerTeam, int playerIndex, long long identifier)
{
	this->playerIp = playerIp;
	this->playerName = playerName;
	this->peerIndex = peerIndex;
	this->playerTeam = playerTeam;
	this->playerIndex = playerIndex;
	this->identifier = identifier;
}

void Player::setIsZombie(bool isZombie) {
	this->isZombie = isZombie;
}

void Player::setGunGameLevel(int level)
{
	this->gunGameLevel = level;
}

std::wstring Player::getPlayerName()
{
	return this->playerName;
}

int Player::getPlayerTeam()
{
	return this->playerTeam;
}

int Player::getPeerIndex()
{
	return this->peerIndex;
}

std::string Player::getIp()
{
	return this->playerIp;
}

int Player::getPlayerIndex()
{
	return this->playerIndex;
}

long long Player::getPlayerIdentifier()
{
	return this->identifier;
}

int Player::getGunGameLevel()
{
	return this->gunGameLevel;
}

BOOL Player::getIsZombie() {
	return this->isZombie;
}