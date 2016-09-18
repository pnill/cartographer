#pragma once
#ifndef H2INFECTION_H
#define H2INFECTION_H
#include <unordered_map>

class Infection
{
public:
	void PlayerInfected(int);
	void PreSpawn(int);
	void SpawnPlayer(int);
	void Initialize();
	void FindZombie();

	std::unordered_map<wchar_t*, bool> infected_players;


};
#endif
