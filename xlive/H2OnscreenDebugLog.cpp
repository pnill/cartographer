#include "H2OnscreenDebugLog.h"
#include "H2Startup.h"
#include "stdafx.h"

char** DebugStr;
int DebugTextArrayLenMax = 160;
int DebugTextArrayPos = 0;
bool DebugTextDisplay = false;
FILE* debugFile = NULL;
bool initialisedDebugText = false;


int getDebugTextArrayMaxLen() {
	return DebugTextArrayLenMax;
}

void addDebugText(wchar_t* wtext) {
	int lenInput = wcslen(wtext);
	char* text = (char*)malloc(sizeof(char) * lenInput + 1);
	snprintf(text, lenInput + 1, "%ls", wtext);
	addDebugText(text);
	free(text);
}

void addDebugText(const char* text) {
	int buflen = strlen(text) + 1;
	char* text2 = (char*)malloc(sizeof(char) * buflen);
	memcpy(text2, text, sizeof(char) * buflen);
	addDebugText(text2);
	free(text2);
}

void addDebugText(char* text) {
	if (!initialisedDebugText) return;

	int lenInput = strlen(text);

	char* endChar = strchr(text, '\n');
	if (endChar) {
		lenInput = endChar - text;
	}

	DebugTextArrayPos++;
	if (DebugTextArrayPos >= DebugTextArrayLenMax) {
		DebugTextArrayPos = 0;
	}

	free(DebugStr[DebugTextArrayPos]);
	DebugStr[DebugTextArrayPos] = (char*)malloc(sizeof(char) * lenInput + 1);
	strncpy(DebugStr[DebugTextArrayPos], text, lenInput);
	memset(DebugStr[DebugTextArrayPos] + lenInput, 0, 1);

	if (debugFile != NULL) {
		char* debug_text = (char*)malloc(sizeof(char) * lenInput + 2);
		strncpy(debug_text, text, lenInput);
		memset(debug_text + lenInput, '\n', 1);
		memset(debug_text + lenInput + 1, 0, 1);
		fputs(debug_text, debugFile);
		fflush(debugFile);
		free(debug_text);
	}

	if (endChar) {
		addDebugText(endChar + 1);
	}
}

void initDebugText() {
	initialisedDebugText = true;
	DebugStr = (char**)malloc(sizeof(char*) * DebugTextArrayLenMax);
	for (int i = 0; i < DebugTextArrayLenMax; i++) {
		DebugStr[i] = (char*)calloc(1, sizeof(char));
	}
	wchar_t debug_file_path[1024];
	swprintf(debug_file_path, 1024, L"%wsh2onscreendebug.log", H2ProcessFilePath);
	debugFile = _wfopen(debug_file_path, L"w");
	char awerg[1034];
	sprintf(awerg, "PATH: %ws", debug_file_path);
	addDebugText(awerg);
	addDebugText("Initialised On Screen Debug Text.");
}

char* getDebugText(int ordered_index) {
	if (ordered_index < DebugTextArrayLenMax) {
		int array_index = ((DebugTextArrayPos - ordered_index) + DebugTextArrayLenMax) % DebugTextArrayLenMax;
		return DebugStr[array_index];
	}
	return "";
}

void setDebugTextDisplay(bool setOn) {
	DebugTextDisplay = setOn;
}

bool getDebugTextDisplay() {
	return DebugTextDisplay;
}
