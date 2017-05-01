#include "H2OnscreenDebugLog.h"
#include "stdafx.h"

char** DebugStr;
int DebugTextArrayLenMax = 160;
int DebugTextArrayPos = 0;
bool DebugTextDisplay = false;


int getDebugTextArrayMaxLen() {
	return DebugTextArrayLenMax;
}

void addDebugText(char* text) {

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
	if (endChar) {
		addDebugText(endChar + 1);
	}
}

void initDebugText() {
	DebugStr = (char**)malloc(sizeof(char*) * DebugTextArrayLenMax);
	for (int i = 0; i < DebugTextArrayLenMax; i++) {
		DebugStr[i] = (char*)calloc(1, sizeof(char));
	}
	addDebugText("Initialised Debug Text");
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
