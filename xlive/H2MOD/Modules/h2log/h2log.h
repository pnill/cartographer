#pragma once

#ifndef SPDLOG_DISABLED
// Disable sign/unsigned comparison mismatch in spdlog
#pragma warning( push )
#pragma warning( disable : 4389)
#pragma warning( disable : 4244)
#include "spdlog/logger.h"
#pragma warning( pop )

enum log_level : unsigned int {
	trace,    //          Tell me *everything*
	debug,    //          Give me the dirty details
	info,     // Default. Occasionally helpful information
	warning,  //          What probably shouldn't be happening
	_log_level_error,    //          Bad news only, please
	critical  //          I only want to see death and destruction
};

class h2log
{
private:
	h2log(const std::string& name);
	h2log() = delete;

public:
	~h2log();

	// Good for checking if the logger is actually able to ouput logs
	bool is_valid() const;

	/// <summary>
	///   <para>Creates a logger which outputs to a file.</para>
	///   <para>Use logger.is_valid() to check if logging is working.</para>
	/// </summary>
	static h2log* create(const std::string &name, const std::wstring &filename, int debugLogLevel);

	/// <summary>
	///   <para>Creates a logger which outputs to a console window.</para>
	///   <para>Use logger.is_valid() to check if logging is working.</para>
	/// </summary>
	static h2log* create_console(const std::string &name, int debugLogLevel);

#define log_a(level) \
do { \
	if (m_output != nullptr)													\
		m_output->level(fmt.data(), args...);									\
	if (!isConsole && console != nullptr && console->m_output != nullptr)		\
		console->m_output->level(("[" + m_sname + "] " + fmt).data(), args...);	\
} while (0)

#define log_b(level) \
do { \
	if (m_output != nullptr)														\
		m_output->level(fmt.data(), args...);										\
	if (!isConsole && console != nullptr && console->m_output != nullptr)			\
		console->m_output->level((L"[" + m_wname + L"] " + fmt).data(), args...);	\
} while (0)

#define log_c(level) \
do { \
	if (m_output != nullptr)												\
		m_output->level(msg.data());										\
	if (!isConsole && console != nullptr && console->m_output != nullptr)	\
		console->m_output->level(("[" + m_sname + "] " + msg).data());		\
} while (0)

#define log_d(level) \
do { \
	if (m_output != nullptr)												\
		m_output->level(msg.data());										\
	if (!isConsole && console != nullptr && console->m_output != nullptr)	\
		console->m_output->level((L"[" + m_wname + L"] " + msg).data());	\
} while (0)

	// For the most unimportant stuff
	template<typename... Args>
	void trace(const std::string& fmt, const Args &... args) { log_a(trace); }

	// For the most unimportant stuff
	template<typename... Args>
	void trace(const std::wstring& fmt, const Args &... args) { log_b(trace); }

	// For the most unimportant stuff
	void trace(const std::string& msg) { log_c(trace); }

	// For the most unimportant stuff
	void trace(const std::wstring& msg) { log_d(trace); }


	// Somewhat more useful information
	template<typename... Args>
	void debug(const std::string& fmt, const Args &... args) { log_a(debug); }

	// Somewhat more useful information
	template<typename... Args>
	void debug(const std::wstring& fmt, const Args &... args) { log_b(debug); }

	// Somewhat more useful information
	void debug(const std::string& msg) { log_c(debug); }

	// Somewhat more useful information
	void debug(const std::wstring& msg) { log_d(debug); }


	// Things that even users may want to see
	template<typename... Args>
	void info(const std::string& fmt, const Args &... args) { log_a(info); }

	// Things that even users may want to see
	template<typename... Args>
	void info(const std::wstring& fmt, const Args &... args) { log_b(info); }

	// Things that even users may want to see
	void info(const std::string& msg) { log_c(info); }

	// Things that even users may want to see
	void info(const std::wstring& msg) { log_d(info); }


	// A surprise to be sure, but not a serious one
	template<typename... Args>
	void warning(const std::string& fmt, const Args &... args) { log_a(warn); }

	// A surprise to be sure, but not a serious one
	template<typename... Args>
	void warning(const std::wstring& fmt, const Args &... args) { log_b(warn); }

	// A surprise to be sure, but not a serious one
	void warning(const std::string& msg) { log_c(warn); }

	// A surprise to be sure, but not a serious one
	void warning(const std::wstring& msg) { log_d(warn); }


	// Absolutely not good, probably game breaking events
	template<typename... Args>
	void error(const std::string& fmt, const Args &... args) { log_a(error); }

	// Absolutely not good, probably game breaking events
	template<typename... Args>
	void error(const std::wstring& fmt, const Args &... args) { log_b(error); }

	// Absolutely not good, probably game breaking events
	void error(const std::string& msg) { log_c(error); }

	// Absolutely not good, probably game breaking events
	void error(const std::wstring& msg) { log_d(error); }


	// "Wait, that's illegal" except it is definitely not a joke
	template<typename... Args>
	void critical(const std::string& fmt, const Args &... args) { log_a(critical); }

	// "Wait, that's illegal" except it is definitely not a joke
	template<typename... Args>
	void critical(const std::wstring& fmt, const Args &... args) { log_b(critical); }

	// "Wait, that's illegal" except it is definitely not a joke
	void critical(const std::string& msg) { log_c(critical); }

	// "Wait, that's illegal" except it is definitely not a joke
	void critical(const std::wstring& msg) { log_d(critical); }

#undef log_a
#undef log_w
#undef log_b

public:
	bool isConsole = false;

private:
	static bool failAlerted;
	static h2log* console;

	std::string m_sname;
	std::wstring m_wname;
	std::shared_ptr<spdlog::logger> m_output = nullptr;
};

extern h2log* g_xlive_log;
extern h2log* g_h2mod_log;
extern h2log* g_network_log;
extern h2log* g_console_log;
extern h2log* g_onscreendebug_log;
extern h2log* voice_log;

#define CHECK_PTR(check, expression) \
do \
{ \
	if ((check)) \
		(expression); \
} while (0)

// to note this is not thread safe
// you might see the max log count reached message more than once in a row
#define LIMITED_LOG(log_limit, logger, ...) \
do \
{ \
	static unsigned int _logged_times_count; \
	if (_logged_times_count < log_limit) { \
		logger(__VA_ARGS__); \
		_logged_times_count++; \
	} \
	else if (_logged_times_count == log_limit) \
	{ \
		logger("	reached max log count of {} for: ", log_limit); \
		logger(__VA_ARGS__); \
		_logged_times_count++; \
	} \
} while(0)

#define LIMITED_LOGW(log_limit, logger, ...) \
do \
{ \
	static unsigned int _logged_times_count; \
	if (_logged_times_count < log_limit) { \
		logger(__VA_ARGS__); \
		_logged_times_count++; \
	} \
	else if (_logged_times_count == log_limit) \
	{ \
		logger(L"	reached max log count of {} for: ", log_limit); \
		logger(__VA_ARGS__); \
		_logged_times_count++; \
	} \
} while(0)

// Generic logging
// For the most unimportant stuff
#define LOG_TRACE(logger, msg, ...)      CHECK_PTR((logger), (logger)->trace    (msg, __VA_ARGS__))

// Somewhat more useful information
#define LOG_DEBUG(logger, msg, ...)      CHECK_PTR((logger), (logger)->debug    (msg, __VA_ARGS__))

// Things that even users may want to see
#define LOG_INFO(logger, msg, ...)       CHECK_PTR((logger), (logger)->info     (msg, __VA_ARGS__))

// A surprise to be sure, but not a serious one
#define LOG_WARNING(logger, msg, ...)    CHECK_PTR((logger), (logger)->warning  (msg, __VA_ARGS__))

// Absolutely not good, probably game breaking events
#define LOG_ERROR(logger, msg, ...)      CHECK_PTR((logger), (logger)->error    (msg, __VA_ARGS__))

// "Wait, that's illegal" except it is definitely not a joke
#define LOG_CRITICAL(logger, msg, ...)   CHECK_PTR((logger), (logger)->critical (msg, __VA_ARGS__))

// Mod-specific logging
// For the most unimportant stuff related to H2mod specifically
#define LOG_TRACE_GAME(msg, ...)         LOG_TRACE     (g_h2mod_log, msg, __VA_ARGS__)

// Somewhat more useful information related to H2mod specifically
#define LOG_DEBUG_GAME(msg, ...)         LOG_DEBUG     (g_h2mod_log, msg, __VA_ARGS__)

// Things that even users may want to see related to H2mod specifically
#define LOG_INFO_GAME(msg, ...)          LOG_INFO      (g_h2mod_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to H2mod specifically
#define LOG_WARNING_GAME(msg, ...)       LOG_WARNING   (g_h2mod_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to H2mod specifically
#define LOG_ERROR_GAME(msg, ...)         LOG_ERROR     (g_h2mod_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to H2mod specifically
#define LOG_CRITICAL_GAME(msg, ...)      LOG_CRITICAL  (g_h2mod_log, msg, __VA_ARGS__)

// Function calls
// For the most unimportant stuff, prepends the current function name
#define LOG_TRACE_FUNC(msg, ...)         LOG_TRACE     (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// For the most unimportant stuff, prepends the current function name
#define LOG_TRACE_FUNCW(msg, ...)        LOG_TRACE     (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Somewhat more useful information, prepends the current function name
#define LOG_DEBUG_FUNC(msg, ...)         LOG_DEBUG     (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// Somewhat more useful information, prepends the current function name
#define LOG_DEBUG_FUNCW(msg, ...)        LOG_DEBUG     (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Things that even users may want to see, prepends the current function name
#define LOG_INFO_FUNC(msg, ...)          LOG_INFO      (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// Things that even users may want to see, prepends the current function name
#define LOG_INFO_FUNCW(msg, ...)         LOG_INFO      (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// A surprise to be sure, but not a serious one, prepends the current function name
#define LOG_WARNING_FUNC(msg, ...)       LOG_WARNING   (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// A surprise to be sure, but not a serious one, prepends the current function name
#define LOG_WARNING_FUNCW(msg, ...)      LOG_WARNING   (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Absolutely not good, probably game breaking events, prepends the current function name
#define LOG_ERROR_FUNC(msg, ...)         LOG_ERROR     (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// Absolutely not good, probably game breaking events, prepends the current function name
#define LOG_ERROR_FUNCW(msg, ...)        LOG_ERROR     (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke, prepends the current function name
#define LOG_CRITICAL_FUNC(msg, ...)      LOG_CRITICAL  (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// "Wait, that's illegal" except it is definitely not a joke, prepends the current function name
#define LOG_CRITICAL_FUNCW(msg, ...)     LOG_CRITICAL  (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Networking
// For the most unimportant stuff related to networking
#define LOG_TRACE_NETWORK(msg, ...)      LOG_TRACE     (g_network_log, msg, __VA_ARGS__)

// Somewhat more useful information related to networking
#define LOG_DEBUG_NETWORK(msg, ...)      LOG_DEBUG     (g_network_log, msg, __VA_ARGS__)

// Things that even users may want to see related to networking
#define LOG_INFO_NETWORK(msg, ...)       LOG_INFO      (g_network_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to networking
#define LOG_WARNING_NETWORK(msg, ...)    LOG_WARNING   (g_network_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to networking
#define LOG_ERROR_NETWORK(msg, ...)      LOG_ERROR     (g_network_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to networking
#define LOG_CRITICAL_NETWORK(msg, ...)   LOG_CRITICAL  (g_network_log, msg, __VA_ARGS__)

// xLiveLess
// For the most unimportant stuff related to xLiveLess
#define LOG_TRACE_XLIVE(msg, ...)        LOG_TRACE     (g_xlive_log, msg, __VA_ARGS__)

// Somewhat more useful information related to xLiveLess
#define LOG_DEBUG_XLIVE(msg, ...)        LOG_DEBUG     (g_xlive_log, msg, __VA_ARGS__)

// Things that even users may want to see related to xLiveLess
#define LOG_INFO_XLIVE(msg, ...)         LOG_INFO      (g_xlive_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to xLiveLess
#define LOG_WARNING_XLIVE(msg, ...)      LOG_WARNING   (g_xlive_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to xLiveLess
#define LOG_ERROR_XLIVE(msg, ...)        LOG_ERROR     (g_xlive_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to xLiveLess
#define LOG_CRITICAL_XLIVE(msg, ...)     LOG_CRITICAL  (g_xlive_log, msg, __VA_ARGS__)

#else

#define LIMITED_LOG(log_limit, logger, ...) (void)0

#define LOG_TRACE(logger, msg, ...) (void)0
#define LOG_DEBUG(logger, msg, ...) (void)0
#define LOG_INFO(logger, msg, ...) (void)0
#define LOG_WARNING(logger, msg, ...) (void)0
#define LOG_ERROR(logger, msg, ...) (void)0
#define LOG_CRITICAL(logger, msg, ...) (void)0

// Mod-specific logging
#define LOG_TRACE_GAME(msg, ...) (void)0
#define LOG_DEBUG_GAME(msg, ...) (void)0
#define LOG_INFO_GAME(msg, ...) (void)0
#define LOG_WARNING_GAME(msg, ...) (void)0
#define LOG_ERROR_GAME(msg, ...) (void)0
#define LOG_CRITICAL_GAME(msg, ...) (void)0

// Function calls
#define LOG_TRACE_FUNC(msg, ...) (void)0
#define LOG_TRACE_FUNCW(msg, ...) (void)0
#define LOG_DEBUG_FUNC(msg, ...) (void)0
#define LOG_DEBUG_FUNCW(msg, ...) (void)0
#define LOG_INFO_FUNC(msg, ...) (void)0
#define LOG_INFO_FUNCW(msg, ...) (void)0
#define LOG_WARNING_FUNC(msg, ...) (void)0
#define LOG_WARNING_FUNCW(msg, ...) (void)0
#define LOG_ERROR_FUNC(msg, ...) (void)0
#define LOG_ERROR_FUNCW(msg, ...) (void)0
#define LOG_CRITICAL_FUNC(msg, ...) (void)0
#define LOG_CRITICAL_FUNCW(msg, ...) (void)0

// Networking
#define LOG_TRACE_NETWORK(msg, ...) (void)0
#define LOG_DEBUG_NETWORK(msg, ...) (void)0
#define LOG_INFO_NETWORK(msg, ...) (void)0
#define LOG_WARNING_NETWORK(msg, ...) (void)0
#define LOG_ERROR_NETWORK(msg, ...) (void)0
#define LOG_CRITICAL_NETWORK(msg, ...) (void)0

// xLiveLess
#define LOG_TRACE_XLIVE(msg, ...) (void)0
#define LOG_DEBUG_XLIVE(msg, ...) (void)0
#define LOG_INFO_XLIVE(msg, ...) (void)0
#define LOG_WARNING_XLIVE(msg, ...) (void)0
#define LOG_ERROR_XLIVE(msg, ...) (void)0
#define LOG_CRITICAL_XLIVE(msg, ...) (void)0

#define LOG_CHECK(...) (__VA_ARGS__)

#endif

void h2log_initialize();

void h2log_onscreen_initialize(void);

void h2log_onscreen_log(const char* text);

void h2log_dispose();
