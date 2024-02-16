#pragma once

// _Shell because Shell namespace causes compile errors for some reason
namespace _Shell
{
	void Initialize();
	void FileErrorDialog(int fpErrNo);
	void OpenMessageBox(HWND hWnd, UINT uType, const char* caption, const char* format, ...);

	int GetInstanceId();
};
