#pragma once
#include <wtypes.h>

class c_animation_channel
{
	int unk0;
	short unk4;
	short unk6;
	int unk8;
	char unkC;
	char unkD;
	short unkE;
	char unk10;
	char unk11;
	short unk12;
	float unk14;
	float unk18;
	float unk1C;

public:
	c_animation_channel();
	~c_animation_channel();
	void create();
	void initialize();
};
