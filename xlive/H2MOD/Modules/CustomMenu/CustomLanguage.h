#pragma once

void InitCustomLanguage();
void DeinitCustomLanguage();
char* add_cartographer_label(int label_menu_id, int label_id, const char* label);
char* add_cartographer_label(int label_menu_id, int label_id, const char* label, bool is_dynamic);
char* add_cartographer_label(int label_menu_id, int label_id, int labelBufferLen);
char* add_cartographer_label(int label_menu_id, int label_id, int labelBufferLen, bool is_dynamic);
void setCustomLanguage(int);
void setCustomLanguage(int, int);
bool reloadCustomLanguages();
void saveCustomLanguages();

typedef struct {
	int lang_base;
	int lang_variant;
	bool other;
	char* lang_name;
	char* font_table_filename;
	std::unordered_map<int, std::unordered_map<int, char*>>* label_map;
} custom_language;

extern std::vector<custom_language*> custom_languages;

