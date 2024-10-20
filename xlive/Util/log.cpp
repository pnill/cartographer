#include "stdafx.h"

#include "log.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

h2log* h2log::console = nullptr;
bool h2log::failAlerted = false;

h2log::h2log(const std::string& name)
{
	sname = name;
	wname = std::wstring(name.begin(), name.end());

	// should be fine to do this for every logger, better here once than someplace else
	spdlog::flush_on(spdlog::level::trace);
}

h2log::~h2log()
{
	if (is_valid()) output->info("End of log\n");
	output = nullptr;
}

bool h2log::is_valid() const
{
	return output != nullptr;
}

h2log* h2log::create(const std::string &name, const std::wstring &filename, bool shouldCreateLog, int debugLogLevel)
{
	if (shouldCreateLog)
	{
		auto new_h2log = new h2log(name);
		// try opening file to check permissions
		FILE* fp = _wfopen(filename.c_str(), L"a+");
		if (fp)
		{
			fclose(fp);
			new_h2log->output = spdlog::rotating_logger_mt(name, filename, 1048576 * 2, 3);
			new_h2log->output->set_level(debugLogLevel ? (spdlog::level::level_enum)debugLogLevel : spdlog::level::trace);
			new_h2log->output->set_pattern("%d/%m/%Y %H:%M:%S.%e [%n] [%l] : %v");
		}
		else if (!failAlerted)
		{
			// Alert user but don't do anything else
			MessageBoxW(NULL, (L"Logging was enabled in the config file, but was unable to create the log file \"" + filename + L"\". If you require logging, please try to restart Halo 2 in Administrator mode.").c_str(), L"Logging Error", MB_OK);
			failAlerted = true;
		}

		new_h2log->debug("Initialized");
		return new_h2log;
	}

	return nullptr;
}

h2log* h2log::create_console(const std::string &name, bool shouldCreateLog, int debugLogLevel)
{
	if (shouldCreateLog)
	{
		auto new_h2log = new h2log(name);
		new_h2log->isConsole = true;

		if (console == nullptr) // first time console setup
		{
			AllocConsole(); // spawn the console window
			console = new h2log("CONSOLE MAIN");
			console->output = spdlog::stdout_color_mt("CONSOLE MAIN");
			console->output->set_level(debugLogLevel ? (spdlog::level::level_enum)debugLogLevel : spdlog::level::trace);
			console->output->set_pattern("%^%H:%M:%S.%e [CONSOLE MAIN] : %v%$");
		}

		new_h2log->output = console->output->clone(name);
		new_h2log->output->set_pattern("%^%H:%M:%S.%e [CONSOLE] [%n] : %v%$");

		new_h2log->debug("Initialized");
		return new_h2log;
	}
	
	return nullptr;
}
