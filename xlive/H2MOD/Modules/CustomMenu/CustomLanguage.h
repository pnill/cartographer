#pragma once

void InitCustomLanguage();
void DeinitCustomLanguage();
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

