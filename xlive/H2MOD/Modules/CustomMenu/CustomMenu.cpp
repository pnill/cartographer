
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\CustomMenu\CustomLanguage.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\Accounts\AccountCreate.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Updater\Updater.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\UI\XboxLiveTaskProgress.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"

#include "H2MOD/Tags/TagInterface.h"
#include "H2MOD/Modules/AdvLobbySettings/AdvLobbySettings.h"
#include "H2MOD/Modules/Networking/Networking.h"

#include "Util\Hooks\Hook.h"

#include "c_screen_with_menu.h"

extern DWORD H2BaseAddr;
extern bool H2IsDediServer;

#define PUSH_REGISTERS \
__asm push ebp \
__asm push edi \
__asm push esi \
__asm push ecx \
__asm push ebx \

#define POP_REGISTERS \
__asm pop ebx \
__asm pop ecx \
__asm pop esi \
__asm pop edi \
__asm pop ebp \

void CallWgit(int WgitScreenfunctionPtr);
void CallWgit(int WgitScreenfunctionPtr, int open_method);
void CallWgit(int WgitScreenfunctionPtr, int open_method, int menu_wgit_type);
void CMSetupVFTables(DWORD** menu_vftable_1, DWORD** menu_vftable_2, DWORD CM_LabelButtons, DWORD sub_2111ab_CMLTD_nak, DWORD CM_FuncPtrHelper, DWORD CM_ButtonPreselection, bool isInternalMenuWindow, DWORD sub_248beb_nak_deconstructor);
int __stdcall BtnHandlerCaller(void* thisptr, int a2, int a3);
int __stdcall sub_20F790_CM(int thisptr, __int16 selected_button_id);
int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description);
int __stdcall sub_23bf3e_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description);
int __cdecl CustomMenu_CallHead2(int a1, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type);
int __cdecl CustomMenu_CallHead(int a1, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type);
void __stdcall sub_21bf85_CMLTD(int thisptr, int label_id, int label_menu_id);
void __stdcall sub_28870B_CM(int a1, int a2, int a3, void*(__stdcall* a4)(int), int a5);

void GSCustomMenuCall_AccountList();


const int CMLabelMenuId_EscSettings = 0xFF000004;
#pragma region CM_EscSettings

#pragma region CM_EscSettings_FailLocalEditToVftable

void __stdcall CMLabelButtons_EscSettings(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EscSettings);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EscSettings() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EscSettings
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EscSettings(int button_id) {
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_EscSettings() {//__thiscall
	__asm {

		PUSH_REGISTERS

		push 0
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EscSettings(int);

int(__cdecl *CustomMenuFuncPtrHelp_EscSettings())(int) {
	return CustomMenu_EscSettings;
}

DWORD* menu_vftable_1_EscSettings = 0;
DWORD* menu_vftable_2_EscSettings = 0;

void CMSetupVFTables2(DWORD** menu_vftable_1, DWORD** menu_vftable_2, DWORD CM_LabelButtons, DWORD sub_2111ab_CMLTD_nak, DWORD CM_FuncPtrHelper, DWORD CM_ButtonPreselection) {

	//clone a brightness menu_vftable_1
	*menu_vftable_1 = (DWORD*)malloc(0x98);
	memcpy(*menu_vftable_1, (BYTE*)H2BaseAddr + 0x3d96fc, 0x98);//Brightness - 0x3d96fc

	//Button Labels
	*(DWORD*)((DWORD)*menu_vftable_1 + 0x90) = (DWORD)CM_LabelButtons;


	//clone a brightness menu_vftable_2
	*menu_vftable_2 = (DWORD*)malloc(0xA8);
	memcpy(*menu_vftable_2, (BYTE*)H2BaseAddr + 0x3D7ABC, 0xA8);//Brightness - 0x3D7ABC

	*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x212CD8;//internal
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x23D64E;

	*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24DC0D;//internal
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x23D8AE;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)sub_23D8AE;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)CM_MenuCloseInner;

	//CALL for Title & Description
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x58) = (DWORD)sub_2111ab_CMLTD_nak;
	//Menu Button Preselected Option
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x5C) = (DWORD)CM_ButtonPreselection;
	//Menu function pointer helper
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x98) = (DWORD)CM_FuncPtrHelper;
}

void CMSetupVFTables3(DWORD** menu_vftable_1, DWORD** menu_vftable_2, DWORD CM_LabelButtons, DWORD sub_2111ab_CMLTD_nak, DWORD CM_FuncPtrHelper, DWORD CM_ButtonPreselection) {

	//clone a brightness menu_vftable_1
	*menu_vftable_1 = (DWORD*)malloc(0x98);
	memcpy(*menu_vftable_1, (BYTE*)H2BaseAddr + 0x3d8EC4 - 0x30, 0x98);//Brightness - 0x3d96fc

	//Button Labels
	*(DWORD*)((DWORD)*menu_vftable_1 + 0x90) = (DWORD)CM_LabelButtons;


	//clone a brightness menu_vftable_2
	*menu_vftable_2 = (DWORD*)malloc(0xA8);
	memcpy(*menu_vftable_2, (BYTE*)H2BaseAddr + 0x3D357C, 0xA8);//Brightness - 0x3D7ABC

	*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x212CD8;//internal
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x23D64E;

	*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24DC0D;//internal
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x23D8AE;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)sub_23D8AE;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)CM_MenuCloseInner;

	//CALL for Title & Description
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x58) = (DWORD)sub_2111ab_CMLTD_nak;
	//Menu Button Preselected Option
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x5C) = (DWORD)CM_ButtonPreselection;
	//Menu function pointer helper
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x98) = (DWORD)CM_FuncPtrHelper;
}

void CMSetupVFTables_EscSettings() {
	CMSetupVFTables(&menu_vftable_1_EscSettings, &menu_vftable_2_EscSettings, (DWORD)CMLabelButtons_EscSettings, (DWORD)sub_2111ab_CMLTD_nak_EscSettings, (DWORD)CustomMenuFuncPtrHelp_EscSettings, (DWORD)sub_20F790_CM_nak_EscSettings, true, 0);
}

void GSCustomMenuCall_EscSettingsFailLocalEditToVftable() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EscSettings);
	CallWgit(WgitScreenfunctionPtr, 0);
}

#pragma endregion


int __stdcall sub_24E330(int thisptr, int a2)
{
	void*(__thiscall* sub_213B1C)(int, int) = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x213B1C);
	void*(__thiscall* sub_21FFC9)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x21FFC9);
	void(__stdcall* sub_28870B)(int, int, int, void(__thiscall*)(DWORD), int) = (void(__stdcall*)(int, int, int, void(__thiscall*)(DWORD), int))((char*)H2BaseAddr + 0x28870B);
	int(__thiscall* sub_2113C6)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x2113C6);
	int(__cdecl* sub_20D1FD)(char*, int numberOfButtons, int) = (int(__cdecl*)(char*, int, int))((char*)H2BaseAddr + 0x20D1FD);
	int(__cdecl* sub_66B33)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x66B33);
	int(__cdecl* sub_667A0)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x667A0);
	void*(__thiscall* sub_2113D3)(int, int) = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2113D3);

	int(__thiscall* sub_A551)(int thisptr) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0xA551);

	int(__thiscall* sub_24E1AA)(int thisptr, int a2, int* a3) = (int(__thiscall*)(int, int, int*))((char*)H2BaseAddr + 0x24E1AA);
	int(__thiscall* sub_250DD8)(int thisptr, int a2, int* a3) = (int(__thiscall*)(int, int, int*))((char*)H2BaseAddr + 0x250DD8);

	void* v2; // esi@1
	int v3; // ebp@1
	int v4; // eax@1

	v2 = (void*)thisptr;
	sub_213B1C(thisptr, a2);
	v3 = 0;
	//*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D8EC4);
	*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D972C);
	sub_28870B((int)((char*)v2 + 176), 132, 5, (void(__thiscall*)(DWORD))sub_21FFC9, (int)sub_A551);
	sub_2113C6((int)((char*)v2 + 840));
	//*((DWORD*)v2 + 209) = (DWORD)((char*)H2BaseAddr + 0x3D8D54);
	*((DWORD*)v2 + 209) = (DWORD)((char*)H2BaseAddr + 0x3D9700);
	*((DWORD*)v2 + 213) = (int)v2;
	//*((DWORD*)v2 + 214) = (DWORD)sub_24E1AA;
	*((DWORD*)v2 + 214) = (DWORD)sub_250DD8;
	v4 = sub_20D1FD("pause settings list", 4, 4);
	*((DWORD*)v2 + 28) = v4;
	sub_66B33(v4);
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 0;
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 1;
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 2;
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 3;
	if (v2 != (void*)-836)
		v3 = (int)((char*)v2 + 840);
	sub_2113D3((int)v2 + 172, v3);
	return (int)v2;
}

void* __stdcall sub_23DDB4(void* thisptr, int a2, int a3, int a4)
{
	void*(__thiscall* sub_211159)(void*, int wgit_id, int, int, int, int) = (void*(__thiscall*)(void*, int, int, int, int, int))((char*)H2BaseAddr + 0x211159);
	//int(__thiscall* sub_250E22)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x250E22);

	//int(__thiscall* sub_24E330)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x24E330);

	void* v4; // esi@1
	char* v5; // edi@1

	v4 = thisptr;
	v5 = (char*)thisptr + 2656;
	sub_211159(thisptr, 272, a2, a3, a4, (int)((char*)thisptr + 2656));
	//*(DWORD*)v4 = (DWORD)((char*)H2BaseAddr + 0x3D357C);
	*(DWORD*)v4 = (DWORD)((char*)H2BaseAddr + 0x3D7ABC);
	sub_24E330((int)v5, a4);
	return v4;
}

int __cdecl CustomMenu_EscSettings2(int a1)
{
	int(__cdecl* Allocator)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	int(__cdecl* sub_20B8C3)(int, int) = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x20B8C3);

	//int(__thiscall* sub_23DDB4)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x23DDB4);

	int v2;
	int v3 = Allocator(3516);
	if (v3)
		v2 = (int)sub_23DDB4((void*)v3, *(DWORD*)(a1 + 4), *(DWORD*)(a1 + 8), *(WORD*)(a1 + 2));
	*(BYTE*)(v2 + 108) = 1;
	sub_20B8C3(v2, a1);
	return v2;
}

int __cdecl CustomMenu_EscSettings(int a1) {
	return CustomMenu_CallHead2(a1, menu_vftable_1_EscSettings, menu_vftable_2_EscSettings, (DWORD)&CMButtonHandler_EscSettings, 2, 272);//259
}

void GSCustomMenuCall_EscSettings() {
	int WgitScreenfunctionPtr;
	//WgitScreenfunctionPtr = (int)(CustomMenu_EscSettings2);
	WgitScreenfunctionPtr = (int)(CustomMenu_EscSettings);
	CallWgit(WgitScreenfunctionPtr, 0);
}


void GSCustomMenuCall_EscSettings2() {
	int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x23E05F);
	CallWgit(WgitScreenfunctionPtr, 0);
}

#pragma endregion


const int CMLabelMenuId_VKeyTest = 0xFF00000B;
#pragma region CM_VKeyTest

#pragma region CM_OldTests
void __cdecl sub_23C9F6(char a1, int VKbMenuType, wchar_t* textBuffer, __int16 textBufferLen) {

	int(__thiscall*sub_20B0BC)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B0BC);
	signed int(__thiscall*sub_20C226)(void*, __int16, int, int, int) = (signed int(__thiscall*)(void*, __int16, int, int, int))((char*)H2BaseAddr + 0x20C226);
	signed int(__thiscall*sub_20C258)(void*, __int16, __int16, int, int, int) = (signed int(__thiscall*)(void*, __int16, __int16, int, int, int))((char*)H2BaseAddr + 0x20C258);
	void*(__thiscall*sub_20B11E)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B11E);

	unsigned int(__thiscall*sub_23B118)(void*, wchar_t*, int) = (unsigned int(__thiscall*)(void*, wchar_t*, int))((char*)H2BaseAddr + 0x23B118);
	int(__cdecl* get_language_id)() = (int(__cdecl*)())((char*)H2BaseAddr + 0x381FD);
	bool(*sub_38F02)() = (bool(*)())((char*)H2BaseAddr + 0x38F02);
	int(*sub_38F74)() = (int(*)())((char*)H2BaseAddr + 0x38F74);
	void(__thiscall*sub_38F7A)(void*, int, int) = (void(__thiscall*)(void*, int, int))((char*)H2BaseAddr + 0x38F7A);

	int v4; // esi@1
	char v8[0x20]; // [sp+8h] [bp-20h]@1

	sub_20B0BC(&v8);
	sub_20C258(&v8, 0, 1 << a1, 2, 4, (int)(char*)H2BaseAddr + 0x23C72F);// (int)sub_13BC72F);
	v4 = (int)sub_20B11E(&v8);
	*(DWORD*)(v4 + 2652) = VKbMenuType;
	sub_23B118((void*)v4, textBuffer, textBufferLen);
	return;
	if (VKbMenuType < 0 || VKbMenuType > 6 && VKbMenuType != 16 || get_language_id() != 1 && get_language_id() != 6 && get_language_id() != 7)
	{
		if (sub_38F02())
		{
			int v5 = (*(int(__thiscall**)(int))(*(DWORD*)v4 + 132))(v4);
			int v6 = (*(int(__thiscall**)(int))(*(DWORD*)v4 + 128))(v4);
			void* v7 = (void*)sub_38F74();
			sub_38F7A(v7, v6, v5);
		}
		*(BYTE*)(v4 + 15229) = 1;
	}
}

void __cdecl sub_23CAB2(char a1, int VKbMenuType, int a3, wchar_t* textBuffer, __int16 textBufferLen) {

	int(__thiscall*sub_20B0BC)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B0BC);
	signed int(__thiscall*sub_20C226)(void*, __int16, int, int, int) = (signed int(__thiscall*)(void*, __int16, int, int, int))((char*)H2BaseAddr + 0x20C226);
	signed int(__thiscall*sub_20C258)(void*, __int16, __int16, int, int, int) = (signed int(__thiscall*)(void*, __int16, __int16, int, int, int))((char*)H2BaseAddr + 0x20C258);
	void*(__thiscall*sub_20B11E)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B11E);

	unsigned int(__thiscall*sub_23B118)(void*, wchar_t*, int) = (unsigned int(__thiscall*)(void*, wchar_t*, int))((char*)H2BaseAddr + 0x23B118);
	int(__cdecl* sub_381FD)() = (int(__cdecl*)())((char*)H2BaseAddr + 0x381FD);
	bool(*sub_38F02)() = (bool(*)())((char*)H2BaseAddr + 0x38F02);
	int(*sub_38F74)() = (int(*)())((char*)H2BaseAddr + 0x38F74);
	void(__thiscall*sub_38F7A)(void*, int, int) = (void(__thiscall*)(void*, int, int))((char*)H2BaseAddr + 0x38F7A);

	int v4; // esi@1
	char v8[0x20]; // [sp+8h] [bp-20h]@1

	sub_20B0BC(&v8);
	sub_20C258(&v8, 0, 1 << a1, 2, 4, (int)(char*)H2BaseAddr + 0x23C72F);// (int)sub_13BC72F);
	v4 = (int)sub_20B11E(&v8);
	*(DWORD*)(v4 + 2652) = VKbMenuType;
	sub_23B118((void*)v4, textBuffer, textBufferLen);
	*(DWORD*)(v4 + 2656) = a3;
}

wchar_t testVirtualText[32];
signed int testVirtualType = 16;

void testVKeyboardCall() {
	testVirtualText[0] = 0;
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x23C72F);//Virtual Keyboard
	//CallWgit(WgitScreenfunctionPtr, 3);


	//int(__cdecl*sub_23C9F6)(char a1, signed int VirtualKeyboardMenuType, wchar_t* textBuffer, __int16 textBufferLen) = (int(__cdecl*)(char, signed int, wchar_t*, __int16))((char*)H2BaseAddr + 0x23C9F6);

	signed int* wreg = &testVirtualType;

	sub_23C9F6(0, testVirtualType, testVirtualText, 32);
}

#pragma endregion

#define VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX 17

// we just re-use the first seventeen keyboard id's in default code for now
#define VIRTUAL_KEYBOARD_MENU_TYPE_MAX_NEW (VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX * 2)

char __stdcall sub_23CC18_CM(int thisptr)//__thiscall
{
	int(__cdecl* sub_4BEB3)(int, int, int, int) = (int(__cdecl*)(int, int, int, int))((char*)H2BaseAddr + 0x4BEB3);
	signed int(__cdecl* sub_287567)(__int16*, unsigned int, __int16*) = (signed int(__cdecl*)(__int16*, unsigned int, __int16*))((char*)H2BaseAddr + 0x287567);
	char(__thiscall* sub_23C7B2)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23C7B2);
	char(__thiscall* sub_23B251)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23B251);
	char(__thiscall* sub_23C8F3)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23C8F3);
	char(__thiscall* sub_23B38C)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23B38C);
	char(__thiscall* sub_23B4A9)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23B4A9);
	char(__thiscall* sub_23CBA3)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23CBA3);
	char(__thiscall* sub_23B543)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23B543);
	char(__thiscall* sub_23B583)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23B583);
	char(__thiscall* sub_23B597)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23B597);
	int(__thiscall* sub_212604)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x212604);

	BYTE& byte_978DFE = *((BYTE*)H2BaseAddr + 0x978DFE);

	int v1; // esi
	char result; // al
	//char v3[1024]; // [esp+4h] [ebp-804h]

	v1 = thisptr;
	int VKbMenuTypeDefault = *(DWORD*)(thisptr + 2652);
	int VKbMenuTypeNew = *(DWORD*)(thisptr + 2652) + VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX;
	if (VKbMenuTypeDefault == -1)
		VKbMenuTypeNew = -1;

	/*if (VKbMenuTypeDefault != 15)//type: enter message text. hmm something interesting is happening here
	{
		sub_4BEB3((int)v3, 1024, *(DWORD*)(thisptr + 3172), *(signed __int16*)(thisptr + 3180));
		sub_287567(*(__int16**)(v1 + 3172), *(signed __int16*)(v1 + 3180), (__int16*)v3);
	}*/

	result = sub_212604(v1, 3);

	// reconvert back to single byte string, because the rest of the menus code apparently uses single byte
	// but for some retarded reason the virtual keyboard uses wide string
	wchar_t* returnString = *(wchar_t**)(thisptr + 0xC64);
	int returnStrLen = wcslen(returnString) + 1;
	char* end = (char*)malloc(sizeof(char) * returnStrLen);
	wcstombs2(end, returnString, returnStrLen);
	strcpy((char*)returnString, end);
	free(end);

	return result;
}

//typedef void(__stdcall *tsub_23CD58)(void*, __int16);
//tsub_23CD58 psub_23CD58;
void __stdcall sub_23CD58_CM(void* thisptr, __int16 a2)//__thiscall
{
	//virtual key press handler
	//return psub_23CD58(thisptr, a2);

	//void(__thiscall* sub_23CD58)(void*, __int16) = (void(__thiscall*)(void*, __int16))((char*)H2BaseAddr + 0x23CD58);
	//return sub_23CD58(thisptr, a2);

	char(__cdecl*sub_4C6E0)(__int16) = (char(__cdecl*)(__int16))((char*)H2BaseAddr + 0x4C6E0);
	char(__thiscall*sub_23C4C9)(int, __int16) = (char(__thiscall*)(int, __int16))((char*)H2BaseAddr + 0x23C4C9);
	int(__thiscall*sub_23B9DE)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x23B9DE);
	int(__cdecl*sub_21DD04)(signed int) = (int(__cdecl*)(signed int))((char*)H2BaseAddr + 0x21DD04);
	signed int(__thiscall*sub_23B8F1)(void*) = (signed int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23B8F1);
	int(__cdecl*sub_20E1D8)(int, int, int, int, int, int) = (int(__cdecl*)(int, int, int, int, int, int))((char*)H2BaseAddr + 0x20E1D8);
	//char(__thiscall*sub_23CC18)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x23CC18);
	int(__thiscall*sub_23BADA)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BADA);
	void(__thiscall*sub_23BB13)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BB13);
	void(__thiscall*sub_23BB4C)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BB4C);
	void(__thiscall*sub_23BB85)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BB85);

	int v4; // eax
	__int16 *v5; // eax
	__int16 *v6; // ebx
	int v7; // edi
	signed __int16 v8; // bp
	unsigned int v9; // eax
	bool v10; // cf
	bool v11; // zf
	int v12; // eax
	WORD* v13; // eax
	int error_message_id; // edi
	signed int v15; // eax

	int VKbMenuTypeDefault = *((DWORD*)thisptr + 663);
	int VKbMenuTypeNew = *((DWORD*)thisptr + 663) + VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX;
	if (VKbMenuTypeDefault == -1)
		VKbMenuTypeNew = -1;

	if (VKbMenuTypeDefault != 12 || *((DWORD*)thisptr + 3806) == -1)
	{
		switch (a2)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35://all normal/alphabet vkeyboard keys
			v4 = (*(int(__thiscall**)(void*))(*((DWORD*)thisptr + 64 * a2 + 798) + 76))((void*)((a2 << 8) + (DWORD)thisptr + 0xc78));
			v5 = (__int16 *)(*(int(__thiscall**)(int))(*(DWORD*)v4 + 12))(v4);
			v6 = v5;
			v7 = 0;
			if (*v5)
			{
				do
				{
					v8 = *v5;
					if (sub_4C6E0(*v5))
						v8 = 9633;
					if (!sub_23C4C9((int)thisptr, v8))
						break;
					v5 = &v6[++v7];
				} while (v6[v7]);
			}
			break;
		case 36://backspace
			sub_23B9DE((int)thisptr);
			break;
		case 37://space
			sub_23C4C9((int)thisptr, 32);
			break;
		case 38://move cursor left
			v9 = *((DWORD*)thisptr + 794);
			v10 = v9 < *((DWORD*)thisptr + 793);
			v11 = v9 == *((DWORD*)thisptr + 793);
			*((BYTE*)thisptr + 3188) = 0;
			if (v10 || v11)
				goto LABEL_17;
			v12 = v9 - 2;
			goto LABEL_16;
		case 39://move cursor right
			v13 = (WORD*)*((DWORD*)thisptr + 794);
			*((BYTE*)thisptr + 3188) = 0;
			if (*v13)
			{
				v12 = (int)(v13 + 1);
			LABEL_16:
				*((DWORD*)thisptr + 794) = v12;
			}
		LABEL_17:
			sub_21DD04(6);
			break;
		case 40: // Done button
			if (VKbMenuTypeDefault != -1 && (error_message_id = 0, (v15 = sub_23B8F1(thisptr)) != 0))
			{
				switch (VKbMenuTypeDefault)//error with message type
				{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
					switch (VKbMenuTypeNew)
					{
					case 17:
					case 18:
					case 19:
					case 20:
					case 21:
						if (v15 == 1)
						{
							error_message_id = 88;
						}
						else if (v15 == 2)
						{
							// error_message_id = 70;

							// replace already exist error message with success
							goto success;
						}

					default:
						break;
					}
					break;
					
				case 5:
				case 6:
				case 7:
					if (v15 == 1)
					{
						error_message_id = 89;
					}
					else if (v15 == 2)
					{
						error_message_id = 71;
					}
				default:
					break;
				}

				sub_20E1D8(1, error_message_id, 4, *((unsigned __int16 *)thisptr + 4), 0, 0);//can get rid of this and do custom message
			}
			else
			{
				// success. however it will open other menus like customise new variant
				// we replace the original function that would call the default menus
			success:
				sub_23CC18_CM((int)thisptr);
			}
			break;
		case 41://shift button
			sub_23BADA(thisptr);
			break;
		case 42://caps lock
			sub_23BB13(thisptr);
			break;
		case 43://"ACCENTS" button
			if (VKbMenuTypeDefault == 12 || VKbMenuTypeDefault == 15 || VKbMenuTypeDefault == 16)
				sub_23BB4C(thisptr);
			break;
		case 44://"SYMBOLS" button
			if (VKbMenuTypeDefault == 12 || VKbMenuTypeDefault == 15 || VKbMenuTypeDefault == 16)
				sub_23BB85(thisptr);
			break;
		default:
			return;
		}
	}
}

void sub_23bf3e_CMLTD_nak_VKeyTest();
void sub_23D060_CM_nak_VKeyTest();
void sub_23CF88_CM_nak_VKeyTest();

void CMSetupVFTablesVKb(DWORD** menu_vftable_2, DWORD** menu_vftable_3, DWORD sub_23bf3e_CMLTD_nak) {

	//clone a brightness menu_vftable_2
	*menu_vftable_2 = (DWORD*)malloc(0xA8);
	memcpy(*menu_vftable_2, (BYTE*)H2BaseAddr + 0x3D302C, 0xA8);//Virtual Keyboard

	//CALL for Title & Description
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x58) = (DWORD)sub_23bf3e_CMLTD_nak;

	//CALL for real keyboard handler end/Done.
	*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)sub_23D060_CM_nak_VKeyTest;

	//clone a brightness menu_vftable_3
	*menu_vftable_3 = (DWORD*)malloc(0x60);
	memcpy(*menu_vftable_3, (BYTE*)H2BaseAddr + 0x3D30D4, 0x60);//Virtual Keyboard

	//CALL for key handler.
	*(DWORD*)((DWORD)*menu_vftable_3 + 0x30) = (DWORD)sub_23CF88_CM_nak_VKeyTest;
}

//DWORD* menu_vftable_1_VKeyTest = 0;
DWORD* menu_vftable_2_VKeyTest = 0;
DWORD* menu_vftable_3_VKeyTest = 0;

void CMSetupVFTablesVKb_VKeyTest() {
	CMSetupVFTablesVKb(&menu_vftable_2_VKeyTest, &menu_vftable_3_VKeyTest, (DWORD)sub_23bf3e_CMLTD_nak_VKeyTest);
}

#pragma region CM_VKbHead
void* __stdcall sub_23BC45_CM(void* thisptr)//__thiscall
{
	void*(__thiscall* sub_23F756)(void*, __int16, __int16) = (void*(__thiscall*)(void*, __int16, __int16))((char*)H2BaseAddr + 0x23F756);

	void* v1 = thisptr;
	sub_23F756(thisptr, -1, 0);
	//*v1 = &c_virtual_keyboard_button::`vftable';
	//*(DWORD*)v1 = (DWORD)(char*)H2BaseAddr + 0x3D30D4;
	*(DWORD*)v1 = (DWORD)menu_vftable_3_VKeyTest;
	return v1;
}

void* __stdcall sub_23BDF6_CM(void* thisptr, int a2, int a3, int a4) { //__thiscall

	void*(__thiscall* sub_2106A2)(void*, int, int, int, __int16) = (void*(__thiscall*)(void*, int, int, int, __int16))((char*)H2BaseAddr + 0x2106A2);
	//void(__stdcall* sub_28870B)(int, int, int, void(__thiscall*)(DWORD), int) = (void(__stdcall*)(int, int, int, void(__thiscall*)(DWORD), int))((char*)H2BaseAddr + 0x28870B);
	char(__cdecl* sub_2067FC)(char) = (char(__cdecl*)(char))((char*)H2BaseAddr + 0x2067FC);

	//void*(__thiscall* sub_23BC45)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BC45);
	int(*sub_23B7B2)() = (int(*)())((char*)H2BaseAddr + 0x23B7B2);

	void* v4; // esi@1
	int v5; // eax@1
	int v6; // ecx@2
	//void *v8; // [sp+0h] [bp-10h]@1
	//int v9; // [sp+Ch] [bp-4h]@1

	v4 = thisptr;
	//v8 = thisptr;
	sub_2106A2(thisptr, 167, a2, a3, a4);
	//v9 = 0;
	//*(DWORD*)v4 = &c_screen_virtual_keyboard::`vftable';
	//*(DWORD*)v4 = (DWORD)(char*)H2BaseAddr + 0x3D302C;
	*(DWORD*)v4 = (DWORD)menu_vftable_2_VKeyTest;
	*((DWORD*)v4 + 663) = -1;//VKbMenuType
	*((DWORD*)v4 + 664) = -1;
	memset((char*)v4 + 2660, 0, 0x200u);
	*((DWORD*)v4 + 793) = 0;
	*((DWORD*)v4 + 794) = 0;
	*((WORD*)v4 + 1590) = 0;
	*((WORD*)v4 + 1591) = 4;
	*((WORD*)v4 + 1592) = 10;
	*((WORD*)v4 + 1593) = 0;
	*((BYTE*)v4 + 3188) = 0;
	sub_28870B_CM((int)((char*)v4 + 3192), 256, 47, (void*(__stdcall*)(int))sub_23BC45_CM, (int)sub_23B7B2);
	//LOBYTE(v9) = 1;
	*((DWORD*)v4 + 3806) = -1;
	*((BYTE*)v4 + 15228) = 1;
	*((BYTE*)v4 + 15229) = 0;
	v5 = 0;
	do
	{
		v6 = (int)((char *)v4 + 256 * (signed __int16)v5);
		*(WORD*)(v6 + 3440) = v5++;
		*(WORD*)(v6 + 3200) = a4;
	} while ((unsigned __int16)v5 < 0x2Fu);
	sub_2067FC(1);
	return v4;
}

int __cdecl sub_23C72F_CM(int a1) {
	int(__cdecl* sub_20D2D8)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	int(__cdecl* sub_20B8C3)(int, int) = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x20B8C3);

	//void*(__thiscall* sub_23BDF6)(void*, int, int, int) = (void*(__thiscall*)(void*, int, int, int))((char*)H2BaseAddr + 0x23BDF6);

	int v2 = 0;
	int v3 = sub_20D2D8(15232);
	if (v3) {
		v2 = (int)sub_23BDF6_CM((void*)v3, *(DWORD*)(a1 + 4), *(DWORD*)(a1 + 8), *(WORD*)(a1 + 2));
	}
	*(BYTE*)(v2 + 108) = 1;
	sub_20B8C3(v2, a1);
	return v2;
}

#pragma endregion

void PoSmbstowcs(wchar_t* destsrc, int maxCount) {
	wchar_t* end = (wchar_t*)calloc(maxCount, sizeof(wchar_t));
	mbstowcs(end, (char*)destsrc, maxCount);
	memcpy(destsrc, end, maxCount * sizeof(wchar_t));
	destsrc[maxCount-1] = 0;
	free(end);
}

void GSCustomMenuCall_VKeyboard_Inner(wchar_t* textBuffer, __int16 textBufferLen, int menuType, int menuIdTitle, int title, int menuIdDesc, int description) {
	char* lblTitle = H2CustomLanguageGetLabel(menuIdTitle, title);
	char* lblDesc = H2CustomLanguageGetLabel(menuIdDesc, description);
//	add_cartographer_label(CMLabelMenuId_VKeyTest, 0xFFFFFFF0, lblTitle, true);
//	add_cartographer_label(CMLabelMenuId_VKeyTest, 0xFFFFFFF1, lblDesc, true);

	int(__thiscall*sub_20B0BC)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B0BC);
	signed int(__thiscall*sub_20C226)(void*, __int16, int, int, int) = (signed int(__thiscall*)(void*, __int16, int, int, int))((char*)H2BaseAddr + 0x20C226);
	signed int(__thiscall*sub_20C258)(void*, __int16, __int16, int, int, int) = (signed int(__thiscall*)(void*, __int16, __int16, int, int, int))((char*)H2BaseAddr + 0x20C258);
	void*(__thiscall*sub_20B11E)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B11E);

	unsigned int(__thiscall*sub_23B118)(void*, wchar_t*, int) = (unsigned int(__thiscall*)(void*, wchar_t*, int))((char*)H2BaseAddr + 0x23B118);
	int(__cdecl* sub_381FD)() = (int(__cdecl*)())((char*)H2BaseAddr + 0x381FD);
	bool(*sub_38F02)() = (bool(*)())((char*)H2BaseAddr + 0x38F02);
	int(*sub_38F74)() = (int(*)())((char*)H2BaseAddr + 0x38F74);
	void(__thiscall*sub_38F7A)(void*, int, int) = (void(__thiscall*)(void*, int, int))((char*)H2BaseAddr + 0x38F7A);

	// VirtualKeyboardTypes - original
	// -1 allows all symbols and does not check the output string if valid for any purpose as bellow options do

	// 0 to 4 - profile name
	// 5,6 - variant name
	// 7 - variant name default
	// 8,9 - playlist name
	// 10, 11 - film name
	// 12, 15 - enter message text
	// 13 - rename squad
	// 16 - search for games by its description

	// from 17 to 33 we got the same as above, but new to match our new code
	// 17 - 21 = 0 to 4 and so on, but diferent indices so we determine what our code will execute

	int VKbMenuTypeDefault = menuType;//enter message text
	int VKbMenuTypeNew = menuType + VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX;//enter message text
	
	if (VKbMenuTypeDefault == -1)
		VKbMenuTypeNew = -1;

	int a1 = 0;
	int a3 = 0;

	int v4; // esi@1
	char v8[0x20]; // [sp+8h] [bp-20h]@1

	sub_20B0BC(&v8);
	//sub_20C258(&v8, 0, 1 << a1, 2, 4, (int)(char*)H2BaseAddr + 0x23C72F);
	sub_20C258(&v8, 0, 1 << a1, 2, 4, (int)sub_23C72F_CM);
	v4 = (int)sub_20B11E(&v8);
	*(DWORD*)(v4 + 2652) = VKbMenuTypeDefault;
	if ((VKbMenuTypeNew >= VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX && VKbMenuTypeNew < VIRTUAL_KEYBOARD_MENU_TYPE_MAX_NEW) || VKbMenuTypeNew == -1) {
		PoSmbstowcs(textBuffer, textBufferLen); // convert multibyte to wide charaters, uses same character buffer
	}
	sub_23B118((void*)v4, textBuffer, textBufferLen);
	//*(DWORD*)(v4 + 2656) = a3;
}

#pragma endregion


const int CMLabelMenuId_Error = 0xFF000001;
#pragma region CM_Error

void __stdcall CMLabelButtons_Error(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Error);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Error() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Error
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Error(int button_id) {
	return true;
}

__declspec(naked) void sub_20F790_CM_nak_Error() {//__thiscall
	__asm {

		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

#pragma region CM_Error_Inner
int __cdecl CustomMenu_Error_Inner(int);

int(__cdecl *CustomMenuFuncPtrHelp_Error_Inner())(int) {
	return CustomMenu_Error_Inner;
}

DWORD* menu_vftable_1_Error_Inner = 0;
DWORD* menu_vftable_2_Error_Inner = 0;

void CMSetupVFTables_Error_Inner() {
	CMSetupVFTables(&menu_vftable_1_Error_Inner, &menu_vftable_2_Error_Inner, (DWORD)CMLabelButtons_Error, (DWORD)sub_2111ab_CMLTD_nak_Error, (DWORD)CustomMenuFuncPtrHelp_Error_Inner, (DWORD)sub_20F790_CM_nak_Error, true, 0);
}

int __cdecl CustomMenu_Error_Inner(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_Error_Inner, menu_vftable_2_Error_Inner, (DWORD)&CMButtonHandler_Error, 0, 272);
}

void GSCustomMenuCall_Error_Inner() {
	if (!H2IsDediServer) {
		int WgitScreenfunctionPtr = (int)(CustomMenu_Error_Inner);
		CallWgit(WgitScreenfunctionPtr, 1);
	}
	char* lblTitle = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFFFF0);
	char* lblDesc = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFFFF1);
	if (!lblTitle || !lblDesc) {
		if (lblTitle)
			addDebugText(lblTitle);
		if (lblDesc)
			addDebugText(lblDesc);
	}
	else {
		addDebugText("%s - %s", lblTitle, lblDesc);
	}
}

void GSCustomMenuCall_Error_Inner(char* title, char* description) {
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF0, title, true);
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF1, description, true);
	GSCustomMenuCall_Error_Inner();
}

void GSCustomMenuCall_Error_Inner(int menuIdTitle, int title, int menuIdDesc, int description) {
	char* lblTitle = H2CustomLanguageGetLabel(menuIdTitle, title);
	char* lblDesc = H2CustomLanguageGetLabel(menuIdDesc, description);
	GSCustomMenuCall_Error_Inner(lblTitle, lblDesc);
}

void GSCustomMenuCall_Error_Inner(int menuId, int title, int description) {
	GSCustomMenuCall_Error_Inner(menuId, title, menuId, description);
}
#pragma endregion

#pragma region CM_Error_Outer
int __cdecl CustomMenu_Error_Outer(int);

int(__cdecl *CustomMenuFuncPtrHelp_Error_Outer())(int) {
	return CustomMenu_Error_Outer;
}

DWORD* menu_vftable_1_Error_Outer = 0;
DWORD* menu_vftable_2_Error_Outer = 0;

void CMSetupVFTables_Error_Outer() {
	CMSetupVFTables(&menu_vftable_1_Error_Outer, &menu_vftable_2_Error_Outer, (DWORD)CMLabelButtons_Error, (DWORD)sub_2111ab_CMLTD_nak_Error, (DWORD)CustomMenuFuncPtrHelp_Error_Outer, (DWORD)sub_20F790_CM_nak_Error, false, 0);
}

int __cdecl CustomMenu_Error_Outer(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_Error_Outer, menu_vftable_2_Error_Outer, (DWORD)&CMButtonHandler_Error, 0, 272);
}

void GSCustomMenuCall_Error_Outer() {
	if (!H2IsDediServer) {
		int WgitScreenfunctionPtr = (int)(CustomMenu_Error_Outer);
		CallWgit(WgitScreenfunctionPtr, 2);
	}
	char* lblTitle = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFFFF0);
	char* lblDesc = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFFFF1);
	addDebugText("%s - %s", lblTitle, lblDesc);
}

void GSCustomMenuCall_Error_Outer(char* title, char* description) {
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF0, title, true);
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF1, description, true);
	GSCustomMenuCall_Error_Outer();
}

void GSCustomMenuCall_Error_Outer(int menuIdTitle, int title, int menuIdDesc, int description) {
	char* lblTitle = H2CustomLanguageGetLabel(menuIdTitle, title);
	char* lblDesc = H2CustomLanguageGetLabel(menuIdDesc, description);
	GSCustomMenuCall_Error_Outer(lblTitle, lblDesc);
}

void GSCustomMenuCall_Error_Outer(int menuId, int title, int description) {
	GSCustomMenuCall_Error_Outer(menuId, title, menuId, description);
}
#pragma endregion

#pragma endregion


const int CMLabelMenuId_Language = 0xFF000002;
#pragma region CM_Language

#pragma region CM_Language_Sub

int CM_Language_Main = 0;
int Language_Sub_Count = 0;
std::map<int, int> cm_lang_variant_map;
std::map<int, int> cm_lang_other_lang_map;

void CM_Language_Sub_Setup_Buttons() {
	Language_Sub_Count = 0;
	cm_lang_variant_map.clear();
	cm_lang_other_lang_map.clear();
	for (auto const &ent1 : custom_languages) {
		if (CM_Language_Main == -1) {
			if (ent1->other) {
				add_cartographer_label(CMLabelMenuId_Language, 0x40 + Language_Sub_Count, ent1->lang_name, true);
				cm_lang_variant_map[Language_Sub_Count] = ent1->lang_variant;
				cm_lang_other_lang_map[Language_Sub_Count] = ent1->lang_base;
				Language_Sub_Count++;
			}
		}
		else {
			if (ent1->lang_base == CM_Language_Main && !ent1->other) {
				if (ent1->lang_variant == 0) {
					char* base_var_proto = H2CustomLanguageGetLabel(CMLabelMenuId_Language, 0xFFFFFFF2);
					char* Base_Var_Name = (char*)malloc(strlen(base_var_proto) + 256);
					sprintf(Base_Var_Name, base_var_proto, ent1->lang_name);
					add_cartographer_label(CMLabelMenuId_Language, 0x40, Base_Var_Name, true);
					free(Base_Var_Name);
				}
				else {
					add_cartographer_label(CMLabelMenuId_Language, 0x40 + Language_Sub_Count, ent1->lang_name, true);
				}
				cm_lang_variant_map[Language_Sub_Count] = ent1->lang_variant;
				Language_Sub_Count++;
			}
		}
	}
}

void __stdcall CMLabelButtons_Language_Sub(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, 0x40 + button_id, CMLabelMenuId_Language);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Language_Sub() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF4//label_id_description
		push 0xFFFFFFF3//label_id_title
		push CMLabelMenuId_Language
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall
		
		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Language_Sub(int button_id) {
	addDebugText("Button ID: %d", button_id);
	setCustomLanguage(CM_Language_Main == -1 ? cm_lang_other_lang_map[button_id] : CM_Language_Main, cm_lang_variant_map[button_id]);
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_Language_Sub() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_Language_Sub(int);

int(__cdecl *CustomMenuFuncPtrHelp_Language_Sub())(int) {
	return CustomMenu_Language_Sub;
}

DWORD* menu_vftable_1_Language_Sub = 0;
DWORD* menu_vftable_2_Language_Sub = 0;

void CMSetupVFTables_Language_Sub() {
	CMSetupVFTables(&menu_vftable_1_Language_Sub, &menu_vftable_2_Language_Sub, (DWORD)CMLabelButtons_Language_Sub, (DWORD)sub_2111ab_CMLTD_nak_Language_Sub, (DWORD)CustomMenuFuncPtrHelp_Language_Sub, (DWORD)sub_20F790_CM_nak_Language_Sub, true, 0);
}

int __cdecl CustomMenu_Language_Sub(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_Language_Sub, menu_vftable_2_Language_Sub, (DWORD)&CMButtonHandler_Language_Sub, Language_Sub_Count, 272);
}

bool GSCustomMenuCall_Language_Sub() {
	CM_Language_Sub_Setup_Buttons();
	if (Language_Sub_Count > 0) {
		int WgitScreenfunctionPtr = (int)(CustomMenu_Language_Sub);
		CallWgit(WgitScreenfunctionPtr);
		return true;
	}
	else {
		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 4, 5);
	}
	return false;
}


#pragma endregion

#pragma region CM_Language_Main

void languageCaptureSetLabel() {
	if (H2Config_custom_labels_capture_missing) {
		add_cartographer_label(CMLabelMenuId_Language, 13, H2CustomLanguageGetLabel(CMLabelMenuId_Language, 0xFFFFF002), true);
	}
	else {
		char* asdg = H2CustomLanguageGetLabel(CMLabelMenuId_Language, 0xFFFFF003);
		add_cartographer_label(CMLabelMenuId_Language, 13, asdg, true);
	}
}

void toggleLanguageCapture() {
	H2Config_custom_labels_capture_missing = !H2Config_custom_labels_capture_missing;
	languageCaptureSetLabel();
}

void __stdcall CMLabelButtons_Language(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Language);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Language() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Language
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Language(int button_id) {
	if (button_id == 0) {
		setCustomLanguage(-1);
		return true;
	}
	else if (button_id > 0 && button_id < 2 + 8) {
		CM_Language_Main = button_id - 2;
		GSCustomMenuCall_Language_Sub();
		return false;
	}
	else if (button_id == 2 + 8 + 0) {
		if (!reloadCustomLanguages()) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 6, 7);
		}
		return false;
	}
	else if (button_id == 2 + 8 + 1) {
		saveCustomLanguages();
		return true;
	}
	else if (button_id == 2 + 8 + 2) {
		toggleLanguageCapture();
		return false;
	}
	return false;
}


__declspec(naked) void sub_20F790_CM_nak_Language() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_Language(int);

int(__cdecl *CustomMenuFuncPtrHelp_Language())(int) {
	return CustomMenu_Language;
}

DWORD* menu_vftable_1_Language = 0;
DWORD* menu_vftable_2_Language = 0;

void CMSetupVFTables_Language() {
	CMSetupVFTables(&menu_vftable_1_Language, &menu_vftable_2_Language, (DWORD)CMLabelButtons_Language, (DWORD)sub_2111ab_CMLTD_nak_Language, (DWORD)CustomMenuFuncPtrHelp_Language, (DWORD)sub_20F790_CM_nak_Language, true, 0);
}

int __cdecl CustomMenu_Language(int a1) {
	languageCaptureSetLabel();
	return CustomMenu_CallHead(a1, menu_vftable_1_Language, menu_vftable_2_Language, (DWORD)&CMButtonHandler_Language, 13, 272);
}

void GSCustomMenuCall_Language() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_Language);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion

#pragma endregion


const int CMLabelMenuId_EditCrosshair = 0xFF000010;
#pragma region CM_EditCrosshair

static void loadLabelCrosshairOffset() {
	if (!FloatIsNaN(H2Config_crosshair_offset)) {
		char* lblFpsLimitNum = H2CustomLanguageGetLabel(CMLabelMenuId_EditCrosshair, 0xFFFF0003);
		if (!lblFpsLimitNum)
			return;
		int buildLimitLabelLen = strlen(lblFpsLimitNum) + 20;
		char* buildLimitLabel = (char*)malloc(sizeof(char) * buildLimitLabelLen);
		snprintf(buildLimitLabel, buildLimitLabelLen, lblFpsLimitNum, H2Config_crosshair_offset);
		add_cartographer_label(CMLabelMenuId_EditCrosshair, 3, buildLimitLabel, true);
		free(buildLimitLabel);
	}
	else {
		char* lblFpsLimitDisabled = H2CustomLanguageGetLabel(CMLabelMenuId_EditCrosshair, 0xFFFF0013);
		add_cartographer_label(CMLabelMenuId_EditCrosshair, 3, lblFpsLimitDisabled, true);
	}
}

void __stdcall CMLabelButtons_EditCrosshair(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EditCrosshair);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditCrosshair() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EditCrosshair
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditCrosshair(int button_id) {
	const float upper_limit = 0.53f;
	const float major_inc = 0.02f;
	const float minor_inc = 0.001f;
	/*if (button_id == 0) {
		if (H2Config_crosshair_offset <= upper_limit - major_inc)
			H2Config_crosshair_offset += major_inc;
		else
			H2Config_crosshair_offset = upper_limit;
	}
	else if (button_id == 1) {
		if (H2Config_crosshair_offset <= upper_limit - minor_inc)
			H2Config_crosshair_offset += minor_inc;
		else
			H2Config_crosshair_offset = upper_limit;
	}
	else if (button_id == 3) {
		if (H2Config_crosshair_offset > 0.0f + minor_inc)
			H2Config_crosshair_offset -= minor_inc;
		else
			H2Config_crosshair_offset = 0.0f;
	}
	else if (button_id == 4) {
		if (H2Config_crosshair_offset > 0.0f + major_inc)
			H2Config_crosshair_offset -= major_inc;
		else
			H2Config_crosshair_offset = 0.0f;
	}
	else if (button_id == 2) {
		if (FloatIsNaN(H2Config_crosshair_offset))
			H2Config_crosshair_offset = 0.165f;
		else {
			H2Config_crosshair_offset = NAN;
			H2Tweaks::setCrosshairPos(0.165f);
		}
	}
	loadLabelCrosshairOffset();
	H2Tweaks::setCrosshairPos(H2Config_crosshair_offset);*/
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_EditCrosshair() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EditCrosshair(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditCrosshair())(int) {
	return CustomMenu_EditCrosshair;
}

DWORD* menu_vftable_1_EditCrosshair = 0;
DWORD* menu_vftable_2_EditCrosshair = 0;

void CMSetupVFTables_EditCrosshair() {
	CMSetupVFTables(&menu_vftable_1_EditCrosshair, &menu_vftable_2_EditCrosshair, (DWORD)CMLabelButtons_EditCrosshair, (DWORD)sub_2111ab_CMLTD_nak_EditCrosshair, (DWORD)CustomMenuFuncPtrHelp_EditCrosshair, (DWORD)sub_20F790_CM_nak_EditCrosshair, true, 0);
}

int __cdecl CustomMenu_EditCrosshair(int a1) {
	loadLabelCrosshairOffset();
	return CustomMenu_CallHead(a1, menu_vftable_1_EditCrosshair, menu_vftable_2_EditCrosshair, (DWORD)&CMButtonHandler_EditCrosshair, 5, 272);
}

void GSCustomMenuCall_EditCrosshair() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditCrosshair);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_EditFOV = 0xFF00000F;
#pragma region CM_EditFOV

static void loadLabelFOVNum() {
	if (H2Config_field_of_view != 70) {
		char* lblFpsLimitNum = H2CustomLanguageGetLabel(CMLabelMenuId_EditFOV, 0xFFFF0003);
		if (!lblFpsLimitNum)
			return;
		int buildLimitLabelLen = strlen(lblFpsLimitNum) + 20;
		char* buildLimitLabel = (char*)malloc(sizeof(char) * buildLimitLabelLen);
		snprintf(buildLimitLabel, buildLimitLabelLen, lblFpsLimitNum, H2Config_field_of_view);
		add_cartographer_label(CMLabelMenuId_EditFOV, 3, buildLimitLabel, true);
		free(buildLimitLabel);
	}
	else {
		char* lblFpsLimitDisabled = H2CustomLanguageGetLabel(CMLabelMenuId_EditFOV, 0xFFFF0013);
		add_cartographer_label(CMLabelMenuId_EditFOV, 3, lblFpsLimitDisabled, true);
	}
}

void __stdcall CMLabelButtons_EditFOV(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EditFOV);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditFOV() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EditFOV
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditFOV(int button_id) {
	const int upper_limit = 110;
	if (button_id == 0) {
		if (H2Config_field_of_view <= upper_limit - 10)
			H2Config_field_of_view += 10;
		else
			H2Config_field_of_view = upper_limit;
	}
	else if (button_id == 1) {
		if (H2Config_field_of_view < upper_limit)
			H2Config_field_of_view += 1;
	}
	else if (button_id == 3) {
		if (H2Config_field_of_view > 0)
			H2Config_field_of_view -= 1;
	}
	else if (button_id == 4) {
		if (H2Config_field_of_view > 10)
			H2Config_field_of_view -= 10;
		else
			H2Config_field_of_view = 0;
	}
	else if (button_id == 2) {
			H2Config_field_of_view = 70;
	}
	loadLabelFOVNum();
//	H2Tweaks::setFOV(H2Config_field_of_view);
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_EditFOV() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EditFOV(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditFOV())(int) {
	return CustomMenu_EditFOV;
}

DWORD* menu_vftable_1_EditFOV = 0;
DWORD* menu_vftable_2_EditFOV = 0;

void CMSetupVFTables_EditFOV() {
	CMSetupVFTables(&menu_vftable_1_EditFOV, &menu_vftable_2_EditFOV, (DWORD)CMLabelButtons_EditFOV, (DWORD)sub_2111ab_CMLTD_nak_EditFOV, (DWORD)CustomMenuFuncPtrHelp_EditFOV, (DWORD)sub_20F790_CM_nak_EditFOV, true, 0);
}

int __cdecl CustomMenu_EditFOV(int a1) {
	loadLabelFOVNum();
	return CustomMenu_CallHead(a1, menu_vftable_1_EditFOV, menu_vftable_2_EditFOV, (DWORD)&CMButtonHandler_EditFOV, 5, 272);
}

void GSCustomMenuCall_EditFOV() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditFOV);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion

const int CMLabelMenuId_VehicleEditFOV = 0xFF000020;
#pragma region CM_EditVehicleFOV

static void loadLabelVehicleFOVNum() {
	if (H2Config_vehicle_field_of_view != 70) {
		char* lblFpsLimitNum = H2CustomLanguageGetLabel(CMLabelMenuId_VehicleEditFOV, 0xFFFF0003);
		if (!lblFpsLimitNum)
			return;
		int buildLimitLabelLen = strlen(lblFpsLimitNum) + 20;
		char* buildLimitLabel = (char*)malloc(sizeof(char) * buildLimitLabelLen);
		snprintf(buildLimitLabel, buildLimitLabelLen, lblFpsLimitNum, H2Config_vehicle_field_of_view);
		add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 3, buildLimitLabel, true);
		free(buildLimitLabel);
	}
	else {
		char* lblFpsLimitDisabled = H2CustomLanguageGetLabel(CMLabelMenuId_VehicleEditFOV, 0xFFFF0013);
		add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 3, lblFpsLimitDisabled, true);
	}
}

void __stdcall CMLabelButtons_EditVehicleFOV(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_VehicleEditFOV);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditVehicleFOV() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_VehicleEditFOV
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditVehicleFOV(int button_id) {
	const int upper_limit = 110;
	if (button_id == 0) {
		if (H2Config_vehicle_field_of_view <= upper_limit - 10)
			H2Config_vehicle_field_of_view += 10;
		else
			H2Config_vehicle_field_of_view = upper_limit;
	}
	else if (button_id == 1) {
		if (H2Config_vehicle_field_of_view < upper_limit)
			H2Config_vehicle_field_of_view += 1;
	}
	else if (button_id == 3) {
		if (H2Config_vehicle_field_of_view > 0)
			H2Config_vehicle_field_of_view -= 1;
	}
	else if (button_id == 4) {
		if (H2Config_vehicle_field_of_view > 10)
			H2Config_vehicle_field_of_view -= 10;
		else
			H2Config_vehicle_field_of_view = 0;
	}
	else if (button_id == 2) {
			H2Config_vehicle_field_of_view = 70;
	}
	loadLabelVehicleFOVNum();
//	H2Tweaks::setVehicleFOV(H2Config_vehicle_field_of_view);
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_EditVehicleFOV() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EditVehicleFOV(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditVehicleFOV())(int) {
	return CustomMenu_EditVehicleFOV;
}

DWORD* menu_vftable_1_EditVehicleFOV = 0;
DWORD* menu_vftable_2_EditVehicleFOV = 0;

void CMSetupVFTables_EditVehicleFOV() {
	CMSetupVFTables(&menu_vftable_1_EditVehicleFOV, &menu_vftable_2_EditVehicleFOV, (DWORD)CMLabelButtons_EditVehicleFOV, (DWORD)sub_2111ab_CMLTD_nak_EditVehicleFOV, (DWORD)CustomMenuFuncPtrHelp_EditVehicleFOV, (DWORD)sub_20F790_CM_nak_EditVehicleFOV, true, 0);
}

int __cdecl CustomMenu_EditVehicleFOV(int a1) {
	loadLabelVehicleFOVNum();
	return CustomMenu_CallHead(a1, menu_vftable_1_EditVehicleFOV, menu_vftable_2_EditVehicleFOV, (DWORD)&CMButtonHandler_EditVehicleFOV, 5, 272);
}

void GSCustomMenuCall_EditVehicleFOV() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditVehicleFOV);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_EditHz = 0xFF000018;
#pragma region CM_EditHz

static void loadLabelHzNum() {
	if (H2Config_refresh_rate) {
		char* lblHzLimitNum = H2CustomLanguageGetLabel(CMLabelMenuId_EditHz, 0xFFFF0003);
		if (!lblHzLimitNum)
			return;
		int buildLimitLabelLen = strlen(lblHzLimitNum) + 20;
		char* buildLimitLabel = (char*)malloc(sizeof(char) * buildLimitLabelLen);
		snprintf(buildLimitLabel, buildLimitLabelLen, lblHzLimitNum, H2Config_refresh_rate);
		add_cartographer_label(CMLabelMenuId_EditHz, 3, buildLimitLabel, true);
		free(buildLimitLabel);
	}
	else {
		char* lblHzLimitDisabled = H2CustomLanguageGetLabel(CMLabelMenuId_EditHz, 0xFFFF0013);
		add_cartographer_label(CMLabelMenuId_EditHz, 3, lblHzLimitDisabled, true);
	}
}

void __stdcall CMLabelButtons_EditHz(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EditHz);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditHz() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EditHz
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditHz(int button_id) {
	const int upper_limit = 240;
	if (button_id == 0) {
		if (H2Config_refresh_rate <= upper_limit - 10)
			H2Config_refresh_rate += 10;
		else
			H2Config_refresh_rate = upper_limit;
	}
	else if (button_id == 1) {
		if (H2Config_refresh_rate < upper_limit)
			H2Config_refresh_rate += 1;
	}
	else if (button_id == 3) {
		if (H2Config_refresh_rate > 0)
			H2Config_refresh_rate -= 1;
	}
	else if (button_id == 4) {
		if (H2Config_refresh_rate > 10)
			H2Config_refresh_rate -= 10;
		else
			H2Config_refresh_rate = 0;
	}
	else if (button_id == 2) {
		if (H2Config_refresh_rate)
			H2Config_refresh_rate = 0;
		else
			H2Config_refresh_rate = 60;
	}
	loadLabelHzNum();
	H2Tweaks::setHz();
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_EditHz() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EditHz(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditHz())(int) {
	return CustomMenu_EditHz;
}

DWORD* menu_vftable_1_EditHz = 0;
DWORD* menu_vftable_2_EditHz = 0;

void CMSetupVFTables_EditHz() {
	CMSetupVFTables(&menu_vftable_1_EditHz, &menu_vftable_2_EditHz, (DWORD)CMLabelButtons_EditHz, (DWORD)sub_2111ab_CMLTD_nak_EditHz, (DWORD)CustomMenuFuncPtrHelp_EditHz, (DWORD)sub_20F790_CM_nak_EditHz, true, 0);
}

int __cdecl CustomMenu_EditHz(int a1) {
	loadLabelHzNum();
	return CustomMenu_CallHead(a1, menu_vftable_1_EditHz, menu_vftable_2_EditHz, (DWORD)&CMButtonHandler_EditHz, 5, 272);
}

void GSCustomMenuCall_EditHz() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditHz);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion



const int CMLabelMenuId_EditFPS = 0xFF00000E;
#pragma region CM_EditFPS

static void loadLabelFPSLimit() {
	if (H2Config_fps_limit) {
		char* lblFpsLimitNum = H2CustomLanguageGetLabel(CMLabelMenuId_EditFPS, 0xFFFF0003);
		if (!lblFpsLimitNum)
			return;
		int buildLimitLabelLen = strlen(lblFpsLimitNum) + 20;
		char* buildLimitLabel = (char*)malloc(sizeof(char) * buildLimitLabelLen);
		snprintf(buildLimitLabel, buildLimitLabelLen, lblFpsLimitNum, H2Config_fps_limit);
		add_cartographer_label(CMLabelMenuId_EditFPS, 3, buildLimitLabel, true);
		free(buildLimitLabel);
	}
	else {
		char* lblFpsLimitDisabled = H2CustomLanguageGetLabel(CMLabelMenuId_EditFPS, 0xFFFF0013);
		add_cartographer_label(CMLabelMenuId_EditFPS, 3, lblFpsLimitDisabled, true);
	}
}

void __stdcall CMLabelButtons_EditFPS(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EditFPS);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditFPS() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EditFPS
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditFPS(int button_id) {
	if (button_id == 0) {
		H2Config_fps_limit += 10;
	}
	else if (button_id == 1) {
		H2Config_fps_limit += 1;
	}
	else if (button_id == 3) {
		if (H2Config_fps_limit > 0)
			H2Config_fps_limit -= 1;
	}
	else if (button_id == 4) {
		if (H2Config_fps_limit > 10)
			H2Config_fps_limit -= 10;
		else
			H2Config_fps_limit = 0;
	}
	else if (button_id == 2) {
		if (H2Config_fps_limit)
			H2Config_fps_limit = 0;
		else
			H2Config_fps_limit = 60;
	}
	
	extern std::chrono::high_resolution_clock::duration desiredRenderTime;
	desiredRenderTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double>(1.0 / (double)H2Config_fps_limit));

	loadLabelFPSLimit();
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_EditFPS() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EditFPS(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditFPS())(int) {
	return CustomMenu_EditFPS;
}

DWORD* menu_vftable_1_EditFPS = 0;
DWORD* menu_vftable_2_EditFPS = 0;

void CMSetupVFTables_EditFPS() {
	CMSetupVFTables(&menu_vftable_1_EditFPS, &menu_vftable_2_EditFPS, (DWORD)CMLabelButtons_EditFPS, (DWORD)sub_2111ab_CMLTD_nak_EditFPS, (DWORD)CustomMenuFuncPtrHelp_EditFPS, (DWORD)sub_20F790_CM_nak_EditFPS, true, 0);
}

int __cdecl CustomMenu_EditFPS(int a1) {
	loadLabelFPSLimit();
	return CustomMenu_CallHead(a1, menu_vftable_1_EditFPS, menu_vftable_2_EditFPS, (DWORD)&CMButtonHandler_EditFPS, 5, 272);
}

void GSCustomMenuCall_EditFPS() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditFPS);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_EditStaticLoD = 0xFF000014;
#pragma region CM_EditStaticLoD

void __stdcall CMLabelButtons_EditStaticLoD(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EditStaticLoD);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditStaticLoD() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EditStaticLoD
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditStaticLoD(int button_id) {
	H2Config_static_lod_state = button_id;
	return true;
}

__declspec(naked) void sub_20F790_CM_nak_EditStaticLoD() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push H2Config_static_lod_state//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EditStaticLoD(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditStaticLoD())(int) {
	return CustomMenu_EditStaticLoD;
}

DWORD* menu_vftable_1_EditStaticLoD = 0;
DWORD* menu_vftable_2_EditStaticLoD = 0;

void CMSetupVFTables_EditStaticLoD() {
	CMSetupVFTables(&menu_vftable_1_EditStaticLoD, &menu_vftable_2_EditStaticLoD, (DWORD)CMLabelButtons_EditStaticLoD, (DWORD)sub_2111ab_CMLTD_nak_EditStaticLoD, (DWORD)CustomMenuFuncPtrHelp_EditStaticLoD, (DWORD)sub_20F790_CM_nak_EditStaticLoD, true, 0);
}

int __cdecl CustomMenu_EditStaticLoD(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_EditStaticLoD, menu_vftable_2_EditStaticLoD, (DWORD)&CMButtonHandler_EditStaticLoD, 7, 272);
}

void GSCustomMenuCall_EditStaticLoD() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditStaticLoD);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_EditCrosshairSize = 0xFF000015;
#pragma region CM_EditCrosshairSize

void __stdcall CMLabelButtons_EditCrosshairSize(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EditCrosshairSize);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditCrosshairSize() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EditCrosshairSize
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditCrosshairSize(int button_id) {
//	H2Tweaks::setCrosshairSize(button_id, true);
	return true;
}

__declspec(naked) void sub_20F790_CM_nak_EditCrosshairSize() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int CustomMenu_EditCrosshairSize(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditCrosshairSize())(int) {
	return CustomMenu_EditCrosshairSize;
}

DWORD * menu_vftable_1_EditCrosshairSize = 0;
DWORD * menu_vftable_2_EditCrosshairSize = 0;

void CMSetupVFTables_EditCrosshairSize() {
	CMSetupVFTables(&menu_vftable_1_EditCrosshairSize, &menu_vftable_2_EditCrosshairSize, (DWORD)CMLabelButtons_EditCrosshairSize, (DWORD)sub_2111ab_CMLTD_nak_EditCrosshairSize, (DWORD)CustomMenuFuncPtrHelp_EditCrosshairSize, (DWORD)sub_20F790_CM_nak_EditCrosshairSize, true, 0);
}

int CustomMenu_EditCrosshairSize(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_EditCrosshairSize, menu_vftable_2_EditCrosshairSize, (DWORD)&CMButtonHandler_EditCrosshairSize, 5, 272);
}

void GSCustomMenuCall_EditCrosshairSize() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditCrosshairSize);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_Update = 0xFF000011;
#pragma region CM_Update

static bool force_keep_open_Update = false;

void __stdcall CMLabelButtons_Update(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Update);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Update() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Update
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Update(int button_id) {

	if (button_id == 0) {
		GSDownloadCheck();
	}
	else if (button_id == 1 && GSDownload_files_to_download) {
		GSDownloadDL();
	}
	else if (button_id == 2 && GSDownload_files_to_install) {
		GSDownloadInstall();
	}
	else if (button_id == 3) {
		GSDownloadCancel();
		force_keep_open_Update = false;
		return true;
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_Update() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

void GSCustomMenuCall_Update();

void* __stdcall sub_248beb_deconstructor_Update(LPVOID lpMem, char a2)//__thiscall
{
	if (force_keep_open_Update) {
		GSCustomMenuCall_Update();
	}

	int(__thiscall* sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl* sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	sub_248b90((void*)lpMem);
	if (a2 & 1) {
		sub_287c23((void*)lpMem);
	}
	return (void*)lpMem;
}

__declspec(naked) void sub_248beb_nak_deconstructor_Update() {//__thiscall
	__asm {
		mov  eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_248beb_deconstructor_Update//__stdcall

		POP_REGISTERS

		retn 4
	}
}

int __cdecl CustomMenu_Update(int);

int(__cdecl *CustomMenuFuncPtrHelp_Update())(int) {
	return CustomMenu_Update;
}

DWORD* menu_vftable_1_Update = 0;
DWORD* menu_vftable_2_Update = 0;

void CMSetupVFTables_Update() {
	CMSetupVFTables(&menu_vftable_1_Update, &menu_vftable_2_Update, (DWORD)CMLabelButtons_Update, (DWORD)sub_2111ab_CMLTD_nak_Update, (DWORD)CustomMenuFuncPtrHelp_Update, (DWORD)sub_20F790_CM_nak_Update, true, (DWORD)sub_248beb_nak_deconstructor_Update);
}

int __cdecl CustomMenu_Update(int a1) {
	force_keep_open_Update = true;
	GSDownloadInit();
	return CustomMenu_CallHead(a1, menu_vftable_1_Update, menu_vftable_2_Update, (DWORD)&CMButtonHandler_Update, 4, 272);
}

void GSCustomMenuCall_Update() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_Update);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_Update_Note = 0xFF000012;
#pragma region CM_Update_Note

void __stdcall CMLabelButtons_Update_Note(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Update_Note);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Update_Note() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Update_Note
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Update_Note(int button_id) {

	if (button_id == 0) {
		GSCustomMenuCall_Update();
	}
	return true;
}

__declspec(naked) void sub_20F790_CM_nak_Update_Note() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_Update_Note(int);

int(__cdecl *CustomMenuFuncPtrHelp_Update_Note())(int) {
	return CustomMenu_Update_Note;
}

DWORD* menu_vftable_1_Update_Note = 0;
DWORD* menu_vftable_2_Update_Note = 0;

void CMSetupVFTables_Update_Note() {
	CMSetupVFTables(&menu_vftable_1_Update_Note, &menu_vftable_2_Update_Note, (DWORD)CMLabelButtons_Update_Note, (DWORD)sub_2111ab_CMLTD_nak_Update_Note, (DWORD)CustomMenuFuncPtrHelp_Update_Note, (DWORD)sub_20F790_CM_nak_Update_Note, true, 0);
}

int __cdecl CustomMenu_Update_Note(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_Update_Note, menu_vftable_2_Update_Note, (DWORD)&CMButtonHandler_Update_Note, 2, 272);
}

void GSCustomMenuCall_Update_Note() {
	if (!H2IsDediServer) {
		int WgitScreenfunctionPtr = (int)(CustomMenu_Update_Note);
		CallWgit(WgitScreenfunctionPtr);
	}
	char* lblTitle = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFF004);
	char* lblDesc = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFF005);
	if (!lblTitle || !lblDesc) {
		if (lblTitle)
			addDebugText(lblTitle);
		if (lblDesc)
			addDebugText(lblDesc);
	}
	else {
		addDebugText("%s - %s", lblTitle, lblDesc);
	}
}

#pragma endregion


const int CMLabelMenuId_Login_Warn = 0xFF000013;
#pragma region CM_Login_Warn

void __stdcall CMLabelButtons_Login_Warn(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Login_Warn);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Login_Warn() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Login_Warn
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Login_Warn(int button_id) {
	return true;
}

__declspec(naked) void sub_20F790_CM_nak_Login_Warn() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}


void* __stdcall sub_248beb_deconstructor_Login_Warn(LPVOID lpMem, char a2)//__thiscall
{
	//show select profile gui
	extern int notify_xlive_ui;
	notify_xlive_ui = 0;

	int(__thiscall* sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl* sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	sub_248b90((void*)lpMem);
	if (a2 & 1) {
		sub_287c23((void*)lpMem);
	}
	return (void*)lpMem;
}

__declspec(naked) void sub_248beb_nak_deconstructor_Login_Warn() {//__thiscall
	__asm {
		mov  eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_248beb_deconstructor_Login_Warn//__stdcall

		POP_REGISTERS

		retn 4
	}
}

int __cdecl CustomMenu_Login_Warn(int);

int(__cdecl *CustomMenuFuncPtrHelp_Login_Warn())(int) {
	return CustomMenu_Login_Warn;
}

DWORD* menu_vftable_1_Login_Warn = 0;
DWORD* menu_vftable_2_Login_Warn = 0;

void CMSetupVFTables_Login_Warn() {
	CMSetupVFTables(&menu_vftable_1_Login_Warn, &menu_vftable_2_Login_Warn, (DWORD)CMLabelButtons_Login_Warn, (DWORD)sub_2111ab_CMLTD_nak_Login_Warn, (DWORD)CustomMenuFuncPtrHelp_Login_Warn, (DWORD)sub_20F790_CM_nak_Login_Warn, true, (DWORD)sub_248beb_nak_deconstructor_Login_Warn);
}

int __cdecl CustomMenu_Login_Warn(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_Login_Warn, menu_vftable_2_Login_Warn, (DWORD)&CMButtonHandler_Login_Warn, 0, 272);
}

void GSCustomMenuCall_Login_Warn() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_Login_Warn);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion



#pragma region Setting_Modifications

static bool blind_fp = false;
static bool blind_hud = false;

bool __cdecl sub_BD114_blind_fp(unsigned int a1)//render first person model
{
	bool result = blind_fp ? true : false;
	if (AdvLobbySettings_mp_blind & 0b10)
		result = true;
	return result;
}

bool __cdecl sub_BD114_blind_hud(unsigned int a1)//render hud
{
	// TODO: cleanup
	static bool hud_opacity_reset = true;
	DWORD new_hud_globals = *(DWORD*)(H2BaseAddr + 0x9770F4);
	float& hud_opacity = *(float*)(new_hud_globals + 0x228); // set the opacity

	if (blind_hud || AdvLobbySettings_mp_blind & 0b01)
	{
		hud_opacity = 0.f;
		hud_opacity_reset = false;
	}
	else if (!hud_opacity_reset)
	{
		hud_opacity = 1.f;
		hud_opacity_reset = true;
	}

	return false;
}

static BYTE enableKeyboard3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void RefreshToggleIngameKeyboardControls() {
	if (!enableKeyboard3[0]) {
		for (int i = 0; i < 6; i++) {
			enableKeyboard3[i] = *((BYTE*)H2BaseAddr + 0x2FA67 + i);
		}
	}

	//multi-process splitscreen input hacks
	if (H2Config_disable_ingame_keyboard) {
		//Allows to repeat last movement when lose focus in mp, unlocks METHOD E from point after intro vid
		BYTE getFocusB[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x2E3C5, getFocusB, 1);
		//Allows input when not in focus.
		BYTE getFocusE[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		WriteBytes(H2BaseAddr + 0x2F9EA, getFocusE, 6);
		WriteBytes(H2BaseAddr + 0x2F9FC, getFocusE, 6);
		WriteBytes(H2BaseAddr + 0x2FA09, getFocusE, 6);
		//Disables the keyboard only when in-game and not in a menu.
		BYTE disableKeyboard1[] = { 0x90, 0x90, 0x90 };
		WriteBytes(H2BaseAddr + 0x2FA8A, disableKeyboard1, 3);
		BYTE disableKeyboard2[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x2FA92, disableKeyboard2, 1);
		BYTE disableKeyboard3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		WriteBytes(H2BaseAddr + 0x2FA67, disableKeyboard3, 6);
	}
	else {
		//Reset them all back.
		BYTE getFocusB[] = { 0x01 };
		WriteBytes(H2BaseAddr + 0x2E3C5, getFocusB, 1);
		
		BYTE getFocusE[] = { 0x0F, 0x85, 0x02, 0x02, 0x00, 0x00 };
		WriteBytes(H2BaseAddr + 0x2F9EA, getFocusE, 6);
		getFocusE[2] = 0xF0;
		getFocusE[3] = 0x01;
		WriteBytes(H2BaseAddr + 0x2F9FC, getFocusE, 6);
		getFocusE[2] = 0xE3;
		WriteBytes(H2BaseAddr + 0x2FA09, getFocusE, 6);
		
		BYTE disableKeyboard1[] = { 0x56, 0xFF, 0xD3 };
		WriteBytes(H2BaseAddr + 0x2FA8A, disableKeyboard1, 3);
		BYTE disableKeyboard2[] = { 0x01 };
		WriteBytes(H2BaseAddr + 0x2FA92, disableKeyboard2, 1);
		WriteBytes(H2BaseAddr + 0x2FA67, enableKeyboard3, 6);
	}
}

void RefreshTogglexDelay() {
	BYTE xDelayJMP[] = { 0x74 };
	if (!H2Config_xDelay)
		xDelayJMP[0] = 0xEB;
	WriteBytes(H2BaseAddr + (H2IsDediServer ? 0x1a1316 : 0x1c9d8e), xDelayJMP, 1);
}

#pragma endregion



const int CMLabelMenuId_EditHudGui = 0xFF000007;
#pragma region CM_EditHudGui

static void loadLabelToggle_EditHudGui(int lblIndex, int lblTogglePrefix, bool isEnabled) {
	combineCartographerLabels(CMLabelMenuId_EditHudGui, lblTogglePrefix + (isEnabled ? 1 : 0), 0xFFFF0000 + lblIndex, lblIndex);
}

void __stdcall CMLabelButtons_EditHudGui(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_EditHudGui);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_EditHudGui() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_EditHudGui
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_EditHudGui(int button_id) {
	if (button_id == 0) {
		GSCustomMenuCall_EditFOV();
	}
	else if (button_id == 1) {
		GSCustomMenuCall_EditVehicleFOV();
	}
	else if (button_id == 2) {
		GSCustomMenuCall_EditCrosshair();
	}
	else if (button_id == 3) {
		GSCustomMenuCall_EditCrosshairSize();
	}
	else if (button_id == 4) {
		loadLabelToggle_EditHudGui(button_id + 1, 0xFFFFFFF4, !(H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat));
	}
	else if (button_id == 5) {
		loadLabelToggle_EditHudGui(button_id + 1, 0xFFFFFFF2, !(blind_hud = !blind_hud));
	}
	else if (button_id == 6) {
		loadLabelToggle_EditHudGui(button_id + 1, 0xFFFFFFF2, !(blind_fp = !blind_fp));
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_EditHudGui() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_EditHudGui(int);

int(__cdecl *CustomMenuFuncPtrHelp_EditHudGui())(int) {
	return CustomMenu_EditHudGui;
}

DWORD* menu_vftable_1_EditHudGui = 0;
DWORD* menu_vftable_2_EditHudGui = 0;

void CMSetupVFTables_EditHudGui() {
	CMSetupVFTables(&menu_vftable_1_EditHudGui, &menu_vftable_2_EditHudGui, (DWORD)CMLabelButtons_EditHudGui, (DWORD)sub_2111ab_CMLTD_nak_EditHudGui, (DWORD)CustomMenuFuncPtrHelp_EditHudGui, (DWORD)sub_20F790_CM_nak_EditHudGui, true, 0);
}

int __cdecl CustomMenu_EditHudGui(int a1) {
	loadLabelToggle_EditHudGui(5, 0xFFFFFFF4, !H2Config_hide_ingame_chat);
	loadLabelToggle_EditHudGui(6, 0xFFFFFFF2, !blind_hud);
	loadLabelToggle_EditHudGui(7, 0xFFFFFFF2, !blind_fp);
	return CustomMenu_CallHead(a1, menu_vftable_1_EditHudGui, menu_vftable_2_EditHudGui, (DWORD)&CMButtonHandler_EditHudGui, 7, 272);
}

void GSCustomMenuCall_EditHudGui() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_EditHudGui);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_ToggleSkulls = 0xFF000008;
#pragma region CM_ToggleSkulls

static void loadSkullLabel(int lblIndex, bool isEnabled) {
	if (lblIndex == 3) {
		if (isEnabled) {
			blind_hud = !blind_hud;
			blind_fp = blind_hud;
		}
		isEnabled = blind_hud;
	}
	combineCartographerLabels(CMLabelMenuId_ToggleSkulls, 0xFFFFFFF2 + (isEnabled ? 1 : 0), 0xFFFF0000 + lblIndex, lblIndex + 1);
}

int getSkullIndexOffset(int lblIndex) {
	if (lblIndex == 0)
		return 0x4D832D;//Anger
	else if (lblIndex == 1)
		return 0x4D8322;//Assassins
	else if (lblIndex == 2)
		return 0x4D8328;//Black Eye
	else if (lblIndex == 3)
		return 0x4D8326;//Blind
	else if (lblIndex == 4)
		return 0x4D8329;//Catch
	else if (lblIndex == 5)
		return 0x4D8320;//Envy
	else if (lblIndex == 6)
		return 0x4D8324;//Famine
	else if (lblIndex == 7)
		return 0x4D8327;//Ghost
	else if (lblIndex == 8)
		return 0x4D8321;//Grunt Birthday Party
	else if (lblIndex == 9)
		return 0x4D832B;//Iron
	else if (lblIndex == 10)
		return 0x4D8325;//IWHBYD
	else if (lblIndex == 11)
		return 0x4D832C;//Mythic
	else if (lblIndex == 12)
		return 0x4D832A;//Sputnik
	else if (lblIndex == 13)
		return 0x4D8323;//Thunderstorm
	else if (lblIndex == 14)
		return 0x4D832E;//Whuppopotamus
	return 0;
}



void __stdcall CMLabelButtons_ToggleSkulls(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_ToggleSkulls);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_ToggleSkulls() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_ToggleSkulls
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_ToggleSkulls(int button_id) {
	if (button_id == 3) {
		loadSkullLabel(button_id, true);
	}
	else if (button_id >= 0 && button_id < 15) {
		BYTE& skull = *(BYTE*)((char*)H2BaseAddr + getSkullIndexOffset(button_id));
		skull = !skull;
		loadSkullLabel(button_id, skull == 1);
	}
	
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_ToggleSkulls() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_ToggleSkulls(int);

int(__cdecl *CustomMenuFuncPtrHelp_ToggleSkulls())(int) {
	return CustomMenu_ToggleSkulls;
}

DWORD* menu_vftable_1_ToggleSkulls = 0;
DWORD* menu_vftable_2_ToggleSkulls = 0;

void CMSetupVFTables_ToggleSkulls() {
	
	

	CMSetupVFTables(&menu_vftable_1_ToggleSkulls, &menu_vftable_2_ToggleSkulls, (DWORD)CMLabelButtons_ToggleSkulls, (DWORD)sub_2111ab_CMLTD_nak_ToggleSkulls, (DWORD)CustomMenuFuncPtrHelp_ToggleSkulls, (DWORD)sub_20F790_CM_nak_ToggleSkulls, true, 0);
}

int __cdecl CustomMenu_ToggleSkulls(int a1) {
	for (int i = 0; i < 15; i++) {
		BYTE& skull = *(BYTE*)((char*)H2BaseAddr + getSkullIndexOffset(i));
		if (i == 3)
			loadSkullLabel(i, false);
		else
			loadSkullLabel(i, skull == 1);
	}
	return CustomMenu_CallHead(a1, menu_vftable_1_ToggleSkulls, menu_vftable_2_ToggleSkulls, (DWORD)&CMButtonHandler_ToggleSkulls, 15, 272);
}

void GSCustomMenuCall_ToggleSkulls() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_ToggleSkulls);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_OtherSettings = 0xFF00000D;
#pragma region CM_OtherSettings

static bool vehicleFlipoverEject = true;
void refreshVehicleFlipoverEject() {
	//Disables Vehicle Eject on Flipover (host required)
	BYTE assmDisableVehicleFlipEject[] = { 0xEB };
	if (vehicleFlipoverEject)
		assmDisableVehicleFlipEject[0] = 0x7E;
	WriteBytes(H2BaseAddr + 0x159e5d, assmDisableVehicleFlipEject, 1);
}

static void loadLabelToggle_OtherSettings(int lblIndex, int lblTogglePrefix, bool isEnabled) {
	combineCartographerLabels(CMLabelMenuId_OtherSettings, lblTogglePrefix + (isEnabled ? 1 : 0), 0xFFFF0000 + lblIndex, lblIndex);
}

void __stdcall CMLabelButtons_OtherSettings(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_OtherSettings);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_OtherSettings() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_OtherSettings
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_OtherSettings(int button_id) {
	if (button_id == 0) {
		GSCustomMenuCall_EditFPS();
	}
	else if (button_id == 1) {
		GSCustomMenuCall_EditStaticLoD();
	}
	else if (button_id == 2) {
		GSCustomMenuCall_EditHz();
	}
//	else if (button_id == 2) {
//		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0x8, 0x9);
//		//loadLabelToggle_OtherSettings(button_id + 1, 0xFFFFFFF2, (H2Config_controller_aim_assist = !H2Config_controller_aim_assist));
//		RefreshToggleDisableControllerAimAssist();
//	}
	else if (button_id == 3) {
		loadLabelToggle_OtherSettings(button_id + 1, 0xFFFFFFF2, (H2Config_discord_enable = !H2Config_discord_enable));
		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF02A, 0xFFFFF02B);
	}
	else if (button_id == 4) {
		loadLabelToggle_OtherSettings(button_id + 1, 0xFFFFFFF2, (H2Config_xDelay = !H2Config_xDelay));
		RefreshTogglexDelay();
	}
	else if (button_id == 5) {
		loadLabelToggle_OtherSettings(button_id + 1, 0xFFFFFFF6, !(H2Config_skip_intro = !H2Config_skip_intro));
	}
	else if (button_id == 6) {
		loadLabelToggle_OtherSettings(button_id + 1, 0xFFFFFFF2, !(H2Config_disable_ingame_keyboard = !H2Config_disable_ingame_keyboard));
		RefreshToggleIngameKeyboardControls();
	}
	else if (button_id == 7) {
		loadLabelToggle_OtherSettings(button_id + 1, 0xFFFFFFF2, (H2Config_raw_input = !H2Config_raw_input));
		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF02A, 0xFFFFF02B);
	}
	else if (button_id == 8) {
		loadLabelToggle_OtherSettings(button_id + 1, 0xFFFFFFF2, (H2Config_hiresfix = !H2Config_hiresfix));
		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF02A, 0xFFFFF02B);
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_OtherSettings() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_OtherSettings(int);

int(__cdecl *CustomMenuFuncPtrHelp_OtherSettings())(int) {
	return CustomMenu_OtherSettings;
}

DWORD* menu_vftable_1_OtherSettings = 0;
DWORD* menu_vftable_2_OtherSettings = 0;

void CMSetupVFTables_OtherSettings() {
	CMSetupVFTables(&menu_vftable_1_OtherSettings, &menu_vftable_2_OtherSettings, (DWORD)CMLabelButtons_OtherSettings, (DWORD)sub_2111ab_CMLTD_nak_OtherSettings, (DWORD)CustomMenuFuncPtrHelp_OtherSettings, (DWORD)sub_20F790_CM_nak_OtherSettings, true, 0);
}

int __cdecl CustomMenu_OtherSettings(int a1) {
//	loadLabelToggle_OtherSettings(3, 0xFFFFFFF2, H2Config_controller_aim_assist);
	loadLabelToggle_OtherSettings(4, 0xFFFFFFF2, H2Config_discord_enable);
	loadLabelToggle_OtherSettings(5, 0xFFFFFFF2, H2Config_xDelay);
	loadLabelToggle_OtherSettings(6, 0xFFFFFFF6, !H2Config_skip_intro);
	loadLabelToggle_OtherSettings(7, 0xFFFFFFF2, !H2Config_disable_ingame_keyboard);
	loadLabelToggle_OtherSettings(8, 0xFFFFFFF2, H2Config_raw_input);
	loadLabelToggle_OtherSettings(9, 0xFFFFFFF2, H2Config_hiresfix);
	return CustomMenu_CallHead(a1, menu_vftable_1_OtherSettings, menu_vftable_2_OtherSettings, (DWORD)&CMButtonHandler_OtherSettings, 9, 272);
}

void GSCustomMenuCall_OtherSettings() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_OtherSettings);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_AdvSettings = 0xFF000005;
#pragma region CM_AdvSettings

void __stdcall CMLabelButtons_AdvSettings(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_AdvSettings);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_AdvSettings() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_AdvSettings
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_AdvSettings(int button_id) {
	if (button_id == 0) {
		GSCustomMenuCall_Language();
	}
	else if (button_id == 1) {
		GSCustomMenuCall_EditHudGui();
	}
	else if (button_id == 2) {
		GSCustomMenuCall_OtherSettings();
	}
	else if (button_id == 3) {
		GSCustomMenuCall_ToggleSkulls();
	}
//	else if (button_id == 4) {
//		GSCustomMenuCall_AdvLobbySettings();
//	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_AdvSettings() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_AdvSettings(int);

int(__cdecl *CustomMenuFuncPtrHelp_AdvSettings())(int) {
	return CustomMenu_AdvSettings;
}

DWORD* menu_vftable_1_AdvSettings = 0;
DWORD* menu_vftable_2_AdvSettings = 0;

void CMSetupVFTables_AdvSettings() {
	CMSetupVFTables(&menu_vftable_1_AdvSettings, &menu_vftable_2_AdvSettings, (DWORD)CMLabelButtons_AdvSettings, (DWORD)sub_2111ab_CMLTD_nak_AdvSettings, (DWORD)CustomMenuFuncPtrHelp_AdvSettings, (DWORD)sub_20F790_CM_nak_AdvSettings, true, 0);
}

int __cdecl CustomMenu_AdvSettings(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_AdvSettings, menu_vftable_2_AdvSettings, (DWORD)&CMButtonHandler_AdvSettings, NetworkSession::localPeerIsSessionHost() && h2mod->GetEngineType() == e_engine_type::Multiplayer ? 4 : 4, 272);
}

void GSCustomMenuCall_AdvSettings() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_AdvSettings);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_AdvLobbySettings = 0xFF000016;
#pragma region CM_AdvLobbySettings

static void loadLabelToggle_AdvLobbySettings(int lblIndex, int lblTogglePrefix, bool isEnabled) {
	combineCartographerLabels(CMLabelMenuId_AdvLobbySettings, lblTogglePrefix + (isEnabled ? 1 : 0), 0xFFFF0000 + lblIndex, lblIndex);
}

void __stdcall CMLabelButtons_AdvLobbySettings(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_AdvLobbySettings);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_AdvLobbySettings() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_AdvLobbySettings
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_AdvLobbySettings(int button_id) {
	
	if (button_id == 0) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, (vehicleFlipoverEject = !vehicleFlipoverEject));
		refreshVehicleFlipoverEject();
	}
	else if (button_id == 1) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, !(AdvLobbySettings_disable_kill_volumes = !AdvLobbySettings_disable_kill_volumes));
		if (NetworkSession::localPeerIsSessionHost() && h2mod->GetEngineType() == e_engine_type::Multiplayer && !AdvLobbySettings_disable_kill_volumes) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0x8, 0x9);
		}
		H2Tweaks::toggleKillVolumes(!AdvLobbySettings_disable_kill_volumes);
	}
	else if (button_id == 2) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, (AdvLobbySettings_mp_explosion_physics = !AdvLobbySettings_mp_explosion_physics));
	}
	else if (button_id == 3) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, (AdvLobbySettings_mp_sputnik = !AdvLobbySettings_mp_sputnik));
	}
	else if (button_id == 4) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, (AdvLobbySettings_mp_grunt_bday_party = !AdvLobbySettings_mp_grunt_bday_party));
	}
	else if (button_id == 5) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, (AdvLobbySettings_grenade_chain_react = !AdvLobbySettings_grenade_chain_react));
	}
	else if (button_id == 6) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, (AdvLobbySettings_banshee_bomb = !AdvLobbySettings_banshee_bomb));
	}
	else if (button_id == 7) {
		AdvLobbySettings_mp_blind = (AdvLobbySettings_mp_blind & ~0b01) | (~AdvLobbySettings_mp_blind & 0b01);
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, !(AdvLobbySettings_mp_blind & 0b01));
	}
	else if (button_id == 8) {
		AdvLobbySettings_mp_blind = (AdvLobbySettings_mp_blind & ~0b10) | (~AdvLobbySettings_mp_blind & 0b10);
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, !(AdvLobbySettings_mp_blind & 0b10));
	}
	else if (button_id == 9) {
		loadLabelToggle_AdvLobbySettings(button_id + 1, 0xFFFFFFF2, (AdvLobbySettings_flashlight = !AdvLobbySettings_flashlight));
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_AdvLobbySettings() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

void* __stdcall sub_248beb_deconstructor_AdvLobbySettings(LPVOID lpMem, char a2)//__thiscall
{
	if (NetworkSession::localPeerIsSessionHost() && h2mod->GetEngineType() == e_engine_type::Multiplayer) {
		//advLobbySettings->sendLobbySettingsPacket();
	}
	
	int(__thiscall* sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl* sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	sub_248b90((void*)lpMem);
	if (a2 & 1) {
		sub_287c23((void*)lpMem);
	}
	return (void*)lpMem;
}

__declspec(naked) void sub_248beb_nak_deconstructor_AdvLobbySettings() {//__thiscall
	__asm {
		mov  eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_248beb_deconstructor_AdvLobbySettings//__stdcall

		POP_REGISTERS

		retn 4
	}
}

int __cdecl CustomMenu_AdvLobbySettings(int);

int(__cdecl *CustomMenuFuncPtrHelp_AdvLobbySettings())(int) {
	return CustomMenu_AdvLobbySettings;
}

DWORD* menu_vftable_1_AdvLobbySettings = 0;
DWORD* menu_vftable_2_AdvLobbySettings = 0;

void CMSetupVFTables_AdvLobbySettings() {
	CMSetupVFTables(&menu_vftable_1_AdvLobbySettings, &menu_vftable_2_AdvLobbySettings, (DWORD)CMLabelButtons_AdvLobbySettings, (DWORD)sub_2111ab_CMLTD_nak_AdvLobbySettings, (DWORD)CustomMenuFuncPtrHelp_AdvLobbySettings, (DWORD)sub_20F790_CM_nak_AdvLobbySettings, true, (DWORD)sub_248beb_nak_deconstructor_AdvLobbySettings);
}

int __cdecl CustomMenu_AdvLobbySettings(int a1) {
	loadLabelToggle_AdvLobbySettings(1, 0xFFFFFFF2, vehicleFlipoverEject);
	loadLabelToggle_AdvLobbySettings(2, 0xFFFFFFF2, !AdvLobbySettings_disable_kill_volumes);
	loadLabelToggle_AdvLobbySettings(3, 0xFFFFFFF2, AdvLobbySettings_mp_explosion_physics);
	loadLabelToggle_AdvLobbySettings(4, 0xFFFFFFF2, AdvLobbySettings_mp_sputnik);
	loadLabelToggle_AdvLobbySettings(5, 0xFFFFFFF2, AdvLobbySettings_mp_grunt_bday_party);
	loadLabelToggle_AdvLobbySettings(6, 0xFFFFFFF2, AdvLobbySettings_grenade_chain_react);
	loadLabelToggle_AdvLobbySettings(7, 0xFFFFFFF2, AdvLobbySettings_banshee_bomb);
	loadLabelToggle_AdvLobbySettings(8, 0xFFFFFFF2, !(AdvLobbySettings_mp_blind & 0b01));
	loadLabelToggle_AdvLobbySettings(9, 0xFFFFFFF2, !(AdvLobbySettings_mp_blind & 0b10));
	loadLabelToggle_AdvLobbySettings(10, 0xFFFFFFF2, AdvLobbySettings_flashlight);
	return CustomMenu_CallHead(a1, menu_vftable_1_AdvLobbySettings, menu_vftable_2_AdvLobbySettings, (DWORD)&CMButtonHandler_AdvLobbySettings, 11, 272);
}

void GSCustomMenuCall_AdvLobbySettings() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_AdvLobbySettings);
	CallWgit(WgitScreenfunctionPtr);
}

void GSCustomMenuCall_AdvLobbySettings3() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_AdvLobbySettings);
	CallWgit(WgitScreenfunctionPtr, 3);
}

#pragma endregion


const int CMLabelMenuId_Credits = 0xFF000006;
#pragma region CM_Credits

void __stdcall CMLabelButtons_Credits(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Credits);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Credits() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Credits
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Credits(int button_id) {
	return false;

	if (button_id == 1) {
		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFFF04, 0xFFFFFF05);
	}
	else if (button_id == 2) {
		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFFF02, 0xFFFFFF03);
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_Credits() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_Credits(int);

int(__cdecl *CustomMenuFuncPtrHelp_Credits())(int) {
	return CustomMenu_Credits;
}


DWORD* menu_vftable_1_Credits = 0;
DWORD* menu_vftable_2_Credits = 0;

int __fastcall test_credits_construct(void* a1,DWORD _EDX, char a2)
{
	return sub_2111ab_CMLTD((int)a1, a2, CMLabelMenuId_Credits, 0xFFFFFFF0, 0xFFFFFFF1);
}

void CMSetupVFTables_Credits() {
	//CMSetupVFTables(&menu_vftable_1_Credits, &menu_vftable_2_Credits, (DWORD)CMLabelButtons_Credits, (DWORD)test_credits_construct, (DWORD)CustomMenuFuncPtrHelp_Credits, (DWORD)sub_20F790_CM_nak_Credits, true, 0);
	CMSetupVFTables(&menu_vftable_1_Credits, &menu_vftable_2_Credits, (DWORD)CMLabelButtons_Credits, (DWORD)sub_2111ab_CMLTD_nak_Credits, (DWORD)CustomMenuFuncPtrHelp_Credits, (DWORD)sub_20F790_CM_nak_Credits, true, 0);
}

int __cdecl CustomMenu_Credits(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_Credits, menu_vftable_2_Credits, (DWORD)&CMButtonHandler_Credits, 16, 272);
}

extern void GSCustomMenuCall_Credits_();

void GSCustomMenuCall_Credits() {
	GSCustomMenuCall_Credits_();
	//int WgitScreenfunctionPtr = (int)(CustomMenu_Credits);
	//CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


static int accountingActiveHandleCount = 0;

static bool isAccountingActiveHandle() {
	return accountingActiveHandleCount > 0;
}

static void updateAccountingActiveHandle(bool active) {
	accountingActiveHandleCount += active ? 1 : -1;
	addDebugText("Accounting Active: %d", accountingActiveHandleCount);
	if (accountingActiveHandleCount <= 0) {
		SaveH2Accounts();
	}
}

static bool accountingGoBackToList = false;

void GSCustomMenuCall_AccountEdit();

const int CMLabelMenuId_Invalid_Login_Token = 0xFF000017;
#pragma region CM_Invalid_Login_Token

void __stdcall CMLabelButtons_Invalid_Login_Token(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Invalid_Login_Token);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Invalid_Login_Token() {//__thiscall
	__asm {
		mov eax, [esp + 4h]
		
		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Invalid_Login_Token
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Invalid_Login_Token(int button_id) {
	return true;
}

__declspec(naked) void sub_20F790_CM_nak_Invalid_Login_Token() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_Invalid_Login_Token(int);

int(__cdecl *CustomMenuFuncPtrHelp_Invalid_Login_Token())(int) {
	return CustomMenu_Invalid_Login_Token;
}


void* __stdcall sub_248beb_deconstructor_Login_Token(LPVOID lpMem, char a2)//__thiscall
{
	if (accountingGoBackToList && isAccountingActiveHandle()) {
		GSCustomMenuCall_AccountEdit();
		accountingGoBackToList = true;
	}

	updateAccountingActiveHandle(false);

	int(__thiscall* sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl* sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	sub_248b90((void*)lpMem);
	if (a2 & 1) {
		sub_287c23((void*)lpMem);
	}
	return (void*)lpMem;
}

__declspec(naked) void sub_248beb_nak_deconstructor_Login_Token() {//__thiscall
	__asm {
		mov  eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_248beb_deconstructor_Login_Token//__stdcall

		POP_REGISTERS

		retn 4
	}
}

DWORD* menu_vftable_1_Invalid_Login_Token = 0;
DWORD* menu_vftable_2_Invalid_Login_Token = 0;

void CMSetupVFTables_Invalid_Login_Token() {
	CMSetupVFTables(&menu_vftable_1_Invalid_Login_Token, &menu_vftable_2_Invalid_Login_Token, (DWORD)CMLabelButtons_Invalid_Login_Token, (DWORD)sub_2111ab_CMLTD_nak_Invalid_Login_Token, (DWORD)CustomMenuFuncPtrHelp_Invalid_Login_Token, (DWORD)sub_20F790_CM_nak_Invalid_Login_Token, true, (DWORD)sub_248beb_nak_deconstructor_Login_Token);
}

int __cdecl CustomMenu_Invalid_Login_Token(int a1) {
	accountingGoBackToList = true;
	updateAccountingActiveHandle(true);
	return CustomMenu_CallHead(a1, menu_vftable_1_Invalid_Login_Token, menu_vftable_2_Invalid_Login_Token, (DWORD)&CMButtonHandler_Invalid_Login_Token, 0, 272);
}

void GSCustomMenuCall_Invalid_Login_Token() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_Invalid_Login_Token);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_AccountCreate = 0xFF00000C;
#pragma region CM_AccountCreate

void setupAccountCreateLabels() {
	char* bufferUsername = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1);
	if (strlen(bufferUsername) <= 0) {
		char* bufferUsername2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 0xFFFFFFF2);
		strcpy_s(bufferUsername, XUSER_NAME_SIZE, bufferUsername2);
	}
	char* bufferEmail = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 2);
	if (strlen(bufferEmail) <= 0) {
		char* bufferEmail2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 0xFFFFFFF3);
		strcpy_s(bufferEmail, 256, bufferEmail2);
	}
	char* bufferPassword = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 3);
	if (strlen(bufferPassword) <= 0) {
		char* bufferPassword2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 0xFFFFFFF4);
		strcpy_s(bufferPassword, 256, bufferPassword2);
	}
}

void __stdcall CMLabelButtons_AccountCreate(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_AccountCreate);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_AccountCreate() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_AccountCreate
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static HANDLE hThreadCreate = 0;

static DWORD WINAPI AccountCreateThread(LPVOID lParam)
{
	//gotta delay it a little to make sure the menu's decide to render correctly.
	Sleep(200L);

	char* username = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1);
	char* email = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 2);
	char* pass = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 3);
	//submit account creation.
	if (HandleGuiAccountCreate(username, email, pass)) {
		//account created.
		//GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF026, 0xFFFFF027);
		//verification email.
		GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF028, 0xFFFFF029);

		char* username2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
		snprintf(username2, strlen(username) + 1, username);
		char* pass2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 2);
		snprintf(pass2, strlen(pass) + 1, pass);
		SecureZeroMemory(username, strlen(username));
		SecureZeroMemory(email, strlen(email));
		SecureZeroMemory(pass, strlen(pass));
	}

	updateAccountingActiveHandle(false);

	hThreadCreate = 0;
	return 0;
}

static bool CMButtonHandler_AccountCreate(int button_id) {
	if (button_id == 0) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1); // account name
		GSCustomMenuCall_VKeyboard_Inner(textBuffer, XUSER_NAME_SIZE /* Wide string buffer size */, 0, CMLabelMenuId_AccountCreate, 0xFFFFF002, CMLabelMenuId_AccountCreate, 0xFFFFF003);
	}
	else if (button_id == 1) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 2); // password
		GSCustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountCreate, 0xFFFFF004, CMLabelMenuId_AccountCreate, 0xFFFFF005);
	}
	else if (button_id == 2) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 3); // email
		GSCustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountCreate, 0xFFFFF006, CMLabelMenuId_AccountCreate, 0xFFFFF007);
	}
	else if (button_id == 3) {
		if (!hThreadCreate) {
			accountingGoBackToList = false;
			updateAccountingActiveHandle(true);
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF02C, 0xFFFFF02D);
			hThreadCreate = CreateThread(NULL, 0, AccountCreateThread, (LPVOID)0, 0, NULL);
		}
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_AccountCreate() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

void* __stdcall sub_248beb_deconstructor_AccountCreate(LPVOID lpMem, char a2)//__thiscall
{
	if (accountingGoBackToList && isAccountingActiveHandle()) {
		GSCustomMenuCall_AccountList();
		accountingGoBackToList = false;
	}

	updateAccountingActiveHandle(false);

	int(__thiscall* sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl* sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	sub_248b90((void*)lpMem);
	if (a2 & 1) {
		sub_287c23((void*)lpMem);
	}
	return (void*)lpMem;
}

__declspec(naked) void sub_248beb_nak_deconstructor_AccountCreate() {//__thiscall
	__asm {
		mov  eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_248beb_deconstructor_AccountCreate//__stdcall

		POP_REGISTERS

		retn 4
	}
}

int __cdecl CustomMenu_AccountCreate(int);

int(__cdecl *CustomMenuFuncPtrHelp_AccountCreate())(int) {
	return CustomMenu_AccountCreate;
}

DWORD* menu_vftable_1_AccountCreate = 0;
DWORD* menu_vftable_2_AccountCreate = 0;

void CMSetupVFTables_AccountCreate() {
	CMSetupVFTables(&menu_vftable_1_AccountCreate, &menu_vftable_2_AccountCreate, (DWORD)CMLabelButtons_AccountCreate, (DWORD)sub_2111ab_CMLTD_nak_AccountCreate, (DWORD)CustomMenuFuncPtrHelp_AccountCreate, (DWORD)sub_20F790_CM_nak_AccountCreate, true, (DWORD)sub_248beb_nak_deconstructor_AccountCreate);
}

int __cdecl CustomMenu_AccountCreate(int a1) {
	accountingGoBackToList = true;
	updateAccountingActiveHandle(true);
	setupAccountCreateLabels();
	return CustomMenu_CallHead(a1, menu_vftable_1_AccountCreate, menu_vftable_2_AccountCreate, (DWORD)&CMButtonHandler_AccountCreate, 4, 272);
}

void GSCustomMenuCall_AccountCreate() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_AccountCreate);
	CallWgit(WgitScreenfunctionPtr);
}


#pragma endregion /* CM_AccountCreate */


const int CMLabelMenuId_AccountEdit = 0xFF00000A;
#pragma region CM_AccountEdit

void setupAccountEditLabels() {
	add_cartographer_label(CMLabelMenuId_AccountEdit, 3, H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 0xFFFFFFF2 + (AccountEdit_remember ? 1 : 0)), true);

	char* bufferUsername = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
	if (strlen(bufferUsername) <= 0) {
		char* bufferUsername2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 0xFFFFFFF4);
		strcpy_s(bufferUsername, 255, bufferUsername2);
	}
	char* bufferPassword = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 2);
	if (strlen(bufferPassword) <= 0) {
		char* bufferPassword2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 0xFFFFFFF5);
		strcpy_s(bufferPassword, 255, bufferPassword2);
	}
}

void __stdcall CMLabelButtons_AccountEdit(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_AccountEdit);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_AccountEdit() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_AccountEdit
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static int master_login_code;
static HANDLE hThreadLogin = INVALID_HANDLE_VALUE;

void xbox_live_task_progress_callback(DWORD a1)
{
	// if the hThreadLogin handle is INVALID_HANDLE_VALUE, it means that the login thread has ended
	if (hThreadLogin == INVALID_HANDLE_VALUE)
	{
		// this is the ptr of the callback, if it gets set to null, it will close the menu
		*(DWORD*)(a1 + 2652) = NULL;

		if (master_login_code < 0)
		{
			switch (master_login_code)
			{
			case ERROR_CODE_CURL_SOCKET_FAILED:
			case ERROR_CODE_CURL_HANDLE:
			case ERROR_CODE_ACCOUNT_DATA:
			case ERROR_CODE_INVALID_PARAM:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF014, 0xFFFFF015);
				return;

			case ERROR_CODE_CURL_EASY_PERF:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF030, 0xFFFFF031);
				return;

			case ERROR_CODE_INVALID_VERSION:
				GSCustomMenuCall_Update_Note();
				return;

			case ERROR_CODE_INVALID_LOGIN_TOKEN:
				GSCustomMenuCall_Invalid_Login_Token();
				return;

			case ERROR_CODE_INVALID_LOGIN_ID:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF008, 0xFFFFF009);
				return;

			case ERROR_CODE_INVALID_PASSWORD:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF00A, 0xFFFFF00B);
				return;

			case ERROR_CODE_MACHINE_BANNED:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF00C, 0xFFFFF00D);
				return;

			case ERROR_CODE_ACCOUNT_BANNED:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF00E, 0xFFFFF00F);
				return;

			case ERROR_CODE_ACCOUNT_DISABLED:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF010, 0xFFFFF011);
				return;

			case ERROR_CODE_MACHINE_SERIAL_INSUFFICIENT:
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF018, 0xFFFFF019);
				return;

			default:
				//unknown error!
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF012, 0xFFFFF013);
				return;
			}
		}
		else {
			if (master_login_code == SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT) {
				GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF018, 0xFFFFF019);
				return;
			}
			else {
				imgui_handler::ToggleWindow("motd");
				//extern int notify_xlive_ui;
				//notify_xlive_ui = 0;
				//sub_248beb_nak_deconstructor_Login_Warn();
				//GSCustomMenuCall_Login_Warn();
				return;
			}
		}
	}
}

static DWORD WINAPI ThreadLogin(LPVOID lParam)
{
	int button_id = (int)lParam;

	//gotta delay it a little to make sure the menu's decide to render correctly.
	Sleep(200L);

	if (button_id == -1) {
		char* identifier = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
		char* identifier_pass = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 2);
		//login to account
		
		if (HandleGuiLogin(0, identifier, identifier_pass, &master_login_code)) {
			H2AccountLastUsed = 0;
		}
		SecureZeroMemory(identifier_pass, strlen(identifier_pass));
	}
	else {
		//login to account
		if (HandleGuiLogin(H2AccountArrayLoginToken[button_id], 0, 0, &master_login_code)) {
			H2AccountLastUsed = button_id;
		}
	}

	updateAccountingActiveHandle(false);

	hThreadLogin = INVALID_HANDLE_VALUE;
	return 0;
}

static bool CMButtonHandler_AccountEdit(int button_id) {
	if (button_id == 0) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
		GSCustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountEdit, 0xFFFFF002, CMLabelMenuId_AccountEdit, 0xFFFFF003);
	}
	else if (button_id == 1) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 2);
		GSCustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountEdit, 0xFFFFF004, CMLabelMenuId_AccountEdit, 0xFFFFF005);
	}
	else if (button_id == 2) {
		AccountEdit_remember = !AccountEdit_remember;
		add_cartographer_label(CMLabelMenuId_AccountEdit, 3, H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 0xFFFFFFF2 + (AccountEdit_remember ? 1 : 0)), true);
	}
	else if (button_id == 3) { // login button id
		if (hThreadLogin == INVALID_HANDLE_VALUE) {
			accountingGoBackToList = false;
			updateAccountingActiveHandle(true);
			hThreadLogin = CreateThread(NULL, 0, ThreadLogin, (LPVOID)-1, 0, NULL);
			c_xbox_live_task_progress_menu::Open(xbox_live_task_progress_callback);
		}
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_AccountEdit() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

void* __stdcall sub_248beb_deconstructor_AccountEdit(LPVOID lpMem, char a2)//__thiscall
{
	if (accountingGoBackToList && isAccountingActiveHandle()) {
		GSCustomMenuCall_AccountList();
		accountingGoBackToList = false;
	}

	updateAccountingActiveHandle(false);

	int(__thiscall* sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl* sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	sub_248b90((void*)lpMem);
	if (a2 & 1) {
		sub_287c23((void*)lpMem);
	}
	return (void*)lpMem;
}

__declspec(naked) void sub_248beb_nak_deconstructor_AccountEdit() {//__thiscall
	__asm {
		mov  eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_248beb_deconstructor_AccountEdit//__stdcall

		POP_REGISTERS
		retn 4
	}
}

int __cdecl CustomMenu_AccountEdit(int);

int(__cdecl *CustomMenuFuncPtrHelp_AccountEdit())(int) {
	return CustomMenu_AccountEdit;
}

DWORD* menu_vftable_1_AccountEdit = 0;
DWORD* menu_vftable_2_AccountEdit = 0;

void CMSetupVFTables_AccountEdit() {
	CMSetupVFTables(&menu_vftable_1_AccountEdit, &menu_vftable_2_AccountEdit, (DWORD)CMLabelButtons_AccountEdit, (DWORD)sub_2111ab_CMLTD_nak_AccountEdit, (DWORD)CustomMenuFuncPtrHelp_AccountEdit, (DWORD)sub_20F790_CM_nak_AccountEdit, true, (DWORD)sub_248beb_nak_deconstructor_AccountEdit);
}

int __cdecl CustomMenu_AccountEdit(int a1) {
	accountingGoBackToList = true;
	updateAccountingActiveHandle(true);
	setupAccountEditLabels();
	return CustomMenu_CallHead(a1, menu_vftable_1_AccountEdit, menu_vftable_2_AccountEdit, (DWORD)&CMButtonHandler_AccountEdit, 4, 272);
}

void GSCustomMenuCall_AccountEdit() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_AccountEdit);
	CallWgit(WgitScreenfunctionPtr);
}

#pragma endregion


const int CMLabelMenuId_AccountList = 0xFF000009;
#pragma region CM_AccountList

bool mode_remove_account = false;

static void CM_AccountList_Setup_Buttons() {
	mode_remove_account = false;

	for (int i = 0; i < H2AccountCount; i++) {
		add_cartographer_label(CMLabelMenuId_AccountList, 1 + i, H2AccountArrayUsername[i] ? H2AccountArrayUsername[i] : H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0005), true);
	}

	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0004), true);
	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 1, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0003), true);
	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 2, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0000), true);
	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 3, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0001 + (mode_remove_account ? 1 : 0)), true);
}

void __stdcall CMLabelButtons_AccountList(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_AccountList);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_AccountList() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_AccountList
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

void GSCustomMenuCall_AccountList();
//using namespace MapChecksumSync;
static bool CMButtonHandler_AccountList(int button_id) {
	if (button_id == H2AccountCount + 1) {
		if (!mode_remove_account) {
			GSCustomMenuCall_AccountCreate();
		}
	}
	else if (button_id == H2AccountCount + 2) {
		if (!mode_remove_account) {
			GSCustomMenuCall_AccountEdit();
		}
	}
	else if (button_id == H2AccountCount + 3) {
		//remove next selected account.
		mode_remove_account = !mode_remove_account;
		add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 3, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0001 + (mode_remove_account ? 1 : 0)), true);
	}
	else if (H2AccountCount > 0 && button_id >= 0 && button_id < H2AccountCount) {
		if (mode_remove_account) {
			H2AccountAccountRemove(button_id);
			GSCustomMenuCall_AccountList();
			H2AccountLastUsed = 0;
			return true;
		}
		else {
			if (hThreadLogin == INVALID_HANDLE_VALUE) {
				accountingGoBackToList = false;
				updateAccountingActiveHandle(true);
				hThreadLogin = CreateThread(NULL, 0, ThreadLogin, (LPVOID)button_id, 0, NULL);
				c_xbox_live_task_progress_menu::Open(xbox_live_task_progress_callback);
			}
		}
	}
	else if (button_id == H2AccountCount) {
		if (!mode_remove_account) {
			//play offline
			BYTE abEnet[6];
			BYTE abOnline[20];
			XNetRandom(abEnet, 6);
			XNetRandom(abOnline, 20);
			if (ConfigureUserDetails("[Username]", "12345678901234567890123456789012", rand(), 0, H2Config_ip_lan, ByteToHexStr(abEnet, 6).c_str(), ByteToHexStr(abOnline, 20).c_str(), false)) {
				//show select profile gui
				extern int notify_xlive_ui;
				notify_xlive_ui = 0;
				H2Config_master_ip = inet_addr("127.0.0.1");
				H2Config_master_port_relay = 2001;
			}
		}
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_AccountList() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push H2AccountLastUsed//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

void* __stdcall sub_248beb_deconstructor_AccountList(LPVOID lpMem, char a2)//__thiscall
{
	updateAccountingActiveHandle(false);

	int(__thiscall* sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl* sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	sub_248b90((void*)lpMem);
	if (a2 & 1) {
		sub_287c23((void*)lpMem);
	}
	return (void*)lpMem;
}

__declspec(naked) void sub_248beb_nak_deconstructor_AccountList() {//__thiscall
	__asm {
		mov  eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_248beb_deconstructor_AccountList//__stdcall

		POP_REGISTERS

		retn 4
	}
}

int __cdecl CustomMenu_AccountList(int);

int(__cdecl *CustomMenuFuncPtrHelp_AccountList())(int) {
	return CustomMenu_AccountList;
}

DWORD* menu_vftable_1_AccountList = 0;
DWORD* menu_vftable_2_AccountList = 0;

void CMSetupVFTables_AccountList() {
	CMSetupVFTables(&menu_vftable_1_AccountList, &menu_vftable_2_AccountList, (DWORD)CMLabelButtons_AccountList, (DWORD)sub_2111ab_CMLTD_nak_AccountList, (DWORD)CustomMenuFuncPtrHelp_AccountList, (DWORD)sub_20F790_CM_nak_AccountList, false, (DWORD)sub_248beb_nak_deconstructor_AccountList);
}

int __cdecl CustomMenu_AccountList(int a1) {
	updateAccountingActiveHandle(true);
	CM_AccountList_Setup_Buttons();
	return CustomMenu_CallHead(a1, menu_vftable_1_AccountList, menu_vftable_2_AccountList, (DWORD)&CMButtonHandler_AccountList, 4 + H2AccountCount + (H2AccountCount <= 0 ? -1 : 0), 272);
}

void GSCustomMenuCall_AccountList() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_AccountList);
	CallWgit(WgitScreenfunctionPtr, 3);
}

#pragma endregion


const int CMLabelMenuId_Guide = 0xFF000003;
#pragma region CM_Guide

void __stdcall CMLabelButtons_Guide(int a1, int a2)
{
	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD(v3, button_id + 1, CMLabelMenuId_Guide);
	}
}

__declspec(naked) void sub_2111ab_CMLTD_nak_Guide() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_Guide
		push eax
		push ecx
		call sub_2111ab_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

static bool CMButtonHandler_Guide(int button_id) {
	if (button_id == 0) {
		imgui_handler::ToggleWindow("Advanced Settings");
		//GSCustomMenuCall_AdvSettings();
	}
	else if (button_id == 1) {
		ShellExecuteA(NULL, "open", "https://cartographer.online/", NULL, NULL, SW_SHOWDEFAULT);
	}
	else if (button_id == 2) {
		GSCustomMenuCall_Credits();
	}
	else if (button_id == 3) {
		GSCustomMenuCall_Update();
	}
	return false;
}

__declspec(naked) void sub_20F790_CM_nak_Guide() {//__thiscall
	__asm {
		PUSH_REGISTERS

		push 0//selected button id
		push ecx
		call sub_20F790_CM//__stdcall

		POP_REGISTERS

		retn
	}
}

int __cdecl CustomMenu_Guide(int);

int(__cdecl *CustomMenuFuncPtrHelp_Guide())(int) {
	return CustomMenu_Guide;
}

DWORD* menu_vftable_1_Guide = 0;
DWORD* menu_vftable_2_Guide = 0;

void CMSetupVFTables_Guide() {
	CMSetupVFTables(&menu_vftable_1_Guide, &menu_vftable_2_Guide, (DWORD)CMLabelButtons_Guide, (DWORD)sub_2111ab_CMLTD_nak_Guide, (DWORD)CustomMenuFuncPtrHelp_Guide, (DWORD)sub_20F790_CM_nak_Guide, false, 0);
}

int __cdecl CustomMenu_Guide(int a1) {
	char* guide_desc_base = H2CustomLanguageGetLabel(CMLabelMenuId_Guide, 0xFFFFFFF2);
	char* guide_description = (char*)malloc(strlen(guide_desc_base) + 50);
	sprintf(guide_description, guide_desc_base, GetVKeyCodeString(H2Config_hotkeyIdGuide).c_str());
	add_cartographer_label(CMLabelMenuId_Guide, 0xFFFFFFF1, guide_description, true);
	free(guide_description);
	return CustomMenu_CallHead(a1, menu_vftable_1_Guide, menu_vftable_2_Guide, (DWORD)&CMButtonHandler_Guide, 4, 272);
}

void GSCustomMenuCall_Guide() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_Guide);
	CallWgit(WgitScreenfunctionPtr, 2, 272);
}

#pragma endregion


void GSCustomMenuCall_Esc() {

	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x20CDE7);
	//CallWgit(WgitScreenfunctionPtr, 3);

	//GSCustomMenuCall_EscSettings();

	//GSCustomMenuCall_Error_Outer(CMLabelMenuId_Error, 2, 3);
}


#pragma region some tests

typedef int(__stdcall *tbtnhandler)(void* thisptr, int a2, int a3);
tbtnhandler pbtnHandler;

typedef int(__stdcall *tsub_20C226)(void*, __int16, int, int, int);
tsub_20C226 psub_20C226;
int __stdcall sub_20C226(void* thisptr, __int16 a2, int a3, int a4, int a5) {
	return psub_20C226(thisptr, a2, a3, a4, a5);
}


typedef char(__stdcall *tsub_24DC0D)(void*, int);
tsub_24DC0D psub_24DC0D;
char __stdcall sub_24DC0D_CM(void* thisptr, int a2) {//__thiscall
	
	//char result = psub_24DC0D(thisptr, a2);
	//return result;

	//when sub_20EB2B returns 1 - closing outermost menu
	//when sub_20EB2B returns 0 - closing inner menu's to go back not out completely

	char(__thiscall*sub_20EB2B)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20EB2B);
	int(*sub_209BA3)() = (int(*)())((char*)H2BaseAddr + 0x209BA3);
	signed int(*sub_209D08)() = (signed int(*)())((char*)H2BaseAddr + 0x209D08);

	if (*(DWORD*)a2 == 5 && (*(DWORD*)(a2 + 8) == 1 || *(DWORD*)(a2 + 8) == 13) && sub_209BA3() != -1)
		sub_209D08();
	char result = sub_20EB2B(thisptr, a2);
	return result;
}

int nak_24DC0D_CM_thisptr;
int nak_24DC0D_CM_a2;
int nak_24DC0D_CM_result;

__declspec(naked) void sub_24DC0D_CM_nak() {
	__asm {
		mov     nak_24DC0D_CM_thisptr, ecx
		mov     nak_24DC0D_CM_a2, edi
	}
	nak_24DC0D_CM_result = sub_24DC0D_CM((void*)nak_24DC0D_CM_thisptr, nak_24DC0D_CM_a2);
	__asm {
		mov     eax, nak_24DC0D_CM_result
		retn    4
	}
}

typedef char(__thiscall *tsub_23D8AE)(void*, int);
tsub_23D8AE psub_23D8AE;
char __stdcall sub_23D8AE_CM(void* thisptr, int a2) {//__thiscall - cannot hook. must manually asm

	//char result = psub_23D8AE(thisptr, a2);

	//return result;
	
	char(__thiscall*sub_20EB2B)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20EB2B);
	int(__thiscall*sub_212604)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x212604);

	int v2; // eax@2
	char result; // al@4

	if (*(DWORD*)a2 != 5 || (v2 = *(DWORD*)(a2 + 8), v2 != 13) && v2 != 1)
	{
		result = sub_20EB2B(thisptr, a2);
	}
	else
	{
		sub_212604(thisptr, 3);
		result = 1;
	}
	return result;
}

int nak_23D8AE_CM_thisptr;
int nak_23D8AE_CM_a2;
int nak_23D8AE_CM_result;

__declspec(naked) void sub_23D8AE_CM_nak() {
	__asm {
		mov     nak_23D8AE_CM_thisptr, ecx
		mov     nak_23D8AE_CM_a2, edi
	}
	nak_23D8AE_CM_result = sub_23D8AE_CM((void*)nak_23D8AE_CM_thisptr, nak_23D8AE_CM_a2);
	__asm {
		mov     eax, nak_23D8AE_CM_result
		retn    4
	}
}

typedef char(__stdcall *tsub_20EEBE)(void*, int);
tsub_20EEBE psub_20EEBE;
char __stdcall sub_20EEBE_CM(void* thisptr, int a2) {//__thiscall
	
	//char result = psub_20EEBE(thisptr, a2);
	//return result;

	//when sub_20EB2B returns 1 - closing outermost menu
	//when sub_20EB2B returns 0 - closing inner menu's to go back not out completely

	char(__thiscall*sub_20EB2B)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20EB2B);
	char(__thiscall*sub_214DDD)(void*) = (char(__thiscall*)(void*))((char*)H2BaseAddr + 0x214DDD);

	char result = sub_20EB2B(thisptr, a2);
	if (!result)
	{
		void* v4 = (void*)*((DWORD*)thisptr + 663);
		if (!v4 || *(DWORD*)a2 != 6 || (result = sub_214DDD(v4)) == 0)
		{
			result = sub_20EB2B(thisptr, a2);
			if (!result && *(DWORD*)a2 == 6)
				result = (*(int(__thiscall**)(void*, DWORD))(*(DWORD*)thisptr + 108))(thisptr, a2);
		}
	}
	return result;
}

int nak_20EEBE_CM_thisptr;
int nak_20EEBE_CM_a2;
int nak_20EEBE_CM_result;

__declspec(naked) void sub_20EEBE_CM_nak() {
	__asm {
		mov     nak_20EEBE_CM_thisptr, ecx
		mov     nak_20EEBE_CM_a2, edi
	}
	nak_20EEBE_CM_result = sub_20EEBE_CM((void*)nak_20EEBE_CM_thisptr, nak_20EEBE_CM_a2);
	__asm {
		mov     eax, nak_20EEBE_CM_result
		retn    4
	}
}

int nak_keyHandler_CM_thisptr;
int nak_keyHandler_CM_a2;
int nak_keyHandler_CM_result;
int nak_keyHandler_CM_func;

__declspec(naked) void CM_keyHandler_inner_nak() {
	__asm {
		mov     nak_keyHandler_CM_thisptr, ecx
		mov     nak_keyHandler_CM_a2, edi
	}
	nak_keyHandler_CM_func = *(DWORD*)((int)*(int*)((DWORD)H2BaseAddr + 0x482D3C) + 0x8);
	if (nak_keyHandler_CM_func == 3) {
		nak_keyHandler_CM_func = 0x20EEBE;
	}
	else {
		nak_keyHandler_CM_func = 0x24DC0D;
	}
	nak_keyHandler_CM_func = (int)((char*)H2BaseAddr + nak_keyHandler_CM_func);
	__asm {
		mov     ecx, nak_keyHandler_CM_thisptr
		mov     edi, nak_keyHandler_CM_a2
		jmp     nak_keyHandler_CM_func
	}
}


__declspec(naked) void CM_keyHandler_outer_nak() {
	__asm {
		mov     nak_keyHandler_CM_thisptr, ecx
		mov     nak_keyHandler_CM_a2, edi
	}
	nak_keyHandler_CM_func = *(DWORD*)((int)*(int*)((DWORD)H2BaseAddr + 0x482D3C) + 0x8);
	if (nak_keyHandler_CM_func == 3) {
		nak_keyHandler_CM_func = 0x20EEBE;
	}
	else {
		nak_keyHandler_CM_func = 0x23D8AE;
	}
	nak_keyHandler_CM_func = (int)((char*)H2BaseAddr + nak_keyHandler_CM_func);
	__asm {
		mov     ecx, nak_keyHandler_CM_thisptr
		mov     edi, nak_keyHandler_CM_a2
		jmp     nak_keyHandler_CM_func
	}
}

__declspec(naked) void CM_keyHandler_inner_nak1() {
	__asm {
		mov     nak_keyHandler_CM_thisptr, ecx
		mov     nak_keyHandler_CM_a2, edi
	}
	nak_keyHandler_CM_result = *(DWORD*)((int)*(int*)((DWORD)H2BaseAddr + 0x482D3C) + 0x8);
	if (nak_keyHandler_CM_result == 3) {
		nak_keyHandler_CM_result = sub_20EEBE_CM((void*)nak_keyHandler_CM_thisptr, nak_keyHandler_CM_a2);
	}
	else {
		nak_keyHandler_CM_result = sub_24DC0D_CM((void*)nak_keyHandler_CM_thisptr, nak_keyHandler_CM_a2);
	}
	__asm {
		mov     eax, nak_keyHandler_CM_result
		retn    4
	}
}

__declspec(naked) void CM_keyHandler_outer_nak1() {
	__asm {
		mov     nak_keyHandler_CM_thisptr, ecx
		mov     nak_keyHandler_CM_a2, edi
	}
	nak_keyHandler_CM_result = *(DWORD*)((int)*(int*)((DWORD)H2BaseAddr + 0x482D3C) + 0x8);
	if (nak_keyHandler_CM_result == 3) {
		nak_keyHandler_CM_result = sub_20EEBE_CM((void*)nak_keyHandler_CM_thisptr, nak_keyHandler_CM_a2);
	}
	else {
		nak_keyHandler_CM_result = sub_23D8AE_CM((void*)nak_keyHandler_CM_thisptr, nak_keyHandler_CM_a2);
	}
	__asm {
		mov     eax, nak_keyHandler_CM_result
		retn    4
	}
}

int* func_array;

void setupSomeTests() {
	func_array = (int*)malloc(16 * sizeof(int));

	int func_array2[14] = {
		0x24DC0D
		,0x20EEBE
		,0x23D8AE
		,0x24FE49
		,0xD04C
		,0x21FCEE//odd behaviour taking over network lobby cannot esc //(incorrect?)odd inner behaviour taking over network lobby cannot esc, 80, 81, 82, 83, 84
		//,0x23D060//16 crashes outer, 6, 22
		//,0x23F0BA//16 corupts outer, 7, 23
		,0x2415C4
		,0x245BB1
		,0x246B55
		,0x24A5D9
		,0x24C714
		,0x2531AC
		,0x25C9E8
		,0x25D2D0//odd outer behaviour taking over network lobby cannot esc.
	};

	memcpy(func_array, func_array2, 16 * sizeof(int*));

	//psub_20C226 = (tsub_20C226)DetourClassFunc((BYTE*)H2BaseAddr + 0x20C226, (BYTE*)sub_20C226, 9);

	//psub_24DC0D = (tsub_24DC0D)DetourClassFunc((BYTE*)H2BaseAddr + 0x24DC0D, (BYTE*)sub_24DC0D_CM, 8);

	//psub_23D8AE = (tsub_23D8AE)DetourClassFunc((BYTE*)H2BaseAddr + 0x23D8AE, (BYTE*)sub_23D8AE, 9);

	//psub_20EEBE = (tsub_20EEBE)DetourClassFunc((BYTE*)H2BaseAddr + 0x20EEBE, (BYTE*)sub_20EEBE_CM, 9);
}


int keyHandler_itr = 148;

__declspec(naked) void CM_keyHandler_inner_nak2() {
	__asm {
		mov     nak_keyHandler_CM_thisptr, ecx
		mov     nak_keyHandler_CM_a2, edi
	}
	nak_keyHandler_CM_func = func_array[(keyHandler_itr / 14)];
	nak_keyHandler_CM_func = (int)((char*)H2BaseAddr + nak_keyHandler_CM_func);
	__asm {
		mov     ecx, nak_keyHandler_CM_thisptr
		mov     edi, nak_keyHandler_CM_a2
	}
	__asm {
		jmp nak_keyHandler_CM_func
	}
}


__declspec(naked) void CM_keyHandler_outer_nak2() {
	__asm {
		mov     nak_keyHandler_CM_thisptr, ecx
		mov     nak_keyHandler_CM_a2, edi
	}
	nak_keyHandler_CM_func = func_array[(keyHandler_itr % 14)];
	nak_keyHandler_CM_func = (int)((char*)H2BaseAddr + nak_keyHandler_CM_func);
	__asm {
		mov     ecx, nak_keyHandler_CM_thisptr
		mov     edi, nak_keyHandler_CM_a2
	}
	__asm {
		jmp nak_keyHandler_CM_func
	}
}

#pragma endregion

typedef char(__stdcall *tsub_23d060)(int, int*);
tsub_23d060 psub_23d060;
char __stdcall sub_23d060(int thisptr, int* a2) //__thiscall
{
	return psub_23d060(thisptr, a2);//keyboard keypress handler
}


typedef void*(__stdcall *tsub_23BC45)(void*);
tsub_23BC45 psub_23BC45;
void* __stdcall sub_23BC45(void* thisptr)//__thiscall
{
	return psub_23BC45(thisptr);

	void*(__thiscall* sub_23F756)(void*, __int16, __int16) = (void*(__thiscall*)(void*, __int16, __int16))((char*)H2BaseAddr + 0x23F756);

	void* v1 = thisptr;
	sub_23F756(thisptr, -1, 0);
	//*v1 = &c_virtual_keyboard_button::`vftable';
	*(DWORD*)v1 = (DWORD)(char*)H2BaseAddr + 0x3D30D4;
	return v1;
}

void XUiShowSignInH2() {
	if (!isAccountingActiveHandle() && ReadH2Accounts()) {
		GSCustomMenuCall_AccountList();
	}
	else {
		if (!isAccountingActiveHandle())
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF016, 0xFFFFF017);
	}
}

typedef int(__cdecl *tsub_23f6b7)(int);
tsub_23f6b7 psub_23f6b7;
int __cdecl sub_23f6b7(int a1)
{
	if (userSignedIn(0)) {
		XUserSignOut(0);
		UpdateConnectionStatus();
	}
	return psub_23f6b7(a1);
}

#pragma region Obscure_Menus

static bool CMButtonHandler_Obscure(int button_id) {
	return false;
}

DWORD* menu_vftable_1_Obscure = 0;
DWORD* menu_vftable_2_Obscure = 0;

void CMSetupVFTables_Obscure() {
	CMSetupVFTables(&menu_vftable_1_Obscure, &menu_vftable_2_Obscure, 0, 0, 0, 0, false, 0);
}

int Obscure_wgit_id = 296;

int __cdecl CustomMenu_Obscure(int a1) {
	return CustomMenu_CallHead(a1, menu_vftable_1_Obscure, menu_vftable_2_Obscure, (DWORD)&CMButtonHandler_Obscure, 14, Obscure_wgit_id);
}

void GSCustomMenuCall_Obscure() {
	int WgitScreenfunctionPtr = (int)(CustomMenu_Obscure);
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x0021f5f8);//Network Adapter list.
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x0021f681);//About dialog
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x0025692c);//keyboard layout
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x00259d05);//thumbstick layout
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x00259dc0);//button layout
	CallWgit(WgitScreenfunctionPtr);
}


#pragma endregion


typedef void(__cdecl *tsub_bd137)(unsigned int);
tsub_bd137 psub_bd137;
void __cdecl sub_bd137(unsigned int skull_id) {
	//psub_bd137(skull_id);

	BYTE*(*sub_22CE83)() = (BYTE*(*)())((BYTE*)H2BaseAddr + 0x22CE83);
	signed int(*sub_5343F)() = (signed int(*)())((BYTE*)H2BaseAddr + 0x5343F);
	int(__cdecl* sub_22DEA4)(int, int) = (int(__cdecl*)(int, int))((BYTE*)H2BaseAddr + 0x22DEA4);
	DWORD(__cdecl* sub_A402C)(float, float, float, __int16) = (DWORD(__cdecl*)(float, float, float, __int16))((BYTE*)H2BaseAddr + 0xA402C);
	DWORD(__cdecl* sub_8836C)(DWORD, float) = (DWORD(__cdecl*)(DWORD, float))((BYTE*)H2BaseAddr + 0x8836C);

	BYTE* byte_4D8320 = (BYTE*)((char*)H2BaseAddr + 0x4D8320);
	DWORD* dword_3BCAF8 = (DWORD*)((char*)H2BaseAddr + 0x3BCAF8);
	DWORD dword_482290 = *(DWORD*)((char*)H2BaseAddr + 0x482290);

	int v1; // ST0C_4
	int v2; // eax
	int v3; // eax
	int v4; // ecx
	unsigned int v5; // ecx

	bool skull_activated = false;

	if (skull_id == 0x6) {
		skull_activated = blind_fp = blind_hud = true;
	}
	else if (skull_id <= 0xE && !byte_4D8320[skull_id]) {
		byte_4D8320[skull_id] = 1;
		skull_activated = true;
	}

	if (skull_activated) {
		sub_22CE83();
		v1 = dword_3BCAF8[skull_id];
		v2 = sub_5343F();
		sub_22DEA4(v2, v1);
		sub_A402C(1.0f, 1.0f, 1.0f, 20);//r, g, b, flash length
		v3 = *(DWORD*)(tags::get_matg_globals_ptr() + 308);
		if (v3 != -1) {
			v4 = v3 + dword_482290;
			v5 = *(DWORD*)(v4 + 280);
			if (v5 != -1)
				sub_8836C(v5, 1.0f);//sound_id?, volume 1.0-2.9?
		}
	}
}



void initGSCustomMenu() {
	
#pragma region Init_Cartographer_Labels

	add_cartographer_label(CMLabelMenuId_EscSettings, 0xFFFFFFF0, "Esc Settings");
	add_cartographer_label(CMLabelMenuId_EscSettings, 0xFFFFFFF1, "Esc Settings Here.");
	add_cartographer_label(CMLabelMenuId_EscSettings, 1, "btn 1");
	add_cartographer_label(CMLabelMenuId_EscSettings, 2, "btn 2");


	//add_cartographer_label(CMLabelMenuId_VKeyTest, 0xFFFFFFF0, "Virtual Keyboard Test!", true);
	//add_cartographer_label(CMLabelMenuId_VKeyTest, 0xFFFFFFF1, "Enter something... please?", true);


	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF0, "Error!", true);
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF1, "Generic Error.", true);
	add_cartographer_label(CMLabelMenuId_Error, 2, "You... didn't listen didn't you!?");
	add_cartographer_label(CMLabelMenuId_Error, 3, "Good! You are part of the #dev_preview group (or you got this from the asshole who leaked it). I want you to try and break things and find bugs; since that's what your damn job is.");
	add_cartographer_label(CMLabelMenuId_Error, 4, "None");
	add_cartographer_label(CMLabelMenuId_Error, 5, "There are no custom languages catergorised as Other.");
	add_cartographer_label(CMLabelMenuId_Error, 6, "Error");
	add_cartographer_label(CMLabelMenuId_Error, 7, "An error occured when trying to read the custom language file.\r\nNo Changes have been made.\r\nReview the on screen debug log for more details.");
	add_cartographer_label(CMLabelMenuId_Error, 0x8, "Incomplete Feature");
	add_cartographer_label(CMLabelMenuId_Error, 0x9, "This feature is incomplete.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFF02, "Glitchy Scripts");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFF03, "Created/reversed custom GUIs.\r\nCoded the entire account creation/login system.\r\nCreated Custom Languages.\r\nIs the Halo 2 Master Server overlord!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFF04, "PermaNull");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFF05, "Wrote/reversed all the functionality for online play! And created GunGame & Zombies.\r\nNow that's no easy feat!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF004, "Outdated Version!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF005, "You are using an outdated version of Project Cartographer! Please install the latest version.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF006, "Invalid Login Token!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF007, "Login Again.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF008, "Invalid Account ID!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF009, "The Username or Email Address you entered is not one of an existing account. Please check your spelling or create a new account if you don't have one already.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF00A, "Incorrect Password!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF00B, "The password you entered is incorrect!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF00C, "BANNED!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF00D, "The computer you are currently using is banned! If you think this is a mistake, post on the online forum pleading your innocence; if you have any you cheater!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF00E, "BANNED!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF00F, "Your account is banned! If you think this is a mistake, post on the online forum pleading your innocence; if you have any you cheater!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF010, "Account Disabled!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF011, "Your account is currently disabled, closed or deleted! Please contact an administrator if this was a mistake.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF012, "Unknown Error!?");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF013, "The error that has occured has not been handled. Please contact an admin including any log files you have and what you did.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF014, "Something's Broken!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF015, "An internal error has occured. We've probably noticed this already but contact an admin if this persists.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF016, "Accounts in Use");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF017, "Another instance of Halo 2 / H2Server is currently signing in, please try again after it finishes.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF018, "Insufficient PC Identification!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF019, "Your PC does not have sufficient unique identifiers available. Please contact an admin about linking this PC with another that you own that does have sufficient info. This is to ensure fair online play.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF01A, "Invalid Email!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF01B, "The Email address you have entered is invalid! Please double check your spelling.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF01C, "Invalid Username!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF01D, "The Username you have entered is invalid! Please ensure you have formed it correctly using only allowed symbols!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF01E, "Invalid Password!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF01F, "The Password you have entered is invalid! Please ensure you have formed it correctly using only allowed symbols!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF020, "Email Already in Use!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF021, "The Email Address you have entered is already in use! You cannot use an email for multiple accounts. Please use a different email, sign in to that account or reset its password if you have forgotton it.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF022, "Username Taken!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF023, "The Username you have entered is already in use!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF024, "BANNED Email Provider!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF025, "The Email Address you have entered is using a domain name that has been banned! We do not allow disposable email addresses! If this is a mistake please contact an admin.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF026, "Account Successfully Created!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF027, "The account you just entered has been successfully created! You may now use those details to login.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF028, "Verification Email Sent!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF029, "An email has been sent to the email address submitted. Please follow the instuctions in the email to activate your account.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF02A, "Restart Required");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF02B, "The setting you have just changed requires that you restart your game for it to take effect.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF02C, "Creating Account...");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF02D, "Processing your new account...\r\nPlease wait.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF02E, "Logging in...");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF02F, "Please wait while you are being logged in.");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF030, "Connection Failed!");
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF031, "Please visit:\r\nhttps://h2pc.org/connection\r\nfor help with troubleshooting potential issues.");


	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF0, "Select Language");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF1, "Select the language you would like to play the game in.");
	add_cartographer_label(CMLabelMenuId_Language, 1, "System Default");
	add_cartographer_label(CMLabelMenuId_Language, 2, "Other >");
	add_cartographer_label(CMLabelMenuId_Language, 3, "English >");
	add_cartographer_label(CMLabelMenuId_Language, 4, "Japanese >");
	add_cartographer_label(CMLabelMenuId_Language, 5, "German >");
	add_cartographer_label(CMLabelMenuId_Language, 6, "French >");
	add_cartographer_label(CMLabelMenuId_Language, 7, "Spanish >");
	add_cartographer_label(CMLabelMenuId_Language, 8, "Italian >");
	add_cartographer_label(CMLabelMenuId_Language, 9, "Korean >");
	add_cartographer_label(CMLabelMenuId_Language, 10, "Chinese >");
	add_cartographer_label(CMLabelMenuId_Language, 11, "Reload Language File");
	add_cartographer_label(CMLabelMenuId_Language, 12, "Save Language File");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFF002, "Ignore New Labels");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFF003, "Capture New Labels");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF2, "--- Base %s Variant ---");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF3, "Select Language Variant");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF4, "Select the variant of the language you would like to play the game in.");


	add_cartographer_label(CMLabelMenuId_EditCrosshair, 0xFFFFFFF0, "Edit Crosshair Offset");
	add_cartographer_label(CMLabelMenuId_EditCrosshair, 0xFFFFFFF1, "Use the buttons below to modify the in-game Crosshair Offset.");
	add_cartographer_label(CMLabelMenuId_EditCrosshair, 1, "+0.02");
	add_cartographer_label(CMLabelMenuId_EditCrosshair, 2, "+0.001");
	add_cartographer_label(CMLabelMenuId_EditCrosshair, 0xFFFF0003, "Offset: %f");
	add_cartographer_label(CMLabelMenuId_EditCrosshair, 0xFFFF0013, "Offset Alteration Disabled");
	add_cartographer_label(CMLabelMenuId_EditCrosshair, 4, "-0.001");
	add_cartographer_label(CMLabelMenuId_EditCrosshair, 5, "-0.02");


	add_cartographer_label(CMLabelMenuId_EditFOV, 0xFFFFFFF0, "Edit Player Field of View");
	add_cartographer_label(CMLabelMenuId_EditFOV, 0xFFFFFFF1, "Use the buttons below to modify the in-game first person Field of View (FoV).");
	add_cartographer_label(CMLabelMenuId_EditFOV, 1, "+10");
	add_cartographer_label(CMLabelMenuId_EditFOV, 2, "+1");
	add_cartographer_label(CMLabelMenuId_EditFOV, 0xFFFF0003, "FoV: %d");
	add_cartographer_label(CMLabelMenuId_EditFOV, 0xFFFF0013, "FoV Alteration Disabled");
	add_cartographer_label(CMLabelMenuId_EditFOV, 4, "-1");
	add_cartographer_label(CMLabelMenuId_EditFOV, 5, "-10");


	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 0xFFFFFFF0, "Edit Vehicle Field of View");
	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 0xFFFFFFF1, "Use the buttons below to modify the Field of View (FoV) of vehicles.");
	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 1, "+10");
	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 2, "+1");
	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 0xFFFF0003, "FoV: %d");
	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 0xFFFF0013, "FoV Alteration Disabled");
	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 4, "-1");
	add_cartographer_label(CMLabelMenuId_VehicleEditFOV, 5, "-10");


	add_cartographer_label(CMLabelMenuId_EditFPS, 0xFFFFFFF0, "Edit FPS Limit");
	add_cartographer_label(CMLabelMenuId_EditFPS, 0xFFFFFFF1, "Use the buttons below to modify the FPS limit of Halo 2.");
	add_cartographer_label(CMLabelMenuId_EditFPS, 1, "+10");
	add_cartographer_label(CMLabelMenuId_EditFPS, 2, "+1");
	add_cartographer_label(CMLabelMenuId_EditFPS, 0xFFFF0003, "FPS Limit: %d");
	add_cartographer_label(CMLabelMenuId_EditFPS, 0xFFFF0013, "Xlive FPS Limiter Disabled");
	add_cartographer_label(CMLabelMenuId_EditFPS, 4, "-1");
	add_cartographer_label(CMLabelMenuId_EditFPS, 5, "-10");

	add_cartographer_label(CMLabelMenuId_EditHz, 0xFFFFFFF0, "Force a refresh rate");
	add_cartographer_label(CMLabelMenuId_EditHz, 0xFFFFFFF1, "Use the buttons below to force your refresh rate on startup. Requires a restart");
	add_cartographer_label(CMLabelMenuId_EditHz, 1, "+10");
	add_cartographer_label(CMLabelMenuId_EditHz, 2, "+1");
	add_cartographer_label(CMLabelMenuId_EditHz, 0xFFFF0003, "Refresh Rate Value: %d");
	add_cartographer_label(CMLabelMenuId_EditHz, 0xFFFF0013, "No Refresh Rate Forced");
	add_cartographer_label(CMLabelMenuId_EditHz, 4, "-1");
	add_cartographer_label(CMLabelMenuId_EditHz, 5, "-10");


	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 0xFFFFFFF0, "Static Model Level of Detail");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 0xFFFFFFF1, "Use the buttons below to set a static level on a model's Level of Detail.");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 1, "Default");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 2, "L1 - Very Low");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 3, "L2 - Low");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 4, "L3 - Medium");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 5, "L4 - High");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 6, "L5 - Very High");
	add_cartographer_label(CMLabelMenuId_EditStaticLoD, 7, "L6 - Cinematic");
	

	add_cartographer_label(CMLabelMenuId_EditCrosshairSize, 0xFFFFFFF0, "Crosshair Settings");
	add_cartographer_label(CMLabelMenuId_EditCrosshairSize, 0xFFFFFFF1, "Use the buttons below to set a preset crosshair size. Use the config file to modify crosshairs in more detail.");
	add_cartographer_label(CMLabelMenuId_EditCrosshairSize, 1, "Default");
	add_cartographer_label(CMLabelMenuId_EditCrosshairSize, 2, "Disabled");
	add_cartographer_label(CMLabelMenuId_EditCrosshairSize, 3, "Very Small");
	add_cartographer_label(CMLabelMenuId_EditCrosshairSize, 4, "Small");
	add_cartographer_label(CMLabelMenuId_EditCrosshairSize, 5, "Large");
	

	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFFF0, "Update");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFFF1, "Update Project Cartographer.");
	add_cartographer_label(CMLabelMenuId_Update, 1, (char*)0, true);
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFF0001, "Check for Updates");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFF01, "Checking For Updates...");
	add_cartographer_label(CMLabelMenuId_Update, 2, (char*)0, true);
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFF0002, "Download Updates");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFF02, "Downloading Updates...");
	add_cartographer_label(CMLabelMenuId_Update, 3, (char*)0, true);
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFF0003, "Install Updates");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFF03, "Installing Updates...");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFF003, "Failed to run updater app!");
	add_cartographer_label(CMLabelMenuId_Update, 4, "Cancel");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFFF2, "Download the following:\n");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFFF3, "Install the following:\n");
	add_cartographer_label(CMLabelMenuId_Update, 0xFFFFFFF4, "Up to date!");


	add_cartographer_label(CMLabelMenuId_Update_Note, 0xFFFFFFF0, "Outdated Version!");
	add_cartographer_label(CMLabelMenuId_Update_Note, 0xFFFFFFF1, "You are using an outdated version of Project Cartographer! Would you like to go install the latest version?");
	add_cartographer_label(CMLabelMenuId_Update_Note, 1, "Yes");
	add_cartographer_label(CMLabelMenuId_Update_Note, 2, "No");


	add_cartographer_label(CMLabelMenuId_Login_Warn, 0xFFFFFFF0, "Message of the Day!");
	add_cartographer_label(CMLabelMenuId_Login_Warn, 0xFFFFFFF1, "DO NOT CHEAT ONLINE!\r\nRanks are on the way!\r\nInitial testing phase is beginning now.\r\nNew variant OGH2, try it out!");


	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFFFFF0, "Customise HUD / GUI");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFFFFF1, "Customise your heads up display and user interface with the following settings.");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFFFFF2, "Enable %s");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFFFFF3, "Disable %s");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFFFFF4, "Show %s");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFFFFF5, "Hide %s");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 1, "> Player FOV");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 2, "> Vehicle FOV");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 3, "> Crosshair Offset");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 4, "> Crosshair Size");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFF0005, "Ingame Chat");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFF0006, "HUD");
	add_cartographer_label(CMLabelMenuId_EditHudGui, 0xFFFF0007, "First Person Model");


	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFFFFF0, "Toggle Skulls");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFFFFF1, "Enable and disable Halo 2's Skulls.\r\nWhat ice cream flavor would you like to choose today?");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFFFFF2, "Enable %s");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFFFFF3, "Disable %s");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFFFFF4, "???");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0000, "Anger");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0001, "Assassins");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0002, "Black Eye");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0003, "Blind");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0004, "Catch");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0005, "Envy");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0006, "Famine");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0007, "Ghost");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0008, "Grunt Birthday Party");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF0009, "Iron");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF000A, "IWHBYD");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF000B, "Mythic");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF000C, "Sputnik");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF000D, "Thunderstorm");
	add_cartographer_label(CMLabelMenuId_ToggleSkulls, 0xFFFF000E, "Whuppopotamus");


	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF0, "Other Settings");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF1, "Customise other settings and features of Halo 2 / Project Cartographer.");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF2, "Enable %s");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF3, "Disable %s");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF4, "Show %s");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF5, "Hide %s");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF6, "Play %s");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFFFFF7, "Skip %s");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 1, "> FPS Limit");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 2, "> Static Model LoD");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 3, "> Refresh Rate");
//	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFF0003, "Controller Aim-Assist");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFF0004, "Discord Rich Presence");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFF0005, "xDelay");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFF0006, "Game Intro Video");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFF0007, "In-game Keyb. CTRLs");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFF0008, "Raw Mouse Input");
	add_cartographer_label(CMLabelMenuId_OtherSettings, 0xFFFF0009, "Hi Res Fix");

	add_cartographer_label(CMLabelMenuId_AdvSettings, 0xFFFFFFF0, "Advanced Settings");
	add_cartographer_label(CMLabelMenuId_AdvSettings, 0xFFFFFFF1, "Alter additional settings for the game.");
	add_cartographer_label(CMLabelMenuId_AdvSettings, 1, "Change Language");
	add_cartographer_label(CMLabelMenuId_AdvSettings, 2, "Customise HUD/GUI");
	add_cartographer_label(CMLabelMenuId_AdvSettings, 3, "Other Settings");
	add_cartographer_label(CMLabelMenuId_AdvSettings, 4, "Toggle Skulls");
//	add_cartographer_label(CMLabelMenuId_AdvSettings, 5, "Extra Game Settings");


//	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF0, "Extra Game Settings");
//	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF1, "Customise the game with some extra features / hacks.");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF2, "Enable %s");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF3, "Disable %s");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF4, "Show %s");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF5, "Hide %s");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF6, "Play %s");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFFFFF7, "Skip %s");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0002, "Vehicle Flip Eject");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0003, "Kill Volumes");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0004, "MP Explosion Physics");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0005, "MP Sputnik");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0006, "MP Grunt B-Day Party");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0007, "Grenade Chain React");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0008, "Banshee Bomb");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF0009, "HUD");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF000A, "First Person Model");
	add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 0xFFFF000B, "Flashlight");
	//add_cartographer_label(CMLabelMenuId_AdvLobbySettings, 7, "Zombie Movement Speed");


	add_cartographer_label(CMLabelMenuId_Credits, 0xFFFFFFF0, "Credits");
	add_cartographer_label(CMLabelMenuId_Credits, 0xFFFFFFF1, "Praise the Following.");
	add_cartographer_label(CMLabelMenuId_Credits, 1, "--- The Devs ---");
	add_cartographer_label(CMLabelMenuId_Credits, 2, "PermaNull");
	add_cartographer_label(CMLabelMenuId_Credits, 3, "Glitchy Scripts");
	add_cartographer_label(CMLabelMenuId_Credits, 4, "Himanshu01");
	add_cartographer_label(CMLabelMenuId_Credits, 5, "Hootspa");
	add_cartographer_label(CMLabelMenuId_Credits, 6, "NukeULater");
	add_cartographer_label(CMLabelMenuId_Credits, 7, "Num005");
	add_cartographer_label(CMLabelMenuId_Credits, 8, "Rude Yoshi");
	add_cartographer_label(CMLabelMenuId_Credits, 9, "Supersniper");
	add_cartographer_label(CMLabelMenuId_Credits, 10, "--- Content Creators ---");
	add_cartographer_label(CMLabelMenuId_Credits, 11, "General_101");
	add_cartographer_label(CMLabelMenuId_Credits, 12, "--- Additional 7hanks ---");
	add_cartographer_label(CMLabelMenuId_Credits, 13, "Project Cartographer Staff");
	add_cartographer_label(CMLabelMenuId_Credits, 14, "Dev Preview Members");
	add_cartographer_label(CMLabelMenuId_Credits, 15, "And the many many more");
	add_cartographer_label(CMLabelMenuId_Credits, 16, "from the Halo 2 Community!");


	add_cartographer_label(CMLabelMenuId_Invalid_Login_Token, 0xFFFFFFF0, "Invalid Login Token!");
	add_cartographer_label(CMLabelMenuId_Invalid_Login_Token, 0xFFFFFFF1, "Login Again.");


	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFFFF0, "Create Account");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFFFF1, "Enter a username, email and password for your new account.");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFFFF2, "[Username]");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFFFF3, "[Email]");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFFFF4, "[Password]");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFF002, "Create a Username");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFF003, "Create the Username for your new account below.\r\nHowever beware! Not all special characters are supported!");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFF004, "Enter an Email Address");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFF005, "Enter the Email Address to be linked to your new account below.");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFF006, "Create a Password");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 0xFFFFF007, "Create the Password for your new account below.");
	add_cartographer_label(CMLabelMenuId_AccountCreate, 1, XUSER_NAME_SIZE * sizeof(wchar_t), true); // if the buffer is going to be used in Virtual Keyboard, allocate wide string buffer (tho mixing multibyte with wide strings is kinda retarded)
	add_cartographer_label(CMLabelMenuId_AccountCreate, 2, 256 * sizeof(wchar_t), true);
	add_cartographer_label(CMLabelMenuId_AccountCreate, 3, 256 * sizeof(wchar_t), true);
	add_cartographer_label(CMLabelMenuId_AccountCreate, 4, "Create Account");


	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFFFF0, "Add Account");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFFFF1, "Enter your account's Username\r\n[or Email] and Password to Login.");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFFFF2, "-Remember me");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFFFF3, "-Don't remember me");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFFFF4, "[Username]");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFFFF5, "[Password]");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFF002, "Enter Account Username or Email Address");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFF003, "Enter the Username or Email Address of your account below.");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFF004, "Enter Account Password");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 0xFFFFF005, "Enter the Password of your account below.");
	add_cartographer_label(CMLabelMenuId_AccountEdit, 1, 256 * sizeof(wchar_t), true);
	add_cartographer_label(CMLabelMenuId_AccountEdit, 2, 256 * sizeof(wchar_t), true);
	add_cartographer_label(CMLabelMenuId_AccountEdit, 4, "Login");


	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFFFFF0, "Online Accounts");
	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFFFFF1, "Select an Account to Sign in to or use options to create/add/remove them.");
	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFF0000, ">Add Account");
	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFF0001, "-Remove Account");
	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFF0002, "-Cancel Remove");
	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFF0003, ">Create Account");
	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFF0004, ">Play Offline");
	add_cartographer_label(CMLabelMenuId_AccountList, 0xFFFF0005, "<Unnamed>");


	add_cartographer_label(CMLabelMenuId_Guide, 0xFFFFFFF0, "Project Cartographer Guide");
	add_cartographer_label(CMLabelMenuId_Guide, 0xFFFFFFF2, "Press the %s Key to open this guide from anywhere.");
	add_cartographer_label(CMLabelMenuId_Guide, 1, "Advanced Settings");
	add_cartographer_label(CMLabelMenuId_Guide, 2, "Website");
	add_cartographer_label(CMLabelMenuId_Guide, 3, "Credits");
	add_cartographer_label(CMLabelMenuId_Guide, 4, "Update");


#pragma endregion
	
	if (H2IsDediServer)
		return;

	CMSetupVFTables_Obscure();

	setupSomeTests();

	pbtnHandler = (tbtnhandler)DetourClassFunc((BYTE*)H2BaseAddr + 0x213af2, (BYTE*)BtnHandlerCaller, 8);

	psub_23f6b7 = (tsub_23f6b7)DetourFunc((BYTE*)H2BaseAddr + 0x23f6b7, (BYTE*)sub_23f6b7, 7);

	//psub_248beb = (tsub_248beb)DetourClassFunc((BYTE*)H2BaseAddr + 0x248beb, (BYTE*)sub_248beb, 8);

	psub_bd137 = (tsub_bd137)DetourFunc((BYTE*)H2BaseAddr + 0xbd137, (BYTE*)sub_bd137, 5);

	RefreshToggleIngameKeyboardControls();

	CMSetupVFTables_EscSettings();
	
	CMSetupVFTablesVKb_VKeyTest();

	CMSetupVFTables_Error_Inner();
	CMSetupVFTables_Error_Outer();
	
	CMSetupVFTables_Language_Sub();
	CMSetupVFTables_Language();

	CMSetupVFTables_EditCrosshair();

	CMSetupVFTables_EditFOV();

	CMSetupVFTables_EditVehicleFOV();

	CMSetupVFTables_EditFPS();

	CMSetupVFTables_EditHz();

	CMSetupVFTables_EditStaticLoD();

	CMSetupVFTables_EditCrosshairSize();

	CMSetupVFTables_Update();

	CMSetupVFTables_Update_Note();

	CMSetupVFTables_Login_Warn();

	CMSetupVFTables_EditHudGui();

	CMSetupVFTables_ToggleSkulls();

	CMSetupVFTables_OtherSettings();

	CMSetupVFTables_AdvSettings();

	CMSetupVFTables_AdvLobbySettings();

	CMSetupVFTables_Credits();

	CMSetupVFTables_Invalid_Login_Token();

	CMSetupVFTables_AccountCreate();

	CMSetupVFTables_AccountEdit();

	CMSetupVFTables_AccountList();

	CMSetupVFTables_Guide();
	
	c_screen_with_menu::applyPatches();
}

void deinitGSCustomMenu() {
	if (H2IsDediServer)
		return;

}

#pragma region CM_Init_Runtime

/*
int aaa2 = 1;
int aaa3 = 5;
int aaa4 = 4;//this config works ingame and at main-mainmenu

int aab2 = -1;
int aab3 = 3;//this value works for some other menu positions in mainmenu
int aab4 = 4;
*/


void CallWgit(int WgitScreenfunctionPtr) {
	CallWgit(WgitScreenfunctionPtr, 1, 0);
}
std::chrono::time_point<std::chrono::high_resolution_clock> lastOuterMenuUse;
int lastOuterMenuFuncPtr = 0;
void CallWgit(int WgitScreenfunctionPtr, int open_method2) {
	CallWgit(WgitScreenfunctionPtr, open_method2, 0);
}
int prevOpenMethod = 3;
//bool hacked21 = false;
void CallWgit(int WgitScreenfunctionPtr, int open_method2, int menu_wgit_type) {
	//int(__thiscall*WgitInitialize)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B0BC);
	signed int(__thiscall*WgitLoad)(void*, __int16, int, int, int) = (signed int(__thiscall*)(void*, __int16, int, int, int))((char*)H2BaseAddr + 0x20C226);
	//0x0020C258 is another one.
	//void*(__thiscall*WgitFinalize)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B11E);

	int open_method = open_method2;
	if (open_method == 1) {
		open_method = prevOpenMethod;
	}
	else if (open_method == 2) {
		int CurrentWgitID = *(int*)((BYTE*)H2BaseAddr + 0x9758D8);
		if (menu_wgit_type == 0) {
			open_method = 3;
		}
		else if (lastOuterMenuFuncPtr > 0 && lastOuterMenuFuncPtr == WgitScreenfunctionPtr) {
			if (CurrentWgitID != menu_wgit_type) {
				std::chrono::milliseconds difference = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastOuterMenuUse);
				long long timeDiff = difference.count();
				if (timeDiff < 1500) {
					open_method = 3;
					//hacked21 = true;
				}
			}
			//else if (hacked21) {
			//	return;
			//}
		}
	}
	lastOuterMenuFuncPtr = WgitScreenfunctionPtr;
	lastOuterMenuUse = std::chrono::high_resolution_clock::now();
	prevOpenMethod = open_method;

	//char* menu_setup = (char*)malloc(sizeof(char) * 0x20);
	//WgitInitialize(menu_setup);
	DWORD menu_setup[8];
	menu_setup[3] = 0;

	switch (open_method) {
	case 3:
		WgitLoad(menu_setup, 1, 3, 4, WgitScreenfunctionPtr);
		break;
	case 0:
	default:
		WgitLoad(menu_setup, 1, 5, 4, WgitScreenfunctionPtr);
	}

	void*(__cdecl *MenuHeadSetup)(DWORD*) = (void*(__cdecl*)(DWORD*))menu_setup[7];
	void* menu_struct = MenuHeadSetup(menu_setup);

	//void* menu_struct = WgitFinalize(menu_setup);

	//free(menu_setup);
}

void CMSetupVFTables(DWORD** menu_vftable_1, DWORD** menu_vftable_2, DWORD CM_LabelButtons, DWORD sub_2111ab_CMLTD_nak, DWORD CM_FuncPtrHelper, DWORD CM_ButtonPreselection, bool isInternalMenuWindow, DWORD sub_248beb_nak_deconstructor) {

	//clone a brightness menu_vftable_1
	*menu_vftable_1 = (DWORD*)malloc(0x98);
	memcpy(*menu_vftable_1, (BYTE*)H2BaseAddr + 0x3d96fc, 0x98);//Brightness

	//Button Labels
	if (CM_LabelButtons)
		*(DWORD*)((DWORD)*menu_vftable_1 + 0x90) = (DWORD)CM_LabelButtons;


	//clone a brightness menu_vftable_2
	*menu_vftable_2 = (DWORD*)malloc(0xA8);
	memcpy(*menu_vftable_2, (BYTE*)H2BaseAddr + 0x3D7ABC, 0xA8);//Brightness

	if (sub_248beb_nak_deconstructor)
		*(DWORD*)((DWORD)*menu_vftable_2 + 0x0) = (DWORD)sub_248beb_nak_deconstructor;

	if (isInternalMenuWindow) {
		//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24DC0D;//inner ingame
		//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x20EEBE;//both main-mainmenu
		*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)CM_keyHandler_inner_nak;
	}
	else {
		//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x23D8AE;//outer ingame
		//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x20EEBE;//both main-mainmenu
		*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)CM_keyHandler_outer_nak;

		//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)sub_23D8AE_CM_nak;

		//*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x212CD8;//outer main-mainmenu?
		//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24fe49;//gameoptions
	}
	//i have a feeling it could be because 1,3,4 doesn't work. need 1, 5, 4 somehow.

	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24DC0D;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x20EEBE;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x23D8AE;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x23E76C;//same as 0x23D8AE

	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24FE49;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0xD04C;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x21FCEE;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x23D060;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x23F0BA;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x2415C4;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x245BB1;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x246B55;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24A5D9;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x24C714;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x2531AC;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x25C9E8;
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0x30) = (DWORD)H2BaseAddr + 0x25D2D0;
	

	//*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x212CD8;//text related?
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x23D64E;//text related?
	//*(DWORD*)((DWORD)*menu_vftable_2 + 0xC) = (DWORD)H2BaseAddr + 0x24f0eb;//gameoptions breaks text

	//CALL for Title & Description
	if (sub_2111ab_CMLTD_nak)
		*(DWORD*)((DWORD)*menu_vftable_2 + 0x58) = (DWORD)sub_2111ab_CMLTD_nak;
	//Menu Button Preselected Option
	if (CM_ButtonPreselection)
		*(DWORD*)((DWORD)*menu_vftable_2 + 0x5C) = (DWORD)CM_ButtonPreselection;
	//Menu function pointer helper
	if (CM_FuncPtrHelper)
		*(DWORD*)((DWORD)*menu_vftable_2 + 0x98) = (DWORD)CM_FuncPtrHelper;
}

int __stdcall BtnHandlerCaller(void* thisptr, int a2, int a3) {
	int result = 0;//doesn't /always/ matter
	//addDebugText("Hooked Button Press Handler.");
	//result = pbtnHandler(thisptr, a2, a3);
	int firstFuncPtr = *(int*)(*(DWORD*)thisptr - 4);
	if (!firstFuncPtr) {
		bool(__cdecl* customHandlerFunc)(int) = (bool(__cdecl*)(int))(*(int*)(*(DWORD*)thisptr + 16));
		int button_id = *(int*)a3 & 0xFFFF;
		bool success = customHandlerFunc(button_id);

		if (success) {
			int v3 = ((int)thisptr) - 0xac;
			int v4 = (*(int(__thiscall**)(int))(*(DWORD*)v3 + 56))(v3);//__thiscall
			int v5 = (*(int(__thiscall**)(int))(*(DWORD*)v3 + 52))(v3);//__thiscall

			int(__cdecl* sub_5A96DA)(int a1, int a2);
			sub_5A96DA = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x2096DA);
			result = sub_5A96DA(v5, v4);
		}
	}
	else {
		for (int i = *(DWORD*)thisptr; i; i = *(DWORD*)(i + 4)) {
			result = (**(int(__thiscall ***)(DWORD, DWORD, DWORD))(i - 4))(i - 4, a2, a3);
		}
	}
	return result;
}

int __stdcall sub_20F790_CM(int thisptr, __int16 selected_button_id)
{
	int(__thiscall* sub_213F50)(int, __int16) = (int(__thiscall*)(int, __int16))((char*)H2BaseAddr + 0x213F50);
	int(__thiscall* sub_20F790)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x20F790);

	sub_213F50(thisptr + 2656, selected_button_id);
	return sub_20F790(thisptr);
}

int __cdecl sub_250E22_CM(int thisptr, int a2, DWORD* menu_vftable_1, DWORD menu_button_handler, int number_of_buttons)
{
	void*(__thiscall* sub_213B1C)(int, int) = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x213B1C);
	void*(__thiscall* sub_21FFC9)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x21FFC9);
	void(__stdcall* sub_28870B)(int, int, int, void(__thiscall*)(DWORD), int) = (void(__stdcall*)(int, int, int, void(__thiscall*)(DWORD), int))((char*)H2BaseAddr + 0x28870B);
	int(__thiscall* sub_2113C6)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x2113C6);
	int(__cdecl* sub_20D1FD)(char*, int numberOfButtons, int) = (int(__cdecl*)(char*, int, int))((char*)H2BaseAddr + 0x20D1FD);
	int(__cdecl* sub_66B33)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x66B33);
	int(__cdecl* sub_667A0)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x667A0);
	void*(__thiscall* sub_2113D3)(int, int) = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2113D3);

	int v5; // eax@4
	int v2 = thisptr;

	sub_213B1C(thisptr, a2);

	//cloned vftable
	*(DWORD*)v2 = (DWORD)menu_vftable_1 + 0x30;

	int(__thiscall* sub_E5A551)(void* thisptr) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0xA551);

	sub_28870B((int)((char*)v2 + 176), 132, 4, (void(__thiscall*)(DWORD))sub_21FFC9, (int)sub_E5A551);

	*((BYTE*)v2 + 704) = 0;

	sub_2113C6((int)((char*)v2 + 712));

	//*((DWORD*)v2 + 177) = &off_1229700;
	*((DWORD*)v2 + 177) = 0;

	*((DWORD*)v2 + 181) = (DWORD)v2;

	*((DWORD*)v2 + 182) = (DWORD)menu_button_handler;
	DWORD* safds = ((DWORD*)v2 + 182);

	char editliststr[] = "brightness level settings edit list";

	int v3 = sub_20D1FD(editliststr, number_of_buttons, 4);

	*((DWORD*)v2 + 28) = v3;

	sub_66B33(v3);

	for (unsigned int i = 0; i < *(DWORD*)(*((DWORD*)v2 + 28) + 32); ++i) {
		sub_667A0(*((DWORD*)v2 + 28));
	}
	if (v2 == -708) {
		v5 = 0;
	}
	else {
		v5 = v2 + 712;
	}

	sub_2113D3(v2 + 172, v5);

	return (int)v2;
}

int __cdecl sub_248B17_CM(int thisptr, int a2, int a3, int a4, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type)
{
	void*(__thiscall* sub_211159)(void*, int wgit_id, int, int, int, int) = (void*(__thiscall*)(void*, int, int, int, int, int))((char*)H2BaseAddr + 0x211159);
	//int(__thiscall* sub_250E22)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x250E22);

	//The GUI type and the Labels on it are pre-written from that WgitID
	//272, 259
	sub_211159((void*)thisptr, menu_wgit_type, a2, a3, a4, thisptr + 2656);

	*(DWORD*)thisptr = (DWORD)menu_vftable_2;

	sub_250E22_CM(thisptr + 2656, a4, menu_vftable_1, menu_button_handler, number_of_buttons);

	return thisptr;
}

int __cdecl CustomMenu_CallHead(int a1, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type)
{
	int(__cdecl* Allocator)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	int(__cdecl* sub_20B8C3)(int, int) = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x20B8C3);

	int menu_struct = Allocator(3388);
	int menu_id = ((int*)menu_struct)[28];
	if (menu_struct) {
		menu_struct = sub_248B17_CM(menu_struct, *(DWORD*)(a1 + 4), *(DWORD*)(a1 + 8), *(WORD*)(a1 + 2), menu_vftable_1, menu_vftable_2, menu_button_handler, number_of_buttons, menu_wgit_type);
	}
	*(BYTE *)(menu_struct + 0x6C) = 1;
	//*(BYTE *)(menu_struct + 0xd20) = 1;
	sub_20B8C3(menu_struct, a1);

	return menu_struct;
}

#pragma region CM_TestHead_EscSettings

int __stdcall sub_24E330(int thisptr, int a2, DWORD* menu_vftable_1, DWORD menu_button_handler, int number_of_buttons)
{
	void*(__thiscall* sub_213B1C)(int, int) = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x213B1C);
	void*(__thiscall* sub_21FFC9)(void*) = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x21FFC9);
	void(__stdcall* sub_28870B)(int, int, int, void(__thiscall*)(DWORD), int) = (void(__stdcall*)(int, int, int, void(__thiscall*)(DWORD), int))((char*)H2BaseAddr + 0x28870B);
	int(__thiscall* sub_2113C6)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x2113C6);
	int(__cdecl* sub_20D1FD)(char*, int numberOfButtons, int) = (int(__cdecl*)(char*, int, int))((char*)H2BaseAddr + 0x20D1FD);
	int(__cdecl* sub_66B33)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x66B33);
	int(__cdecl* sub_667A0)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x667A0);
	void*(__thiscall* sub_2113D3)(int, int) = (void*(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2113D3);

	int(__thiscall* sub_A551)(int thisptr) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0xA551);

	int(__thiscall* sub_24E1AA)(int thisptr, int a2, int* a3) = (int(__thiscall*)(int, int, int*))((char*)H2BaseAddr + 0x24E1AA);
	int(__thiscall* sub_250DD8)(int thisptr, int a2, int* a3) = (int(__thiscall*)(int, int, int*))((char*)H2BaseAddr + 0x250DD8);

	void* v2; // esi@1
	int v3; // ebp@1
	int v4; // eax@1

	v2 = (void*)thisptr;
	sub_213B1C(thisptr, a2);
	v3 = 0;

	//*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D8EC4);
	//*(DWORD*)v2 = (DWORD)((char*)H2BaseAddr + 0x3D972C);
	*(DWORD*)v2 = (DWORD)menu_vftable_1 + 0x30;

	sub_28870B((int)((char*)v2 + 176), 132, 5, (void(__thiscall*)(DWORD))sub_21FFC9, (int)sub_A551);
	sub_2113C6((int)((char*)v2 + 840));
	//*((DWORD*)v2 + 209) = (DWORD)((char*)H2BaseAddr + 0x3D8D54);
	*((DWORD*)v2 + 209) = (DWORD)((char*)H2BaseAddr + 0x3D9700);
	//*((DWORD*)v2 + 209) = 0;
	*((DWORD*)v2 + 213) = (int)v2;
	//*((DWORD*)v2 + 214) = (DWORD)sub_24E1AA;
	*((DWORD*)v2 + 214) = (DWORD)sub_250DD8;
	//*((DWORD*)v2 + 214) = (DWORD)menu_button_handler;
	v4 = sub_20D1FD("pause settings list", number_of_buttons, 4);
	*((DWORD*)v2 + 28) = v4;
	sub_66B33(v4);
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 0;
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 1;
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 2;
	*(WORD*)(*(DWORD*)(*((DWORD*)v2 + 28) + 68) + 4 * (unsigned __int16)sub_667A0(*((DWORD*)v2 + 28)) + 2) = 3;
	if (v2 != (void*)-836)
		v3 = (int)((char*)v2 + 840);
	sub_2113D3((int)v2 + 172, v3);
	return (int)v2;
}

int __cdecl sub_248B17_CM2(int thisptr, int a2, int a3, int a4, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type)
{
	void*(__thiscall* sub_211159)(void*, int wgit_id, int, int, int, int) = (void*(__thiscall*)(void*, int, int, int, int, int))((char*)H2BaseAddr + 0x211159);
	//int(__thiscall* sub_250E22)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x250E22);

	//The GUI type and the Labels on it are pre-written from that WgitID
	//272
	sub_211159((void*)thisptr, menu_wgit_type, a2, a3, a4, thisptr + 2656);

	*(DWORD*)thisptr = (DWORD)menu_vftable_2;

	sub_24E330(thisptr + 2656, a4, menu_vftable_1, menu_button_handler, number_of_buttons);

	return thisptr;
}

int __cdecl CustomMenu_CallHead2(int a1, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type)
{
	int(__cdecl* Allocator)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	int(__cdecl* sub_20B8C3)(int, int) = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x20B8C3);

	//int(__thiscall* sub_23DDB4)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x23DDB4);

	int v3 = Allocator(3516);
	if (v3) {
		int v2 = (int)sub_248B17_CM2(v3, *(DWORD*)(a1 + 4), *(DWORD*)(a1 + 8), *(WORD*)(a1 + 2), menu_vftable_1, menu_vftable_2, menu_button_handler, number_of_buttons, menu_wgit_type);
		*(BYTE*)(v2 + 108) = 1;
		sub_20B8C3(v2, a1);
		return v2;
	}
	return 0;
}

#pragma endregion
#pragma endregion

#pragma region CMLTD - CustomMenuLabelTitleDescription

void __cdecl sub_3e3ac_CMLTD(int a1, int label_id, wchar_t* rtn_label, int label_menu_id)
{
	int(__cdecl* sub_381fd)() = (int(__cdecl*)())((char*)H2BaseAddr + 0x381fd);
	void(__thiscall* sub_3e332)(int, int, wchar_t*, int, int) = (void(__thiscall*)(int, int, wchar_t*, int, int))((char*)H2BaseAddr + 0x3e332);

	if (a1 != -1) {
		int v3 = sub_381fd();
		char* v4 = &tags::get_tag_data()[tags::get_tag_instances()[a1 & 0xFFFF].data_offset];

		sub_3e332(
			(int)tags::get_matg_globals_ptr() + 28 * (v3 + 14),
			label_id,
			rtn_label,
			label_menu_id,//*(WORD*)(v4 + 4 * (v3 + 14) - 40),
			*(WORD*)(v4 + 4 * (v3 + 14) - 38));
	}
}

void __stdcall sub_2101a4_CMLTD(int thisptr, int label_id, wchar_t* rtn_label, int label_menu_id)
{
	int(__cdecl* sub_20c701)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20c701);
	int(__cdecl* sub_15C9623)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x239623);
	//void(__cdecl* sub_3e3ac)(int, int, wchar_t*) = (void(__cdecl*)(int, int, wchar_t*))((char*)H2BaseAddr + 0x3e3ac);

	*rtn_label = 0;
	if (label_id != -1) {
		int menu_id = *(DWORD*)(thisptr + 112);
		int v3 = sub_20c701(menu_id);
		if (v3 != -1) {
			int v4 = sub_15C9623(v3);
			if (v4) {
				sub_3e3ac_CMLTD(*(DWORD*)(v4 + 28), label_id, rtn_label, label_menu_id);
			}
		}
	}
}

char __stdcall sub_21bb0b_CMLTD(void* thisptr, __int16 a2, int* aa3, int label_menu_id, int label_id_description)
{
	int(__thiscall* sub_211BA1)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x211BA1);
	//void(__thiscall* sub_2101a4)(int, int, wchar_t*) = (void(__thiscall*)(int, int label_id, wchar_t* rtn_label))((char*)H2BaseAddr + 0x2101A4);
	int(__thiscall* sub_2116D2)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2116D2);
	int(__thiscall* sub_22DF53)(void*, wchar_t*, int, int, int, __int16, int, int, int) = (int(__thiscall*)(void*, wchar_t*, int, int, int, __int16, int, int, int))((char*)H2BaseAddr + 0x22DF53);
	int(*sub_20BB89)() = (int(*)())((char*)H2BaseAddr + 0x20BB89);
	char(__cdecl* sub_99D1F)(int, int) = (char(__cdecl*)(int, int))((char*)H2BaseAddr + 0x99D1F);

	int a3 = (int)aa3;
	int v3; // esi@1
	int v4; // eax@1
	int v5; // edx@1
	__int16 v6; // bx@1
	bool v7; // zf@1
	int v8; // ebp@6
	int v9; // ecx@8
	int v10; // edx@8
	void *v11; // eax@8
	int v12; // edi@9
	__int16 v13; // bx@9
	int v14; // ST30_4@10
	int v15; // eax@12
	int v16; // eax@12
	int v18; // [sp+10h] [bp-414h]@2
	int v19[3]; // [sp+14h] [bp-410h]@8
	wchar_t v20[512]; // [sp+20h] [bp-404h]@1

	v3 = (int)thisptr;
	v4 = sub_211BA1((int)thisptr);
	*(WORD *)(v3 + 10) = a2;
	*(DWORD *)(v3 + 112) = *(WORD *)(a3 + 6);
	v5 = *(DWORD *)a3;
	v6 = 0;
	v7 = (*(DWORD *)a3 & 1) == 0;
	v20[0] = 0;
	if (v7)
		v18 = 2 - ((v5 & 2) != 0);
	else
		v18 = 0;
	if (v5 & 4)
		v6 = 1;
	*(WORD *)(v3 + 104) = *(WORD *)(a3 + 4) - 1;
	*(WORD *)(v3 + 106) = *(WORD *)(a3 + 40);
	v8 = 1;
	if (!(*(BYTE *)a3 & 8))
		v8 = *(WORD *)(a3 + 10);
	v9 = *(DWORD *)(a3 + 20);
	v19[0] = *(DWORD *)(a3 + 16);
	v10 = *(DWORD *)(a3 + 24);
	v19[1] = v9;
	v19[2] = v10;
	//sub_2101a4(v4, *(DWORD *)(a3 + 36), v20);//description label_id 2nd par
	sub_2101a4_CMLTD(v4, label_id_description, v20, label_menu_id);
	sub_2116D2(v3, (int)&aa3[7]);
	v11 = (void *)(*(int(__thiscall **)(int))(*(DWORD *)v3 + 76))(v3);
	sub_22DF53(v11, v20, -1, v8, (int)v19, v6, -1, v18, -1);
	if (*(BYTE *)a3 & 8)
	{
		v12 = sub_20BB89();
		v13 = *(WORD *)((*(int(__thiscall **)(int))(*(DWORD *)v3 + 76))(v3) + 20) | 4;
		*(WORD *)((*(int(__thiscall **)(DWORD))(*(DWORD *)v3 + 76))(v3) + 20) = v13;
		if (v12)
		{
			v14 = *(DWORD *)(v12 + 76);
			*(DWORD *)((*(int(__thiscall **)(DWORD))(*(DWORD *)v3 + 76))(v3) + 32) = v14;
		}
		*(WORD *)((*(int(__thiscall **)(DWORD))(*(DWORD *)v3 + 76))(v3) + 60) = 0;
	}
	v15 = (*(int(__thiscall **)(DWORD))(*(DWORD *)v3 + 76))(v3);
	v16 = (*(int(__thiscall **)(int))(*(DWORD *)v15 + 12))(v15);
	return sub_99D1F(v16, v8);
}

void* __stdcall sub_20f8ae_CMLTD(void* thisptr, __int16 a2, int* a3, int label_menu_id, int label_id_description)
{
	int(__cdecl* sub_20D2D8)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20D2D8);
	void*(__thiscall* sub_20F576)(void*, int) = (void*(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20F576);
	void*(__thiscall* sub_20F65D)(void*, __int16) = (void*(__thiscall*)(void*, __int16))((char*)H2BaseAddr + 0x20F65D);
	int(__thiscall* sub_21208E)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x21208E);
	//char(__thiscall* sub_21bb0b)(void*, __int16, int*) = (char(__thiscall*)(void*, __int16, int*))((char*)H2BaseAddr + 0x21bb0b);

	void* v3; // edi@1
	int v4; // eax@2
	int v5; // eax@3
	int v6; // eax@4
	void* v7; // esi@7

	v3 = thisptr;
	v5 = 0;
	if (*(BYTE*)a3 & 0x10)
	{
		v4 = sub_20D2D8(252);
		if (v4)
		{
			v5 = (int)sub_20F576((void*)v4, *((WORD*)v3 + 4));
		}
	}
	else
	{
		v6 = sub_20D2D8(1212);
		if (v6)
		{
			v5 = (int)sub_20F65D((void*)v6, *((WORD*)v3 + 4));
		}
	}
	v7 = (void*)v5;
	if (v5)
	{
		*(BYTE*)(v5 + 108) = 1;
		sub_21208E(v3, v5);
		sub_21bb0b_CMLTD(v7, a2, a3, label_menu_id, label_id_description);
	}
	return v7;
}

int __stdcall sub_20fb1b_CMLTD(void* thisptr, int label_menu_id, int label_id_description)
{
	int dword_482290 = *(int*)((char*)H2BaseAddr + 0x482290);

	int(__thiscall* sub_20F815)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x20F815);
	int(__thiscall* sub_20E8C9)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20E8C9);
	int(__thiscall* sub_212604)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x212604);
	int(__thiscall* sub_20E9CE)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20E9CE);
	int(__thiscall* sub_20EA52)(void*, int, int) = (int(__thiscall*)(void*, int, int))((char*)H2BaseAddr + 0x20EA52);
	//void*(__thiscall* sub_20f8ae)(void*, __int16, int*) = (void*(__thiscall*)(void*, __int16, int*))((char*)H2BaseAddr + 0x20f8ae);
	int(__thiscall* sub_20E94D)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20E94D);

	BYTE* v1; // esi@1
	int v2; // ebx@1
	unsigned int v3; // edi@5
	int v4; // ebp@6
	int v5; // ecx@7
	int v6; // eax@7
	int v7; // eax@9
	int v8; // esi@9
	unsigned int v9; // edi@14
	int v10; // ebp@15
	int v11; // ecx@17
	int v12; // eax@17
	int v13; // esi@19
	int v14; // ebp@24
	int v15; // ecx@26
	int v16; // eax@26
	int v17; // ebp@28
	int i; // edi@28
	int v19; // esi@29
	bool v20; // sf@34
	//unsigned __int8 v21; // of@34
	unsigned int v22; // esi@35
	int v23; // edi@36
	int v24; // ecx@37
	int v25; // eax@37
	void* v26; // eax@39
	unsigned int v27; // edi@43
	int v28; // ecx@45
	int v29; // eax@45
	int v30; // eax@47
	int v31; // esi@47
	char v33; // [sp+Bh] [bp-Dh]@3
	BYTE* v34; // [sp+Ch] [bp-Ch]@1
	int v35; // [sp+10h] [bp-8h]@24
	int v36; // [sp+10h] [bp-8h]@44
	int v37; // [sp+14h] [bp-4h]@25

	v1 = (BYTE*)thisptr;
	v34 = (BYTE*)thisptr;
	v2 = sub_20F815((int)thisptr);//thisptr is from beginning :)
	if (v2)
	{
		if ((v1[66] >> 1) & 1 || (v33 = 0, v1[66] & 1))
			v33 = 1;
		v3 = 0;
		if (*(DWORD*)(v2 + 36) > 0)
		{
			v4 = 0;
			do
			{
				v5 = *(DWORD*)(v2 + 40);
				v6 = 0;
				if (v5 != -1)
					v6 = dword_482290 + v5;
				v7 = sub_20E8C9(v1, v4 + v6);
				v8 = v7;
				if (v7)
				{
					if (v33)
						sub_212604(v7, *((DWORD*)v34 + 13));
					*(WORD*)(v8 + 10) = v3;
				}
				v1 = v34;
				++v3;
				v4 += 56;
			} while (v3 < *(DWORD*)(v2 + 36));
		}
		v9 = 0;
		if (*(DWORD*)(v2 + 60) > 0)
		{
			v10 = 0;
			while (1)
			{
				v11 = *(DWORD*)(v2 + 64);
				v12 = 0;
				if (v11 != -1)
					v12 = v11 + dword_482290;
				v13 = sub_20E9CE(v1, v12 + v10);
				if (v13)
				{
					if (v33)
						sub_212604(v13, *((DWORD*)v34 + 13));
					*(WORD*)(v13 + 10) = v9;
				}
				++v9;
				v10 += 36;
				if (v9 >= *(DWORD*)(v2 + 60))
					break;
				v1 = v34;
			}
		}
		v14 = 0;
		v35 = 0;
		if (*(DWORD*)(v2 + 68) > 0)
		{
			v37 = 0;
			bool varesult = 0;
			do
			{
				v15 = *(DWORD*)(v2 + 72);
				v16 = 0;
				if (v15 != -1)
					v16 = dword_482290 + v15;
				v17 = v16 + v14;
				for (i = 0; i < *(BYTE*)(v17 + 17); ++i)
				{
					v19 = sub_20EA52(v34, i, v17);
					if (v19)
					{
						if (v33)
							sub_212604(v19, *((DWORD*)v34 + 13));
						*(WORD*)(v19 + 10) = v35;
					}
				}
				v14 = v37 + 24;

				//varesult = __OFSUB__(v35 + 1, *(DWORD*)(v2 + 68));

				int va1 = v35 + 1;
				int va2 = *(DWORD*)(v2 + 68);
				int vreax;
				varesult = 0;
				__asm {
					//save register
					mov vreax, eax

					mov eax, va1
					cmp eax, va2
					//jump if Overflow Flag is not set.
					JNO toEnd
					mov al, 1
					mov varesult, al

					toEnd :
					//restore original register
					mov eax, vreax
				}

				v20 = v35++ + 1 - *(DWORD*)(v2 + 68) < 0;
				v37 += 24;
			} while (v20 ^ varesult);
		}
		v22 = 0;
		if (*(DWORD*)(v2 + 28) > 0)
		{
			v23 = 0;
			do
			{
				v24 = *(DWORD*)(v2 + 32);
				v25 = 0;
				if (v24 != -1)
					v25 = dword_482290 + v24;
				v26 = sub_20f8ae_CMLTD(v34, v22, (int*)(v23 + v25), label_menu_id, label_id_description);//(int*)(v23 + v25) contains description
				if (v26 && v33)
					sub_212604((int)v26, *((DWORD*)v34 + 13));
				++v22;
				v23 += 44;
			} while (v22 < *(DWORD*)(v2 + 28));
		}
		v27 = 0;
		if (*(DWORD*)(v2 + 44) <= 0)
		{
			v1 = v34;
		}
		else
		{
			v36 = 0;
			do
			{
				v28 = *(DWORD*)(v2 + 48);
				v29 = 0;
				if (v28 != -1)
					v29 = dword_482290 + v28;
				v30 = sub_20E94D(v34, v29 + v36);
				v31 = v30;
				if (v30)
				{
					if (v33)
						sub_212604(v30, *((DWORD*)v34 + 13));
					*(WORD*)(v31 + 10) = v27;
				}
				v36 += 76;
				++v27;
			} while (v27 < *(DWORD*)(v2 + 44));
			v1 = v34;
		}
	}
	return (*(int(__thiscall **)(BYTE*))(*(DWORD*)v1 + 96))(v1);
}

int __stdcall sub_2107df_CMLTD(int thisptr, int* a2, char a3, int label_menu_id, int label_id_description)
{
	int(__cdecl* sub_20C701)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20C701);
	int(__cdecl* sub_239623)(unsigned __int16) = (int(__cdecl*)(unsigned __int16))((char*)H2BaseAddr + 0x239623);
	int(__thiscall* sub_20F815)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x20F815);
	int(__thiscall* sub_21208E)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x21208E);
	int(__thiscall* sub_214990)(void*, char) = (int(__thiscall*)(void*, char))((char*)H2BaseAddr + 0x214990);
	int(__thiscall* sub_2113D3)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x2113D3);
	//int(__thiscall* sub_20fb1b)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x20fb1b);
	int(__cdecl* sub_20F402)(void*, int) = (int(__cdecl*)(void*, int))((char*)H2BaseAddr + 0x20F402);

	int *v3; // edi@1
	int v4; // esi@1
	int result; // eax@2
	int v6; // ebp@4
	int v7; // eax@4
	int v8; // ebx@6
	unsigned int v9; // ebp@9
	int v10; // eax@10
	bool v11; // zf@10
	int *v12; // eax@10
	void *v13; // edi@14
	int v14; // ecx@17
	int v15; // eax@17
	int v16; // ebx@18
	int v17; // ebp@20
	int v18; // ecx@23
	int v19; // [sp+Ch] [bp+4h]@4

	v3 = a2;
	v4 = thisptr;
	if (a3
		|| (result = *(WORD*)(thisptr + 2552), (signed __int16)result >= 0)
		&& (result = (signed __int16)result, (signed __int16)result < a2[1]))
	{
		v6 = 0;
		v19 = 0;
		v7 = sub_20C701(*(DWORD*)(thisptr + 112));
		if (v7 != -1)
		{
			v19 = sub_239623(v7);
			v6 = v19;
		}
		result = sub_20F815(v4);
		v8 = result;
		if (v6)
		{
			if (result)
			{
				*(WORD*)(v4 + 104) = *(WORD*)(result + 2) - 1;
				if (a3)
				{
					v9 = 0;
					if (*(DWORD*)(result + 4) > 0)
					{
						do
						{
							v10 = v3[4 * *(WORD*)(v4 + 2552) + 3];
							v11 = *(DWORD*)(v10 + 4 * v9) == 0;
							v12 = (int*)(v10 + 4 * v9);
							if (v11)
								break;
							sub_21208E((void*)v4, *v12);
							DWORD aa = **(DWORD**)(v3[4 * *(WORD*)(v4 + 2552) + 3] + 4 * v9++) + 84;
							//(*(void(**)(void))(aa))();
							(*(int(__thiscall**)(int))(aa))(*v12);//wasn't able to trigger and check but assume works due to similar case below.
						} while (v9 < *(DWORD*)(v8 + 4));
					}
					if (*(DWORD*)(v8 + 12) > 0)
					{
						sub_21208E((void*)v4, v3[4 * (*(WORD*)(v4 + 2552) + 1)]);
						sub_214990((void*)v3[4 * (*(WORD*)(v4 + 2552) + 1)], a3);
						if (*(BYTE*)v19 & 2)
						{
							*(BYTE*)(v3[4 * (*(WORD*)(v4 + 2552) + 1)] + 166) = 1;
							v13 = (void*)(v3[4 * (*(WORD*)(v4 + 2552) + 1)] + 168);
							if (v4 == -2628)
							{
								sub_2113D3(v13, 0);
								result = sub_20fb1b_CMLTD((void*)0xFFFFF5BC, label_menu_id, label_id_description);
							}
							else
							{
								sub_2113D3(v13, v4 + 2632);
								result = sub_20fb1b_CMLTD((void*)v4, label_menu_id, label_id_description);
							}
							return result;
						}
					}
					return sub_20fb1b_CMLTD((void*)v4, label_menu_id, label_id_description);
				}
				v14 = (int)&v3[4 * *(WORD*)(v4 + 2552)];
				v15 = *(DWORD*)(v14 + 8);
				if (v15 <= 0)
				{
					v18 = *(DWORD*)(v14 + 16);
					if (v18)
					{
						sub_21208E((void*)v4, v18);
						sub_214990((void*)v3[4 * (*(WORD*)(v4 + 2552) + 1)], 0);
						if (*(BYTE*)v6 & 2)
						{
							*(BYTE*)(v3[4 * (*(WORD*)(v4 + 2552) + 1)] + 166) = 1;
							sub_20F402((void *)(v3[4 * (*(WORD*)(v4 + 2552) + 1)] + 168), v4 + 2628);
						}
					}
					return sub_20fb1b_CMLTD((void*)v4, label_menu_id, label_id_description);
				}
				v16 = *(DWORD*)(v8 + 4);
				if (v16 > v15)
					v16 = *(DWORD*)(v14 + 8);
				v17 = 0;
				if (v16 <= 0)
					return sub_20fb1b_CMLTD((void*)v4, label_menu_id, label_id_description);
				do
				{
					sub_21208E((void*)v4, *(DWORD*)(v3[4 * *(WORD*)(v4 + 0x9F8) + 3] + 4 * v17));
					v10 = v3[4 * *(WORD*)(v4 + 2552) + 3];
					DWORD* ab = (DWORD*)(v10 + 4 * v17++);
					if (*ab == 0)
						break;
					DWORD aa = **(DWORD**)ab + 84;
					(*(int(__thiscall**)(int))(aa))(*ab);
				} while (v17 < v16);
				result = sub_20fb1b_CMLTD((void*)v4, label_menu_id, label_id_description);
			}
		}
	}
	return result;
}

void __stdcall sub_21bf85_CMLTD(int thisptr, int label_id, int label_menu_id)
{
	int(__thiscall* sub_391BA1)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x211ba1);
	void(__thiscall* sub_2101a4)(int, int, wchar_t*) = (void(__thiscall*)(int, int, wchar_t*))((char*)H2BaseAddr + 0x2101a4);

	int v2 = thisptr;
	if (label_id != -1) {
		int v3 = sub_391BA1(thisptr);

		if (v3) {
			wchar_t tmp[512];

			sub_2101a4_CMLTD(v3, label_id, tmp, label_menu_id);

			int v4 = (*(int(__thiscall **)(int))(*(DWORD*)v2 + 76))(v2);
			(*(int(__thiscall **)(int, int))(*(DWORD*)v4 + 4))(v4, (int)tmp);
		}
	}
}

char __stdcall sub_20fd41_CMLTD(void* thisptr, int label_menu_id, int label_id_title)
{
	int dword_482290 = *(int*)((char*)H2BaseAddr + 0x482290);

	int(__cdecl* sub_20bb89)() = (int(__cdecl*)())((char*)H2BaseAddr + 0x20bb89);
	int(__cdecl* sub_20c701)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20c701);
	int(__cdecl* sub_239623)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x239623);
	int(__thiscall* sub_21208e)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21208e);
	void(__thiscall* sub_21bf85)(int, int) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);
	int(__cdecl* sub_4bd27)(wchar_t*, wchar_t*, int) = (int(__cdecl*)(wchar_t*, wchar_t*, int))((char*)H2BaseAddr + 0x4bd27);
	char(__thiscall* sub_21bc81)(void*, __int16, __int16, int*, __int16, int*) = (char(__thiscall*)(void*, __int16, __int16, int*, __int16, int*))((char*)H2BaseAddr + 0x21bc81);
	int(__thiscall* sub_21baa9)(DWORD, wchar_t*) = (int(__thiscall*)(DWORD, wchar_t*))((char*)H2BaseAddr + 0x21baa9);

	void* v1; // ebx@1
	int v2; // edi@1
	int v3; // eax@1
	int v4; // ST10_4@3
	int v5; // esi@3
	int v6; // eax@3
	int v7; // esi@7
	int v8; // eax@7
	__int16 v9; // bp@10
	char result; // al@11
	DWORD v11; // esi@12
	DWORD v112;
	int v12; // eax@14
			 //int v13; // edx@15
	int v14; // eax@21
	wchar_t* v15; // eax@21
	int v16; // eax@22
	int v17; // [sp+10h] [bp-424h]@1
	__int16 v18; // [sp+14h] [bp-420h]@5
	int var41C[6]; // [sp+18h] [bp-41Ch]@3
	wchar_t label[512];

	v1 = thisptr;
	v2 = sub_20bb89();
	v17 = 0;

	DWORD menu_id = ((DWORD*)v1)[0x1C];
	v3 = sub_20c701(menu_id);

	if (v3 != -1) {
		v17 = sub_239623(v3);
	}
	v4 = ((DWORD*)v1)[0x1C];
	label[0] = 0;
	v5 = 0;
	v6 = sub_20c701(v4);

	if (v6 != -1) {
		v5 = sub_239623(v6);
	}
	v18 = 0;
	if (v5 && *(DWORD*)(v5 + 0x20) > 0)
	{
		v7 = *(DWORD*)(v5 + 0x24);
		v8 = 0;
		if (v7 != -1) {
			v8 = v7 + dword_482290;
		}
		v18 = *(WORD*)(v8 + 2);
	}
	var41C[0] = 1065353216;
	var41C[1] = 1060320051;
	var41C[2] = 1060320051;
	var41C[3] = 1060320051;
	v9 = 1;
	if (!v17 || (result = ~(unsigned __int8)(*(DWORD*)v17 >> 2) & 1) != 0)
	{
		v11 = (int)((char*)v1 + 0x80);
		v112 = *(DWORD*)v11 + 0x4C;
		var41C[4] = -6553500;
		var41C[5] = 6553680;

		sub_21208e((int)v1, v11);

		if (v2)
		{
			var41C[0] = *(DWORD*)(v2 + 0x168);
			var41C[1] = *(DWORD*)(v2 + 0x16C);
			var41C[2] = *(DWORD*)(v2 + 0x170);
			var41C[3] = *(DWORD*)(v2 + 0x174);
			if (v17) {
				v12 = *(DWORD*)v17;
				if (*(DWORD*)v17 & 1)
				{
					v9 = *(WORD*)(v2 + 0x166);
					var41C[4] = *(DWORD*)(v2 + 0x1A8);
					var41C[5] = *(DWORD*)(v2 + 0x1AC);
				}
				else if (v12 & 8)
				{
					v9 = *(WORD*)(v2 + 0x164);
					var41C[4] = *(DWORD*)(v2 + 0x198);
					var41C[5] = *(DWORD*)(v2 + 0x19C);
				}
				else if (v12 & 0x10)
				{
					v9 = *(WORD*)(v2 + 0x162);
					var41C[4] = *(DWORD*)(v2 + 0x188);
					var41C[5] = *(DWORD*)(v2 + 0x18C);
				}
				else
				{
					v9 = *(WORD*)(v2 + 0x160);
					var41C[4] = *(DWORD*)(v2 + 0x178);
					var41C[5] = *(DWORD*)(v2 + 0x17C);
				}

				//sub_21bf85(v11, *(DWORD*)(v17 + 0x2C)); //title label_id
				sub_21bf85_CMLTD(v11, label_id_title, label_menu_id);

				v14 = (*(int(__thiscall**)(DWORD))(v112))(v11);
				v15 = (*(wchar_t*(__thiscall**)(int))(*(DWORD*)v14 + 0xC))(v14);

				sub_4bd27(label, v15, 0x200);
			}
		}

		sub_21bc81((void*)v11, 1, v18, var41C, v9, &var41C[4]);

		v16 = (*(int(__thiscall**)(int))(v112))(v11);
		*(WORD*)(v16 + 0x14) |= 2u;

		result = sub_21baa9(v11, label);

		*((WORD*)v1 + 0xEA) = 0x7FFF;
	}
	return result;
}

char __stdcall sub_210a44_CMLTD(int thisptr, int a2, int* a3, int label_menu_id, int label_id_title, int label_id_description)
{
	int dword_482290 = *(int*)((char*)H2BaseAddr + 0x482290);

	int(__cdecl* sub_20c701)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20c701);
	int(__cdecl* sub_239623)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x239623);
	//char(__thiscall* sub_20fd41)(int) = (char(__thiscall*)(int))((char*)H2BaseAddr + 0x20fd41);
	int(__thiscall* sub_20ff73)(int, char) = (int(__thiscall*)(int, char))((char*)H2BaseAddr + 0x20ff73);
	int(__thiscall* sub_20f975)(int, WORD) = (int(__thiscall*)(int, WORD))((char*)H2BaseAddr + 0x20f975);
	//int(__thiscall* sub_2107df)(int, int*, char) = (int(__thiscall*)(int, int*, char))((char*)H2BaseAddr + 0x2107df);
	int(__thiscall* sub_21208e)(int thisptr, int a2) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21208e);

	int v3 = thisptr;
	int v5 = 0;
	int v6 = sub_20c701(*(DWORD*)(thisptr + 0x70));
	if (v6 != -1) {
		v5 = sub_239623(v6);
	}
	sub_20fd41_CMLTD((void*)v3, label_menu_id, label_id_title);
	char result = sub_20ff73(v3, 1);
	if (v5)
	{
		result = a3[1] == -1;
		*(BYTE*)(v3 + 0x9fc) = (*(DWORD*)v5 >> 5) & 1;
		signed int v8 = *(DWORD*)(v5 + 0x20);
		char v13 = result;
		if (v8 > 0)
		{
			int v9 = *(DWORD*)(v5 + 0x24);
			int v10 = 0;
			if (v9 != -1) {
				v10 = v9 + dword_482290;
			}
			bool v11 = v8 == 1;
			if (v8 > 1)
			{
				if (*a3)
				{
					int v12 = *(DWORD*)(*a3 + 0x14);
					*(WORD*)(v3 + 0x9f8) = 0;
					*(WORD*)(v3 + 0x68) = *(WORD*)(v10 + 2) - 1;
					sub_21208e(v3, *a3);
					if (*(DWORD*)(v5 + 0x20) > 0)
					{
						__int16 v4 = 0;
						do
						{
							*(WORD*)(v12 + 0x9f8) = v4;
							sub_2107df_CMLTD(v12, a3, v13, label_menu_id, label_id_description);
							v12 = *(DWORD*)(v12 + 0x18);
							++v4;
						} while ((unsigned int)v4 < *(DWORD*)(v5 + 0x20));
					}
					return sub_20f975(v3, *(WORD*)(v5 + 0x28) - 1);
				}
				v11 = v8 == 1;
			}
			if (v11 || *(BYTE*)v5 & 2)
			{
				*(WORD*)(v3 + 0x9f8) = 0;
				sub_2107df_CMLTD(v3, a3, v13, label_menu_id, label_id_description);
			}
			return sub_20f975(v3, *(WORD*)(v5 + 0x28) - 1);
		}
	}
	return result;
}

int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description)
{
	int(__cdecl* sub_20c701)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20c701);
	//int(__thiscall* sub_210a44)(int, int, int*) = (int(__thiscall*)(int, int, int*))((char*)H2BaseAddr + 0x210a44);
	int(__cdecl* sub_239623)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x239623);
	int(__thiscall* sub_211e23)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x211e23);

	int v2 = thisptr;
	int v3 = sub_20c701(*(DWORD*)(thisptr + 0x70));
	int v4 = *(DWORD*)(v2 + 0xA5C);

	int var68[0x50];
	var68[0] = 0;
	var68[1] = 1;
	var68[2] = 0;
	var68[3] = 0;
	var68[4] = v4;
	var68[5] = 0;

	memset(&var68[6], 0, 0x50u);
	sub_210a44_CMLTD(v2, v3, var68, label_menu_id, label_id_title, label_id_description);
	int v6 = sub_20c701(*(DWORD*)(v2 + 0x70));

	if (v6 != -1)
	{
		int v7 = sub_239623(v6);
		if (v7)
		{
			if (!(*(BYTE*)v7 & 2))
			{
				int v8 = *(DWORD*)(v2 + 0xA5C);
				if (v8) {
					*(BYTE*)(v8 + 0xA2) = 1;
				}
			}
		}
	}
	return sub_211e23(v2);
}

int __stdcall sub_23ae3c_CMLTD(void* thisptr, int label_menu_id, int label_id_title, int label_id_description) {

	int(__thiscall* sub_211973)(int, unsigned __int16) = (int(__thiscall*)(int, unsigned __int16))((char*)H2BaseAddr + 0x211973);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21BF85);
	void(__thiscall* sub_21BA2A)(int) = (void(__thiscall*)(int))((char*)H2BaseAddr + 0x21BA2A);

	int v1 = (int)thisptr;
	int v2 = sub_211973((int)thisptr, 3u);
	//switch (*(DWORD*)(v1 + 2652))//gets the menu_type_id
	sub_21bf85_CMLTD(v1 + 128, label_id_title, label_menu_id);
	if (v2)
		sub_21bf85_CMLTD(v2, label_id_description, label_menu_id);
	int v5 = sub_211973(v1, 2u);
	sub_21BA2A(v5);//*(BYTE*)(v5 + 116) = 1;
	return (*(int(__thiscall**)(int, int))(*(DWORD*)v1 + 36))(v1, v1 + 13432);
}

int __stdcall sub_23bf3e_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description)
{
	int(__cdecl* sub_20c701)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x20c701);
	//int(__thiscall* sub_210a44)(int, int, int*) = (int(__thiscall*)(int, int, int*))((char*)H2BaseAddr + 0x210a44);
	int(__thiscall* sub_211e23)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x211e23);

	void(__thiscall* sub_23BBBE)(void*, void*, int) = (void(__thiscall*)(void*, void*, int))((char*)H2BaseAddr + 0x23BBBE);
	//int(__thiscall* sub_23AE3C)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23AE3C);

	void* dword_3D2E38 = (void*)(H2BaseAddr + 0x3D2E38);
	void* dword_3D2A78 = (void*)(H2BaseAddr + 0x3D2A78);
	void* dword_3D2CB8 = (void*)(H2BaseAddr + 0x3D2CB8);
	void* dword_3D2B38 = (void*)(H2BaseAddr + 0x3D2B38);
	BYTE* byte_3D2F30 = (BYTE*)((char*)H2BaseAddr + 0x3D2F30);

	int* v2; // esi
	int v3; // edi
	unsigned int v4; // eax
	int v5; // ecx
	int result; // eax
	int v7[80];
	int v14[47]; // [esp+74h] [ebp-BCh]

	v2 = (int*)thisptr;
	v3 = sub_20c701(((int*)thisptr)[28]);
	if (v3 != -1)
	{
		v7[0] = 0;
		v7[1] = 1;
		v7[2] = 47;
		v7[3] = (int)v14;
		v7[4] = 0;
		v7[5] = 0;
		memset(&v7[6], 0, 80);
		v4 = 0;
		v5 = (int)(v2 + 798);
		do
		{
			v14[v4++] = v5;
			v5 += 256;
		} while (v4 < 0x2F);
		sub_210a44_CMLTD((int)v2, v3, v7, label_menu_id, label_id_title, label_id_description);
	}
	sub_211e23((int)v2);
	sub_23BBBE(v2, dword_3D2E38, 47);
	sub_23BBBE(v2, dword_3D2A78, 47);
	sub_23BBBE(v2, dword_3D2CB8, 47);
	sub_23BBBE(v2, dword_3D2B38, 47);
	result = sub_23ae3c_CMLTD(v2, label_menu_id, label_id_title, label_id_description);
	int VKbMenuTypeDefault = v2[663];
	int VKbMenuTypeNew = v2[663] + VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX;
	//"SYMBOLS" AND "ACCENTS" buttons are greyed out by default
	if (byte_3D2F30[8 * VKbMenuTypeDefault] & 0xA)
	{
		*((BYTE*)v2 + 14566) = 0;//disables "SYMBOLS" button from being highlightable
		*((BYTE*)v2 + 14310) = 0;//disables "ACCENTS" button from being highlightable
	}
	else
	{
		*((BYTE*)v2 + 15078) = 0;//enable "SYMBOLS" button style
		*((BYTE*)v2 + 14822) = 0;//enable "ACCENTS" button style
	}
	return result;
}

#pragma endregion

#pragma region CM_VirtualKeyboard

void __stdcall sub_28870B_CM(int a1, int a2, int a3, void*(__stdcall* a4)(int), int a5)
{
	for (int i = 0; i < a3; ++i)
	{
		a4(a1);
		a1 += a2;
	}
}

//typedef char(__stdcall *tsub_23CF88)(int, int*);
//tsub_23CF88 psub_23CF88;
char __stdcall sub_23CF88_CM(int thisptr, int* a2) //__thiscall
{
	//return psub_23CF88(thisptr, a2);

	//char(__thiscall* sub_23CF88)(int, int*) = (char(__thiscall*)(int, int*))((char*)H2BaseAddr + 0x23CF88);
	//return sub_23CF88(thisptr, a2);

	int(__thiscall* sub_211BA1)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x211BA1);
	int(*sub_209A8C)() = (int(*)())((char*)H2BaseAddr + 0x209A8C);
	void(__cdecl* sub_43233)(signed int) = (void(__cdecl*)(signed int))((char*)H2BaseAddr + 0x43233);
	char(__thiscall* sub_212152)(int, int) = (char(__thiscall*)(int, int))((char*)H2BaseAddr + 0x212152);
	int(__thiscall* sub_20E807)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x20E807);
	char(__thiscall* sub_212660)(int, int, char) = (char(__thiscall*)(int, int, char))((char*)H2BaseAddr + 0x212660);
	//void(__thiscall* sub_23CD58)(void*, __int16) = (void(__thiscall*)(void*, __int16))((char*)H2BaseAddr + 0x23CD58);
	char(__thiscall* sub_2118F0)(int, int) = (char(__thiscall*)(int, int))((char*)H2BaseAddr + 0x2118F0);

	int v2; // esi
	char v3; // bl
	DWORD* v4; // ebp
	int v5; // ecx
	int VKbMenuTypeDefault; // eax
	//int v7; // eax
	DWORD* v8; // eax
	char v10; // [esp+13h] [ebp-1h]

	bool v20 = false;

	v2 = thisptr;
	v10 = 0;
	v3 = 0;
	v4 = (DWORD*)sub_211BA1(thisptr);
	if (*a2 == 5)
	{
		v5 = a2[2];
		if (v5)
		{
			if (v5 == 1 || v5 == 13)
				v3 = 1;//close menu button pressed (cancel)
		}
		else
		{
			v10 = 1;
		}
	}
	if (*a2 == 7 && a2[2] == 27 || v3)//close menu esc key
	{
		VKbMenuTypeDefault = v4[663];
		/*if (VKbMenuType == 7 || VKbMenuType == 5)
		{
			v7 = sub_209A8C();
			if (v7 != -1)
				sub_43233(v7);
		}*/
		v20 = true;
	}
	if (*a2 == 6)//virtual key pressed
	{
		if (!sub_212152(v2, 0))
		{
			if (sub_20E807((int)v4))
			{
				v8 = (DWORD*)sub_20E807((int)v4);
				sub_212660((int)v8, 0, 0);
			}
			sub_212660(v2, 0, 1);
		}
		//goto LABEL_20;
	}
	char result;
	if (v10 || *a2 == 6)
	{
		//LABEL_20:
		sub_23CD58_CM((void*)v4, *(WORD*)(v2 + 0xF8));//handles type input.
		result = 1;
	}
	else {
		result = sub_2118F0(thisptr, (int)a2);//only handles all kinds of menu exiting.
	}
	if (v20) {
		wchar_t* returnString = (wchar_t*)*(DWORD*)((BYTE*)v4 + 0xC64);
		int returnStrLen = wcslen(returnString) + 1;
		char* end = (char*)calloc(returnStrLen, sizeof(char));
		wcstombs2(end, returnString, returnStrLen);
		strcpy((char*)returnString, end);
		free(end);
	}
	return result;
}

__declspec(naked) void sub_23CF88_CM_nak_VKeyTest() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_23CF88_CM//__stdcall

		POP_REGISTERS

		retn 4
	}
}

//typedef char(__stdcall *tsub_23D060)(void*, int*);
//tsub_23D060 psub_23D060;
char __stdcall sub_23D060_CM(void* thisptr, int* a2) //__thiscall
{
	//return psub_23D060(thisptr, a2);

	char(__thiscall* sub_23AF4E)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x23AF4E);
	void(__thiscall* sub_23BBBE)(void*, void*, int) = (void(__thiscall*)(void*, void*, int))((char*)H2BaseAddr + 0x23BBBE);
	int(__thiscall*sub_23B080)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23B080);
	int(__cdecl*sub_21DD04)(signed int) = (int(__cdecl*)(signed int))((char*)H2BaseAddr + 0x21DD04);
	int(__thiscall*sub_23B9DE)(int) = (int(__thiscall*)(int))((char*)H2BaseAddr + 0x23B9DE);
	int(__thiscall*sub_23BADA)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x23BADA);
	char(__thiscall*sub_23C4C9)(int, __int16) = (char(__thiscall*)(int, __int16))((char*)H2BaseAddr + 0x23C4C9);
	//void(__thiscall* sub_23CD58)(void*, __int16) = (void(__thiscall*)(void*, __int16))((char*)H2BaseAddr + 0x23CD58);
	char(__thiscall*sub_20EB2B)(void*, int*) = (char(__thiscall*)(void*, int*))((char*)H2BaseAddr + 0x20EB2B);
	void(__thiscall*sub_23B5C1)(void*) = (void(__thiscall*)(void*))((char*)H2BaseAddr + 0x23B5C1);
	int(__cdecl*sub_4BD54)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x4BD54);
	char(__cdecl*sub_4C6E0)(__int16) = (char(__cdecl*)(__int16))((char*)H2BaseAddr + 0x4C6E0);

	void* dword_3D2D78 = (void*)(H2BaseAddr + 0x3D2D78);
	void* dword_3D2A78 = (void*)(H2BaseAddr + 0x3D2A78);
	void* dword_3D2B38 = (void*)(H2BaseAddr + 0x3D2B38);
	void* dword_3D2CB8 = (void*)(H2BaseAddr + 0x3D2CB8);
	void* dword_3D2E38 = (void*)(H2BaseAddr + 0x3D2E38);
	BYTE* byte_3D2F30 = (BYTE*)((char*)H2BaseAddr + 0x3D2F30);

	bool v4; // zf
	unsigned int v6; // eax
	bool v7; // cf
	int v8; // eax
	char result; // al
	WORD* v10; // eax
	WORD* v11; // eax
	char v12; // bl
	void* v13 = nullptr; // eax

	signed __int16 v20;

	v4 = a2[0] == 1;
	if (v4 || a2[0] == 2 || a2[0] == 3 || a2[0] == 4)
	{
		v12 = sub_23AF4E((signed __int16*)thisptr, a2[0]);
		switch (v12)
		{
		case 41:
			if (*((BYTE*)thisptr + 3186) & 1)
				goto LABEL_47;
			v13 = dword_3D2D78;
			goto LABEL_48;
		case 42:
			if (*((BYTE*)thisptr + 3186) & 2)
				goto LABEL_47;
			v13 = dword_3D2A78;
			goto LABEL_48;
		case 43:
			if (*((BYTE*)thisptr + 3186) & 4)
				goto LABEL_47;
			v13 = dword_3D2B38;
			goto LABEL_48;
		case 44:
			v13 = dword_3D2CB8;
			if (*((BYTE*)thisptr + 3186) & 8)
				LABEL_47:
			v13 = dword_3D2E38;
		LABEL_48:
			sub_23BBBE(thisptr, v13, 47);
			break;
		default:
			break;
		}
		int VKbMenuType = *((DWORD*)thisptr + 663);
		if (byte_3D2F30[8 * VKbMenuType] & 0xA && (v12 == 43 || v12 == 44))
			v12 = sub_23AF4E((signed __int16 *)thisptr, *a2);
		(*(void(__thiscall **)(void *, BYTE*))(*(DWORD*)thisptr + 36))(thisptr, (BYTE*)thisptr + 256 * v12 + 3192);
		result = 1;
	}
	else
	{
		if (a2[0] == 5)
		{
			switch (a2[2])
			{
			case 1:
			case 13:
				sub_23B080(thisptr);
				break;
			case 6:
			LABEL_9:
				v6 = *((DWORD*)thisptr + 794);
				v7 = v6 < *((DWORD*)thisptr + 793);
				v4 = v6 == *((DWORD*)thisptr + 793);
				*((BYTE*)thisptr + 3188) = 0;
				if (v7 || v4)
					goto LABEL_12;
				v8 = v6 - 2;
				goto LABEL_11;
			case 7:
			LABEL_15:
				v10 = (WORD*)*((DWORD*)thisptr + 794);
				*((BYTE*)thisptr + 3188) = 0;
				if (!*v10)
				{
				LABEL_12:
					sub_21DD04(6);
					return 1;
				}
				v8 = (int)(v10 + 1);
			LABEL_11:
				*((DWORD*)thisptr + 794) = v8;
				goto LABEL_12;
			case 2:
			LABEL_33:
				sub_23B9DE((int)thisptr);
				return 1;
			case 14:
			case 15:
				sub_23BADA(thisptr);
				return 1;
			case 5:
				sub_23C4C9((int)thisptr, 32);
				return 1;
			case 12:
			LABEL_23:
				sub_23CD58_CM(thisptr, 40);
				return 1;
			}
			return sub_20EB2B(thisptr, a2);
		}
		if (a2[0] != 7)
			return sub_20EB2B(thisptr, a2);
		switch (a2[2])
		{
		case 8:
			goto LABEL_33;
		case 13:
			goto LABEL_23;
		case 27:
			sub_23B080(thisptr);
			a2[0] = 5;
			a2[2] = 1;
			return sub_20EB2B(thisptr, a2);
		case 37:
			goto LABEL_9;
		case 38:
		case 40://Up & Down Arrows
			return 1;
		case 39:
			goto LABEL_15;
		case 46:
			sub_23B5C1(thisptr);
			v11 = (WORD*)*((DWORD*)thisptr + 794);
			if (!*v11)
				return sub_20EB2B(thisptr, a2);
			*((BYTE*)thisptr + 3188) = 0;
			if (*v11)
				*((DWORD*)thisptr + 794) = (DWORD)(v11 + 1);
			sub_21DD04(6);
			goto LABEL_33;
		default:
			v20 = *(signed __int16*)&a2[3];
			if (v20 == -1 || sub_4BD54(v20) || sub_4C6E0(v20))
				return sub_20EB2B(thisptr, a2);
			sub_23C4C9((int)thisptr, v20);
			result = 1;
			break;
		}
	}
	return result;
}

__declspec(naked) void sub_23D060_CM_nak_VKeyTest() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push eax
		push ecx
		call sub_23D060_CM//__stdcall

		POP_REGISTERS

		retn 4
	}
}

__declspec(naked) void sub_23bf3e_CMLTD_nak_VKeyTest() {//__thiscall
	__asm {
		mov eax, [esp + 4h]

		PUSH_REGISTERS

		push 0xFFFFFFF1//label_id_description
		push 0xFFFFFFF0//label_id_title
		push CMLabelMenuId_VKeyTest
		push eax
		push ecx
		call sub_23bf3e_CMLTD//__stdcall

		POP_REGISTERS

		retn 4
	}
}

#pragma endregion
