#include "stdafx.h"
#include "log.h"

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <codecvt>

h2log* h2log::console = nullptr;
bool h2log::failAlerted = false;
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> h2log::string_convert;

extern bool H2Config_debug_log;
extern int H2Config_debug_log_level;
extern bool H2Config_debug_log_console;
	
h2log::h2log(const std::string& name)
{
	sname = name;
	wname = string_convert.from_bytes(name);
}

h2log::~h2log()
{
	if (is_valid()) output->info("End of log");
	output = nullptr;
}

bool h2log::is_valid()
{
	return output != nullptr;
}

log_level h2log::set_log_level(log_level level)
{
	if (level == H2Config_debug_log_level || level < log_level::trace || level > log_level::critical)
		return (log_level)H2Config_debug_log_level;
	log_level temp = (log_level)H2Config_debug_log_level;
	H2Config_debug_log_level = level;

	// spdlog levels map 1:1 with our log_level enum, but in the future conversions may be desired
	spdlog::set_level((spdlog::level::level_enum)level);

	return temp;
}

h2log* h2log::create(const std::string &name, std::wstring &filename)
{
	h2log* new_h2log = new h2log(name);
	if (H2Config_debug_log) // Prevent anything from accidentally creating this without checking first
	{
		// try opening file to check permissions
		FILE* fp = _wfopen(filename.c_str(), L"a+");
		if (fp)
		{
			fclose(fp);
			new_h2log->output = spdlog::rotating_logger_mt(name, filename, 1048576 * 2, 3);
			new_h2log->output->set_level(H2Config_debug_log_level ? (spdlog::level::level_enum)H2Config_debug_log_level : spdlog::level::trace);
			new_h2log->output->set_pattern("%d/%m/%Y %H:%M:%S.%e [%n] [%l] : %v");
		}
		else if (!failAlerted)
		{
			// Alert user but don't do anything else
			MessageBoxW(NULL, (L"Logging was enabled in the config file, but was unable to create the log file \"" + filename + L"\". If you require logging, please try to restart Halo 2 in Administrator mode.").c_str(), L"Logging Error", MB_OK);
			failAlerted = true;
		}
	}
	new_h2log->debug("Initialized");
	return new_h2log;
}

h2log* h2log::create_console(const std::string &name)
{
	auto new_h2log = new h2log(name);
	new_h2log->isConsole = true;

	if (H2Config_debug_log && H2Config_debug_log_console) // Prevent anything from accidentally creating this without checking first
	{
		if (console == nullptr) // first time console setup
		{
			AllocConsole(); // spawn the console window
			console = new h2log("CONSOLE MAIN");
			console->output = spdlog::stdout_color_mt("CONSOLE MAIN");
			console->output->set_level(H2Config_debug_log_level ? (spdlog::level::level_enum)H2Config_debug_log_level : spdlog::level::trace);
			console->output->set_pattern("%^%H:%M:%S.%e [CONSOLE MAIN] : %v%$");
		}

		new_h2log->output = console->output->clone(name);
		new_h2log->output->set_pattern("%^%H:%M:%S.%e [CONSOLE] [%n] : %v%$");
	}
	new_h2log->debug("Initialized");
	return new_h2log;
}
