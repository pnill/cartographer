#pragma once

namespace Debug 
{
	void Initialize();

	LONG WINAPI UnhandledExceptionCb(_In_ struct _EXCEPTION_POINTERS* ExceptionInfo);
}
