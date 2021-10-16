#pragma once

#include <string>

namespace ClipboardAPI
{
	bool set(const std::string &text, HWND owner = nullptr);
	bool read(std::string &contents, HWND owner = nullptr);
}