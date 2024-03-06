#include "stdafx.h"
#include "cseries_errors.h"

void error_internal(int32 category, int32 priority, const char* format, va_list va_args);


void error(int32 priority, const char* format, ...)
{
	va_list va_args;
	va_start(va_args, format);
	error_internal(0, priority, format, va_args);
	return;
}

void error(int32 category, int32 priority, const char* format, ...)
{
	va_list va_args;
	va_start(va_args, format);
	error_internal(category, priority, format, va_args);
	return;
}


void error_internal(int32 category, int32 priority, const char* format, va_list va_args)
{
	LOG_ERROR_GAME(format, va_args);
	return;
}
