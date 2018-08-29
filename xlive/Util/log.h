#pragma once
#include <stdio.h>
#include <mutex>

class logger
{
public:
	logger(const std::string &filename)
	{
		log_handle = fopen(filename.c_str(), "wt");
	}

	logger() = delete;
	~logger()
	{
		fflush(log_handle);
		fclose(log_handle);
		log_handle = nullptr;
	}

	void write(char *format, ...)
	{
		std::unique_lock<std::mutex> lock(write_mutex);
		write_time();

		va_list	arg;
		va_start(arg, format);

		vfprintf(log_handle, format, arg);
		fprintf(log_handle, "\n");

		va_end(arg);

		fflush(log_handle);
	}

	void write(wchar_t *format, ...)
	{
		std::unique_lock<std::mutex> lock(write_mutex);
		write_time();

		va_list	arg;
		va_start(arg, format);

		vfwprintf(log_handle, format, arg);
		fwprintf(log_handle, L"\n");

		va_end(arg);

		fflush(log_handle);
	}

private:

	inline void write_time()
	{
		SYSTEMTIME t;
		GetLocalTime(&t);
		fwprintf(log_handle, L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	}

	FILE *log_handle = nullptr;
	std::mutex write_mutex;
};
