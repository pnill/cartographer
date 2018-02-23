#include "Globals.h"

Player::Player()
{
	this->peerIndex = -1;
	this->playerIndex = -1;
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

void Player::setPlayerName(std::wstring playerName)
{
	this->playerName = playerName;
}

int Player::getPlayerTeam()
{
	return this->playerTeam;
}

void Player::setPlayerTeam(int playerTeam)
{
	this->playerTeam = playerTeam;
}

int Player::getPeerIndex()
{
	return this->peerIndex;
}

void Player::setPeerIndex(int peerIndex)
{
	this->peerIndex = peerIndex;
}

std::string Player::getIp()
{
	return this->playerIp;
}

void Player::setIp(std::string ip)
{
	this->playerIp = ip;
}

int Player::getPlayerIndex()
{
	return this->playerIndex;
}

void Player::setPlayerIndex(int playerIndex)
{
	this->playerIndex = playerIndex;
}

long long Player::getPlayerIdentifier()
{
	return this->identifier;
}

void Player::setPlayerIdentifier(long long playerIdentifier)
{
	this->identifier = playerIdentifier;
}

int Player::getGunGameLevel()
{
	return this->gunGameLevel;
}

BOOL Player::getIsZombie() {
	return this->isZombie;
}
