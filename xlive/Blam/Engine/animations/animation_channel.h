#pragma once
#include <wtypes.h>

class c_animation_channel
{
	int unk0;
	__int16 unk4;
	__int16 unk6;
	int unk8;
	char unkC;
	char unkD;
	__int16 unkE;
	char unk10;
	char unk11;
	__int16 unk12;
	float unk14;
	float unk18;
	float unk1C;

public:
	c_animation_channel();
	~c_animation_channel();
	void create();
	void initialize();
};
