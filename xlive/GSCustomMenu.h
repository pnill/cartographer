#pragma once

void initGSCustomMenu();
void deinitGSCustomMenu();

void GSCustomMenuCall_Language();
void GSCustomMenuCall_Guide();
void GSCustomMenuCall_Esc();
void GSCustomMenuCall_Error_Inner(int menuId, int title, int description);
void GSCustomMenuCall_Error_Outer(int menuId, int title, int description);
void GSCustomMenuCall_Error_Inner(int menuIdTitle, int title, int menuIdDesc, int description);
void GSCustomMenuCall_Error_Outer(int menuIdTitle, int title, int menuIdDesc, int description);

extern const int CMLabelMenuId_Error;
extern const int CMLabelMenuId_AccountEdit;
extern const int CMLabelMenuId_AccountCreate;

void RefreshToggleDisableControllerAimAssist();
