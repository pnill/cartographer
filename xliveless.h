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
extern void trace_game_network(LPSTR message, ...);

#define TRACE_GAME(msg, ...) trace_game (L ## msg, __VA_ARGS__)
#define TRACE_GAME_N(msg, ...) trace_game_narrow( ## msg, __VA_ARGS__ )
#define TRACE_GAME_NETWORK(msg, ...) trace_game_network( ## msg, __VA_ARGS__ )
#define TRACE(msg, ...) trace (L ## msg, __VA_ARGS__)
#define TRACE2(msg, ...) trace2 (L ## msg, __VA_ARGS__)

//#define trace()
#else
#define TRACE()
#define TRACE2()
#define TRACE_GAME_N()
#define TRACE_GAME()
#define TRACE_GAME_NETWORK()
#endif

#endif
