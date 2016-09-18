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

	std::unordered_map<bool, int> infected_players;


};
#endif
