#include "GSMenu.h"
#include "H2Startup.h"
#include "stdafx.h"
#include <emmintrin.h>

extern bool IsCustomMenu;

int __cdecl MenuGameBrightnessIngame1(int a1) {
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x258C8C);
	int(__cdecl* WgitScreenfunctionPtr)(int);
	WgitScreenfunctionPtr = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x258C8C);
	return WgitScreenfunctionPtr(a1);
}

/*int __cdecl sub_105D239(int a1, int a2, int a3)
{
int v3; // ecx@0
int result; // eax@2
int v5; // [sp+0h] [bp-4h]@1

v5 = v3;
if (sub_EDC08E(dword_17C58E4, (int)&v5, a1, a2, a3))
{
sub_EDBDBF(dword_17C58E4, &v5);
result = v5;
}
else
{
sub_1059616();
if (sub_EDC08E(dword_17C58E4, (int)&v5, a1, a2, a3))
{
sub_EDBDBF(dword_17C58E4, &v5);
result = v5;
}
else
{
nullsub_15();
result = 0;
}
}
return result;
}*/

int __cdecl Allocator(int a1) {
	int(__cdecl* sub_105D239)(int a1, int a2, int a3);
	sub_105D239 = (int(__cdecl*)(int, int, int))((char*)H2BaseAddr + 0x20D239);
	return sub_105D239(a1, 0, 0);
}

void __cdecl sub_10E0C3E(int a1, unsigned int a2) {
	int v2 = a1;
	unsigned int v3 = a2 >> 7;

	do {
		__m128i sizepos = _mm_load_si128(0i64);
		_mm_store_si128((__m128i*)v2, sizepos);
		_mm_store_si128((__m128i*)(v2 + 16), sizepos);
		_mm_store_si128((__m128i*)(v2 + 32), sizepos);
		_mm_store_si128((__m128i*)(v2 + 48), sizepos);
		_mm_store_si128((__m128i*)(v2 + 64), sizepos);
		_mm_store_si128((__m128i*)(v2 + 80), sizepos);
		_mm_store_si128((__m128i*)(v2 + 96), sizepos);
		_mm_store_si128((__m128i*)(v2 + 112), sizepos);
		v2 += 128;
		--v3;
	} while (v3);
}

void* __cdecl sub_10E0C8B(void* a1, int a2, int a3) {

	void* result = a1;
	int v4 = (unsigned __int8)((char)a1 % -16);

	if ((char)a1 % -16) {
		memset(a1, 0, 16 - v4);
		sub_10E0C8B((char*)a1 + 16 - v4, 0, a3 - (16 - v4));
		result = a1;
	}
	else {
		int v5 = a3 & 0x7F;
		if (a3 != v5) {
			//void(__cdecl* sub_10E0C3E)(int a1, unsigned int a2);
			//sub_10E0C3E = (void(__cdecl*)(int, unsigned int))((char*)H2BaseAddr + 0x290C3E);
			sub_10E0C3E((int)a1, a3 - v5);
			result = a1;
		}
		if (v5) {
			memset(&((BYTE*)result)[a3] - v5, 0, v5);
			result = a1;
		}
	}
	return result;
}

void* __cdecl sub_10D7BA9(void *a1, int a2, unsigned int a3)
{
	unsigned int v3; // edx@1
	int v4; // eax@2
	void* result; // eax@5
	void* v6; // edi@6
	int v7; // ecx@7
	unsigned int v8; // ecx@10
	unsigned int v9; // ecx@10

	v3 = a3;
	if (a3) {
		//LOBYTE(v4) = a2;
		v4 = a2;
		DWORD dword_18EE9E4 = *(DWORD*)((char*)H2BaseAddr + 0xA9E9E4);
		if ((BYTE)a2 || a3 < 0x100 || !dword_18EE9E4) {
			v6 = a1;
			if (a3 < 4) {
				goto LABEL_17;
			}
			v7 = -(signed int)a1 & 3;
			if (v7) {
				v3 = a3 - v7;
				do {
					*(BYTE*)v6 = a2;
					v6 = (char*)v6 + 1;
					--v7;
				} while (v7);
			}
			v4 = 16843009 * (unsigned __int8)a2;
			v8 = v3;
			v3 &= 3u;
			v9 = v8 >> 2;
			if (!v9 || (memset(v6, v4, v9), v6 = (char*)v6 + 4 * v9, v3)) {
			LABEL_17:
				do
				{
					*(BYTE*)v6 = v4;
					v6 = (char*)v6 + 1;
					--v3;
				} while (v3);
			}
			result = a1;
		}
		else {
			//void*(__cdecl* sub_10E0C8B)(void* a1, int a2, int a3);
			//sub_10E0C8B = (void*(__cdecl*)(void*, int, int))((char*)H2BaseAddr + 0x290C8B);
			result = sub_10E0C8B(a1, a2, a3);
		}
	}
	else {
		result = a1;
	}
	return result;
}

signed int __cdecl sub_10DA189(int a3, char* a4, int a5, int a6) {//__usercall

																  //int(__cdecl* sub_10DEE22)(void);
																  //sub_10DEE22 = (int(__cdecl*)(void))((char*)H2BaseAddr + 0x28EE22);

																  //int v6; // esi@8

	if (a6) {
		if (!a3) {
		LABEL_8:
			exit(22);
			//v6 = 22;
			//*(DWORD*)sub_10DEE22() = 22;
			//LABEL_9:
			//int(__cdecl* sub_10DEDC3)(int a1, void* a2, int a3);
			//sub_10DEDC3 = (int(__cdecl*)(int, void*, int))((char*)H2BaseAddr + 0x28EDC3);
			//sub_10DEDC3(0, a1, v6);
			//return v6;
		}
	}
	else if (!a3) {
		if (!a4) {
			return 0;
		}
		goto LABEL_8;
	}
	char* v1 = a4;
	if (a4 <= 0) {
		goto LABEL_8;
	}
	if (!a6) {
		*(BYTE*)a3 = 0;
		return 0;
	}
	int v7 = a5;
	if (!a5) {
		*(BYTE*)a3 = 0;
		goto LABEL_8;
	}
	int v8 = a3;
	if (a6 == -1) {
		do {
			char v9 = *(BYTE*)v7;//Access violation reading location 0x00000020.
			*(BYTE*)v8++ = *(BYTE*)v7++;
			if (!v9)
				break;
			--v1;
		} while (v1);
	}
	else {
		do {
			char v10 = *(BYTE*)v7;
			*(BYTE*)v8++ = *(BYTE*)v7++;
			if (!v10)
				break;
			--v1;
			if (!v1)
				break;
			--a6;
		} while (a6);
		if (!a6)
			*(BYTE*)v8 = 0;
	}
	if (v1) {
		return 0;
	}
	if (a6 != -1) {
		*(BYTE*)a3 = 0;
		exit(34);
		//*(DWORD*)sub_10DEE22() = 34;
		//v6 = 34;
		//goto LABEL_9;
	}
	*(BYTE*)(a3 + a4 - 1) = 0;
	return 80;
}

int __cdecl CopyStrToMaxLen(int a1, char* a2, int a3) {//__usercall

													   //Something about this following call is very wrong.
													   //the psudo-code in sub_10DA189 throws the same error as in the real function.
	signed int(__cdecl* sub_10DA189)(int a1, int a2, int a3, int a4);
	sub_10DA189 = (signed int(__cdecl*)(int, int, int, int))((char*)H2BaseAddr + 0x28A189);
	sub_10DA189(a1, (int)a2, a3, -1);

	return a1;
}

void* __cdecl sub_EB65AD(int a1, char* a2, int a3, int a4, char a5, int a6)
{
	//int v7; // edx@1

	int v6 = ~((1 << a5) - 1) & ((1 << a5) - 1 + a1 + 76);

	//void*(__cdecl* sub_10D7BA9)(void* a1, int a2, unsigned int a3);
	//sub_10D7BA9 = (void*(__cdecl*)(void*, int, unsigned int))((char*)H2BaseAddr + 0x287BA9);
	void* v7 = sub_10D7BA9((void*)a1, 0, 0x4Cu);

	//helps for debugging
	void* addr = (void*)a1;

	//appears to copy the string a2 into a1 but leaves off any chars after (a3-1) and adds \0
	//if a2 is smaller than a3, it will copy whole of a2 into a1 but no more.
	//a1: "brightness level settings edit "
	//a2: "brightness level settings edit list"
	//a3: 32
	int(__cdecl* CopyStrToMaxLen)(int a1, char* a2, int a3);
	CopyStrToMaxLen = (int(__cdecl*)(int, char*, int))((char*)H2BaseAddr + 0x3802C);
	CopyStrToMaxLen(a1, a2, 32);

	*(WORD*)(a1 + 0x2A) &= 0xFFFCu;
	*(DWORD*)(a1 + 0x20) = a3;
	*(DWORD*)(a1 + 0x24) = a4;
	DWORD* v8 = (DWORD*)(v6 + a4 * a3);
	*(BYTE*)(a1 + 0x28) = a5;
	*(DWORD*)(a1 + 0x2C) = 0x64407440;
	*(BYTE*)(a1 + 0x29) = 0;
	*(DWORD*)(a1 + 0x48) = (int)v8;
	*(DWORD*)(a1 + 0x30) = a6;
	*(DWORD*)(a1 + 0x44) = v6;
	return sub_10D7BA9(v8, 0, 4 * ((a3 + 31) >> 5));
}

int __cdecl sub_EB6AD5(char* a1, int a2, int a3, int a4, int a5)
{
	int result = (**(int(__thiscall***)(DWORD, DWORD))a5)(a5, a3 * a2 + 4 * ((a2 + 31) >> 5) + (1 << a4) + 75);
	int v6 = result;
	if (result)
	{
		//int(__cdecl* sub_EB65AD)(int a1, char* a2, int a3, int a4, char a5, int a6);
		//sub_EB65AD = (int(__cdecl*)(int, char*, int, int, char, int))((char*)H2BaseAddr + 0x665AD);
		sub_EB65AD(result, a1, a2, a3, a4, a5);
		*(WORD*)(v6 + 42) |= 4u;
		result = v6;
	}
	return result;
}

int __cdecl sub_105D1FD(char* a1, int a2, int a3)
{
	DWORD* off_12B37BC = (DWORD*)((char*)H2BaseAddr + 0x4637BC);

	//int(__cdecl* sub_EB6AD5)(char* a1, int a2, int a3, int a4, int a5);
	//sub_EB6AD5 = (int(__cdecl*)(char*, int, int, int, int))((char*)H2BaseAddr + 0x66AD5);
	return sub_EB6AD5(a1, a2, a3, 0, (int)off_12B37BC);
}

//assuming
//a2 - previous value
//a3 - new value
int __cdecl sub_5F0DD8(int thisptr, int a2, int a3) {//__thiscall
													 //int(__thiscall* sub_10A0DD8)(int thisptr, int a2, int a3);
													 //sub_10A0DD8 = (int(__thiscall*)(int, int, int))((char*)H2BaseAddr + 0x250DD8);
													 //int rtnval = sub_10A0DD8(thisptr, a2, a3);
													 //return rtnval;

	int v3 = thisptr;
	a3 = *(DWORD*)a3 & 0xFFFF;

	signed int(__cdecl* sub_603D0E)(signed int* a1);
	sub_603D0E = (signed int(__cdecl*)(signed int*))((char*)H2BaseAddr + 0x263D0E);
	sub_603D0E(&a3);

	//There's something wrong with the following:
	//Access violation reading location 0x00000007.
	//bug elsewhere.
	int v4 = (*(int(__cdecl**)(int))(*(DWORD*)v3 + 56))(v3);//__thiscall
	int v5 = (*(int(__cdecl**)(int))(*(DWORD*)v3 + 52))(v3);//__thiscall

	int(__cdecl* sub_5A96DA)(int a1, int a2);
	sub_5A96DA = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x2096DA);
	return sub_5A96DA(v5, v4);
}

int __cdecl sub_10A0E22(int thisptr, int a2) {//__thiscall

	int v5; // eax@4
	int v2 = thisptr;

	void*(__thiscall* sub_1063B1C)(int thisptr, int a2);
	sub_1063B1C = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x213B1C);
	sub_1063B1C(thisptr, a2);

	//*(DWORD*)v2 = &off_122972C;
	*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D972C);

	void*(__thiscall* sub_106FFC9)(void* thisptr);
	sub_106FFC9 = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x21FFC9);

	int(__thiscall* sub_E5A551)(void* thisptr);
	sub_E5A551 = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0xA551);

	void(__stdcall* sub_10D870B)(int a1, int a2, int a3, void(__thiscall* a4)(DWORD), int a5);
	sub_10D870B = (void(__stdcall*)(int, int, int, void(__thiscall*)(DWORD), int))((char*)H2BaseAddr + 0x28870B);
	sub_10D870B((int)((char*)v2 + 176), 132, 4, (void(__thiscall*)(DWORD))sub_106FFC9, (int)sub_E5A551);

	*((BYTE*)v2 + 704) = 0;

	int(__thiscall* sub_10613C6)(int thisptr);
	sub_10613C6 = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x2113C6);
	sub_10613C6((int)((char*)v2 + 712));

	//*((DWORD*)v2 + 177) = &off_1229700;
	*((DWORD*)v2 + 177) = (DWORD)((char*)H2BaseAddr + 0x3D9700);

	*((DWORD*)v2 + 181) = (DWORD)v2;

	int(__cdecl* sub_5F0DD8)(int thisptr, int a2, int a3);
	sub_5F0DD8 = (int(__cdecl*)(int, int, int))((char*)H2BaseAddr + 0x250DD8);
	*((DWORD*)v2 + 182) = (DWORD)sub_5F0DD8;//CUSTOMFUNC MODIFIED

	char editliststr[] = "brightness level settings edit list";

	int(__cdecl* sub_105D1FD)(char* a1, int numberOfButtons, int a3);
	sub_105D1FD = (int(__cdecl*)(char*, int, int))((char*)H2BaseAddr + 0x20D1FD);
	int v3 = sub_105D1FD(editliststr, 5, 4);

	*((DWORD*)v2 + 28) = v3;

	int(__cdecl* sub_EB6B33)(int a1);
	sub_EB6B33 = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x66B33);
	sub_EB6B33(v3);

	for (int i = 0; i < *(DWORD*)(*((DWORD*)v2 + 28) + 32); ++i) {
		int(__cdecl* sub_EB67A0)(int a1);
		sub_EB67A0 = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x667A0);
		sub_EB67A0(*((DWORD*)v2 + 28));
	}
	if (v2 == -708) {
		v5 = 0;
	}
	else {
		v5 = v2 + 712;
	}

	void*(__thiscall* sub_10613D3)(int thisptr, int a2);
	sub_10613D3 = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2113D3);
	sub_10613D3(v2 + 172, v5);

	return (int)v2;
}

int __cdecl sub_20B351(int a1) {
	int* dword_9758D8 = (int*)((char*)H2BaseAddr + 0x9758D8);
	*dword_9758D8 = a1;
	return a1;
}

int __cdecl sub_2106A2(void* thisptr, int a2, int a3, int a4, __int16 a5) {//__thiscall

	int v5 = (int)thisptr;
	void* v8 = thisptr;

	int(__thiscall* sub_211D81)(int thisptr, int a2, __int16 a3);
	sub_211D81 = (int(__thiscall*)(int, int, __int16))((char*)H2BaseAddr + 0x211D81);
	sub_211D81((int)thisptr, 0, a5);

	*(DWORD*)(v5 + 116) = a3;

	//int v9 = 0;

	//*(DWORD*)v5 = &off_3CF2F4;
	*(DWORD*)v5 = (DWORD)((char*)H2BaseAddr + 0x3CF2F4);

	//on menu close it reads from offset 21229B many times
	//it writes to it once from here

	*(DWORD*)(v5 + 112) = a2;//112 = 0x70
	*(DWORD*)(v5 + 120) = a4;
	*(DWORD*)(v5 + 124) = -1;

	void*(__thiscall* sub_20F65D)(void* thisptr, int a2);
	sub_20F65D = (void*(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20F65D);
	sub_20F65D((void*)(v5 + 128), 0);

	//LOBYTE(v9) = 1;

	sub_20F65D((void*)(v5 + 1340), 0);

	//LOBYTE(v9) = 2;

	*(WORD*)(v5 + 2552) = -1;
	*(BYTE*)(v5 + 2554) = 0;
	*(BYTE*)(v5 + 2555) = 0;
	*(BYTE*)(v5 + 2556) = 0;
	*(BYTE*)(v5 + 2557) = 0;

	int(__thiscall* sub_2113C6)(int thisptr);
	sub_2113C6 = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x2113C6);
	sub_2113C6(v5 + 2632);

	//*(DWORD*)(v5 + 2628) = &off_3CF39C;
	*(DWORD*)(v5 + 2628) = (DWORD)((char*)H2BaseAddr + 0x3CF39C);

	*(DWORD*)(v5 + 2644) = v5;

	int(__thiscall* sub_210227)(int thisptr, int a2);
	sub_210227 = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x210227);
	*(DWORD*)(v5 + 2648) = (DWORD)sub_210227;

	//LOBYTE(v9) = 3;

	//int(__cdecl* sub_20B351)(int thisptr);
	//sub_20B351 = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20B351);
	sub_20B351(a2);

	++*(DWORD*)(v5 + 124);
	int v6 = *(DWORD*)(v5 + 124);
	*(DWORD*)(v5 + 4) = 0;
	*(BYTE*)(v5 + 109) = 1;
	*(DWORD*)(v5 + 12) = v6;

	DWORD* dword_A3E418 = (DWORD*)((char*)H2BaseAddr + 0xA3E418);

	int(__thiscall* sub_2116D2)(int thisptr, int a2);
	sub_2116D2 = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2116D2);
	sub_2116D2(v5, (int)dword_A3E418);

	void*(__cdecl* sub_287BA9)(void* thisptr, int a2, unsigned int a3);
	sub_287BA9 = (void*(__cdecl*)(void*, int, unsigned int))((char*)H2BaseAddr + 0x287BA9);
	sub_287BA9((void*)(v5 + 2564), 0, 0x40u);

	*(DWORD*)(v5 + 2558) = 0;
	return v5;
}

void* __cdecl sub_211159(void* thisptr, int a2, int a3, int a4, int a5, int a6) {//__thiscall
	void* v6 = thisptr;

	//int(__thiscall* sub_6106A2)(void* thisptr, int a2, int a3, int a4, __int16 a5);
	//sub_6106A2 = (int(__thiscall*)(void*, int, int, int, __int16))((char*)H2BaseAddr + 0x2106A2);
	sub_2106A2(thisptr, a2, a3, a4, a5);

	//*(DWORD*)v6 = &off_7CF3A4;
	*(DWORD*)v6 = (DWORD)((char*)H2BaseAddr + 0x3CF3A4);

	*((DWORD*)v6 + 663) = a6;
	return v6;
}

int __cdecl sub_1098B17(int thisptr, int a2, int a3, int a4) {//__thiscall
	int v4 = thisptr;
	int v5 = thisptr + 2656;

	//The GUI type and the Labels on it are pre-written from that WgitID
	//WgitID loads all the text.
	//Does not relate to functionality
	//272 - Brightness
	//263 - Volume
	//void*(__thiscall* sub_1061159)(void* thisptr, int WgitID, int a3, int a4, int a5, int a6);
	//sub_1061159 = (void*(__thiscall*)(void*, int, int, int, int, int))((char*)H2BaseAddr + 0x211159);
	sub_211159((void*)thisptr, 272, a2, a3, a4, thisptr + 2656);

	//*(DWORD*)v4 = &off_1227ABC;
	*(DWORD*)v4 = (DWORD)((char*)H2BaseAddr + 0x3D7ABC);

	//int(__thiscall* sub_10A0E22)(int thisptr, int a1);
	//sub_10A0E22 = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x250E22);
	sub_10A0E22(v5, a4);

	return v4;
}

int __cdecl MenuGameBrightnessIngame(int a1) {
	IsCustomMenu = true;

	int v2 = 0;

	//int(__cdecl* Allocator)(int a1);
	//Allocator = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	int v3 = Allocator(3388);

	int v5 = v3;
	int v6 = 0;

	if (v3) {
		//void*(__thiscall* sub_1098B17)(void* thisptr, int a2, int a3, int a4);
		//sub_1098B17 = (void*(__thiscall*)(void*, int, int, int))((char*)H2BaseAddr + 0x248B17);
		v2 = sub_1098B17(v3, *(DWORD*)(a1 + 4), *(DWORD*)(a1 + 8), *(WORD*)(a1 + 2));
	}

	v6 = -1;
	*(BYTE *)(v2 + 108) = 1;
	*(BYTE *)(v2 + 3360) = 1;

	int(__cdecl* sub_105B8C3)(int a1, int a2);
	sub_105B8C3 = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x20B8C3);
	sub_105B8C3(v2, a1);

	return v2;
}

int __cdecl sub_5EB75C(void* thisptr, int a2, int a3) {//__thiscall
	return 1;/*
			 __int16 v3 = *(WORD*)a3;
			 void* v4 = thisptr;
			 if (*(WORD*)a3 >= 0) {
			 if (v3 > 10) {
			 LOBYTE(v3) = 10;
			 }
			 }
			 else {
			 LOBYTE(v3) = 0;
			 }
			 sub_3E69CC(10 - v3);
			 int v5 = (*(int(__thiscall**)(void*))(*(DWORD*)v4 + 56))(v4);
			 int v6 = (*(int(__thiscall**)(void*))(*(DWORD*)v4 + 52))(v4);
			 return sub_5A96DA(v6, v5);*/
}

void* __cdecl sub_5EB7D9(void* thisptr, int a2) {//__thiscall

	int v5; // eax@4
	void* v2 = thisptr;

	void*(__thiscall* sub_1063B1C)(void* thisptr, int a2);
	sub_1063B1C = (void*(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x213B1C);
	sub_1063B1C(thisptr, a2);

	//Where to find the Button Text. Loaded via WgitID
	//Does not relate to functionality
	//*(DWORD*)v2 = &off_122972C;
	//*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D972C);//BRIGHTNESS
	//*(DWORD*)v2 = &off_77843C;
	*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D843C);//MODIFIED

	void*(__thiscall* sub_106FFC9)(void* thisptr);
	sub_106FFC9 = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x21FFC9);

	int(__thiscall* sub_E5A551)(void* thisptr);
	sub_E5A551 = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0xA551);

	void(__stdcall* sub_10D870B)(DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5);
	sub_10D870B = (void(__stdcall*)(DWORD, DWORD, DWORD, DWORD, DWORD))((char*)H2BaseAddr + 0x28870B);
	sub_10D870B((DWORD)v2 + 176, 132, 4, (DWORD)sub_106FFC9, (DWORD)sub_E5A551);

	*((BYTE*)v2 + 704) = 0;

	int(__thiscall* sub_10613C6)(int thisptr);
	sub_10613C6 = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x2113C6);
	sub_10613C6((int)((char*)v2 + 712));

	//No Effect.
	//*((DWORD*)v2 + 177) = &off_1229700;
	*((DWORD*)v2 + 177) = (DWORD)((char*)H2BaseAddr + 0x3D9700);//BRIGHTNESS
																 //*((DWORD*)v2 + 177) = &off_778418;
																 //*((DWORD*)v2 + 177) = (DWORD)((char*)H2BaseAddr + 0x3D8418);//MODIFIED

	*((DWORD*)v2 + 181) = (DWORD)v2;

	//Button functionality.
	int(__thiscall* sub_5F0DD8)(int thisptr, int a2, int a3);
	sub_5F0DD8 = (int(__thiscall*)(int, int, int))((char*)H2BaseAddr + 0x250DD8);//BRIGHTNESS
																				  //int(__thiscall* sub_5EB75C)(void* thisptr, int a2, int a3);
																				  //sub_5EB75C = (int(__thiscall*)(void*, int, int))((char*)H2BaseAddr + 0x24B75C);
	*((DWORD*)v2 + 182) = (DWORD)((char*)sub_5F0DD8);//CUSTOMFUNC MODIFIED

													 //
	char editliststr[] = "volume settings edit list";

	int(__cdecl* sub_105D1FD)(char* a1, int numberOfButtons, int a3);
	sub_105D1FD = (int(__cdecl*)(char*, int, int))((char*)H2BaseAddr + 0x20D1FD);
	int v3 = sub_105D1FD(editliststr, 11, 4);//MODIFIED

	*((DWORD*)v2 + 28) = v3;

	int(__cdecl* sub_EB6B33)(int a1);
	sub_EB6B33 = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x66B33);
	sub_EB6B33(v3);

	for (int i = 0; i < *(DWORD*)(*((DWORD*)v2 + 28) + 32); ++i) {
		int(__cdecl* sub_EB67A0)(int a1);
		sub_EB67A0 = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x667A0);
		sub_EB67A0(*((DWORD*)v2 + 28));
	}
	if (v2 == (void*)-708) {
		v5 = 0;
	}
	else {
		v5 = (int)((char*)v2 + 712);
	}

	void*(__thiscall* sub_10613D3)(void* thisptr, int a2);
	sub_10613D3 = (void*(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x2113D3);
	sub_10613D3((char*)v2 + 172, v5);

	return v2;
}

int __cdecl sub_5C7261(int thisptr, int a2, int a3, int a4) {//__thiscall
	int v4 = thisptr;
	int v5 = thisptr + 2656;

	//WgitID loads all the text.
	//Does not relate to functionality
	//272 - Brightness
	//263 - Volume
	void*(__thiscall* sub_1061159)(int thisptr, int WgitID, int a3, int a4, int a5, int a6);
	sub_1061159 = (void*(__thiscall*)(int, int, int, int, int, int))((char*)H2BaseAddr + 0x211159);
	sub_1061159(thisptr, 263, a2, a3, a4, (int)((char*)thisptr + 2656));//MODIFIED

																		//Relates to which option should be selected first. Doesn't affect functionality.
																		//*(DWORD*)v4 = &off_1227ABC;
																		//*(DWORD*)v4 = (DWORD)((char*)H2BaseAddr + 0x3D7ABC);//BRIGHTNESS
																		//*(DWORD*)v4 = &off_770A14;
	*(DWORD*)v4 = (DWORD)((char*)H2BaseAddr + 0x3D0A14);//MODIFIED

														 //int(__thiscall* sub_5EB7D9)(int thisptr, int a1);
														 //sub_5EB7D9 = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x24B7D9);
	sub_5EB7D9((void*)v5, a4);//CUSTOMFUNC MODIFIED

	return v4;
}

int __cdecl MenuGameVolumeIngame(int a1) {

	int v2 = 0;

	//int(__cdecl* Allocator)(int a1);
	//Allocator = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	int v3 = Allocator(3388);

	int v5 = v3;
	int v6 = 0;

	if (v3) {
		//void*(__thiscall* sub_5C7261)(void* thisptr, int a2, int a3, int a4);
		//sub_5C7261 = (void*(__thiscall*)(void*, int, int, int))((char*)H2BaseAddr + 0x227261);
		v2 = sub_5C7261(v3, *(DWORD*)(a1 + 4), *(DWORD*)(a1 + 8), *(WORD*)(a1 + 2));//CUSTOMFUNC MODIFIED
	}

	v6 = -1;
	*(BYTE *)(v2 + 108) = 1;
	*(BYTE *)(v2 + 3360) = 1;

	int(__cdecl* sub_105B8C3)(int a1, int a2);
	sub_105B8C3 = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x20B8C3);
	sub_105B8C3(v2, a1);

	return v2;
}

void* __cdecl sub_5F02B3(void* thisptr, int a2) {//__thiscall

	int v5; // eax@4
	void* v2 = thisptr;

	void*(__thiscall* sub_1063B1C)(void* thisptr, int a2);
	sub_1063B1C = (void*(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x213B1C);
	sub_1063B1C(thisptr, a2);

	//*(DWORD*)v2 = &off_77944C;
	*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D944C);//MODIFIED

	void*(__thiscall* sub_106FFC9)(void* thisptr);
	sub_106FFC9 = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x21FFC9);

	int(__thiscall* sub_E5A551)(void* thisptr);
	sub_E5A551 = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0xA551);

	void(__stdcall* sub_10D870B)(int a1, int a2, int a3, void(__thiscall* a4)(DWORD), int a5);
	sub_10D870B = (void(__stdcall*)(int, int, int, void(__thiscall*)(DWORD), int))((char*)H2BaseAddr + 0x28870B);
	sub_10D870B((int)((char*)v2 + 176), 132, 4, (void(__thiscall*)(DWORD))sub_106FFC9, (int)sub_E5A551);

	*((BYTE*)v2 + 704) = 0;

	int(__thiscall* sub_10613C6)(int thisptr);
	sub_10613C6 = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x2113C6);
	sub_10613C6((int)((char*)v2 + 712));

	//*((DWORD*)v2 + 177) = &off_779378;
	*((DWORD*)v2 + 177) = (DWORD)((char*)H2BaseAddr + 0x3D9378);//MODIFIED

	*((DWORD*)v2 + 181) = (DWORD)v2;

	int(__thiscall* sub_5F021B)(void* thisptr, int a2, int a3);
	sub_5F021B = (int(__thiscall*)(void*, int, int))((char*)H2BaseAddr + 0x25021B);
	*((DWORD*)v2 + 182) = (DWORD)sub_5F021B;//CUSTOMFUNC MODIFIED

	char editliststr[] = "resolution settings edit list";

	//Number Of Different Resolutions Available
	signed int(__thiscall* sub_603BE1)(void);
	sub_603BE1 = (signed int(__thiscall*)(void))((char*)H2BaseAddr + 0x263BE1);
	int numOfResBtns = sub_603BE1();

	int(__cdecl* sub_105D1FD)(char* a1, int numberOfButtons, int a3);
	sub_105D1FD = (int(__cdecl*)(char*, int, int))((char*)H2BaseAddr + 0x20D1FD);
	int v3 = sub_105D1FD(editliststr, numOfResBtns, 4);//MODIFIED

	*((DWORD*)v2 + 28) = v3;

	int(__cdecl* sub_EB6B33)(int a1);
	sub_EB6B33 = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x66B33);
	sub_EB6B33(v3);

	for (int i = 0; i < *(DWORD*)(*((DWORD*)v2 + 28) + 32); ++i) {
		int(__cdecl* sub_EB67A0)(int a1);
		sub_EB67A0 = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x667A0);
		sub_EB67A0(*((DWORD*)v2 + 28));
	}
	if (v2 == (void*)-708) {
		v5 = 0;
	}
	else {
		v5 = (int)((char*)v2 + 712);
	}

	void*(__thiscall* sub_10613D3)(void* thisptr, int a2);
	sub_10613D3 = (void*(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x2113D3);
	sub_10613D3((char*)v2 + 172, v5);

	return v2;
}

int __cdecl sub_5E90E1(int thisptr, int a2, int a3, int a4) {//__thiscall
	int v4 = thisptr;
	int v5 = thisptr + 2656;

	void*(__thiscall* sub_1061159)(int thisptr, int a2, int a3, int a4, int a5, int a6);
	sub_1061159 = (void*(__thiscall*)(int, int, int, int, int, int))((char*)H2BaseAddr + 0x211159);
	sub_1061159(thisptr, 269, a2, a3, a4, (int)((char*)thisptr + 2656));//MODIFIED

																		//*(DWORD*)v4 = &off_777EAC;
	*(DWORD*)v4 = (DWORD)((char*)H2BaseAddr + 0x3D7EAC);//MODIFIED

														 //int(__thiscall* sub_5F02B3)(int thisptr, int a1);
														 //sub_5F02B3 = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2502B3);
	sub_5F02B3((void*)v5, a4);//CUSTOMFUNC MODIFIED

	return v4;
}

int __cdecl MenuGameResolutionMM(int a1) {

	int v2 = 0;

	//int(__cdecl* Allocator)(int a1);
	//Allocator = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	int v3 = Allocator(3388);

	int v5 = v3;
	int v6 = 0;

	if (v3) {
		//int(__thiscall* sub_5E90E1)(int thisptr, int a2, int a3, int a4);
		//sub_5E90E1 = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x2490E1);
		v2 = sub_5E90E1(v3, *(DWORD*)(a1 + 4), *(DWORD*)(a1 + 8), *(WORD*)(a1 + 2));//CUSTOMFUNC MODIFIED
	}

	v6 = -1;
	*(BYTE *)(v2 + 108) = 1;
	//*(BYTE *)(v2 + 3360) = 1;//MODIFIED

	int(__cdecl* sub_105B8C3)(int a1, int a2);
	sub_105B8C3 = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x20B8C3);
	sub_105B8C3(v2, a1);

	return v2;
}