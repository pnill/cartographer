#include "stdafx.h"
#include "camera.h"

s_camera* get_global_camera()
{
	return Memory::GetAddress<s_camera*>(0x4E66C8);
}