#pragma once
#ifndef XUSERPROPERTY_H
#define XUSERPROPERTY_H
#include "XLive\XUser\XUser.h"

typedef struct _XUSER_PROPERTY
{
	DWORD                               dwPropertyId;
	XUSER_DATA                          value;
} XUSER_PROPERTY, *PXUSER_PROPERTY;
#endif XUSERPROPERTY_H