#include "stdafx.h"

std::random_device rd;

// for laptops with configured with multiple GPUs
// signal the operating system to prefer the high performance GPU over the integrated one when possible
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}