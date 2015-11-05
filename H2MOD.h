#pragma once
#ifndef H2MOD_H
#define H2MOD_H
#include "Hook.h"


class H2MOD
{
public:
		void Initialize();
		DWORD GetBase();
		BOOL Server;
private:
		DWORD Base;
};


extern H2MOD* h2mod;


#endif