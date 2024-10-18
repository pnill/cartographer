#pragma once
#include "spdlog/logger.h"

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

	std::string name() const { return this->sname; }

	/// <summary>
	///   <para>Creates a logger which outputs to a file.</para>
	///   <para>Use logger.is_valid() to check if logging is working.</para>
	/// </summary>
	static h2log* create(const std::string &name, const std::wstring &filename, bool shouldCreateLog, int debugLogLevel);

	/// <summary>
	///   <para>Creates a logger which outputs to a console window.</para>
	///   <para>Use logger.is_valid() to check if logging is working.</para>
	/// </summary>
	static h2log* create_console(const std::string &name, bool shouldCreateLog, int debugLogLevel);

#define log_a(level) \
	if (output != nullptr)                      \
		output->level(fmt.data(), args...);		\
	if (!isConsole                              \
		&& console != nullptr                   \
		&& console->output != nullptr)          \
		console->output->level(					\
			("[" + sname + "] " + fmt).data()   \
			, args...)

#define log_b(level) \
	if (output != nullptr)                      \
		output->level(fmt.data(), args...);		\
	if (!isConsole                              \
		&& console != nullptr                   \
		&& console->output != nullptr)          \
		console->output->level(					\
			(L"[" + wname + L"] " + fmt).data() \
			, args...)

#define log_c(level) \
	if (output != nullptr)                      \
		output->level(msg.data());				\
	if (!isConsole                              \
		&& console != nullptr                   \
		&& console->output != nullptr)          \
		console->output->level(					\
			("[" + sname + "] " + msg).data())

#define log_d(level) \
	if (output != nullptr)                      \
		output->level(msg.data());				\
	if (!isConsole								\
		&& console != nullptr                   \
		&& console->output != nullptr)          \
		console->output->level(					\
			(L"[" + wname + L"] " + msg).data())

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

	std::string sname;
	std::wstring wname;
	std::shared_ptr<spdlog::logger> output = nullptr;

};
