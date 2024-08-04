#include "stdafx.h"

#include "CustomMenu.h"

#include "interface/screens/screen_xbox_live_task_progress_dialog.h"

#include "H2MOD/GUI/ImGui_Integration/imgui_handler.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/Accounts/Accounts.h"
#include "H2MOD/Modules/Shell/Config.h"

#include "interface/screens/screen_virtual_keyboard.h"
#include "interface/screens/screen_cartographer_errors.h"
#include "interface/screens/screen_cartographer_account_manager.h"

extern DWORD H2BaseAddr;
extern bool H2IsDediServer;

bool g_force_cartographer_update = false;

#pragma region CM_Language

#pragma region CM_Language_Sub

//int CM_Language_Main = 0;
//int Language_Sub_Count = 0;
//std::map<int, int> cm_lang_variant_map;
//std::map<int, int> cm_lang_other_lang_map;

//static bool CMButtonHandler_Language_Sub(int button_id) {
//	addDebugText("Button ID: %d", button_id);
//	setCustomLanguage(CM_Language_Main == -1 ? cm_lang_other_lang_map[button_id] : CM_Language_Main, cm_lang_variant_map[button_id]);
//	return false;
//}

//bool GSCustomMenuCall_Language_Sub() {
//	CM_Language_Sub_Setup_Buttons();
//	if (Language_Sub_Count > 0) {
//		ui_custom_cartographer_load_menu(CustomMenu_Language_Sub);
//		return true;
//	}
//	else {
//		c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_no_custom_language_categorised_as_other);
//	}
//	return false;
//}


#pragma endregion

#pragma region CM_Language_Main

//void toggleLanguageCapture() {
//	H2Config_custom_labels_capture_missing = !H2Config_custom_labels_capture_missing;
//}

//static bool CMButtonHandler_Language(int button_id) {
//	if (button_id == 0) {
//		setCustomLanguage(-1);
//		return true;
//	}
//	else if (button_id > 0 && button_id < 2 + 8) {
//		CM_Language_Main = button_id - 2;
//		GSCustomMenuCall_Language_Sub();
//		return false;
//	}
//	else if (button_id == 2 + 8 + 0) {
//		if (!reloadCustomLanguages()) {
//			c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_error_reading_custom_language_file);
//		}
//		return false;
//	}
//	else if (button_id == 2 + 8 + 1) {
//		saveCustomLanguages();
//		return true;
//	}
//	else if (button_id == 2 + 8 + 2) {
//		toggleLanguageCapture();
//		return false;
//	}
//	return false;
//}

#pragma endregion

#pragma endregion

#pragma region Setting_Modifications

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

#pragma region CM_Invalid_Login_Token

void* ui_load_cartographer_invalid_login_token() 
{
	c_cartographer_account_manager_menu::accountingGoBackToList = true;
	c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(true);
	return c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_invalid_login_token);
}

#pragma endregion

void XUiShowSignInH2() {
	if (!c_cartographer_account_manager_menu::IsAccountingActiveHandle() && ReadH2Accounts()) {
		c_cartographer_account_manager_menu::open_account_list_context();
	}
	else {
		if (!c_cartographer_account_manager_menu::IsAccountingActiveHandle())
			c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_account_already_in_use);
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

void InitCustomMenu() {

#pragma region Init_Cartographer_Labels

	/*add_cartographer_label(CMLabelMenuId_Language, 11, "Reload Language File");
	add_cartographer_label(CMLabelMenuId_Language, 12, "Save Language File");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFF002, "Ignore New Labels");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFF003, "Capture New Labels");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF2, "--- Base %s Variant ---");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF3, "Select Language Variant");
	add_cartographer_label(CMLabelMenuId_Language, 0xFFFFFFF4, "Select the variant of the language you would like to play the game in.");*/

#pragma endregion

	if (H2IsDediServer)
		return;

	psub_23f6b7 = (tsub_23f6b7)DetourFunc((BYTE*)H2BaseAddr + 0x23f6b7, (BYTE*)sub_23f6b7, 7);

	//psub_248beb = (tsub_248beb)DetourClassFunc((BYTE*)H2BaseAddr + 0x248beb, (BYTE*)sub_248beb, 8);

	RefreshToggleIngameKeyboardControls();

	c_screen_with_menu::apply_patches();

	// replace brightness menu for testing
	// replace_brightness_menu();
}

void DeinitCustomMenu() {
	if (H2IsDediServer)
		return;

}