#pragma once

#ifndef XLIVELESS_H
#define XLIVELESS_H

#ifdef XLIVELESS_EXPORTS
#define XLIVELESS_API extern "C" __declspec(dllexport)
#else
#define XLIVELESS_API extern "C" __declspec(dllimport)
#endif

#define XLIVELESS_VERSION   0x00020000  // 2.0.0

//useful macros
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_FREE(p)  { if(p) { free(p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define ARRAYOF(x) (sizeof(x)/sizeof(*x))
#define IN_RANGE(val, min, max) ((val) > (min) && (val) < (max))
#define IN_RANGE2(val, min, max) ((val) >= (min) && (val) <= (max))
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define BYTESOF(a, b) ((a) * sizeof(b))

extern HMODULE hThis;
extern CRITICAL_SECTION d_lock;

extern h2log *xlive_log;
extern h2log *h2mod_log;
extern h2log *network_log;
extern h2log *console_log;
extern h2log *onscreendebug_log;
extern h2log *voice_log;

#define CHECK_PTR(check, expression) \
	((void) ((!(check)) || ((expression), 0)))

// Generic logging
// For the most unimportant stuff
#define LOG_TRACE(logger, msg, ...)      CHECK_PTR((logger), (logger)->trace    (  ## msg, __VA_ARGS__))

// Somewhat more useful information
#define LOG_DEBUG(logger, msg, ...)      CHECK_PTR((logger), (logger)->debug    (  ## msg, __VA_ARGS__))

// Things that even users may want to see
#define LOG_INFO(logger, msg, ...)       CHECK_PTR((logger), (logger)->info     (  ## msg, __VA_ARGS__))

// A surprise to be sure, but not a serious one
#define LOG_WARNING(logger, msg, ...)    CHECK_PTR((logger), (logger)->warning  (  ## msg, __VA_ARGS__))

// Absolutely not good, probably game breaking events
#define LOG_ERROR(logger, msg, ...)      CHECK_PTR((logger), (logger)->error    (  ## msg, __VA_ARGS__))

// "Wait, that's illegal" except it is definitely not a joke
#define LOG_CRITICAL(logger, msg, ...)   CHECK_PTR((logger), (logger)->critical (  ## msg, __VA_ARGS__))

// Mod-specific logging
// For the most unimportant stuff related to H2mod specifically
#define LOG_TRACE_GAME(msg, ...)         LOG_TRACE     (h2mod_log, msg, __VA_ARGS__)

// Somewhat more useful information related to H2mod specifically
#define LOG_DEBUG_GAME(msg, ...)         LOG_DEBUG     (h2mod_log, msg, __VA_ARGS__)

// Things that even users may want to see related to H2mod specifically
#define LOG_INFO_GAME(msg, ...)          LOG_INFO      (h2mod_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to H2mod specifically
#define LOG_WARNING_GAME(msg, ...)       LOG_WARNING   (h2mod_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to H2mod specifically
#define LOG_ERROR_GAME(msg, ...)         LOG_ERROR     (h2mod_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to H2mod specifically
#define LOG_CRITICAL_GAME(msg, ...)      LOG_CRITICAL  (h2mod_log, msg, __VA_ARGS__)

// Function calls
// For the most unimportant stuff, prepends the current function name
#define LOG_TRACE_FUNC(msg, ...)         LOG_TRACE     (h2mod_log, __FUNCTION__   "(): " msg, __VA_ARGS__)
// For the most unimportant stuff, prepends the current function name
#define LOG_TRACE_FUNCW(msg, ...)        LOG_TRACE     (h2mod_log, __FUNCTIONW__ L"(): " msg, __VA_ARGS__)

// Somewhat more useful information, prepends the current function name
#define LOG_DEBUG_FUNC(msg, ...)         LOG_DEBUG     (h2mod_log, __FUNCTION__   "(): " msg, __VA_ARGS__)
// Somewhat more useful information, prepends the current function name
#define LOG_DEBUG_FUNCW(msg, ...)        LOG_DEBUG     (h2mod_log, __FUNCTIONW__ L"(): " msg, __VA_ARGS__)

// Things that even users may want to see, prepends the current function name
#define LOG_INFO_FUNC(msg, ...)          LOG_INFO      (h2mod_log, __FUNCTION__   "(): " msg, __VA_ARGS__)
// Things that even users may want to see, prepends the current function name
#define LOG_INFO_FUNCW(msg, ...)         LOG_INFO      (h2mod_log, __FUNCTIONW__ L"(): " msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one, prepends the current function name
#define LOG_WARNING_FUNC(msg, ...)       LOG_WARNING   (h2mod_log, __FUNCTION__   "(): " msg, __VA_ARGS__)
// A surprise to be sure, but not a serious one, prepends the current function name
#define LOG_WARNING_FUNCW(msg, ...)      LOG_WARNING   (h2mod_log, __FUNCTIONW__ L"(): " msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events, prepends the current function name
#define LOG_ERROR_FUNC(msg, ...)         LOG_ERROR     (h2mod_log, __FUNCTION__   "(): " msg, __VA_ARGS__)
// Absolutely not good, probably game breaking events, prepends the current function name
#define LOG_ERROR_FUNCW(msg, ...)        LOG_ERROR     (h2mod_log, __FUNCTIONW__ L"(): " msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke, prepends the current function name
#define LOG_CRITICAL_FUNC(msg, ...)      LOG_CRITICAL  (h2mod_log, __FUNCTION__   "(): " msg, __VA_ARGS__)
// "Wait, that's illegal" except it is definitely not a joke, prepends the current function name
#define LOG_CRITICAL_FUNCW(msg, ...)     LOG_CRITICAL  (h2mod_log, __FUNCTIONW__ L"(): " msg, __VA_ARGS__)

// Networking
// For the most unimportant stuff related to networking
#define LOG_TRACE_NETWORK(msg, ...)      LOG_TRACE     (network_log, msg, __VA_ARGS__)

// Somewhat more useful information related to networking
#define LOG_DEBUG_NETWORK(msg, ...)      LOG_DEBUG     (network_log, msg, __VA_ARGS__)

// Things that even users may want to see related to networking
#define LOG_INFO_NETWORK(msg, ...)       LOG_INFO      (network_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to networking
#define LOG_WARNING_NETWORK(msg, ...)    LOG_WARNING   (network_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to networking
#define LOG_ERROR_NETWORK(msg, ...)      LOG_ERROR     (network_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to networking
#define LOG_CRITICAL_NETWORK(msg, ...)   LOG_CRITICAL  (network_log, msg, __VA_ARGS__)

// xLiveLess
// For the most unimportant stuff related to xLiveLess
#define LOG_TRACE_XLIVE(msg, ...)        LOG_TRACE     (xlive_log, msg, __VA_ARGS__)

// Somewhat more useful information related to xLiveLess
#define LOG_DEBUG_XLIVE(msg, ...)        LOG_DEBUG     (xlive_log, msg, __VA_ARGS__)

// Things that even users may want to see related to xLiveLess
#define LOG_INFO_XLIVE(msg, ...)         LOG_INFO      (xlive_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to xLiveLess
#define LOG_WARNING_XLIVE(msg, ...)      LOG_WARNING   (xlive_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to xLiveLess
#define LOG_ERROR_XLIVE(msg, ...)        LOG_ERROR     (xlive_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to xLiveLess
#define LOG_CRITICAL_XLIVE(msg, ...)     LOG_CRITICAL  (xlive_log, msg, __VA_ARGS__)

inline void verify_output_log(const char *expression, const char *func_name, const char* file, const int line)
{
	LOG_TRACE_GAME("'{0}' failed in '{1}' at '{2}:{3:d}'!", func_name, expression, file, line);
	DWORD last_error = GetLastError();
	if (last_error)
	{
		LPWSTR messageBuffer = NULL;
		size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, last_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
		if (size) {
			LOG_ERROR_GAME(L"Last error: '{}'", messageBuffer);
			LocalFree(messageBuffer);
		}
		else {
			LOG_ERROR_GAME("Converting error {} to string failed!", last_error);
		}
		SetLastError(ERROR_SUCCESS);
	}
}

template <typename T>
inline T verify_output(T output, const char *expression, const char *func_name, const char* file, const int line)
{
	if (!output) {
		verify_output_log(expression, func_name, file, line);
	}
	return output;
}

/*
	Isn't actually va-args just looks like that so that commas don't cause issues
*/
#define LOG_CHECK(...) \
	verify_output((__VA_ARGS__), #__VA_ARGS__, __FUNCTION__, __FILE__, __LINE__)


#else
#define LOG_TRACE()
#define LOG_DEBUG()
#define LOG_INFO()
#define LOG_WARNING()
#define LOG_ERROR()
#define LOG_CRITICAL()

// Mod-specific logging
#define LOG_TRACE_GAME()
#define LOG_DEBUG_GAME()
#define LOG_INFO_GAME()
#define LOG_WARNING_GAME()
#define LOG_ERROR_GAME()
#define LOG_CRITICAL_GAME()

// Function calls
#define LOG_TRACE_FUNC()
#define LOG_TRACE_FUNCW()
#define LOG_DEBUG_FUNC()
#define LOG_DEBUG_FUNCW()
#define LOG_INFO_FUNC()
#define LOG_INFO_FUNCW()
#define LOG_WARNING_FUNC()
#define LOG_WARNING_FUNCW()
#define LOG_ERROR_FUNC()
#define LOG_ERROR_FUNCW()
#define LOG_CRITICAL_FUNC()
#define LOG_CRITICAL_FUNCW()

// Networking
#define LOG_TRACE_NETWORK()
#define LOG_DEBUG_NETWORK()
#define LOG_INFO_NETWORK()
#define LOG_WARNING_NETWORK()
#define LOG_ERROR_NETWORK()
#define LOG_CRITICAL_NETWORK()

// xLiveLess
#define LOG_TRACE_XLIVE()
#define LOG_DEBUG_XLIVE()
#define LOG_INFO_XLIVE()
#define LOG_WARNING_XLIVE()
#define LOG_ERROR_XLIVE()
#define LOG_CRITICAL_XLIVE()
#endif

void update_player_count();
