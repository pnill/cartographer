#include "stdafx.h"
#include "ClipboardAPI.h"

#include <mutex>

std::mutex clipboard_mutex;

bool ClipboardAPI::set(const std::string &text, HWND owner)
{
	std::unique_lock<std::mutex> clipboard_lock(clipboard_mutex);
	bool success = false;
	if (LOG_CHECK(OpenClipboard(owner))) {
		LOG_CHECK(EmptyClipboard() != FALSE);

		size_t len = text.length() + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		if (LOG_CHECK(hMem != NULL)) {
			char *clipboard_text = LOG_CHECK(static_cast<char*>(GlobalLock(hMem)));
			if (clipboard_text) {
				strncpy(clipboard_text, text.c_str(), len);
				if (LOG_CHECK(GlobalUnlock(hMem) || GetLastError() == NO_ERROR))
					success = (SetClipboardData(CF_TEXT, hMem) != NULL);
			}
		}
		LOG_CHECK(CloseClipboard());
	}
	return success;
}

bool ClipboardAPI::read(std::string &contents, HWND owner)
{
	std::unique_lock<std::mutex> clipboard_lock(clipboard_mutex);
	if (!IsClipboardFormatAvailable(CF_TEXT))
		return false;
	if (LOG_CHECK(OpenClipboard(owner))) {
		HANDLE data = GetClipboardData(CF_TEXT);
		if (LOG_CHECK(data != NULL)) {
			LPSTR text = static_cast<LPSTR>(GlobalLock(data));
			if (LOG_CHECK(text != NULL)) {
				bool string_is_good = false;
				try {
					contents = text;
					string_is_good = true;
				}
				catch (const std::length_error &e) {
					LOG_TRACE_FUNC("clipboard contents too long: \"{}\"", e.what());
				}
				catch (const std::exception &e) {
					LOG_TRACE_FUNC("{}", e.what());
				}
				if (LOG_CHECK(GlobalUnlock(data) || GetLastError() == NO_ERROR) && string_is_good)
				{
					LOG_CHECK(CloseClipboard());
					return true;
				}
			}
		}
		LOG_CHECK(CloseClipboard());
	}
	return false;
}
