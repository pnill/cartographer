#pragma once

struct hkBaseObject
{
	hkInt32* __vftable /*VFT*/;
	hkUint16 m_memSizeAndCallbackFlag;
	hkUint16 m_referenceCount;
};
