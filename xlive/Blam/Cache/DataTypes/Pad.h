#pragma once
#pragma region PaddingMacros
#define J( symbol1, symbol2 ) _DO_JOIN( symbol1, symbol2 )
#define _DO_JOIN( symbol1, symbol2 ) symbol1##symbol2
//Pad the desired length of BYTES
#define PAD(BYTES) private: BYTE J(Unused_, __LINE__ )[BYTES];  public:             

/// Add an anonymous 8-bit (1 byte) field to a structure.
#define PAD8 unsigned char : 8;
/// Add an anonymous 16-bit (2 byte) field to a structure.
#define PAD16 unsigned short : 16;
/// Add an anonymous 24-bit field to a structure.
#define PAD24 unsigned char : 8; unsigned short : 16;
/// Add an anonymous 32-bit (4 byte) field to a structure.
#define PAD32 unsigned long : 32;
/// Add an anonymous 48-bit field to a structure.
#define PAD48 unsigned short : 16; unsigned long : 32;
/// Add an anonymous 64-bit (8 byte) field to a structure.
#define PAD64 unsigned __int64 : 64;
/// Add an anonymous 128-bit (16 byte) field to a structure.
#define PAD128 unsigned __int64 : 64; unsigned __int64 : 64;
#pragma endregion