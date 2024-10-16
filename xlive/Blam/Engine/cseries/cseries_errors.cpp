#include "stdafx.h"
#include "cseries_errors.h"

/* prototypes */

void error_internal(int32 category, int32 priority, const char* format, va_list va_args);

/* public code */

void error(int32 priority, const char* format, ...)
{
#ifdef ERRORS_ENABLED
	va_list va_args;
	va_start(va_args, format);
	error_internal(0, priority, format, va_args);
	va_end(va_args);
#endif
	return;
}

void error(int32 category, int32 priority, const char* format, ...)
{
#ifdef ERRORS_ENABLED
	va_list va_args;
	va_start(va_args, format);
	error_internal(category, priority, format, va_args);
	va_end(va_args);
#endif
	return;
}

void error_internal(int32 category, int32 priority, const char* format, va_list va_args)
{
#ifdef ERRORS_ENABLED
	LOG_ERROR_GAME(format, va_args);
#endif
	return;
}
