#include "stdafx.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"

char** DebugStr;
int DebugTextArrayLenMax = 160;
int DebugTextArrayPos = 0;
int DebugTextCount = 0;
bool DebugTextDisplay = false;
bool initialisedDebugText = false;

int getDebugTextArrayMaxLen() {
	return DebugTextArrayLenMax;
}

int getDebugTextDisplayCount() {
	return DebugTextCount;
}

void addDebugText(const wchar_t* format, ...)
{
	va_list valist;
	va_start(valist, format);

	/* get the formatted buffer size */
	int stringLength = _vscwprintf(format, valist) + 1; // +1 adds null characeter, "_vscwprintf" doesn't add it

	if (stringLength == -1)
	{
		LOG_TRACE_GAME("{} - error trying to get string length size", __FUNCTION__);
		return;
	}

	wchar_t* textBufferW = (wchar_t*)calloc(stringLength, sizeof(wchar_t));
	_vswprintf(textBufferW, format, valist);

	char* textBufferA = (char*)calloc(stringLength, sizeof(char));
	_snprintf(textBufferA, stringLength, "%ls", textBufferW);

	addDebugText(textBufferA);

	free(textBufferW);
	free(textBufferA);
	va_end(valist);
}

void addDebugText(const char* text) {
	int buflen = strlen(text) + 1;
	char* text2 = (char*)calloc(buflen, sizeof(char));
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

	DebugTextCount++;
	if (DebugTextCount >= DebugTextArrayLenMax)
		DebugTextCount = DebugTextArrayLenMax;

	DebugTextArrayPos++;
	if (DebugTextArrayPos >= DebugTextArrayLenMax)
		DebugTextArrayPos = 0;

	if (DebugStr[DebugTextArrayPos])
		free(DebugStr[DebugTextArrayPos]);

	DebugStr[DebugTextArrayPos] = (char*)calloc(1, sizeof(char) * lenInput + 1);
	strncpy(DebugStr[DebugTextArrayPos], text, lenInput);

	onscreendebug_log->debug(text);

	if (endChar) {
		addDebugText(endChar + 1);
	}
}

void initDebugText() {
	initialisedDebugText = true;
	onscreendebug_log = h2log::create("OnScreenDebug", prepareLogFileName(L"h2onscreendebug"));
	DebugStr = (char**)calloc(1, sizeof(char*) * DebugTextArrayLenMax);
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
