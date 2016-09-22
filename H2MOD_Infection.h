#pragma once
#ifndef H2INFECTION_H
#define H2INFECTION_H
#include <unordered_map>

class InfectionPlayer
{
public:
	wchar_t PlayerName[16];
	bool infected;
};

class Infection
{
public:
	void PlayerInfected(int);
	void PreSpawn(int);
	void SpawnPlayer(int);
	void Initialize();
	void FindZombie();

	std::unordered_map<InfectionPlayer*, bool> infected_players;


};
#endif
