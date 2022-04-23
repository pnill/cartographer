#pragma once

// _Shell because Shell namespace causes compile errors for some reason

namespace _Shell
{
	bool IsGameMinimized();

	void FileErrorDialog(int fpErrNo);
	void OpenMessageBox(HWND hWnd, UINT uType, const char* caption, const char* format, ...);

	// time functions
	long long QPCToTime(long long denominator, LARGE_INTEGER counter, LARGE_INTEGER freq);

	long long QPCToTimeNowSec();
	long long QPCToTimeNowMsec();
	double	  QPCToSecondsPrecise(LARGE_INTEGER counter, LARGE_INTEGER freq);
};
