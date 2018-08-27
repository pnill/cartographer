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

// Print message to the log
extern void trace(LPWSTR message, ...);
extern void trace2(LPWSTR message, ...);
extern void trace_game(LPWSTR message, ...);
extern void trace_game_narrow(LPSTR message, ...);
extern void trace_game_info(LPSTR message, ...);
extern void trace_game_network(LPSTR message, ...);

#define TRACE_GAME(msg, ...) trace_game (L ## msg, __VA_ARGS__)
#define TRACE_GAME_N(msg, ...) trace_game_narrow( ## msg, __VA_ARGS__ )
#define TRACE_FUNC(msg, ...) trace_game (__FUNCTIONW__  L"(): " L ## msg, __VA_ARGS__)
#define TRACE_FUNC_N(msg, ...) trace_game_narrow( __FUNCTION__ "(): "  ## msg, __VA_ARGS__ )
#define TRACE_GAME_NETWORK(msg, ...) trace_game_network( ## msg, __VA_ARGS__ )
#define TRACE(msg, ...) trace (L ## msg, __VA_ARGS__)
#define TRACE2(msg, ...) trace2 (L ## msg, __VA_ARGS__)
#define TRACE_GAME_INFO(msg, ...) trace_game_info( ## msg, __VA_ARGS__ )

template <typename T>
inline T verify_output(T output, const char *expression, const char *func_name, const char* file, const int line)
{
	if (!output) {
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
	return output;
}
#define LOG_CHECK(expression) \
	verify_output(expression, #expression, __FUNCTION__, __FILE__, __LINE__)

//#define trace()
#else
#define TRACE()
#define TRACE2()
#define TRACE_GAME_N()
#define TRACE_GAME()
#define TRACE_GAME_NETWORK()
#define TRACE_FUNC()
#define TRACE_FUNC_N()
#define LOG_CHECK()
#endif

#endif

void update_player_count();