#pragma once
#include <stdio.h>
#include <mutex>

class logger
{
public:
	logger(const std::string &filename, bool append = false)
	{
		log_handle = fopen(filename.c_str(), append ? "at" :"wt");
	}

	logger() = delete;
	~logger()
	{
		write(" === LOG END ===");
		fclose(log_handle);
		log_handle = nullptr;
	}

	bool is_valid()
	{
		return log_handle != nullptr;
	}

	static logger *create(const std::string &filename, bool append = false)
	{
		auto new_logger = new logger(filename, append);
		if (new_logger->is_valid()) {
			return new_logger;
		} else {
			delete new_logger;
			return nullptr;
		}
	}

	void write(char *format, ...)
	{
		va_list	arg;
		va_start(arg, format);

		std::unique_lock<std::mutex> lock(write_mutex);
		write_time();

		vfprintf(log_handle, format, arg);
		fprintf(log_handle, "\n");

		va_end(arg);

		fflush(log_handle);
	}

	void write(wchar_t *format, ...)
	{
		va_list	arg;
		va_start(arg, format);

		std::unique_lock<std::mutex> lock(write_mutex);
		write_time();

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
