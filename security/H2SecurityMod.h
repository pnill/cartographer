#ifndef SECURITY_MOD_H
#define SECURITY_MOD_H

#ifdef H2VSECURITY_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DLLEXPORT __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DLLEXPORT __declspec(dllimport)  
#endif

#include "stdafx.h"

class DLLEXPORT SecurityMod {
public:
	virtual ~SecurityMod() {}
	virtual bool isLobby();
	virtual DWORD getBase();
	virtual void trace(LPSTR message, ...);
};

#endif