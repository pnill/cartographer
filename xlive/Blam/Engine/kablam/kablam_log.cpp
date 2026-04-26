#include "stdafx.h"
#include "kablam_log.h"

#include "kablam.h"

/* typedefs */

typedef void(__cdecl* kablam_log_t)(const char*, ...);

/* globals */

static kablam_log_t p_kablam_log;

/* prototypes */

static void __cdecl kablam_log(const char* format, ...);

static void kablam_error_log_initialize(void);

/* public code */

void kablam_log_apply_patches(void)
{
	// Detour because the base function doesn't work????
	DETOUR_ATTACH(p_kablam_log, Memory::GetAddress<kablam_log_t>(0x0, 0x1D8C0), kablam_log);
	return;
}

void kablam_log_initialize(void)
{
	// Only log if -log parameter is present
	if (kablam_shell_argument_index_get(L"-log") != NONE)
	{
		wchar_t log_name[MAX_PATH] = L"reports\\";

		// Make sure reports folder exists
		CreateDirectoryW(log_name, NULL);

		const wchar_t* instance_name = kablam_shell_argument_get(L"-instance:");
		if (instance_name)
		{
			// Make sure instance folder exists
			ustrncat(log_name, instance_name, NUMBEROF(log_name));
			ustrncat(log_name, L"\\", NUMBEROF(log_name));
			CreateDirectoryW(log_name, NULL);
		}

		ustrncat(log_name, L"kablam.txt", NUMBEROF(log_name));

		*Memory::GetAddress<bool*>(0x0, 0x4904E8) = true;	// Set logging to true
		*Memory::GetAddress<_iobuf**>(0x0, 0x4904EC) = ufopen(log_name, L"a+b");
	}

	kablam_error_log_initialize();

	return;
}

/* private code */

static void __cdecl kablam_log(
	const char* format,
	...)
{
	va_list args;
	va_start(args, format);

	const bool g_kablam_log_enabled = *Memory::GetAddress<bool*>(0x0, 0x4904E8);
	if (g_kablam_log_enabled)
	{
		FILE* g_kablam_log = *Memory::GetAddress<_iobuf**>(0x0, 0x4904EC);
		vfprintf_s(g_kablam_log, format, args);
		fprintf_s(g_kablam_log, EOL_STRING);
		fflush(g_kablam_log);
	}

	va_end(args);
	return;
}

static void kablam_error_log_initialize(void)
{
	char subdirectory[128];

	const wchar_t* instance_name = kablam_shell_argument_get(L"-instance:");

	wchar_string_to_utf8_string(instance_name, subdirectory, NUMBEROF(subdirectory));

#ifdef ERRORS_ENABLED
	errors_set_log_subdirectory(subdirectory);
#endif

	return;
}
