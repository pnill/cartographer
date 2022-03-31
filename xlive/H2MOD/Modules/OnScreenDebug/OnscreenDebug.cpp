#include "stdafx.h"

#include "H2MOD\Modules\Shell\Startup\Startup.h"

char** DebugStr;
const int DebugTextArrayLenMax = 160;
int DebugTextArrayPos = 0;
int DebugTextCount = 0;
bool DebugTextDisplay = false;
bool initialisedDebugText = false;

// we change global variables, async debug text could result in hazzard
std::recursive_mutex addTextMutex;

int getDebugTextArrayMaxLen() {
	return DebugTextArrayLenMax;
}

int getDebugTextDisplayCount() {
	return DebugTextCount;
}

void addDebugTextInternal(char* text) {

	if (!initialisedDebugText) return;

	int lenInput = strlen(text);

	char* endChar = strchr(text, '\n');
	if (endChar) {
		lenInput = endChar - text;
	}

	std::lock_guard<std::recursive_mutex> lg(addTextMutex);

	int _DebugTextCount = DebugTextCount;
	_DebugTextCount++;
	if (_DebugTextCount >= DebugTextArrayLenMax)
		_DebugTextCount = DebugTextArrayLenMax;

	int _DebugTextArrayPos = DebugTextArrayPos;
	_DebugTextArrayPos++;
	if (_DebugTextArrayPos >= DebugTextArrayLenMax)
		_DebugTextArrayPos = 0;

	if (DebugStr[_DebugTextArrayPos])
		free(DebugStr[_DebugTextArrayPos]);

	DebugStr[_DebugTextArrayPos] = (char*)calloc(lenInput + 1, sizeof(char));
	strncpy(DebugStr[_DebugTextArrayPos], text, lenInput);

	onscreendebug_log->debug(text);

	// set global counters just after we added the text
	DebugTextCount = _DebugTextCount;
	DebugTextArrayPos = _DebugTextArrayPos;
	
	if (endChar) {
		return addDebugTextInternal(endChar + 1);
	}
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

	addDebugTextInternal(textBufferA);

	free(textBufferW);
	free(textBufferA);
	va_end(valist);
}

void addDebugText(const char* format, ...)
{
	va_list valist;
	va_start(valist, format);

	/* get the formatted buffer size */
	int stringLength = _vscprintf(format, valist) + 1; // +1 adds null characeter, "_vscwprintf" doesn't add it

	if (stringLength == -1)
	{
		LOG_TRACE_GAME("{} - error trying to get string length size", __FUNCTION__);
		return;
	}

	char* textBufferA = (char*)calloc(stringLength, sizeof(char));
	vsprintf(textBufferA, format, valist);

	addDebugTextInternal(textBufferA);

	free(textBufferA);
	va_end(valist);
}

void InitOnScreenDebugText() {
	initialisedDebugText = true;
	onscreendebug_log = h2log::create("OnScreenDebug", prepareLogFileName(L"h2onscreendebug"), true, 0); // we always create onscreendebuglog, which logs everything (log level 0)
	DebugStr = (char**)calloc(DebugTextArrayLenMax, sizeof(char*));
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
