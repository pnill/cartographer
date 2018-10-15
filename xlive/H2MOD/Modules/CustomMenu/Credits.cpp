#include "Credits.h"
#include "CustomMenuGlobals.h"

CreditsMenu_list *credits_list_widget;
Credits_screen_with_menu *credits_screen_with_menu;

static bool CMButtonHandler_Credits(int button_id) {
	return false;
}

int __cdecl CustomMenu_Credits_(int a1) {
	return CustomMenu_CallHead_(a1, (DWORD*)credits_list_widget, (DWORD*)credits_screen_with_menu, (DWORD)CMButtonHandler_Credits, 16, 272);
}

void GSCustomMenuCall_Credits_() {
	credits_list_widget = new CreditsMenu_list(CreditsMenu_ID);
	//	Credits_screen_with_menu(int menu_id, int label_id_description, int label_id_title)
	credits_screen_with_menu = new Credits_screen_with_menu(CreditsMenu_ID, 0xFFFFFFF1, 0xFFFFFFF0);

	int WgitScreenfunctionPtr = (int)(CustomMenu_Credits_);
	CallWgit_(WgitScreenfunctionPtr);
}
