#ifndef SECURITY_H
#define SECURITY_H

#ifdef H2VSECURITY_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DLLEXPORT __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DLLEXPORT __declspec(dllimport)  
#endif

#include "stdafx.h"
#include "H2SecurityMod.h"

class DLLEXPORT SecurityUtil {
public:
	SecurityUtil(SecurityMod* a);
	void startScanning();
	void addHooks();

private:
	void runScanner();
	SecurityMod* mod;
};

#endif