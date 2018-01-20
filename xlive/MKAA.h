#ifndef MKAA_h
#define MKAA_h

#pragma once

#include <iostream>
#include <Windows.h>
#include <windef.h>
#include <xinput.h>


#pragma comment(lib, "XInput9_1_0.lib")

void Xinput();
void DisableKeyboard();
extern int isPressed;

#endif
