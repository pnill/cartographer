#pragma once
#ifndef H2MOD_H
#define H2MOD_H
#include "Hook.h"


class H2MOD
{
public:
		void Initialize();
		int SetGravity(float);
		int unit_kill(int);
		int get_unit_from_player_index(int);
		int unit_set_active_camo(int);
		int camera_set_fov(float, short);
		DWORD GetBase();
		
		BOOL Server;
private:
		DWORD Base;
};


extern H2MOD* h2mod;


#endif