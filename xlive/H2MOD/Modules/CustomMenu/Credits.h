#pragma once
#include "CustomMenuGlobals.h"
#include <stdio.h>

int __cdecl CustomMenu_Credits_(int a1);
void GSCustomMenuCall_Credits_();


class CreditsMenu_list : c_list_widget
{	
public:
	CreditsMenu_list(int menu_id)
		: c_list_widget(menu_id)
	{
	};

};

class Credits_screen_with_menu : c_screen_with_menu
{
public:
	Credits_screen_with_menu(int menu_id, int label_id_description, int label_id_title)
		: c_screen_with_menu(menu_id,label_id_description,label_id_title)
	{
		this->FuncPtrHelper = &CustomMenu_Credits_;
	};
};

extern CreditsMenu_list *credits_list_widget;
extern Credits_screen_with_menu *credits_screen_with_menu;
