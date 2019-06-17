#include "stdafx.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"

char** DebugStr;
int DebugTextArrayLenMax = 160;
int DebugTextArrayPos = 0;
bool DebugTextDisplay = false;
bool initialisedDebugText = false;
h2log* onscreendebuglog = nullptr;

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

	onscreendebuglog->debug(text);

	if (endChar) {
		addDebugText(endChar + 1);
	}
}

void initDebugText() {
	initialisedDebugText = true;
	onscreendebuglog = h2log::create("OnScreenDebug", prepareLogFileName(L"h2onscreendebug"));
	DebugStr = (char**)malloc(sizeof(char*) * DebugTextArrayLenMax);
	for (int i = 0; i < DebugTextArrayLenMax; i++) {
		DebugStr[i] = (char*)calloc(1, sizeof(char));
	}
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
