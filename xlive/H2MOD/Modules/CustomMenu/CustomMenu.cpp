#include "stdafx.h"

#include "CustomMenu.h"
#include "CustomLanguage.h"

#include "interface/screens/screen_xbox_live_task_progress_dialog.h"
#include "Networking/NetworkMessageTypeCollection.h"


#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/Accounts/AccountCreate.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/Accounts/Accounts.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"
#include "H2MOD/Modules/Updater/Updater.h"
#include "H2MOD/Utils/Utils.h"
#include "H2MOD/Tags/TagInterface.h"

#include "XLive/xnet/IpManagement/XnIp.h"

#include "c_error_menu.h"
#include "c_brightness_menu.h"
#include "c_screen_with_menu.h"
#include "c_virtual_keyboard_menu.h"

#include "Accounts/c_account_list_menu.h"
#include "Accounts/c_account_create_menu.h"

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

void CMSetupVFTables(DWORD** menu_vftable_1, DWORD** menu_vftable_2, DWORD CM_LabelButtons, DWORD sub_2111ab_CMLTD_nak, DWORD CM_FuncPtrHelper, DWORD CM_ButtonPreselection, bool isInternalMenuWindow, DWORD sub_248beb_nak_deconstructor);
int __stdcall BtnHandlerCaller(void* thisptr, int a2, int a3);

bool CMForce_Update = false;

void GSCustomMenuCall_AccountList();

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
	for (auto const& ent1 : custom_languages) {
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
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, 0x40 + button_id, CMLabelMenuId_Language);
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

void* __cdecl CustomMenu_Language_Sub(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Language_Sub())(s_new_ui_screen_parameters*) {
	return CustomMenu_Language_Sub;
}

DWORD* menu_vftable_1_Language_Sub = 0;
DWORD* menu_vftable_2_Language_Sub = 0;

void CMSetupVFTables_Language_Sub() {
	CMSetupVFTables(&menu_vftable_1_Language_Sub, &menu_vftable_2_Language_Sub, (DWORD)CMLabelButtons_Language_Sub, (DWORD)sub_2111ab_CMLTD_nak_Language_Sub, (DWORD)CustomMenuFuncPtrHelp_Language_Sub, (DWORD)sub_20F790_CM_nak_Language_Sub, true, 0);
}

void* __cdecl CustomMenu_Language_Sub(s_new_ui_screen_parameters* a1) {
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Language_Sub, menu_vftable_2_Language_Sub, (DWORD)&CMButtonHandler_Language_Sub, Language_Sub_Count, 272);
}

bool GSCustomMenuCall_Language_Sub() {
	CM_Language_Sub_Setup_Buttons();
	if (Language_Sub_Count > 0) {
		CallWgit(CustomMenu_Language_Sub);
		return true;
	}
	else {
		CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 4, 5);
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
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_Language);
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
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 6, 7);
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

void* __cdecl CustomMenu_Language(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Language())(s_new_ui_screen_parameters*) {
	return CustomMenu_Language;
}

DWORD* menu_vftable_1_Language = 0;
DWORD* menu_vftable_2_Language = 0;

void CMSetupVFTables_Language() {
	CMSetupVFTables(&menu_vftable_1_Language, &menu_vftable_2_Language, (DWORD)CMLabelButtons_Language, (DWORD)sub_2111ab_CMLTD_nak_Language, (DWORD)CustomMenuFuncPtrHelp_Language, (DWORD)sub_20F790_CM_nak_Language, true, 0);
}

void* __cdecl CustomMenu_Language(s_new_ui_screen_parameters* a1) {
	languageCaptureSetLabel();
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Language, menu_vftable_2_Language, (DWORD)&CMButtonHandler_Language, 13, 272);
}

void GSCustomMenuCall_Language() {
	CallWgit(CustomMenu_Language);
}

#pragma endregion

#pragma endregion

#pragma region CM_Update

static bool force_keep_open_Update = false;

void __stdcall CMLabelButtons_Update(int a1, int a2)
{
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_Update);
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
	else if (button_id == 1 && updater_has_files_to_download) {
		GSDownloadDL();
	}
	else if (button_id == 2 && updater_has_files_to_install) {
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

	int(__thiscall * sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl * sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

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

void* __cdecl CustomMenu_Update(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Update())(s_new_ui_screen_parameters*) {
	return CustomMenu_Update;
}

DWORD* menu_vftable_1_Update = 0;
DWORD* menu_vftable_2_Update = 0;

void CMSetupVFTables_Update() {
	CMSetupVFTables(&menu_vftable_1_Update, &menu_vftable_2_Update, (DWORD)CMLabelButtons_Update, (DWORD)sub_2111ab_CMLTD_nak_Update, (DWORD)CustomMenuFuncPtrHelp_Update, (DWORD)sub_20F790_CM_nak_Update, true, (DWORD)sub_248beb_nak_deconstructor_Update);
}

void* __cdecl CustomMenu_Update(s_new_ui_screen_parameters* a1) {
	force_keep_open_Update = true;
	GSDownloadInit();
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Update, menu_vftable_2_Update, (DWORD)&CMButtonHandler_Update, 4, 272);
}

void GSCustomMenuCall_Update() {
	CallWgit(CustomMenu_Update);
}

#pragma endregion


const int CMLabelMenuId_Update_Note = 0xFF000012;
#pragma region CM_Update_Note

void __stdcall CMLabelButtons_Update_Note(int a1, int a2)
{
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_Update_Note);
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

void* __cdecl CustomMenu_Update_Note(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Update_Note())(s_new_ui_screen_parameters*) {
	return CustomMenu_Update_Note;
}

DWORD* menu_vftable_1_Update_Note = 0;
DWORD* menu_vftable_2_Update_Note = 0;

void CMSetupVFTables_Update_Note() {
	CMSetupVFTables(&menu_vftable_1_Update_Note, &menu_vftable_2_Update_Note, (DWORD)CMLabelButtons_Update_Note, (DWORD)sub_2111ab_CMLTD_nak_Update_Note, (DWORD)CustomMenuFuncPtrHelp_Update_Note, (DWORD)sub_20F790_CM_nak_Update_Note, true, 0);
}

void* __cdecl CustomMenu_Update_Note(s_new_ui_screen_parameters* a1) {
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Update_Note, menu_vftable_2_Update_Note, (DWORD)&CMButtonHandler_Update_Note, 2, 272);
}

void GSCustomMenuCall_Update_Note() {
	if (!H2IsDediServer) {
		CallWgit(CustomMenu_Update_Note);
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
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_Login_Warn);
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

	int(__thiscall * sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl * sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

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

void* __cdecl CustomMenu_Login_Warn(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Login_Warn())(s_new_ui_screen_parameters*) {
	return CustomMenu_Login_Warn;
}

DWORD* menu_vftable_1_Login_Warn = 0;
DWORD* menu_vftable_2_Login_Warn = 0;

void CMSetupVFTables_Login_Warn() {
	CMSetupVFTables(&menu_vftable_1_Login_Warn, &menu_vftable_2_Login_Warn, (DWORD)CMLabelButtons_Login_Warn, (DWORD)sub_2111ab_CMLTD_nak_Login_Warn, (DWORD)CustomMenuFuncPtrHelp_Login_Warn, (DWORD)sub_20F790_CM_nak_Login_Warn, true, (DWORD)sub_248beb_nak_deconstructor_Login_Warn);
}

void* __cdecl CustomMenu_Login_Warn(s_new_ui_screen_parameters* a1) {
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Login_Warn, menu_vftable_2_Login_Warn, (DWORD)&CMButtonHandler_Login_Warn, 0, 272);
}

void GSCustomMenuCall_Login_Warn() {
	CallWgit(CustomMenu_Login_Warn);
}

#pragma endregion



#pragma region Setting_Modifications

static bool blind_fp = false;
static bool blind_hud = false;

bool __cdecl sub_BD114_blind_fp(unsigned int a1)//render first person model
{
	bool result = blind_fp ? true : false;
	return result;
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

#pragma endregion

#pragma region CM_Credits

void __stdcall CMLabelButtons_Credits(int a1, int a2)
{
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_Credits);
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
		CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFFF04, 0xFFFFFF05);
	}
	else if (button_id == 2) {
		CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFFF02, 0xFFFFFF03);
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

void* __cdecl CustomMenu_Credits(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Credits())(s_new_ui_screen_parameters*) {
	return CustomMenu_Credits;
}


DWORD* menu_vftable_1_Credits = 0;
DWORD* menu_vftable_2_Credits = 0;

int __fastcall test_credits_construct(void* a1, DWORD _EDX, char a2)
{
	return sub_2111ab_CMLTD((int)a1, a2, CMLabelMenuId_Credits, 0xFFFFFFF0, 0xFFFFFFF1);
}

void CMSetupVFTables_Credits() {
	//CMSetupVFTables(&menu_vftable_1_Credits, &menu_vftable_2_Credits, (DWORD)CMLabelButtons_Credits, (DWORD)test_credits_construct, (DWORD)CustomMenuFuncPtrHelp_Credits, (DWORD)sub_20F790_CM_nak_Credits, true, 0);
	CMSetupVFTables(&menu_vftable_1_Credits, &menu_vftable_2_Credits, (DWORD)CMLabelButtons_Credits, (DWORD)sub_2111ab_CMLTD_nak_Credits, (DWORD)CustomMenuFuncPtrHelp_Credits, (DWORD)sub_20F790_CM_nak_Credits, true, 0);
}

void* __cdecl CustomMenu_Credits(s_new_ui_screen_parameters* a1) {
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Credits, menu_vftable_2_Credits, (DWORD)&CMButtonHandler_Credits, 16, 272);
}

void GSCustomMenuCall_Credits() {
	CallWgit(CustomMenu_Credits);
}

#pragma endregion

void GSCustomMenuCall_AccountEdit();

const int CMLabelMenuId_Invalid_Login_Token = 0xFF000017;
#pragma region CM_Invalid_Login_Token

void __stdcall CMLabelButtons_Invalid_Login_Token(int a1, int a2)
{
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_Invalid_Login_Token);
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

void* __cdecl CustomMenu_Invalid_Login_Token(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Invalid_Login_Token())(s_new_ui_screen_parameters*) {
	return CustomMenu_Invalid_Login_Token;
}


void* __stdcall sub_248beb_deconstructor_Login_Token(LPVOID lpMem, char a2)//__thiscall
{
	int(__thiscall * sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl * sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	if (c_account_list_menu::accountingGoBackToList && c_account_list_menu::IsAccountingActiveHandle()) {
		GSCustomMenuCall_AccountEdit();
		c_account_list_menu::accountingGoBackToList = true;
	}

	c_account_list_menu::UpdateAccountingActiveHandle(false);

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

void* __cdecl CustomMenu_Invalid_Login_Token(s_new_ui_screen_parameters* a1) {
	c_account_list_menu::accountingGoBackToList = true;
	c_account_list_menu::UpdateAccountingActiveHandle(true);
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Invalid_Login_Token, menu_vftable_2_Invalid_Login_Token, (DWORD)&CMButtonHandler_Invalid_Login_Token, 0, 272);
}

void GSCustomMenuCall_Invalid_Login_Token() {
	CallWgit(CustomMenu_Invalid_Login_Token);
}

#pragma endregion

#pragma region CM_AccountCreate

void GSCustomMenuCall_AccountCreate() {
	CallWgit(c_account_create_menu::open);
}

#pragma endregion /* CM_AccountCreate */

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
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_AccountEdit);
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

int master_login_code;
HANDLE hThreadLogin = INVALID_HANDLE_VALUE;

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
			case ERROR_CODE_CURL_HANDLE:
			case ERROR_CODE_CURL_SOCKET_FAILED:
			case ERROR_CODE_ACCOUNT_DATA:
			case ERROR_CODE_INVALID_PARAM:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF014, 0xFFFFF015);
				return;

			case ERROR_CODE_CURL_EASY_PERF:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF030, 0xFFFFF031);
				return;

			case ERROR_CODE_INVALID_VERSION:
				GSCustomMenuCall_Update_Note();
				return;

			case ERROR_CODE_INVALID_LOGIN_TOKEN:
				GSCustomMenuCall_Invalid_Login_Token();
				return;

			case ERROR_CODE_INVALID_LOGIN_ID:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF008, 0xFFFFF009);
				return;

			case ERROR_CODE_INVALID_PASSWORD:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF00A, 0xFFFFF00B);
				return;

			case ERROR_CODE_MACHINE_BANNED:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF00C, 0xFFFFF00D);
				return;

			case ERROR_CODE_ACCOUNT_BANNED:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF00E, 0xFFFFF00F);
				return;

			case ERROR_CODE_ACCOUNT_DISABLED:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF010, 0xFFFFF011);
				return;

			case ERROR_CODE_MACHINE_SERIAL_INSUFFICIENT:
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF018, 0xFFFFF019);
				return;

			default:
				//unknown error!
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF012, 0xFFFFF013);
				return;
			}
		}
		else {
			if (master_login_code == SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT) {
				CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF018, 0xFFFFF019);
				return;
			}
			else {
				if (CMForce_Update)
				{
					//Just in case save the config to prevent a high iq individual from getting themsevles in an update loop.
					SaveH2Config();
					CMForce_Update = false;
					GSCustomMenuCall_Update_Note();
					return;
				}
				ImGuiHandler::ToggleWindow(ImGuiHandler::ImMOTD::windowName);
				// extern int notify_xlive_ui;
				// notify_xlive_ui = 0;
				// sub_248beb_nak_deconstructor_Login_Warn();
				// GSCustomMenuCall_Login_Warn();
				return;
			}
		}
	}
}

DWORD WINAPI ThreadLogin(LPVOID lParam)
{
	int button_id = (int)lParam;

	//gotta delay it a little to make sure the menu's decide to render correctly.
	Sleep(200L);

	if (button_id == -1) {
		char* identifier = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
		char* identifier_pass = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 2);
		//login to account

		if (HandleGuiLogin(NULL, identifier, identifier_pass, &master_login_code)) {
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

	c_account_list_menu::UpdateAccountingActiveHandle(false);

	hThreadLogin = INVALID_HANDLE_VALUE;
	return 0;
}

static bool CMButtonHandler_AccountEdit(int button_id) {
	bool close_menu = false;
	if (button_id == 0) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
		CustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountEdit, 0xFFFFF002, CMLabelMenuId_AccountEdit, 0xFFFFF003);
	}
	else if (button_id == 1) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 2);
		CustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountEdit, 0xFFFFF004, CMLabelMenuId_AccountEdit, 0xFFFFF005);
	}
	else if (button_id == 2) {
		AccountEdit_remember = !AccountEdit_remember;
		add_cartographer_label(CMLabelMenuId_AccountEdit, 3, H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 0xFFFFFFF2 + (AccountEdit_remember ? 1 : 0)), true);
	}
	else if (button_id == 3) { // login button id
		if (hThreadLogin == INVALID_HANDLE_VALUE) {
			c_account_list_menu::accountingGoBackToList = false;
			c_account_list_menu::UpdateAccountingActiveHandle(true);
			hThreadLogin = CreateThread(NULL, 0, ThreadLogin, (LPVOID)-1, 0, NULL);
			c_xbox_live_task_progress_menu::open(xbox_live_task_progress_callback);
			close_menu = true;
		}
	}
	return close_menu;
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
	int(__thiscall * sub_248b90)(void*) = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x248b90);
	int(__cdecl * sub_287c23)(void*) = (int(__cdecl*)(void*))((char*)H2BaseAddr + 0x287c23);

	if (c_account_list_menu::accountingGoBackToList && c_account_list_menu::IsAccountingActiveHandle()) {
		GSCustomMenuCall_AccountList();
		c_account_list_menu::accountingGoBackToList = false;
	}

	c_account_list_menu::UpdateAccountingActiveHandle(false);

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

void* __cdecl CustomMenu_AccountEdit(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_AccountEdit())(s_new_ui_screen_parameters*) {
	return CustomMenu_AccountEdit;
}

DWORD* menu_vftable_1_AccountEdit = 0;
DWORD* menu_vftable_2_AccountEdit = 0;

void CMSetupVFTables_AccountEdit() {
	CMSetupVFTables(&menu_vftable_1_AccountEdit, &menu_vftable_2_AccountEdit, (DWORD)CMLabelButtons_AccountEdit, (DWORD)sub_2111ab_CMLTD_nak_AccountEdit, (DWORD)CustomMenuFuncPtrHelp_AccountEdit, (DWORD)sub_20F790_CM_nak_AccountEdit, true, (DWORD)sub_248beb_nak_deconstructor_AccountEdit);
}

void* __cdecl CustomMenu_AccountEdit(s_new_ui_screen_parameters* a1) {
	c_account_list_menu::accountingGoBackToList = true;
	c_account_list_menu::UpdateAccountingActiveHandle(true);
	setupAccountEditLabels();
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_AccountEdit, menu_vftable_2_AccountEdit, (DWORD)&CMButtonHandler_AccountEdit, 4, 272);
}

void GSCustomMenuCall_AccountEdit() {
	CallWgit(CustomMenu_AccountEdit);
}

#pragma endregion


#pragma region CM_AccountList

void GSCustomMenuCall_AccountList() {
	CallWgit(c_account_list_menu::open, 3);
}

#pragma endregion


const int CMLabelMenuId_Guide = 0xFF000003;
#pragma region CM_Guide

void __stdcall CMLabelButtons_Guide(int a1, int a2)
{
	int(__thiscall * sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall * sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909(a1, 6, 0, 0);
	if (v3)
	{
		set_widget_label_from_string_id_reimpl(v3, button_id + 1, CMLabelMenuId_Guide);
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
		ImGuiHandler::ToggleWindow(k_advanced_settings_window_name);
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

void* __cdecl CustomMenu_Guide(s_new_ui_screen_parameters*);

void* (__cdecl* CustomMenuFuncPtrHelp_Guide())(s_new_ui_screen_parameters*) {
	return CustomMenu_Guide;
}

DWORD* menu_vftable_1_Guide = 0;
DWORD* menu_vftable_2_Guide = 0;

void CMSetupVFTables_Guide() {
	CMSetupVFTables(&menu_vftable_1_Guide, &menu_vftable_2_Guide, (DWORD)CMLabelButtons_Guide, (DWORD)sub_2111ab_CMLTD_nak_Guide, (DWORD)CustomMenuFuncPtrHelp_Guide, (DWORD)sub_20F790_CM_nak_Guide, false, 0);
}

void* __cdecl CustomMenu_Guide(s_new_ui_screen_parameters* a1) {
	char* guide_desc_base = H2CustomLanguageGetLabel(CMLabelMenuId_Guide, 0xFFFFFFF2);
	char* guide_description = (char*)malloc(strlen(guide_desc_base) + 50);
	sprintf(guide_description, guide_desc_base, GetVKeyCodeString(H2Config_hotkeyIdGuide).c_str());
	add_cartographer_label(CMLabelMenuId_Guide, 0xFFFFFFF1, guide_description, true);
	free(guide_description);
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Guide, menu_vftable_2_Guide, (DWORD)&CMButtonHandler_Guide, 4, 272);
}

void GSCustomMenuCall_Guide() {
	CallWgit(CustomMenu_Guide, 2, 272);
}

#pragma endregion


void GSCustomMenuCall_Esc() {

	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x20CDE7);
	//CallWgit(WgitScreenfunctionPtr, 3);

	//GSCustomMenuCall_EscSettings();

	//GSCustomMenuCall_Error_Outer(CMLabelMenuId_Error, 2, 3);
}


#pragma region some tests

typedef int(__stdcall* tbtnhandler)(void* thisptr, int a2, int a3);
tbtnhandler pbtnHandler;

typedef int(__stdcall* tsub_20C226)(void*, __int16, int, int, int);
tsub_20C226 psub_20C226;
int __stdcall sub_20C226(void* thisptr, __int16 a2, int a3, int a4, int a5) {
	return psub_20C226(thisptr, a2, a3, a4, a5);
}


typedef char(__stdcall* tsub_24DC0D)(void*, int);
tsub_24DC0D psub_24DC0D;
char __stdcall sub_24DC0D_CM(void* thisptr, int a2) {//__thiscall

	//char result = psub_24DC0D(thisptr, a2);
	//return result;

	//when sub_20EB2B returns 1 - closing outermost menu
	//when sub_20EB2B returns 0 - closing inner menu's to go back not out completely

	char(__thiscall * sub_20EB2B)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20EB2B);
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

typedef char(__thiscall* tsub_23D8AE)(void*, int);
tsub_23D8AE psub_23D8AE;
char __stdcall sub_23D8AE_CM(void* thisptr, int a2) {//__thiscall - cannot hook. must manually asm

	//char result = psub_23D8AE(thisptr, a2);

	//return result;

	char(__thiscall * sub_20EB2B)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20EB2B);
	int(__thiscall * sub_212604)(void*, int) = (int(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x212604);

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

typedef char(__stdcall* tsub_20EEBE)(void*, int);
tsub_20EEBE psub_20EEBE;
char __stdcall sub_20EEBE_CM(void* thisptr, int a2) {//__thiscall

	//char result = psub_20EEBE(thisptr, a2);
	//return result;

	//when sub_20EB2B returns 1 - closing outermost menu
	//when sub_20EB2B returns 0 - closing inner menu's to go back not out completely

	char(__thiscall * sub_20EB2B)(void*, int) = (char(__thiscall*)(void*, int))((char*)H2BaseAddr + 0x20EB2B);
	char(__thiscall * sub_214DDD)(void*) = (char(__thiscall*)(void*))((char*)H2BaseAddr + 0x214DDD);

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

typedef char(__stdcall* tsub_23d060)(int, int*);
tsub_23d060 psub_23d060;
char __stdcall sub_23d060(int thisptr, int* a2) //__thiscall
{
	return psub_23d060(thisptr, a2);//keyboard keypress handler
}

void XUiShowSignInH2() {
	if (!c_account_list_menu::IsAccountingActiveHandle() && ReadH2Accounts()) {
		GSCustomMenuCall_AccountList();
	}
	else {
		if (!c_account_list_menu::IsAccountingActiveHandle())
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF016, 0xFFFFF017);
	}
}

typedef int(__cdecl* tsub_23f6b7)(int);
tsub_23f6b7 psub_23f6b7;
int __cdecl sub_23f6b7(int a1)
{
	if (UserSignedIn(0)) {
		XUserSignOut(0);
		UpdateMasterLoginStatus();
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

void* __cdecl CustomMenu_Obscure(s_new_ui_screen_parameters* a1) {
	return (void*)CustomMenu_CallHead(a1, menu_vftable_1_Obscure, menu_vftable_2_Obscure, (DWORD)&CMButtonHandler_Obscure, 14, Obscure_wgit_id);
}

void GSCustomMenuCall_Obscure() {
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x0021f5f8);//Network Adapter list.
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x0021f681);//About dialog
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x0025692c);//keyboard layout
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x00259d05);//thumbstick layout
	//int WgitScreenfunctionPtr = (int)(H2BaseAddr + 0x00259dc0);//button layout
	CallWgit(CustomMenu_Obscure);
}
#pragma endregion

void InitCustomMenu() {

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
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFF031, "Please visit:\r\nhttps://halo2pc.com/connection\r\nfor help with troubleshooting potential issues.");


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

	RefreshToggleIngameKeyboardControls();

	CMSetupVFTables_Language_Sub();
	CMSetupVFTables_Language();

	CMSetupVFTables_Update();

	CMSetupVFTables_Update_Note();

	CMSetupVFTables_Login_Warn();

	CMSetupVFTables_Credits();

	CMSetupVFTables_Invalid_Login_Token();

	// CMSetupVFTables_AccountCreate();

	CMSetupVFTables_AccountEdit();

	CMSetupVFTables_Guide();

	c_screen_with_menu::applyPatches();

	// replace brightness menu for testing
	// replace_brightness_menu();
}

void DeinitCustomMenu() {
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


void CMSetupVFTables(DWORD** menu_vftable_1,
	DWORD** menu_vftable_2,
	DWORD CM_LabelButtons,
	DWORD sub_2111ab_CMLTD_nak,
	DWORD CM_FuncPtrHelper,
	DWORD CM_ButtonPreselection,
	bool isInternalMenuWindow,
	DWORD sub_248beb_nak_deconstructor) {

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
		bool(__cdecl * customHandlerFunc)(int) = (bool(__cdecl*)(int))(*(int*)(*(DWORD*)thisptr + 16));
		int button_id = *(int*)a3 & 0xFFFF;
		bool success = customHandlerFunc(button_id);

		if (success) {
			int v3 = ((int)thisptr) - 0xAC;
			int v4 = (*(int(__thiscall**)(int))(*(DWORD*)v3 + 56))(v3);//__thiscall
			int v5 = (*(int(__thiscall**)(int))(*(DWORD*)v3 + 52))(v3);//__thiscall

			int(__cdecl * sub_5A96DA)(int a1, int a2);
			sub_5A96DA = (int(__cdecl*)(int, int))((char*)H2BaseAddr + 0x2096DA);
			result = sub_5A96DA(v5, v4);
		}
	}
	else {
		for (int i = *(DWORD*)thisptr; i; i = *(DWORD*)(i + 4)) {
			result = (**(int(__thiscall***)(DWORD, DWORD, DWORD))(i - 4))(i - 4, a2, a3);
		}
	}
	return result;
}

#pragma endregion
