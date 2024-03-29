#pragma once

struct hkBaseObject
{
	int32* __vftable /*VFT*/;
	uint16 m_memSizeAndCallbackFlag;
	uint16 m_referenceCount;
};
