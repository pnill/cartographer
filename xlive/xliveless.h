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

#ifndef NO_TRACE
#include "Util/log.h"

extern logger *xlive_trace_log;
extern logger *h2mod_log;
extern logger *network_log;

#define CHECK_PTR(check, expression) \
	if (check) \
		expression

#define TRACE_GAME(msg, ...) CHECK_PTR(h2mod_log, h2mod_log->write(L ## msg, __VA_ARGS__))
#define TRACE_GAME_N(msg, ...) CHECK_PTR(h2mod_log, h2mod_log->write( ## msg, __VA_ARGS__ ))
#define TRACE_FUNC(msg, ...)  CHECK_PTR(h2mod_log, h2mod_log->write(__FUNCTIONW__  L"(): " L ## msg, __VA_ARGS__))
#define TRACE_FUNC_N(msg, ...)  CHECK_PTR(h2mod_log, h2mod_log->write( __FUNCTION__ "(): "  ## msg, __VA_ARGS__ ))
#define TRACE_GAME_NETWORK(msg, ...) CHECK_PTR(network_log, network_log->write(L ## msg, __VA_ARGS__ ))
#define TRACE_GAME_NETWORK_N(msg, ...) CHECK_PTR(network_log, network_log->write( ## msg, __VA_ARGS__ ))
#define TRACE(msg, ...) CHECK_PTR(xlive_trace_log, xlive_trace_log->write(L ## msg, __VA_ARGS__))
#define TRACE_N(msg, ...) CHECK_PTR(xlive_trace_log, xlive_trace_log->write( ## msg, __VA_ARGS__))

inline void verify_output_log(const char *expression, const char *func_name, const char* file, const int line)
{
	TRACE_GAME_N("'%s' failed in '%s' at '%s:%d'!", func_name, expression, file, line);
	DWORD last_error = GetLastError();
	if (last_error)
	{
		LPWSTR messageBuffer = NULL;
		size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, last_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
		if (size) {
			TRACE_GAME_N("Last error: '%ws'", messageBuffer);
			LocalFree(messageBuffer);
		}
		else {
			TRACE_GAME_N("Converting error %d to string failed!", last_error);
		}
		SetLastError(0);
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

#define LOG_CHECK(expression) \
	verify_output(expression, #expression, __FUNCTION__, __FILE__, __LINE__)

//#define trace()
#else
#define TRACE()
#define TRACE_GAME_N()
#define TRACE_GAME()
#define TRACE_GAME_NETWORK()
#define TRACE_FUNC()
#define TRACE_FUNC_N()
#define LOG_CHECK()
#endif

#endif

void update_player_count();
extern UINT g_signin[4];
