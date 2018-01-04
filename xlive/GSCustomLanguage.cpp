#include "GSCustomLanguage.h"
#include "H2Startup.h"
#include "GSUtils.h"
#include "Hook.h"
#include "H2OnscreenDebugLog.h"
#include "H2Config.h"

#pragma region Custom Language

std::vector<custom_language*> custom_languages;
std::vector<custom_language*> custom_languages_tempclone;
std::map<int, std::map<int, char*>> cartographer_label_map;
std::map<int, std::map<int, char*>> cartographer_label_map_dyn;

bool custom_labels_updated = false;
bool current_language_isGarbage = false;
custom_language* current_language = 0;
int current_language_main = -1;
int current_language_sub = 0;

#pragma region File I/O

char* add_label(std::map<int, std::map<int, char*>> &label_map, int label_menu_id, int label_id, char* label) {
	if (label_map.count(label_menu_id) && label_map[label_menu_id].count(label_id))
		free(label_map[label_menu_id][label_id]);
	int label_buflen = (label ? strlen(label) : 0) + 1;
	char* new_label = (char*)malloc(sizeof(char) * label_buflen);
	if (label)
		memcpy(new_label, label, sizeof(char) * label_buflen);
	new_label[label_buflen - 1] = 0;
	return label_map[label_menu_id][label_id] = new_label;
}

char* add_cartographer_label(int label_menu_id, int label_id, char* label, bool is_dynamic) {
	if (is_dynamic) {
		return add_label(cartographer_label_map_dyn, label_menu_id, label_id, label);
	}
	if (!label)
		return 0;
	return add_label(cartographer_label_map, label_menu_id, label_id, label);
}

char* add_cartographer_label(int label_menu_id, int label_id, char* label) {
	return add_cartographer_label(label_menu_id, label_id, label, false);
}

char* add_label(std::map<int, std::map<int, char*>> &label_map, int label_menu_id, int label_id, int labelBufferLen) {
	if (label_map.count(label_menu_id) && label_map[label_menu_id].count(label_id))
		free(label_map[label_menu_id][label_id]);
	char* new_label = (char*)malloc(labelBufferLen);
	new_label[0] = new_label[1] = 0;
	return label_map[label_menu_id][label_id] = new_label;
}

char* add_cartographer_label(int label_menu_id, int label_id, int labelBufferLen, bool is_dynamic) {
	if (is_dynamic) {
		return add_label(cartographer_label_map_dyn, label_menu_id, label_id, labelBufferLen);
	}
	return add_label(cartographer_label_map, label_menu_id, label_id, labelBufferLen);
}

char* add_cartographer_label(int label_menu_id, int label_id, int labelBufferLen) {
	return add_cartographer_label(label_menu_id, label_id, labelBufferLen, false);
}

char* get_cartographer_label(int label_menu_id, int label_id, __int8 is_dynamic) {
	if (is_dynamic & 0b10) {
		std::map<int, std::map<int, char*>> &label_map = cartographer_label_map_dyn;
		if (label_map.count(label_menu_id) && label_map[label_menu_id].count(label_id))
			return label_map[label_menu_id][label_id];
	}
	if (is_dynamic & 0b01) {
		std::map<int, std::map<int, char*>> &label_map = cartographer_label_map;
		if (label_map.count(label_menu_id) && label_map[label_menu_id].count(label_id))
			return label_map[label_menu_id][label_id];
	}
	return 0;
}

char* get_cartographer_label(int label_menu_id, int label_id) {
	return get_cartographer_label(label_menu_id, label_id, 0b11);
}

char* add_custom_label(custom_language* language, int label_menu_id, int label_id, char* label) {
	custom_labels_updated = true;
	return add_label(*language->label_map, label_menu_id, label_id, label);
}

char* get_custom_label(custom_language* language, int label_menu_id, int label_id, char* label) {
	std::map<int, std::map<int, char*>> &label_map = *language->label_map;
	if (label_map.count(label_menu_id) && label_map[label_menu_id].count(label_id))
		return label_map[label_menu_id][label_id];
	if (label == 0)
		return 0;
	return add_custom_label(language, label_menu_id, label_id, label);
}

custom_language* get_custom_language(int lang_base, int lang_variant) {
	for (auto const &ent1 : custom_languages) {
		if (ent1->lang_base == lang_base && ent1->lang_variant == lang_variant) {
			return ent1;
		}
	}
	return 0;
}

int add_custom_language(custom_language* custom_lang) {

	if (get_custom_language(custom_lang->lang_base, custom_lang->lang_variant)) {
		return 1;//lang id's are already taken
	}

	custom_languages.push_back(custom_lang);

	return 0;
}

custom_language* create_custom_language(int lang_base, int lang_variant, bool other, char* name, char* font_table_filename) {
	custom_language* custom_lang = (custom_language*)malloc(sizeof(custom_language));

	custom_lang->lang_base = lang_base;

	custom_lang->lang_variant = lang_variant;

	custom_lang->other = other;

	int name_len = strlen(name) + 1;
	custom_lang->lang_name = (char*)malloc(sizeof(char) * name_len);
	strcpy(custom_lang->lang_name, name);

	int filename_len = strlen(font_table_filename) + 1;
	custom_lang->font_table_filename = (char*)malloc(sizeof(char) * filename_len);
	strcpy(custom_lang->font_table_filename, font_table_filename);

	custom_lang->label_map = new std::map<int, std::map<int, char*>>();

	if (add_custom_language(custom_lang) == 0) {
		return custom_lang;
	}

	delete custom_lang->label_map;
	free(custom_lang->lang_name);
	free(custom_lang->font_table_filename);
	free(custom_lang);
	return 0;
}

void delete_custom_language(custom_language* custom_lang) {
	for (auto const &ent1 : *custom_lang->label_map) {
		for (auto const &ent2 : ent1.second) {
			int label_menu_id = ent1.first;
			int label_id = ent2.first;
			free((*custom_lang->label_map)[label_menu_id][label_id]);
		}
	}
	(*custom_lang->label_map).clear();
	delete custom_lang->label_map;

	free(custom_lang->lang_name);

	free(custom_lang->font_table_filename);

	free(custom_lang);
}

void initialise_core_languages() {
	create_custom_language(0, 0, false, "English", "font_table.txt");
	create_custom_language(1, 0, false, "Japanese", "font_table_jpn.txt");
	create_custom_language(2, 0, false, "German", "font_table.txt");
	create_custom_language(3, 0, false, "French", "font_table.txt");
	create_custom_language(4, 0, false, "Spanish", "font_table.txt");
	create_custom_language(5, 0, false, "Italian", "font_table.txt");
	create_custom_language(6, 0, false, "Korean", "font_table_kor.txt");
	create_custom_language(7, 0, false, "Chinese", "font_table_cht.txt");
}

bool read_custom_labels() {
	bool read_file_success = false;

	addDebugText("Reading Custom Languages File...");

	bool prev_capture = H2Config_custom_labels_capture_missing;
	H2Config_custom_labels_capture_missing = false;

	wchar_t labels_file_path[1024];
	swprintf(labels_file_path, 1024, L"%wsh2customlanguage.ini", H2ProcessFilePath);
	FILE* labelsFile = _wfopen(labels_file_path, L"rb");
	addDebugText(labels_file_path);

	if (labelsFile == NULL) {
		addDebugText("Language file does not exist!");
		custom_labels_updated = true;
	}
	else {
		bool readingVersion = true;
		char* fileLine;
		bool keepReading = true;
		int lineNumber = 0;
		int& dword_412818 = *(int*)((char*)H2BaseAddr + 0x412818);
		custom_language* curr_lang = get_custom_language(dword_412818, 0);
		while (keepReading && GetFileLine(labelsFile, fileLine)) {
			lineNumber++;
			if (fileLine) {
				int fileLineLength = strlen(fileLine);
				if (readingVersion || fileLine[0] == '[') {
					if (readingVersion && fileLineLength >= 27 && strstr(&fileLine[1], "H2CustomLanguageVersion:")) {
						int version = 0;
						sscanf(&fileLine[25], "%d", &version);
						if (version != 1) {
							keepReading = false;
							addDebugText("Incorrect version number!");
						}
						readingVersion = false;
					}
					else if (readingVersion) {
						//Won't process anything until version header appears.
					}
					else if (fileLineLength >= 21 && strstr(&fileLine[1], "Language:")) {
						int lang_base = -1;
						int lang_variant = -1;
						bool other = false;
						char* name;
						char* fontFilename;
						int ele_end = -1;
						int ele_start = -1;
						int ele_id = -2;
						for (int i = 1; i < fileLineLength; i++) {
							if (fileLine[i] == ':') {
								ele_start = ele_end;
								ele_end = i;
								ele_id++;
								if (ele_id == 0 || ele_id == 1 || ele_id == 2) {
									fileLine[ele_end] = 0;
									int number = strtol(&fileLine[ele_start + 1], NULL, 10);
									if (ele_end - ele_start + 1 <= 0 || number < 0) {
										break;
									}
									if (ele_id == 0) {
										lang_base = number;
									}
									else if (ele_id == 1) {
										lang_variant = number;
									}
									else if (ele_id == 2) {
										other = (bool)number;
									}
								}
								else if (ele_id == 3 || ele_id == 4) {
									fileLine[ele_end] = 0;
									int ele_len = strlen(&fileLine[ele_start + 1]);
									if (ele_len >= 255 || ele_len <= 0) {
										if (ele_id == 4) {
											free(name);
										}
										break;
									}
									char* alloc_text = (char*)malloc(ele_len + 1);
									strcpy(alloc_text, &fileLine[ele_start + 1]);
									if (ele_id == 3) {
										name = alloc_text;
									}
									else if (ele_id == 4) {
										fontFilename = alloc_text;
										ele_id = -3;
										addDebugText("Successfully interpreted Lang header.");
										read_file_success = true;
										break; // success
									}
								}
								else if (ele_id >= 4) {
									break;
								}
							}
							else if (ele_id == -1 || ele_id == 0 || ele_id == 1) {
								if (fileLine[i] < '0' || fileLine[i] > '9') {
									break;
								}
							}
						}

						bool free_building = false;

						if (ele_id != -3) {
							keepReading = false;
							free_building = true;
							addDebugText("Failed to interpret Language header!");
						}
						else {
							custom_language* next_lang = create_custom_language(lang_base, lang_variant, other, name, fontFilename);
							if (next_lang == 0 && lang_variant == 0 && lang_base >= 0 && lang_base <= 7) {
								next_lang = get_custom_language(lang_base, lang_variant);
								if (strcmp(name, next_lang->lang_name) || strcmp(fontFilename, next_lang->font_table_filename)) {
									//throw warning that not the same as core langs.
									addDebugText("Base Lang clone is not same as core lang!");
								}
								else {
									curr_lang = next_lang; //swapped to next language
									addDebugText("Reading core lang labels...");
								}
								free_building = true;
							}
							else if (next_lang == 0) {
								keepReading = false;
								free_building = true;
								addDebugText("Failed to insert new language!");
							}
							else {
								curr_lang = next_lang; //swapped to next language
								addDebugText("Reading custom lang labels...");
							}
						}

						if (free_building) {
							free(name);
							free(fontFilename);
						}
					}
				}
				else if (fileLine[0] == '#' || fileLine[0] == ';') {
					//Ignore commented lines.
				}
				else {
					if (fileLineLength >= 22) {
						int label_menu_id = 0;
						int label_id = 0;
						int scanResult = sscanf(fileLine, "%x : %x =", &label_menu_id, &label_id);
						if (scanResult == 2) {
							add_custom_label(curr_lang, label_menu_id, label_id, &fileLine[21]);
						}
					}
				}
				free(fileLine);
			}
		}

		fclose(labelsFile);
		custom_labels_updated = false;
	}
	H2Config_custom_labels_capture_missing = prev_capture;
	addDebugText("Finished Reading Custom Languages File.");
	return read_file_success;
}

void langCopyToFrom(std::vector<custom_language*>& langDest, std::vector<custom_language*>& langSrc, bool completelyEraseDest) {
	if (completelyEraseDest) {
		for (auto const &ent1 : langDest) {
			delete_custom_language(ent1);
		}
	}
	langDest.clear();
	for (auto const &ent1 : langSrc) {
		langDest.push_back(ent1);
	}
	langSrc.clear();
}

bool reloadCustomLanguages() {
	langCopyToFrom(custom_languages_tempclone, custom_languages, true);
	initialise_core_languages();
	if (!read_custom_labels()) {
		langCopyToFrom(custom_languages, custom_languages_tempclone, false);
		return false;
	}
	else {
		for (auto const &ent1 : custom_languages_tempclone) {
			if (ent1 != current_language) {
				delete_custom_language(ent1);
			}
		}
		custom_languages_tempclone.clear();
		current_language_isGarbage = true;
		setCustomLanguage(current_language_main, current_language_sub);
		return true;
	}
}

void write_custom_labels() {
	if (custom_labels_updated) {
		bool prev_capture = H2Config_custom_labels_capture_missing;
		H2Config_custom_labels_capture_missing = false;

		wchar_t labels_file_path[1024];
		swprintf(labels_file_path, 1024, L"%wsh2customlanguage.ini", H2ProcessFilePath);
		FILE* labelsFile = _wfopen(labels_file_path, L"wb");
		addDebugText(labels_file_path);

		if (labelsFile == NULL) {
			addDebugText("Error: Cannot Open Language File!");
		}
		else {
			fputs("[H2CustomLanguageVersion:1]\n", labelsFile);
			for (auto const &lang : custom_languages) {
				fputs("\n", labelsFile);
				int line_header_len = 20 + 10 + 10 + 10 + strlen(lang->lang_name) + strlen(lang->font_table_filename);
				char* line_header = (char*)malloc(sizeof(char) * line_header_len);
				snprintf(line_header, line_header_len, "[Language:%d:%d:%d:%s:%s:]\n", lang->lang_base, lang->lang_variant, lang->other ? 1 : 0, lang->lang_name, lang->font_table_filename);
				fputs(line_header, labelsFile);
				free(line_header);

				for (auto const &ent1 : *(lang->label_map)) {
					for (auto const &ent2 : ent1.second) {
						int label_menu_id = ent1.first;
						int label_id = ent2.first;
						char* label_literal = ent2.second;
						char* label_escaped = custom_label_escape(label_literal);
						int line_len = 23 + strlen(label_escaped);
						char* line_entry = (char*)malloc(line_len);
						snprintf(line_entry, line_len, "%08x : %08x =%s\n", label_menu_id, label_id, label_escaped);
						fputs(line_entry, labelsFile);
						free(line_entry);
						free(label_escaped);
					}
				}
				fputs("\n", labelsFile);
			}

			fclose(labelsFile);
		}
		H2Config_custom_labels_capture_missing = prev_capture;
	}
}

void saveCustomLanguages() {
	write_custom_labels();
}

#pragma endregion

typedef char*(__stdcall *tH2GetLabel)(int, int, int, int);
tH2GetLabel pH2GetLabel;
char* __stdcall H2GetLabel(int a1, int label_id, int a3, int a4) { //sub_3defd
	//int label_menu_id = *(int*)(*(int*)a1 + 8 * a3 + 4);
	int label_menu_id = a3;
	char* label = get_cartographer_label(label_menu_id, label_id, 0b10);
	if (label)
		return label;
	label = get_custom_label(current_language, label_menu_id, label_id, 0);
	if (label)
		return label;
	label = get_cartographer_label(label_menu_id, label_id, 0b01);
	if (!label && a1)
		label = pH2GetLabel(a1, label_id, a3, a4);
	//if (strcmp(label, "PROFILE NAME") == 0) {
	//	return label;//in order to breakpoint and get label_id's.
	//}
	if (!H2Config_custom_labels_capture_missing) {
		return label;
	}
	return get_custom_label(current_language, label_menu_id, label_id, label);
}

char* H2ServerGetLabel(int label_menu_id, int label_id) {
	char* label = get_cartographer_label(label_menu_id, label_id, 0b10);
	if (label)
		return label;
	return get_cartographer_label(label_menu_id, label_id, 0b01);
}

char* H2CustomLanguageGetLabel(int label_menu_id, int label_id) {
	if (H2IsDediServer)
		return H2ServerGetLabel(label_menu_id, label_id);
	return H2GetLabel(0, label_id, label_menu_id, 0);
}


void combineCartographerLabels(int menuId, int lbl1, int lbl2, int lblCmb) {
	char* label_1 = H2CustomLanguageGetLabel(menuId, lbl1);
	char* label_2 = H2CustomLanguageGetLabel(menuId, lbl2);
	int label_len = (label_1 ? strlen(label_1) : 0) + (label_2 ? strlen(label_2) : 0) + 1;
	char* label_combined = (char*)malloc(label_len);
	snprintf(label_combined, label_len, label_1 ? label_1 : "", label_2 ? label_2 : "");
	add_cartographer_label(menuId, lblCmb, label_combined, true);
	free(label_combined);
}

void setGameLanguage() {
	BYTE* HasLoadedLanguage = (BYTE*)((char*)H2BaseAddr + 0x481908);
	*HasLoadedLanguage = 0;

	int& dword_412818 = *(int*)((char*)H2BaseAddr + 0x412818);
	if (current_language_main >= 0) {
		dword_412818 = current_language_main;
	}
	else {
		switch (GetUserDefaultLangID() & 0x3FF)
		{
		case 0x11:
			dword_412818 = 1;//Japanese
			break;
		case 7:
			dword_412818 = 2;//German
			break;
		case 0xC:
			dword_412818 = 3;//French
			break;
		case 0xA:
			dword_412818 = 4;//Spanish
			break;
		case 0x10:
			dword_412818 = 5;//Italian
			break;
		case 0x12:
			dword_412818 = 6;//Korean
			break;
		case 4:
			dword_412818 = 7;//Chinese
			break;
		default:
			dword_412818 = 0;//English
			break;
		}
	}
}

DWORD langAfterJmpAddr;
__declspec(naked) void getSystemLanguageMethodJmp() {
	setGameLanguage();
	__asm {
		jmp langAfterJmpAddr
	}
}


typedef char*(__cdecl *tsub_31b97)(int, int);
tsub_31b97 psub_31b97;
char* __cdecl sub_31b97(int buff_len, int a2)//Font Table Filename Override
{
	char* result = psub_31b97(buff_len, a2);
	strcpy_s(result, buff_len, current_language->font_table_filename);
	return result;
}

#pragma endregion


void setCustomLanguage(int main, int variant) {
	H2Config_custom_labels_capture_missing = false;

	if (main >= 0 && main <= 7)
		current_language_main = main;
	else
		current_language_main = -1;
	
	setGameLanguage();

	int GameGlobals = (int)*(int*)((char*)H2BaseAddr + 0x482D3C);
	if (GameGlobals) {
		BYTE& EngineMode = *(BYTE*)(GameGlobals + 0x8);
		EngineMode = 1;
		BYTE& QuitLevel = *(BYTE*)((char*)H2BaseAddr + 0x482251);
		QuitLevel = 1;
	}

	custom_language* old_language = current_language;
	int& dword_412818 = *(int*)((char*)H2BaseAddr + 0x412818);
	if ((current_language = get_custom_language(dword_412818, variant)) == 0)
		current_language = get_custom_language(dword_412818, 0);
	current_language_sub = current_language->lang_variant;

	char langcodeprintbuffer[30];
	snprintf(langcodeprintbuffer, 30, "language_code = %dx%d", current_language_main, current_language_sub);
	addDebugText(langcodeprintbuffer);

	if (GameGlobals) {
		BYTE* LoadedFonts = (BYTE*)((char*)H2BaseAddr + 0x47e7c0);
		*LoadedFonts = 0;
		void*(*sub_31dff)() = (void*(*)())((char*)H2BaseAddr + 0x31dff);
		sub_31dff();
	}
	if (current_language_isGarbage) {
		delete_custom_language(old_language);
		current_language_isGarbage = false;
	}
}

void setCustomLanguage(int main) {
	setCustomLanguage(main, 0);
}


static void overrideCoreH2Labels() {
	/*custom_language* lang_english = get_custom_language(0, 0);
	add_custom_label(lang_english, 0x000003fd, 0x0a000285, "MULTIPLAYER");

	custom_language* lang_japanese = get_custom_language(1, 0);
	add_custom_label(lang_japanese, 0x000003fd, 0x0a000285, "MULTIPLAYER");

	custom_language* lang_german = get_custom_language(2, 0);
	add_custom_label(lang_german, 0x000003fd, 0x0a000285, "MEHRSPIELER");

	custom_language* lang_french = get_custom_language(3, 0);
	add_custom_label(lang_french, 0x000003fd, 0x0a000285, "MULTIJOUEUR");

	custom_language* lang_spanish = get_custom_language(4, 0);
	add_custom_label(lang_spanish, 0x000003fd, 0x0a000285, "MULTIJUGADOR");

	custom_language* lang_italian = get_custom_language(5, 0);
	add_custom_label(lang_italian, 0x000003fd, 0x0a000285, "MULTIPLAYER");

	custom_language* lang_korean = get_custom_language(6, 0);
	add_custom_label(lang_korean, 0x000003fd, 0x0a000285, "MULTIPLAYER");

	custom_language* lang_chinese = get_custom_language(7, 0);
	add_custom_label(lang_chinese, 0x000003fd, 0x0a000285, "MULTIPLAYER");
	*/

	for (int i = 0; i <= 7; i++) {
		custom_language* lang = get_custom_language(i, 0);
		//add_custom_label(lang, 0x000003fd, 0x09000284, "");
		add_custom_label(lang, 0x000003fd, 0x05000986, "CARTOGRAPHER");
	}
} 

void initGSCustomLanguage() {
	if (!H2IsDediServer) {
		setGameLanguage();

		initialise_core_languages();
		overrideCoreH2Labels();
		read_custom_labels();

		DWORD dwBack;

		pH2GetLabel = (tH2GetLabel)DetourClassFunc((BYTE*)H2BaseAddr + 0x3defd, (BYTE*)H2GetLabel, 8);
		VirtualProtect(pH2GetLabel, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//Hook the function that sets the font table filename.
		psub_31b97 = (tsub_31b97)DetourFunc((BYTE*)H2BaseAddr + 0x31b97, (BYTE*)sub_31b97, 6);
		VirtualProtect(psub_31b97, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//Hook the part where it sets the global language id.
		langAfterJmpAddr = (DWORD)(H2BaseAddr + 0x3828c);
		DetourFunc((BYTE*)H2BaseAddr + 0x3820d, (BYTE*)getSystemLanguageMethodJmp, 6);

		bool redoCapture = H2Config_custom_labels_capture_missing;
		setCustomLanguage(H2Config_language_code_main, H2Config_language_code_variant);
		H2Config_custom_labels_capture_missing = redoCapture;
	}
}

void deinitGSCustomLanguage() {
	if (!H2IsDediServer) {
		write_custom_labels();
	}
}
