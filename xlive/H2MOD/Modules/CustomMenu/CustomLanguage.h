#pragma once

static const int CMLabelMenuId_EditCrosshair = 0xFF000010;
static const int CMLabelMenuId_EditFOV = 0xFF00000F;
static const int CMLabelMenuId_VehicleEditFOV = 0xFF000020;
static const int CMLabelMenuId_EditHz = 0xFF000018;
static const int CMLabelMenuId_EditFPS = 0xFF00000E;
static const int CMLabelMenuId_EditStaticLoD = 0xFF000014;
static const int CMLabelMenuId_EditCrosshairSize = 0xFF000015;
static const int CMLabelMenuId_VKeyTest = 0xFF00000B;
static const int CMLabelMenuId_EscSettings = 0xFF000004;
static const int CMLabelMenuId_Error = 0xFF000001;
static const int CMLabelMenuId_AccountList = 0xFF000009;
static const int CMLabelMenuId_AccountEdit = 0xFF00000A;
static const int CMLabelMenuId_AccountCreate = 0xFF00000C;
static const int CMLabelMenuId_EditHudGui = 0xFF000007;
static const int CMLabelMenuId_ToggleSkulls = 0xFF000008;
static const int CMLabelMenuId_OtherSettings = 0xFF00000D;
static const int CMLabelMenuId_AdvSettings = 0xFF000005;
static const int CMLabelMenuId_AdvLobbySettings = 0xFF000016;
static const int CMLabelMenuId_Credits = 0xFF000006;
static const int CMLabelMenuId_Update = 0xFF000011;

void InitCustomLanguage();
void DeinitCustomLanguage();
char* add_cartographer_label(int label_menu_id, int label_id, const char* label);
char* add_cartographer_label(int label_menu_id, int label_id, const char* label, bool is_dynamic);
char* add_cartographer_label(int label_menu_id, int label_id, int labelBufferLen);
char* add_cartographer_label(int label_menu_id, int label_id, int labelBufferLen, bool is_dynamic);
char* H2CustomLanguageGetLabel(int label_menu_id, int label_id);
void setCustomLanguage(int);
void setCustomLanguage(int, int);
bool reloadCustomLanguages();
void saveCustomLanguages();
void combineCartographerLabels(int menuId, int lbl1, int lbl2, int lblCmb);

enum e_language_ids
{
	_lang_id_english,
	_lang_id_japanese,
	_lang_id_german,
	_lang_id_french,
	_lang_id_spanish,
	_lang_id_italian,
	_lang_id_korean,
	_lang_id_chinese,
	_lang_id_end
};

typedef struct {
	int lang_base;
	int lang_variant;
	bool other;
	char* lang_name;
	char* font_table_filename;
	std::unordered_map<int, std::unordered_map<int, char*>>* label_map;
} custom_language;

extern std::vector<custom_language*> custom_languages;

