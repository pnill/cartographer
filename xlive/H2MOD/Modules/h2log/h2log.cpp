#include "stdafx.h"

#ifndef SPDLOG_DISABLED
#include "h2log.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"

h2log* h2log::console = nullptr;
bool h2log::failAlerted = false;

// xLiveLess specific logger
h2log* g_xlive_log = nullptr;

// Mod specific logger
h2log* g_h2mod_log = nullptr;

// General network logger
h2log* g_network_log = nullptr;

// OnScreenDebug logger
h2log* g_onscreendebug_log = nullptr;

// Console logger, receives output from all loggers
h2log* g_console_log = nullptr;

h2log::h2log(const std::string& name)
{
	m_sname = name;
	m_wname = std::wstring(name.begin(), name.end());

	// should be fine to do this for every logger, better here once than someplace else
	spdlog::flush_on(spdlog::level::trace);
}

h2log::~h2log()
{
	if (is_valid()) m_output->info("End of log\n");
	m_output = nullptr;
}

bool h2log::is_valid() const
{
	return m_output != nullptr;
}

h2log* h2log::create(const std::string &name, const std::wstring &filename, int debugLogLevel)
{
	spdlog::level::level_enum logLevel = (spdlog::level::level_enum)debugLogLevel;

	if (!IN_RANGE(logLevel, spdlog::level::level_enum::trace, spdlog::level::level_enum::off)) 
	{
		logLevel = spdlog::level::level_enum::critical;
	}

	auto new_h2log = new h2log(name);
	// try opening file to check permissions
	FILE* fp;
	const errno_t error = _wfopen_s(&fp, filename.c_str(), L"a+");
	if (fp && !error)
	{
		fclose(fp);
		new_h2log->m_output = spdlog::rotating_logger_mt(name, filename, 1048576 * 2, 3);
		new_h2log->m_output->set_level(logLevel);
		new_h2log->m_output->set_pattern("%d/%m/%Y %H:%M:%S.%e [%n] [%l] : %v");
	}
	else if (!failAlerted)
	{
		// Alert user but don't do anything else
		MessageBoxW(NULL, (L"Logging was enabled in the config file, but was unable to create the log file \"" + filename + L"\". If you require logging, pleasetry to restart Halo 2 in Administrator mode.").c_str(), L"Logging Error", MB_OK);
		failAlerted = true;
	}

	new_h2log->debug("Initialized");
	return new_h2log;
}

h2log* h2log::create_console(const std::string& name, int debugLogLevel)
{
	auto new_h2log = new h2log(name);
	new_h2log->isConsole = true;

	if (console == nullptr) // first time console setup
	{
		AllocConsole(); // spawn the console window
		console = new h2log("CONSOLE MAIN");
		console->m_output = spdlog::stdout_color_mt("CONSOLE MAIN");
		console->m_output->set_level(debugLogLevel ? (spdlog::level::level_enum)debugLogLevel : spdlog::level::trace);
		console->m_output->set_pattern("%^%H:%M:%S.%e [CONSOLE MAIN] : %v%$");
	}

	new_h2log->m_output = console->m_output->clone(name);
	new_h2log->m_output->set_pattern("%^%H:%M:%S.%e [CONSOLE] [%n] : %v%$");

	new_h2log->debug("Initialized");
	return new_h2log;
}

#endif

void h2log_initialize()
{
#ifndef SPDLOG_DISABLED
	// prepare default log files if enabled, after we read the H2Config
	bool should_enable_console_log = H2Config_debug_log && H2Config_debug_log_console;
	if (should_enable_console_log) 
	{
		g_console_log = h2log::create_console("CONSOLE MAIN", H2Config_debug_log_level);
	}

	if (H2Config_debug_log)
	{
		c_static_wchar_string<MAX_PATH> path;
		log_file_name_prepare(L"h2xlive", &path);
		g_xlive_log = h2log::create("XLive", path.get_string(), H2Config_debug_log_level);
		LOG_DEBUG_XLIVE(DLL_VERSION_STR);

		log_file_name_prepare(L"h2mod", &path);
		g_h2mod_log = h2log::create("H2MOD", path.get_string(), H2Config_debug_log_level);
		LOG_DEBUG_GAME(DLL_VERSION_STR);

		log_file_name_prepare(L"h2network", &path);
		g_network_log = h2log::create("Network", path.get_string(), H2Config_debug_log_level);
		LOG_DEBUG_NETWORK(DLL_VERSION_STR);
	}
#endif
	return;
}

void h2log_onscreen_initialize(void)
{
#ifndef SPDLOG_DISABLED
	c_static_wchar_string<MAX_PATH> path;
	log_file_name_prepare(L"h2onscreendebug", &path);
	g_onscreendebug_log = h2log::create("OnScreenDebug", path.get_string(), 0); // we always create onscreendebuglog, which logs everything (log level 0)
#endif
	return;
}

void h2log_onscreen_log(const char* text)
{
#ifndef SPDLOG_DISABLED
	g_onscreendebug_log->debug(text);
#endif
	return;
}

void h2log_dispose()
{
#ifndef SPDLOG_DISABLED
	delete g_xlive_log;
	delete g_h2mod_log;
	delete g_network_log;
	delete g_console_log;
	delete g_onscreendebug_log;
#endif
}

