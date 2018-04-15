#pragma once

namespace Debug {
	void init();
	void set_expection_filter(LPTOP_LEVEL_EXCEPTION_FILTER filter);
	LPTOP_LEVEL_EXCEPTION_FILTER get_expection_filter();
	LONG WINAPI On_UnhandledException(struct _EXCEPTION_POINTERS* ExceptionInfo);
}
