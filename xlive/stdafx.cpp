#include "stdafx.h"

// for laptops with configured with multiple GPUs
// signal the operating system to prefer the high performance GPU over the integrated one when possible
extern "C" {
	__declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) unsigned int AmdPowerXpressRequestHighPerformance = 0x00000001;
}