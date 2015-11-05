#ifndef XLIVELESS_H
#define XLIVELESS_H

#ifdef XLIVELESS_EXPORTS
#define XLIVELESS_API extern "C" __declspec(dllexport)
#else
#define XLIVELESS_API extern "C" __declspec(dllimport)
#endif

#define XLIVELESS_VERSION   0x00020000  // 2.0.0

//#define NO_TRACE

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



#if 0
enum GameVersion
{
    GvUnknown       = 0x00000000,   // unknown game
    IvPatch1        = 0x00010001,   // GTA IV 1.0.1.0
    IvPatch2        = 0x00010002,   // GTA IV 1.0.2.0
    IvPatch3        = 0x00010003,   // GTA IV 1.0.3.0
    IvPatch4        = 0x00010004,   // GTA IV 1.0.4.0
    IvPatch5        = 0x00010005,   // GTA IV 1.0.0.4
    IvPatch6        = 0x00010006,   // GTA IV 1.0.6.0
    IvPatch6J       = 0x4A010006,   // GTA IV 1.0.4.2
    IvPatch7        = 0x00010007,   // GTA IV 1.0.7.0
    RfgUnpatched    = 0x00020000,   // Red Faction: Guerilla
    EflcPatch1      = 0x00030001,   // EfLC 1.1.1.0
    EflcPatch1R     = 0x52030001,   // RfLC 1.1.0.1 (R)
    EflcPatch2      = 0x00030001,   // EfLC 1.1.2.0
    DR2Unpatched    = 0x00040000,   // Dead Rising 2
};

// for C/C++ plugins
extern GameVersion   dwGameVersion;  // game
extern DWORD         dwLoadOffset;   // Offset to the "real" loding address

// for Delphi plugins (same as two variables above)
extern GameVersion   getGameVersion();
extern DWORD         getLoadOffset ();

// Replace game function at dwAddress with plugin function (cast pointer to the function/method to DWORD)
extern void injectFunction (DWORD dwAddress, DWORD pfnReplacement);
#endif

#endif