#include "stdafx.h"
#include "animation_channel.h"


c_animation_channel::c_animation_channel()
{
	this->create();
}

c_animation_channel::~c_animation_channel()
{
	this->unk0 = -1;
	this->unk4 = -1;
	this->unk6 = -1;
	this->unk8 = -1;
	this->unk10 = 0;
}

void c_animation_channel::create()
{
	this->unk0 = -1;
	this->unk4 = -1;
	this->unk6 = -1;
	this->unk1C = 0.0;
	this->unk10 = 0;
	this->unk11 = 0;
	this->unk12 = 0;
	this->unk18 = 1.0;
	this->unk14 = 0;
	this->unk8 = -1;
	this->unkC = -1;
	this->unkD = -1;
	this->unkE = -1;
}

void c_animation_channel::initialize()
{
	this->unk0 = -1;
	this->unk4 = -1;
	this->unk6 = -1;
	this->unk1C = 0.0;
	this->unk10 = 0;
	this->unk11 = 0;
	this->unk12 = 0;
	this->unk18 = 1.0;
	this->unk14 = 0;
	this->unk8 = -1;
	this->unkE = -1;
	this->unkC = -1;
	this->unkD = -1;
}
