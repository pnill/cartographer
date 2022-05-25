#pragma once

void InitCustomMenu();
void DeinitCustomMenu();

void GSCustomMenuCall_Language();
void GSCustomMenuCall_Update_Note();
void GSCustomMenuCall_Guide();
void GSCustomMenuCall_Esc();
void GSCustomMenuCall_Invalid_Login_Token();

extern const int CMLabelMenuId_Error;
extern const int CMLabelMenuId_AccountEdit;
extern const int CMLabelMenuId_AccountCreate;
extern const int CMLabelMenuId_Update;

extern bool CMForce_Update;

void RefreshTogglexDelay();

void GSCustomMenuCall_Obscure();
