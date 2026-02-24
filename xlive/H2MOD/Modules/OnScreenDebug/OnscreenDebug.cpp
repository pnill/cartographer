#include "stdafx.h"
#include "OnscreenDebug.h"

#include "H2MOD/GUI/ImGui_Integration/Console/ImGui_ConsoleImpl.h"

static bool initialisedDebugText = false;

// we change global variables, async debug text could result in hazzard
#ifdef TERMINAL_ENABLED
std::recursive_mutex addTextMutex;
#endif

extern void h2log_onscreen_log(const char* text);

/*
//
// TODO remove entirely
// and replace with console code
//
*/

void addDebugTextInternal(char* text) {

	if (!initialisedDebugText) return;

	int lenInput = strlen(text);

	char* endChar = strchr(text, '\n');
	if (endChar) {
		lenInput = endChar - text;
	}

#ifdef TERMINAL_ENABLED
	std::lock_guard lg(addTextMutex);

	CircularStringBuffer* output = GetMainConsoleInstance()->GetTabOutput(_console_tab_logs);
	output->AddString(StringFlag_None, text, lenInput);
#endif
#ifndef SPDLOG_DISABLED
	h2log_onscreen_log(text);
#endif
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
		return;
	}

	wchar_t* textBufferW = (wchar_t*)calloc(stringLength, sizeof(wchar_t));
	uvsnprintf(textBufferW, stringLength, format, valist);

	char* textBufferA = (char*)calloc(stringLength, sizeof(char));
	csprintf(textBufferA, stringLength, "%ls", textBufferW);

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
		return;
	}

	char* textBufferA = (char*)calloc(stringLength, sizeof(char));
	vsprintf(textBufferA, stringLength, format, valist);

	addDebugTextInternal(textBufferA);

	free(textBufferA);
	va_end(valist);
}

void InitOnScreenDebugText() {
#ifndef SPDLOG_DISABLED
	initialisedDebugText = true;
	addDebugText("Initialized onscreendebug log");
#endif
}