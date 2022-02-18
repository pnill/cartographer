// dear imgui, v1.87 WIP
// (drawing and font code)

/*

Index of this file:

// [SECTION] STB libraries implementation
// [SECTION] Style functions
// [SECTION] ImDrawList
// [SECTION] ImDrawListSplitter
// [SECTION] ImDrawData
// [SECTION] Helpers ShadeVertsXXX functions
// [SECTION] ImFontConfig
// [SECTION] ImFontAtlas
// [SECTION] ImFontAtlas glyph ranges helpers
// [SECTION] ImFontGlyphRangesBuilder
// [SECTION] ImFont
// [SECTION] ImGui Internal Render Helpers
// [SECTION] Decompression code
// [SECTION] Default font data (ProggyClean.ttf)

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui_internal.h"
#ifdef IMGUI_ENABLE_FREETYPE
#include "misc/freetype/imgui_freetype.h"
#endif

#include <stdio.h>      // vsnprintf, sscanf, printf
#if !defined(alloca)
#if defined(__GLIBC__) || defined(__sun) || defined(__APPLE__) || defined(__NEWLIB__)
#include <alloca.h>     // alloca (glibc uses <alloca.h>. Note that Cygwin may have _WIN32 defined, so the order matters here)
#elif defined(_WIN32)
#include <malloc.h>     // alloca
#if !defined(alloca)
#define alloca _alloca  // for clang with MS Codegen
#endif
#else
#include <stdlib.h>     // alloca
#endif
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)     // condition expression is constant
#pragma warning (disable: 4505)     // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#pragma warning (disable: 6255)     // [Static Analyzer] _alloca indicates failure by raising a stack overflow exception.  Consider using _malloca instead.
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow : Using operator 'xxx' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator 'xxx' to avoid overflow(io.2).
#pragma warning (disable: 26812)    // [Static Analyzer] The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3). [MSVC Static Analyzer)
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'                      // not all warnings are known by all Clang versions and they tend to be rename-happy.. so ignoring warnings triggers new warnings on some configuration. Great!
#endif
#if __has_warning("-Walloca")
#pragma clang diagnostic ignored "-Walloca"                         // warning: use of function '__builtin_alloca' is discouraged
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast                            // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wglobal-constructors"            // warning: declaration requires a global destructor         // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"                // warning: implicit conversion changes signedness
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                    // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wcomma"                          // warning: possible misuse of comma operator here
#pragma clang diagnostic ignored "-Wreserved-id-macro"              // warning: macro name is a reserved identifier
#pragma clang diagnostic ignored "-Wdouble-promotion"               // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wstack-protector"          // warning: stack protector not protecting local variables: variable length buffer
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries implementation
//-------------------------------------------------------------------------

// Compile time options:
//#define IMGUI_STB_NAMESPACE           ImStb
//#define IMGUI_STB_TRUETYPE_FILENAME   "my_folder/stb_truetype.h"
//#define IMGUI_STB_RECT_PACK_FILENAME  "my_folder/stb_rect_pack.h"
//#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
//#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION

#ifdef IMGUI_STB_NAMESPACE
namespace IMGUI_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4456)                             // declaration of 'xx' hides previous local declaration
#pragma warning (disable: 6011)                             // (stb_rectpack) Dereferencing NULL pointer 'cur->next'.
#pragma warning (disable: 6385)                             // (stb_truetype) Reading invalid data from 'buffer':  the readable size is '_Old_3`kernel_width' bytes, but '3' bytes may be read.
#pragma warning (disable: 28182)                            // (stb_rectpack) Dereferencing NULL pointer. 'cur' contains the same NULL value as 'cur->next' did.
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wcast-qual"              // warning: cast from 'const xxxx *' to 'xxx *' drops const qualifier
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"              // warning: comparison is always true due to limited range of data type [-Wtype-limits]
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'const xxxx *' to type 'xxxx *' casts away qualifiers
#endif

#ifndef STB_RECT_PACK_IMPLEMENTATION                        // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION          // in case the user already have an implementation in another compilation unit
#define STBRP_STATIC
#define STBRP_ASSERT(x)     do { IM_ASSERT(x); } while (0)
#define STBRP_SORT          ImQsort
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#ifdef IMGUI_STB_RECT_PACK_FILENAME
#include IMGUI_STB_RECT_PACK_FILENAME
#else
#include "imstb_rectpack.h"
#endif
#endif

#ifdef  IMGUI_ENABLE_STB_TRUETYPE
#ifndef STB_TRUETYPE_IMPLEMENTATION                         // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION           // in case the user already have an implementation in another compilation unit
#define STBTT_malloc(x,u)   ((void)(u), IM_ALLOC(x))
#define STBTT_free(x,u)     ((void)(u), IM_FREE(x))
#define STBTT_assert(x)     do { IM_ASSERT(x); } while(0)
#define STBTT_fmod(x,y)     ImFmod(x,y)
#define STBTT_sqrt(x)       ImSqrt(x)
#define STBTT_pow(x,y)      ImPow(x,y)
#define STBTT_fabs(x)       ImFabs(x)
#define STBTT_ifloor(x)     ((int)ImFloorSigned(x))
#define STBTT_iceil(x)      ((int)ImCeil(x))
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#ifdef IMGUI_STB_TRUETYPE_FILENAME
#include IMGUI_STB_TRUETYPE_FILENAME
#else
#include "imstb_truetype.h"
#endif
#endif
#endif // IMGUI_ENABLE_STB_TRUETYPE

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

#ifdef IMGUI_STB_NAMESPACE
} // namespace ImStb
using namespace IMGUI_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// [SECTION] Style functions
//-----------------------------------------------------------------------------

void ImGui::StyleColorsDark(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ImGui::StyleColorsClassic(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
void ImGui::StyleColorsLight(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.35f, 0.35f, 0.35f, 0.17f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.90f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.57f, 0.57f, 0.64f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.68f, 0.68f, 0.74f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

//-----------------------------------------------------------------------------
// [SECTION] ImDrawList
//-----------------------------------------------------------------------------

ImDrawListSharedData::ImDrawListSharedData()
{
    memset(this, 0, sizeof(*this));
    for (int i = 0; i < IM_ARRAYSIZE(ArcFastVtx); i++)
    {
        const float a = ((float)i * 2 * IM_PI) / (float)IM_ARRAYSIZE(ArcFastVtx);
        ArcFastVtx[i] = ImVec2(ImCos(a), ImSin(a));
    }
    ArcFastRadiusCutoff = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(IM_DRAWLIST_ARCFAST_SAMPLE_MAX, CircleSegmentMaxError);
}

void ImDrawListSharedData::SetCircleTessellationMaxError(float max_error)
{
    if (CircleSegmentMaxError == max_error)
        return;

    IM_ASSERT(max_error > 0.0f);
    CircleSegmentMaxError = max_error;
    for (int i = 0; i < IM_ARRAYSIZE(CircleSegmentCounts); i++)
    {
        const float radius = (float)i;
        CircleSegmentCounts[i] = (ImU8)((i > 0) ? IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, CircleSegmentMaxError) : 0);
    }
    ArcFastRadiusCutoff = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(IM_DRAWLIST_ARCFAST_SAMPLE_MAX, CircleSegmentMaxError);
}

// Initialize before use in a new frame. We always have a command ready in the buffer.
void ImDrawList::_ResetForNewFrame()
{
    // Verify that the ImDrawCmd fields we want to memcmp() are contiguous in memory.
    // (those should be IM_STATIC_ASSERT() in theory but with our pre C++11 setup the whole check doesn't compile with GCC)
    IM_ASSERT(IM_OFFSETOF(ImDrawCmd, ClipRect) == 0);
    IM_ASSERT(IM_OFFSETOF(ImDrawCmd, TextureId) == sizeof(ImVec4));
    IM_ASSERT(IM_OFFSETOF(ImDrawCmd, VtxOffset) == sizeof(ImVec4) + sizeof(ImTextureID));

    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    Flags = _Data->InitialFlags;
    memset(&_CmdHeader, 0, sizeof(_CmdHeader));
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _Splitter.Clear();
    CmdBuffer.push_back(ImDrawCmd());
    _FringeScale = 1.0f;
}

void ImDrawList::_ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    Flags = ImDrawListFlags_None;
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _Splitter.ClearFreeMemory();
}

ImDrawList* ImDrawList::CloneOutput() const
{
    ImDrawList* dst = IM_NEW(ImDrawList(_Data));
    dst->CmdBuffer = CmdBuffer;
    dst->IdxBuffer = IdxBuffer;
    dst->VtxBuffer = VtxBuffer;
    dst->Flags = Flags;
    return dst;
}

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = _CmdHeader.ClipRect;    // Same as calling ImDrawCmd_HeaderCopy()
    draw_cmd.TextureId = _CmdHeader.TextureId;
    draw_cmd.VtxOffset = _CmdHeader.VtxOffset;
    draw_cmd.IdxOffset = IdxBuffer.Size;

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

// Pop trailing draw command (used before merging or presenting to user)
// Note that this leaves the ImDrawList in a state unfit for further commands, as most code assume that CmdBuffer.Size > 0 && CmdBuffer.back().UserCallback == NULL
void ImDrawList::_PopUnusedDrawCmd()
{
    if (CmdBuffer.Size == 0)
        return;
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount == 0 && curr_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    IM_ASSERT_PARANOID(CmdBuffer.Size > 0);
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    IM_ASSERT(curr_cmd->UserCallback == NULL);
    if (curr_cmd->ElemCount != 0)
    {
        AddDrawCmd();
        curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    }
    curr_cmd->UserCallback = callback;
    curr_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}

// Compare ClipRect, TextureId and VtxOffset with a single memcmp()
#define ImDrawCmd_HeaderSize                        (IM_OFFSETOF(ImDrawCmd, VtxOffset) + sizeof(unsigned int))
#define ImDrawCmd_HeaderCompare(CMD_LHS, CMD_RHS)   (memcmp(CMD_LHS, CMD_RHS, ImDrawCmd_HeaderSize))    // Compare ClipRect, TextureId, VtxOffset
#define ImDrawCmd_HeaderCopy(CMD_DST, CMD_SRC)      (memcpy(CMD_DST, CMD_SRC, ImDrawCmd_HeaderSize))    // Copy ClipRect, TextureId, VtxOffset

// Try to merge two last draw commands
void ImDrawList::_TryMergeDrawCmds()
{
    IM_ASSERT_PARANOID(CmdBuffer.Size > 0);
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    ImDrawCmd* prev_cmd = curr_cmd - 1;
    if (ImDrawCmd_HeaderCompare(curr_cmd, prev_cmd) == 0 && curr_cmd->UserCallback == NULL && prev_cmd->UserCallback == NULL)
    {
        prev_cmd->ElemCount += curr_cmd->ElemCount;
        CmdBuffer.pop_back();
    }
}

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect etc. to not have to perform any check so we always have a command ready in the stack.
// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
void ImDrawList::_OnChangedClipRect()
{
    // If current command is used with different settings we need to add a new command
    IM_ASSERT_PARANOID(CmdBuffer.Size > 0);
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &_CmdHeader.ClipRect, sizeof(ImVec4)) != 0)
    {
        AddDrawCmd();
        return;
    }
    IM_ASSERT(curr_cmd->UserCallback == NULL);

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = curr_cmd - 1;
    if (curr_cmd->ElemCount == 0 && CmdBuffer.Size > 1 && ImDrawCmd_HeaderCompare(&_CmdHeader, prev_cmd) == 0 && prev_cmd->UserCallback == NULL)
    {
        CmdBuffer.pop_back();
        return;
    }

    curr_cmd->ClipRect = _CmdHeader.ClipRect;
}

void ImDrawList::_OnChangedTextureID()
{
    // If current command is used with different settings we need to add a new command
    IM_ASSERT_PARANOID(CmdBuffer.Size > 0);
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != _CmdHeader.TextureId)
    {
        AddDrawCmd();
        return;
    }
    IM_ASSERT(curr_cmd->UserCallback == NULL);

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = curr_cmd - 1;
    if (curr_cmd->ElemCount == 0 && CmdBuffer.Size > 1 && ImDrawCmd_HeaderCompare(&_CmdHeader, prev_cmd) == 0 && prev_cmd->UserCallback == NULL)
    {
        CmdBuffer.pop_back();
        return;
    }

    curr_cmd->TextureId = _CmdHeader.TextureId;
}

void ImDrawList::_OnChangedVtxOffset()
{
    // We don't need to compare curr_cmd->VtxOffset != _CmdHeader.VtxOffset because we know it'll be different at the time we call this.
    _VtxCurrentIdx = 0;
    IM_ASSERT_PARANOID(CmdBuffer.Size > 0);
    ImDrawCmd* curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    //IM_ASSERT(curr_cmd->VtxOffset != _CmdHeader.VtxOffset); // See #3349
    if (curr_cmd->ElemCount != 0)
    {
        AddDrawCmd();
        return;
    }
    IM_ASSERT(curr_cmd->UserCallback == NULL);
    curr_cmd->VtxOffset = _CmdHeader.VtxOffset;
}

int ImDrawList::_CalcCircleAutoSegmentCount(float radius) const
{
    // Automatic segment count
    const int radius_idx = (int)(radius + 0.999999f); // ceil to never reduce accuracy
    if (radius_idx < IM_ARRAYSIZE(_Data->CircleSegmentCounts))
        return _Data->CircleSegmentCounts[radius_idx]; // Use cached value
    else
        return IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, _Data->CircleSegmentMaxError);
}

// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
void ImDrawList::PushClipRect(ImVec2 cr_min, ImVec2 cr_max, bool intersect_with_current_clip_rect)
{
    ImVec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
    if (intersect_with_current_clip_rect)
    {
        ImVec4 current = _CmdHeader.ClipRect;
        if (cr.x < current.x) cr.x = current.x;
        if (cr.y < current.y) cr.y = current.y;
        if (cr.z > current.z) cr.z = current.z;
        if (cr.w > current.w) cr.w = current.w;
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    _ClipRectStack.push_back(cr);
    _CmdHeader.ClipRect = cr;
    _OnChangedClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(ImVec2(_Data->ClipRectFullscreen.x, _Data->ClipRectFullscreen.y), ImVec2(_Data->ClipRectFullscreen.z, _Data->ClipRectFullscreen.w));
}

void ImDrawList::PopClipRect()
{
    _ClipRectStack.pop_back();
    _CmdHeader.ClipRect = (_ClipRectStack.Size == 0) ? _Data->ClipRectFullscreen : _ClipRectStack.Data[_ClipRectStack.Size - 1];
    _OnChangedClipRect();
}

void ImDrawList::PushTextureID(ImTextureID texture_id)
{
    _TextureIdStack.push_back(texture_id);
    _CmdHeader.TextureId = texture_id;
    _OnChangedTextureID();
}

void ImDrawList::PopTextureID()
{
    _TextureIdStack.pop_back();
    _CmdHeader.TextureId = (_TextureIdStack.Size == 0) ? (ImTextureID)NULL : _TextureIdStack.Data[_TextureIdStack.Size - 1];
    _OnChangedTextureID();
}

// Reserve space for a number of vertices and indices.
// You must finish filling your reserved data before calling PrimReserve() again, as it may reallocate or
// submit the intermediate results. PrimUnreserve() can be used to release unused allocations.
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    // Large mesh support (when enabled)
    IM_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);
    if (sizeof(ImDrawIdx) == 2 && (_VtxCurrentIdx + vtx_count >= (1 << 16)) && (Flags & ImDrawListFlags_AllowVtxOffset))
    {
        // FIXME: In theory we should be testing that vtx_count <64k here.
        // In practice, RenderText() relies on reserving ahead for a worst case scenario so it is currently useful for us
        // to not make that check until we rework the text functions to handle clipping and large horizontal lines better.
        _CmdHeader.VtxOffset = VtxBuffer.Size;
        _OnChangedVtxOffset();
    }

    ImDrawCmd* draw_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd->ElemCount += idx_count;

    int vtx_buffer_old_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_old_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_old_size;

    int idx_buffer_old_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_old_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_old_size;
}

// Release the a number of reserved vertices/indices from the end of the last reservation made with PrimReserve().
void ImDrawList::PrimUnreserve(int idx_count, int vtx_count)
{
    IM_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);

    ImDrawCmd* draw_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd->ElemCount -= idx_count;
    VtxBuffer.shrink(VtxBuffer.Size - vtx_count);
    IdxBuffer.shrink(IdxBuffer.Size - idx_count);
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv(_Data->TexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimRectUV(const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// On AddPolyline() and AddConvexPolyFilled() we intentionally avoid using ImVec2 and superfluous function calls to optimize debug/non-inlined builds.
// - Those macros expects l-values and need to be used as their own statement.
// - Those macros are intentionally not surrounded by the 'do {} while (0)' idiom because even that translates to runtime with debug compilers.
#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = ImRsqrt(d2); VX *= inv_len; VY *= inv_len; } } (void)0
#define IM_FIXNORMAL2F_MAX_INVLEN2          100.0f // 500.0f (see #4053, #3366)
#define IM_FIXNORMAL2F(VX,VY)               { float d2 = VX*VX + VY*VY; if (d2 > 0.000001f) { float inv_len2 = 1.0f / d2; if (inv_len2 > IM_FIXNORMAL2F_MAX_INVLEN2) inv_len2 = IM_FIXNORMAL2F_MAX_INVLEN2; VX *= inv_len2; VY *= inv_len2; } } (void)0

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
// We avoid using the ImVec2 math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, ImDrawFlags flags, float thickness)
{
    if (points_count < 2)
        return;

    const bool closed = (flags & ImDrawFlags_Closed) != 0;
    const ImVec2 opaque_uv = _Data->TexUvWhitePixel;
    const int count = closed ? points_count : points_count - 1; // The number of line segments we need to draw
    const bool thick_line = (thickness > _FringeScale);

    if (Flags & ImDrawListFlags_AntiAliasedLines)
    {
        // Anti-aliased stroke
        const float AA_SIZE = _FringeScale;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;

        // Thicknesses <1.0 should behave like thickness 1.0
        thickness = ImMax(thickness, 1.0f);
        const int integer_thickness = (int)thickness;
        const float fractional_thickness = thickness - integer_thickness;

        // Do we want to draw this line using a texture?
        // - For now, only draw integer-width lines using textures to avoid issues with the way scaling occurs, could be improved.
        // - If AA_SIZE is not 1.0f we cannot use the texture path.
        const bool use_texture = (Flags & ImDrawListFlags_AntiAliasedLinesUseTex) && (integer_thickness < IM_DRAWLIST_TEX_LINES_WIDTH_MAX) && (fractional_thickness <= 0.00001f) && (AA_SIZE == 1.0f);

        // We should never hit this, because NewFrame() doesn't set ImDrawListFlags_AntiAliasedLinesUseTex unless ImFontAtlasFlags_NoBakedLines is off
        IM_ASSERT_PARANOID(!use_texture || !(_Data->Font->ContainerAtlas->Flags & ImFontAtlasFlags_NoBakedLines));

        const int idx_count = use_texture ? (count * 6) : (thick_line ? count * 18 : count * 12);
        const int vtx_count = use_texture ? (points_count * 2) : (thick_line ? points_count * 4 : points_count * 3);
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        // The first <points_count> items are normals at each line point, then after that there are either 2 or 4 temp points for each line point
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * ((use_texture || !thick_line) ? 3 : 5) * sizeof(ImVec2)); //-V630
        ImVec2* temp_points = temp_normals + points_count;

        // Calculate normals (tangents) for each line segment
        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            float dx = points[i2].x - points[i1].x;
            float dy = points[i2].y - points[i1].y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i1].x = dy;
            temp_normals[i1].y = -dx;
        }
        if (!closed)
            temp_normals[points_count - 1] = temp_normals[points_count - 2];

        // If we are drawing a one-pixel-wide line without a texture, or a textured line of any width, we only need 2 or 3 vertices per point
        if (use_texture || !thick_line)
        {
            // [PATH 1] Texture-based lines (thick or non-thick)
            // [PATH 2] Non texture-based lines (non-thick)

            // The width of the geometry we need to draw - this is essentially <thickness> pixels for the line itself, plus "one pixel" for AA.
            // - In the texture-based path, we don't use AA_SIZE here because the +1 is tied to the generated texture
            //   (see ImFontAtlasBuildRenderLinesTexData() function), and so alternate values won't work without changes to that code.
            // - In the non texture-based paths, we would allow AA_SIZE to potentially be != 1.0f with a patch (e.g. fringe_scale patch to
            //   allow scaling geometry while preserving one-screen-pixel AA fringe).
            const float half_draw_size = use_texture ? ((thickness * 0.5f) + 1) : AA_SIZE;

            // If line is not closed, the first and last points need to be generated differently as there are no normals to blend
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * half_draw_size;
                temp_points[1] = points[0] - temp_normals[0] * half_draw_size;
                temp_points[(points_count-1)*2+0] = points[points_count-1] + temp_normals[points_count-1] * half_draw_size;
                temp_points[(points_count-1)*2+1] = points[points_count-1] - temp_normals[points_count-1] * half_draw_size;
            }

            // Generate the indices to form a number of triangles for each line segment, and the vertices for the line edges
            // This takes points n and n+1 and writes into n+1, with the first point in a closed line being generated from the final one (as n+1 wraps)
            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx; // Vertex index for start of line segment
            for (int i1 = 0; i1 < count; i1++) // i1 is the first point of the line segment
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1; // i2 is the second point of the line segment
                const unsigned int idx2 = ((i1 + 1) == points_count) ? _VtxCurrentIdx : (idx1 + (use_texture ? 2 : 3)); // Vertex index for end of segment

                // Average normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_FIXNORMAL2F(dm_x, dm_y);
                dm_x *= half_draw_size; // dm_x, dm_y are offset to the outer edge of the AA area
                dm_y *= half_draw_size;

                // Add temporary vertexes for the outer edges
                ImVec2* out_vtx = &temp_points[i2 * 2];
                out_vtx[0].x = points[i2].x + dm_x;
                out_vtx[0].y = points[i2].y + dm_y;
                out_vtx[1].x = points[i2].x - dm_x;
                out_vtx[1].y = points[i2].y - dm_y;

                if (use_texture)
                {
                    // Add indices for two triangles
                    _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 1); // Right tri
                    _IdxWritePtr[3] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[4] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 0); // Left tri
                    _IdxWritePtr += 6;
                }
                else
                {
                    // Add indexes for four triangles
                    _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2); // Right tri 1
                    _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 0); // Right tri 2
                    _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0); // Left tri 1
                    _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1); // Left tri 2
                    _IdxWritePtr += 12;
                }

                idx1 = idx2;
            }

            // Add vertexes for each point on the line
            if (use_texture)
            {
                // If we're using textures we only need to emit the left/right edge vertices
                ImVec4 tex_uvs = _Data->TexUvLines[integer_thickness];
                /*if (fractional_thickness != 0.0f) // Currently always zero when use_texture==false!
                {
                    const ImVec4 tex_uvs_1 = _Data->TexUvLines[integer_thickness + 1];
                    tex_uvs.x = tex_uvs.x + (tex_uvs_1.x - tex_uvs.x) * fractional_thickness; // inlined ImLerp()
                    tex_uvs.y = tex_uvs.y + (tex_uvs_1.y - tex_uvs.y) * fractional_thickness;
                    tex_uvs.z = tex_uvs.z + (tex_uvs_1.z - tex_uvs.z) * fractional_thickness;
                    tex_uvs.w = tex_uvs.w + (tex_uvs_1.w - tex_uvs.w) * fractional_thickness;
                }*/
                ImVec2 tex_uv0(tex_uvs.x, tex_uvs.y);
                ImVec2 tex_uv1(tex_uvs.z, tex_uvs.w);
                for (int i = 0; i < points_count; i++)
                {
                    _VtxWritePtr[0].pos = temp_points[i * 2 + 0]; _VtxWritePtr[0].uv = tex_uv0; _VtxWritePtr[0].col = col; // Left-side outer edge
                    _VtxWritePtr[1].pos = temp_points[i * 2 + 1]; _VtxWritePtr[1].uv = tex_uv1; _VtxWritePtr[1].col = col; // Right-side outer edge
                    _VtxWritePtr += 2;
                }
            }
            else
            {
                // If we're not using a texture, we need the center vertex as well
                for (int i = 0; i < points_count; i++)
                {
                    _VtxWritePtr[0].pos = points[i];              _VtxWritePtr[0].uv = opaque_uv; _VtxWritePtr[0].col = col;       // Center of line
                    _VtxWritePtr[1].pos = temp_points[i * 2 + 0]; _VtxWritePtr[1].uv = opaque_uv; _VtxWritePtr[1].col = col_trans; // Left-side outer edge
                    _VtxWritePtr[2].pos = temp_points[i * 2 + 1]; _VtxWritePtr[2].uv = opaque_uv; _VtxWritePtr[2].col = col_trans; // Right-side outer edge
                    _VtxWritePtr += 3;
                }
            }
        }
        else
        {
            // [PATH 2] Non texture-based lines (thick): we need to draw the solid line core and thus require four vertices per point
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;

            // If line is not closed, the first and last points need to be generated differently as there are no normals to blend
            if (!closed)
            {
                const int points_last = points_count - 1;
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[points_last * 4 + 0] = points[points_last] + temp_normals[points_last] * (half_inner_thickness + AA_SIZE);
                temp_points[points_last * 4 + 1] = points[points_last] + temp_normals[points_last] * (half_inner_thickness);
                temp_points[points_last * 4 + 2] = points[points_last] - temp_normals[points_last] * (half_inner_thickness);
                temp_points[points_last * 4 + 3] = points[points_last] - temp_normals[points_last] * (half_inner_thickness + AA_SIZE);
            }

            // Generate the indices to form a number of triangles for each line segment, and the vertices for the line edges
            // This takes points n and n+1 and writes into n+1, with the first point in a closed line being generated from the final one (as n+1 wraps)
            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx; // Vertex index for start of line segment
            for (int i1 = 0; i1 < count; i1++) // i1 is the first point of the line segment
            {
                const int i2 = (i1 + 1) == points_count ? 0 : (i1 + 1); // i2 is the second point of the line segment
                const unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : (idx1 + 4); // Vertex index for end of segment

                // Average normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_FIXNORMAL2F(dm_x, dm_y);
                float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
                float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
                float dm_in_x = dm_x * half_inner_thickness;
                float dm_in_y = dm_y * half_inner_thickness;

                // Add temporary vertices
                ImVec2* out_vtx = &temp_points[i2 * 4];
                out_vtx[0].x = points[i2].x + dm_out_x;
                out_vtx[0].y = points[i2].y + dm_out_y;
                out_vtx[1].x = points[i2].x + dm_in_x;
                out_vtx[1].y = points[i2].y + dm_in_y;
                out_vtx[2].x = points[i2].x - dm_in_x;
                out_vtx[2].y = points[i2].y - dm_in_y;
                out_vtx[3].x = points[i2].x - dm_out_x;
                out_vtx[3].y = points[i2].y - dm_out_y;

                // Add indexes
                _IdxWritePtr[0]  = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[1]  = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[2]  = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3]  = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4]  = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5]  = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[6]  = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7]  = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8]  = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9]  = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[13] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[14] = (ImDrawIdx)(idx1 + 3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1 + 3); _IdxWritePtr[16] = (ImDrawIdx)(idx2 + 3); _IdxWritePtr[17] = (ImDrawIdx)(idx2 + 2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertices
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i * 4 + 0]; _VtxWritePtr[0].uv = opaque_uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i * 4 + 1]; _VtxWritePtr[1].uv = opaque_uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i * 4 + 2]; _VtxWritePtr[2].uv = opaque_uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i * 4 + 3]; _VtxWritePtr[3].uv = opaque_uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // [PATH 4] Non texture-based, Non anti-aliased lines
        const int idx_count = count * 6;
        const int vtx_count = count * 4;    // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            dx *= (thickness * 0.5f);
            dy *= (thickness * 0.5f);

            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = opaque_uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = opaque_uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = opaque_uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = opaque_uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

// We intentionally avoid using ImVec2 and its math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col)
{
    if (points_count < 3)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;

    if (Flags & ImDrawListFlags_AntiAliasedFill)
    {
        // Anti-aliased Fill
        const float AA_SIZE = _FringeScale;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count - 2)*3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2)); //-V630
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }

        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            float dm_x = (n0.x + n1.x) * 0.5f;
            float dm_y = (n0.y + n1.y) * 0.5f;
            IM_FIXNORMAL2F(dm_x, dm_y);
            dm_x *= AA_SIZE * 0.5f;
            dm_y *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos.x = (points[i1].x - dm_x); _VtxWritePtr[0].pos.y = (points[i1].y - dm_y); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos.x = (points[i1].x + dm_x); _VtxWritePtr[1].pos.y = (points[i1].y + dm_y); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2)*3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + i - 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::_PathArcToFastEx(const ImVec2& center, float radius, int a_min_sample, int a_max_sample, int a_step)
{
    if (radius <= 0.0f)
    {
        _Path.push_back(center);
        return;
    }

    // Calculate arc auto segment step size
    if (a_step <= 0)
        a_step = IM_DRAWLIST_ARCFAST_SAMPLE_MAX / _CalcCircleAutoSegmentCount(radius);

    // Make sure we never do steps larger than one quarter of the circle
    a_step = ImClamp(a_step, 1, IM_DRAWLIST_ARCFAST_TABLE_SIZE / 4);

    const int sample_range = ImAbs(a_max_sample - a_min_sample);
    const int a_next_step = a_step;

    int samples = sample_range + 1;
    bool extra_max_sample = false;
    if (a_step > 1)
    {
        samples            = sample_range / a_step + 1;
        const int overstep = sample_range % a_step;

        if (overstep > 0)
        {
            extra_max_sample = true;
            samples++;

            // When we have overstep to avoid awkwardly looking one long line and one tiny one at the end,
            // distribute first step range evenly between them by reducing first step size.
            if (sample_range > 0)
                a_step -= (a_step - overstep) / 2;
        }
    }

    _Path.resize(_Path.Size + samples);
    ImVec2* out_ptr = _Path.Data + (_Path.Size - samples);

    int sample_index = a_min_sample;
    if (sample_index < 0 || sample_index >= IM_DRAWLIST_ARCFAST_SAMPLE_MAX)
    {
        sample_index = sample_index % IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        if (sample_index < 0)
            sample_index += IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
    }

    if (a_max_sample >= a_min_sample)
    {
        for (int a = a_min_sample; a <= a_max_sample; a += a_step, sample_index += a_step, a_step = a_next_step)
        {
            // a_step is clamped to IM_DRAWLIST_ARCFAST_SAMPLE_MAX, so we have guaranteed that it will not wrap over range twice or more
            if (sample_index >= IM_DRAWLIST_ARCFAST_SAMPLE_MAX)
                sample_index -= IM_DRAWLIST_ARCFAST_SAMPLE_MAX;

            const ImVec2 s = _Data->ArcFastVtx[sample_index];
            out_ptr->x = center.x + s.x * radius;
            out_ptr->y = center.y + s.y * radius;
            out_ptr++;
        }
    }
    else
    {
        for (int a = a_min_sample; a >= a_max_sample; a -= a_step, sample_index -= a_step, a_step = a_next_step)
        {
            // a_step is clamped to IM_DRAWLIST_ARCFAST_SAMPLE_MAX, so we have guaranteed that it will not wrap over range twice or more
            if (sample_index < 0)
                sample_index += IM_DRAWLIST_ARCFAST_SAMPLE_MAX;

            const ImVec2 s = _Data->ArcFastVtx[sample_index];
            out_ptr->x = center.x + s.x * radius;
            out_ptr->y = center.y + s.y * radius;
            out_ptr++;
        }
    }

    if (extra_max_sample)
    {
        int normalized_max_sample = a_max_sample % IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        if (normalized_max_sample < 0)
            normalized_max_sample += IM_DRAWLIST_ARCFAST_SAMPLE_MAX;

        const ImVec2 s = _Data->ArcFastVtx[normalized_max_sample];
        out_ptr->x = center.x + s.x * radius;
        out_ptr->y = center.y + s.y * radius;
        out_ptr++;
    }

    IM_ASSERT_PARANOID(_Path.Data + _Path.Size == out_ptr);
}

void ImDrawList::_PathArcToN(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{
    if (radius <= 0.0f)
    {
        _Path.push_back(center);
        return;
    }

    // Note that we are adding a point at both a_min and a_max.
    // If you are trying to draw a full closed circle you don't want the overlapping points!
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        _Path.push_back(ImVec2(center.x + ImCos(a) * radius, center.y + ImSin(a) * radius));
    }
}

// 0: East, 3: South, 6: West, 9: North, 12: East
void ImDrawList::PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12)
{
    if (radius <= 0.0f)
    {
        _Path.push_back(center);
        return;
    }
    _PathArcToFastEx(center, radius, a_min_of_12 * IM_DRAWLIST_ARCFAST_SAMPLE_MAX / 12, a_max_of_12 * IM_DRAWLIST_ARCFAST_SAMPLE_MAX / 12, 0);
}

void ImDrawList::PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{
    if (radius <= 0.0f)
    {
        _Path.push_back(center);
        return;
    }

    if (num_segments > 0)
    {
        _PathArcToN(center, radius, a_min, a_max, num_segments);
        return;
    }

    // Automatic segment count
    if (radius <= _Data->ArcFastRadiusCutoff)
    {
        const bool a_is_reverse = a_max < a_min;

        // We are going to use precomputed values for mid samples.
        // Determine first and last sample in lookup table that belong to the arc.
        const float a_min_sample_f = IM_DRAWLIST_ARCFAST_SAMPLE_MAX * a_min / (IM_PI * 2.0f);
        const float a_max_sample_f = IM_DRAWLIST_ARCFAST_SAMPLE_MAX * a_max / (IM_PI * 2.0f);

        const int a_min_sample = a_is_reverse ? (int)ImFloorSigned(a_min_sample_f) : (int)ImCeil(a_min_sample_f);
        const int a_max_sample = a_is_reverse ? (int)ImCeil(a_max_sample_f) : (int)ImFloorSigned(a_max_sample_f);
        const int a_mid_samples = a_is_reverse ? ImMax(a_min_sample - a_max_sample, 0) : ImMax(a_max_sample - a_min_sample, 0);

        const float a_min_segment_angle = a_min_sample * IM_PI * 2.0f / IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        const float a_max_segment_angle = a_max_sample * IM_PI * 2.0f / IM_DRAWLIST_ARCFAST_SAMPLE_MAX;
        const bool a_emit_start = (a_min_segment_angle - a_min) != 0.0f;
        const bool a_emit_end = (a_max - a_max_segment_angle) != 0.0f;

        _Path.reserve(_Path.Size + (a_mid_samples + 1 + (a_emit_start ? 1 : 0) + (a_emit_end ? 1 : 0)));
        if (a_emit_start)
            _Path.push_back(ImVec2(center.x + ImCos(a_min) * radius, center.y + ImSin(a_min) * radius));
        if (a_mid_samples > 0)
            _PathArcToFastEx(center, radius, a_min_sample, a_max_sample, 0);
        if (a_emit_end)
            _Path.push_back(ImVec2(center.x + ImCos(a_max) * radius, center.y + ImSin(a_max) * radius));
    }
    else
    {
        const float arc_length = ImAbs(a_max - a_min);
        const int circle_segment_count = _CalcCircleAutoSegmentCount(radius);
        const int arc_segment_count = ImMax((int)ImCeil(circle_segment_count * arc_length / (IM_PI * 2.0f)), (int)(2.0f * IM_PI / arc_length));
        _PathArcToN(center, radius, a_min, a_max, arc_segment_count);
    }
}

ImVec2 ImBezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
{
    float u = 1.0f - t;
    float w1 = u * u * u;
    float w2 = 3 * u * u * t;
    float w3 = 3 * u * t * t;
    float w4 = t * t * t;
    return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y);
}

ImVec2 ImBezierQuadraticCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float t)
{
    float u = 1.0f - t;
    float w1 = u * u;
    float w2 = 2 * u * t;
    float w3 = t * t;
    return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x, w1 * p1.y + w2 * p2.y + w3 * p3.y);
}

// Closely mimics ImBezierCubicClosestPointCasteljau() in imgui.cpp
static void PathBezierCubicCurveToCasteljau(ImVector<ImVec2>* path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = (x2 - x4) * dy - (y2 - y4) * dx;
    float d3 = (x3 - x4) * dy - (y3 - y4) * dx;
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
    {
        path->push_back(ImVec2(x4, y4));
    }
    else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
        float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;
        PathBezierCubicCurveToCasteljau(path, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        PathBezierCubicCurveToCasteljau(path, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}

static void PathBezierQuadraticCurveToCasteljau(ImVector<ImVec2>* path, float x1, float y1, float x2, float y2, float x3, float y3, float tess_tol, int level)
{
    float dx = x3 - x1, dy = y3 - y1;
    float det = (x2 - x3) * dy - (y2 - y3) * dx;
    if (det * det * 4.0f < tess_tol * (dx * dx + dy * dy))
    {
        path->push_back(ImVec2(x3, y3));
    }
    else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        PathBezierQuadraticCurveToCasteljau(path, x1, y1, x12, y12, x123, y123, tess_tol, level + 1);
        PathBezierQuadraticCurveToCasteljau(path, x123, y123, x23, y23, x3, y3, tess_tol, level + 1);
    }
}

void ImDrawList::PathBezierCubicCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
    {
        PathBezierCubicCurveToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, _Data->CurveTessellationTol, 0); // Auto-tessellated
    }
    else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
            _Path.push_back(ImBezierCubicCalc(p1, p2, p3, p4, t_step * i_step));
    }
}

void ImDrawList::PathBezierQuadraticCurveTo(const ImVec2& p2, const ImVec2& p3, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
    {
        PathBezierQuadraticCurveToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, _Data->CurveTessellationTol, 0);// Auto-tessellated
    }
    else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
            _Path.push_back(ImBezierQuadraticCalc(p1, p2, p3, t_step * i_step));
    }
}

IM_STATIC_ASSERT(ImDrawFlags_RoundCornersTopLeft == (1 << 4));
static inline ImDrawFlags FixRectCornerFlags(ImDrawFlags flags)
{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    // Legacy Support for hard coded ~0 (used to be a suggested equivalent to ImDrawCornerFlags_All)
    //   ~0   --> ImDrawFlags_RoundCornersAll or 0
    if (flags == ~0)
        return ImDrawFlags_RoundCornersAll;

    // Legacy Support for hard coded 0x01 to 0x0F (matching 15 out of 16 old flags combinations)
    //   0x01 --> ImDrawFlags_RoundCornersTopLeft (VALUE 0x01 OVERLAPS ImDrawFlags_Closed but ImDrawFlags_Closed is never valid in this path!)
    //   0x02 --> ImDrawFlags_RoundCornersTopRight
    //   0x03 --> ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight
    //   0x04 --> ImDrawFlags_RoundCornersBotLeft
    //   0x05 --> ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBotLeft
    //   ...
    //   0x0F --> ImDrawFlags_RoundCornersAll or 0
    // (See all values in ImDrawCornerFlags_)
    if (flags >= 0x01 && flags <= 0x0F)
        return (flags << 4);

    // We cannot support hard coded 0x00 with 'float rounding > 0.0f' --> replace with ImDrawFlags_RoundCornersNone or use 'float rounding = 0.0f'
#endif

    // If this triggers, please update your code replacing hardcoded values with new ImDrawFlags_RoundCorners* values.
    // Note that ImDrawFlags_Closed (== 0x01) is an invalid flag for AddRect(), AddRectFilled(), PathRect() etc...
    IM_ASSERT((flags & 0x0F) == 0 && "Misuse of legacy hardcoded ImDrawCornerFlags values!");

    if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
        flags |= ImDrawFlags_RoundCornersAll;

    return flags;
}

void ImDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, ImDrawFlags flags)
{
    flags = FixRectCornerFlags(flags);
    rounding = ImMin(rounding, ImFabs(b.x - a.x) * ( ((flags & ImDrawFlags_RoundCornersTop)  == ImDrawFlags_RoundCornersTop)  || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f ) - 1.0f);
    rounding = ImMin(rounding, ImFabs(b.y - a.y) * ( ((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight)  == ImDrawFlags_RoundCornersRight)  ? 0.5f : 1.0f ) - 1.0f);

    if (rounding <= 0.0f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone)
    {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x, a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x, b.y));
    }
    else
    {
        const float rounding_tl = (flags & ImDrawFlags_RoundCornersTopLeft)     ? rounding : 0.0f;
        const float rounding_tr = (flags & ImDrawFlags_RoundCornersTopRight)    ? rounding : 0.0f;
        const float rounding_br = (flags & ImDrawFlags_RoundCornersBottomRight) ? rounding : 0.0f;
        const float rounding_bl = (flags & ImDrawFlags_RoundCornersBottomLeft)  ? rounding : 0.0f;
        PathArcToFast(ImVec2(a.x + rounding_tl, a.y + rounding_tl), rounding_tl, 6, 9);
        PathArcToFast(ImVec2(b.x - rounding_tr, a.y + rounding_tr), rounding_tr, 9, 12);
        PathArcToFast(ImVec2(b.x - rounding_br, b.y - rounding_br), rounding_br, 0, 3);
        PathArcToFast(ImVec2(a.x + rounding_bl, b.y - rounding_bl), rounding_bl, 3, 6);
    }
}

void ImDrawList::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1 + ImVec2(0.5f, 0.5f));
    PathLineTo(p2 + ImVec2(0.5f, 0.5f));
    PathStroke(col, 0, thickness);
}

// p_min = upper-left, p_max = lower-right
// Note we don't render 1 pixels sized rectangles properly.
void ImDrawList::AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
        PathRect(p_min + ImVec2(0.50f, 0.50f), p_max - ImVec2(0.50f, 0.50f), rounding, flags);
    else
        PathRect(p_min + ImVec2(0.50f, 0.50f), p_max - ImVec2(0.49f, 0.49f), rounding, flags); // Better looking lower-right corner and rounded non-AA shapes.
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (rounding <= 0.0f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone)
    {
        PrimReserve(6, 4);
        PrimRect(p_min, p_max, col);
    }
    else
    {
        PathRect(p_min, p_max, rounding, flags);
        PathFillConvex(col);
    }
}

// p_min = upper-left, p_max = lower-right
void ImDrawList::AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 3));
    PrimWriteVtx(p_min, uv, col_upr_left);
    PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    PrimWriteVtx(p_max, uv, col_bot_right);
    PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
}

void ImDrawList::AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathFillConvex(col);
}

void ImDrawList::AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathFillConvex(col);
}

void ImDrawList::AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    if (num_segments <= 0)
    {
        // Use arc with automatic segment count
        _PathArcToFastEx(center, radius - 0.5f, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
        _Path.Size--;
    }
    else
    {
        // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
        num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);

        // Because we are filling a closed shape we remove 1 from the count of segments/points
        const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
        PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    }

    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    if (num_segments <= 0)
    {
        // Use arc with automatic segment count
        _PathArcToFastEx(center, radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
        _Path.Size--;
    }
    else
    {
        // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
        num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);

        // Because we are filling a closed shape we remove 1 from the count of segments/points
        const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
        PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
    }

    PathFillConvex(col);
}

// Guaranteed to honor 'num_segments'
void ImDrawList::AddNgon(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

// Guaranteed to honor 'num_segments'
void ImDrawList::AddNgonFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

// Cubic Bezier takes 4 controls points
void ImDrawList::AddBezierCubic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathBezierCubicCurveTo(p2, p3, p4, num_segments);
    PathStroke(col, 0, thickness);
}

// Quadratic Bezier takes 3 controls points
void ImDrawList::AddBezierQuadratic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(p1);
    PathBezierQuadraticCurveTo(p2, p3, num_segments);
    PathStroke(col, 0, thickness);
}

void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Pull default font/size from the shared ImDrawListSharedData instance
    if (font == NULL)
        font = _Data->Font;
    if (font_size == 0.0f)
        font_size = _Data->FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID == _CmdHeader.TextureId);  // Use high-level ImGui::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    ImVec4 clip_rect = _CmdHeader.ClipRect;
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
}

void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(NULL, 0.0f, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(p_min, p_max, uv_min, uv_max, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1, const ImVec2& uv2, const ImVec2& uv3, const ImVec2& uv4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimQuadUV(p1, p2, p3, p4, uv1, uv2, uv3, uv4, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    flags = FixRectCornerFlags(flags);
    if (rounding <= 0.0f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone)
    {
        AddImage(user_texture_id, p_min, p_max, uv_min, uv_max, col);
        return;
    }

    const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
    if (push_texture_id)
        PushTextureID(user_texture_id);

    int vert_start_idx = VtxBuffer.Size;
    PathRect(p_min, p_max, rounding, flags);
    PathFillConvex(col);
    int vert_end_idx = VtxBuffer.Size;
    ImGui::ShadeVertsLinearUV(this, vert_start_idx, vert_end_idx, p_min, p_max, uv_min, uv_max, true);

    if (push_texture_id)
        PopTextureID();
}


//-----------------------------------------------------------------------------
// [SECTION] ImDrawListSplitter
//-----------------------------------------------------------------------------
// FIXME: This may be a little confusing, trying to be a little too low-level/optimal instead of just doing vector swap..
//-----------------------------------------------------------------------------

void ImDrawListSplitter::ClearFreeMemory()
{
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == _Current)
            memset(&_Channels[i], 0, sizeof(_Channels[i]));  // Current channel is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i]._CmdBuffer.clear();
        _Channels[i]._IdxBuffer.clear();
    }
    _Current = 0;
    _Count = 1;
    _Channels.clear();
}

void ImDrawListSplitter::Split(ImDrawList* draw_list, int channels_count)
{
    IM_UNUSED(draw_list);
    IM_ASSERT(_Current == 0 && _Count <= 1 && "Nested channel splitting is not supported. Please use separate instances of ImDrawListSplitter.");
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
    {
        _Channels.reserve(channels_count); // Avoid over reserving since this is likely to stay stable
        _Channels.resize(channels_count);
    }
    _Count = channels_count;

    // Channels[] (24/32 bytes each) hold storage that we'll swap with draw_list->_CmdBuffer/_IdxBuffer
    // The content of Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy draw_list->_CmdBuffer/_IdxBuffer into Channels[0] and then Channels[1] into draw_list->CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        }
        else
        {
            _Channels[i]._CmdBuffer.resize(0);
            _Channels[i]._IdxBuffer.resize(0);
        }
    }
}

void ImDrawListSplitter::Merge(ImDrawList* draw_list)
{
    // Note that we never use or rely on _Channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_Count <= 1)
        return;

    SetCurrentChannel(draw_list, 0);
    draw_list->_PopUnusedDrawCmd();

    // Calculate our final buffer sizes. Also fix the incorrect IdxOffset values in each command.
    int new_cmd_buffer_count = 0;
    int new_idx_buffer_count = 0;
    ImDrawCmd* last_cmd = (_Count > 0 && draw_list->CmdBuffer.Size > 0) ? &draw_list->CmdBuffer.back() : NULL;
    int idx_offset = last_cmd ? last_cmd->IdxOffset + last_cmd->ElemCount : 0;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];

        // Equivalent of PopUnusedDrawCmd() for this channel's cmdbuffer and except we don't need to test for UserCallback.
        if (ch._CmdBuffer.Size > 0 && ch._CmdBuffer.back().ElemCount == 0)
            ch._CmdBuffer.pop_back();

        if (ch._CmdBuffer.Size > 0 && last_cmd != NULL)
        {
            ImDrawCmd* next_cmd = &ch._CmdBuffer[0];
            if (ImDrawCmd_HeaderCompare(last_cmd, next_cmd) == 0 && last_cmd->UserCallback == NULL && next_cmd->UserCallback == NULL)
            {
                // Merge previous channel last draw command with current channel first draw command if matching.
                last_cmd->ElemCount += next_cmd->ElemCount;
                idx_offset += next_cmd->ElemCount;
                ch._CmdBuffer.erase(ch._CmdBuffer.Data); // FIXME-OPT: Improve for multiple merges.
            }
        }
        if (ch._CmdBuffer.Size > 0)
            last_cmd = &ch._CmdBuffer.back();
        new_cmd_buffer_count += ch._CmdBuffer.Size;
        new_idx_buffer_count += ch._IdxBuffer.Size;
        for (int cmd_n = 0; cmd_n < ch._CmdBuffer.Size; cmd_n++)
        {
            ch._CmdBuffer.Data[cmd_n].IdxOffset = idx_offset;
            idx_offset += ch._CmdBuffer.Data[cmd_n].ElemCount;
        }
    }
    draw_list->CmdBuffer.resize(draw_list->CmdBuffer.Size + new_cmd_buffer_count);
    draw_list->IdxBuffer.resize(draw_list->IdxBuffer.Size + new_idx_buffer_count);

    // Write commands and indices in order (they are fairly small structures, we don't copy vertices only indices)
    ImDrawCmd* cmd_write = draw_list->CmdBuffer.Data + draw_list->CmdBuffer.Size - new_cmd_buffer_count;
    ImDrawIdx* idx_write = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch._CmdBuffer.Size) { memcpy(cmd_write, ch._CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch._IdxBuffer.Size) { memcpy(idx_write, ch._IdxBuffer.Data, sz * sizeof(ImDrawIdx)); idx_write += sz; }
    }
    draw_list->_IdxWritePtr = idx_write;

    // Ensure there's always a non-callback draw command trailing the command-buffer
    if (draw_list->CmdBuffer.Size == 0 || draw_list->CmdBuffer.back().UserCallback != NULL)
        draw_list->AddDrawCmd();

    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount == 0)
        ImDrawCmd_HeaderCopy(curr_cmd, &draw_list->_CmdHeader); // Copy ClipRect, TextureId, VtxOffset
    else if (ImDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
        draw_list->AddDrawCmd();

    _Count = 1;
}

void ImDrawListSplitter::SetCurrentChannel(ImDrawList* draw_list, int idx)
{
    IM_ASSERT(idx >= 0 && idx < _Count);
    if (_Current == idx)
        return;

    // Overwrite ImVector (12/16 bytes), four times. This is merely a silly optimization instead of doing .swap()
    memcpy(&_Channels.Data[_Current]._CmdBuffer, &draw_list->CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&_Channels.Data[_Current]._IdxBuffer, &draw_list->IdxBuffer, sizeof(draw_list->IdxBuffer));
    _Current = idx;
    memcpy(&draw_list->CmdBuffer, &_Channels.Data[idx]._CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&draw_list->IdxBuffer, &_Channels.Data[idx]._IdxBuffer, sizeof(draw_list->IdxBuffer));
    draw_list->_IdxWritePtr = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size;

    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = (draw_list->CmdBuffer.Size == 0) ? NULL : &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
    if (curr_cmd == NULL)
        draw_list->AddDrawCmd();
    else if (curr_cmd->ElemCount == 0)
        ImDrawCmd_HeaderCopy(curr_cmd, &draw_list->_CmdHeader); // Copy ClipRect, TextureId, VtxOffset
    else if (ImDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
        draw_list->AddDrawCmd();
}

//-----------------------------------------------------------------------------
// [SECTION] ImDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        if (cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}

// Helper to scale the ClipRect field of each ImDrawCmd.
// Use if your final output buffer is at a different scale than draw_data->DisplaySize,
// or if there is a difference between your window resolution and framebuffer resolution.
void ImDrawData::ScaleClipRects(const ImVec2& fb_scale)
{
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = ImVec4(cmd->ClipRect.x * fb_scale.x, cmd->ClipRect.y * fb_scale.y, cmd->ClipRect.z * fb_scale.x, cmd->ClipRect.w * fb_scale.y);
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Helpers ShadeVertsXXX functions
//-----------------------------------------------------------------------------

// Generic linear color gradient, write to RGB fields, leave A untouched.
void ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    const int col0_r = (int)(col0 >> IM_COL32_R_SHIFT) & 0xFF;
    const int col0_g = (int)(col0 >> IM_COL32_G_SHIFT) & 0xFF;
    const int col0_b = (int)(col0 >> IM_COL32_B_SHIFT) & 0xFF;
    const int col_delta_r = ((int)(col1 >> IM_COL32_R_SHIFT) & 0xFF) - col0_r;
    const int col_delta_g = ((int)(col1 >> IM_COL32_G_SHIFT) & 0xFF) - col0_g;
    const int col_delta_b = ((int)(col1 >> IM_COL32_B_SHIFT) & 0xFF) - col0_b;
    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
        int r = (int)(col0_r + col_delta_r * t);
        int g = (int)(col0_g + col_delta_g * t);
        int b = (int)(col0_b + col_delta_b * t);
        vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
    }
}

// Distribute UV over (a, b) rectangle
void ImGui::ShadeVertsLinearUV(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, bool clamp)
{
    const ImVec2 size = b - a;
    const ImVec2 uv_size = uv_b - uv_a;
    const ImVec2 scale = ImVec2(
        size.x != 0.0f ? (uv_size.x / size.x) : 0.0f,
        size.y != 0.0f ? (uv_size.y / size.y) : 0.0f);

    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    if (clamp)
    {
        const ImVec2 min = ImMin(uv_a, uv_b);
        const ImVec2 max = ImMax(uv_a, uv_b);
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = ImClamp(uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale), min, max);
    }
    else
    {
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontConfig
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
    memset(this, 0, sizeof(*this));
    FontDataOwnedByAtlas = true;
    OversampleH = 3; // FIXME: 2 may be a better default?
    OversampleV = 1;
    GlyphMaxAdvanceX = FLT_MAX;
    RasterizerMultiply = 1.0f;
    EllipsisChar = (ImWchar)-1;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
// The 2x2 white texels on the top left are the ones we'll use everywhere in Dear ImGui to render filled shapes.
// (This is used when io.MouseDrawCursor = true)
const int FONT_ATLAS_DEFAULT_TEX_DATA_W = 122; // Actual texture will be 2 times that + 1 spacing.
const int FONT_ATLAS_DEFAULT_TEX_DATA_H = 27;
static const char FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[FONT_ATLAS_DEFAULT_TEX_DATA_W * FONT_ATLAS_DEFAULT_TEX_DATA_H + 1] =
{
    "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-     XX          - XX       XX "
    "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-    X..X         -X..X     X..X"
    "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-    X..X         -X...X   X...X"
    "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-    X..X         - X...X X...X "
    "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-    X..X         -  X...X...X  "
    "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-    X..XXX       -   X.....X   "
    "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-    X..X..XXX    -    X...X    "
    "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -    X..X..X..XX  -     X.X     "
    "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -    X..X..X..X.X -    X...X    "
    "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        -XXX X..X..X..X..X-   X.....X   "
    "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         -X..XX........X..X-  X...X...X  "
    "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          -X...X...........X- X...X X...X "
    "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - X..............X-X...X   X...X"
    "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  X.............X-X..X     X..X"
    "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  X.............X- XX       XX "
    "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -   X............X--------------"
    "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -   X...........X -             "
    "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------    X..........X -             "
    "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           -    X..........X -             "
    "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           -     X........X  -             "
    "      X..X  -       -  X...X  -         X...X         -  X..X           X..X  -           -     X........X  -             "
    "       XX   -       -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -     XXXXXXXXXX  -             "
    "-------------       -    X    -           X           -X.....................X-           -------------------             "
    "                    ----------------------------------- X...XXXXXXXXXXXXX...X -                                           "
    "                                                      -  X..X           X..X  -                                           "
    "                                                      -   X.X           X.X   -                                           "
    "                                                      -    XX           XX    -                                           "
};

static const ImVec2 FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[ImGuiMouseCursor_COUNT][3] =
{
    // Pos ........ Size ......... Offset ......
    { ImVec2( 0,3), ImVec2(12,19), ImVec2( 0, 0) }, // ImGuiMouseCursor_Arrow
    { ImVec2(13,0), ImVec2( 7,16), ImVec2( 1, 8) }, // ImGuiMouseCursor_TextInput
    { ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // ImGuiMouseCursor_ResizeAll
    { ImVec2(21,0), ImVec2( 9,23), ImVec2( 4,11) }, // ImGuiMouseCursor_ResizeNS
    { ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 4) }, // ImGuiMouseCursor_ResizeEW
    { ImVec2(73,0), ImVec2(17,17), ImVec2( 8, 8) }, // ImGuiMouseCursor_ResizeNESW
    { ImVec2(55,0), ImVec2(17,17), ImVec2( 8, 8) }, // ImGuiMouseCursor_ResizeNWSE
    { ImVec2(91,0), ImVec2(17,22), ImVec2( 5, 0) }, // ImGuiMouseCursor_Hand
    { ImVec2(109,0),ImVec2(13,15), ImVec2( 6, 7) }, // ImGuiMouseCursor_NotAllowed
};

ImFontAtlas::ImFontAtlas()
{
    memset(this, 0, sizeof(*this));
    TexGlyphPadding = 1;
    PackIdMouseCursors = PackIdLines = -1;
}

ImFontAtlas::~ImFontAtlas()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    Clear();
}

void    ImFontAtlas::ClearInputData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            IM_FREE(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }

    // When clearing this we lose access to the font name and other information used to build the font.
    for (int i = 0; i < Fonts.Size; i++)
        if (Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size)
        {
            Fonts[i]->ConfigData = NULL;
            Fonts[i]->ConfigDataCount = 0;
        }
    ConfigData.clear();
    CustomRects.clear();
    PackIdMouseCursors = PackIdLines = -1;
    // Important: we leave TexReady untouched
}

void    ImFontAtlas::ClearTexData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    if (TexPixelsAlpha8)
        IM_FREE(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        IM_FREE(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexPixelsUseColors = false;
    // Important: we leave TexReady untouched
}

void    ImFontAtlas::ClearFonts()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    Fonts.clear_delete();
    TexReady = false;
}

void    ImFontAtlas::Clear()
{
    ClearInputData();
    ClearTexData();
    ClearFonts();
}

void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Build atlas on demand
    if (TexPixelsAlpha8 == NULL)
        Build();

    *out_pixels = TexPixelsAlpha8;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 1;
}

void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Convert to RGBA32 format on demand
    // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels = NULL;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        if (pixels)
        {
            TexPixelsRGBA32 = (unsigned int*)IM_ALLOC((size_t)TexWidth * (size_t)TexHeight * 4);
            const unsigned char* src = pixels;
            unsigned int* dst = TexPixelsRGBA32;
            for (int n = TexWidth * TexHeight; n > 0; n--)
                *dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
        }
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig* font_cfg)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
        Fonts.push_back(IM_NEW(ImFont));
    else
        IM_ASSERT(!Fonts.empty() && "Cannot use MergeMode for the first font"); // When using MergeMode make sure that a font has already been added before. You can use ImGui::GetIO().Fonts->AddFontDefault() to add the default imgui font.

    ConfigData.push_back(*font_cfg);
    ImFontConfig& new_font_cfg = ConfigData.back();
    if (new_font_cfg.DstFont == NULL)
        new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = IM_ALLOC(new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
    }

    if (new_font_cfg.DstFont->EllipsisChar == (ImWchar)-1)
        new_font_cfg.DstFont->EllipsisChar = font_cfg->EllipsisChar;

    // Invalidate texture
    TexReady = false;
    ClearTexData();
    return new_font_cfg.DstFont;
}

// Default font TTF is compressed with stb_compress then base85 encoded (see misc/fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(const unsigned char* input);
static unsigned int stb_decompress(unsigned char* output, const unsigned char* input, unsigned int length);
static const char*  GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c)                                    { return c >= '\\' ? c-36 : c-35; }
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = Decode85Byte(src[0]) + 85 * (Decode85Byte(src[1]) + 85 * (Decode85Byte(src[2]) + 85 * (Decode85Byte(src[3]) + 85 * Decode85Byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
ImFont* ImFontAtlas::AddFontDefault(const ImFontConfig* font_cfg_template)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (!font_cfg_template)
    {
        font_cfg.OversampleH = font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }
    if (font_cfg.SizePixels <= 0.0f)
        font_cfg.SizePixels = 13.0f * 1.0f;
    if (font_cfg.Name[0] == '\0')
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "ProggyClean.ttf, %dpx", (int)font_cfg.SizePixels);
    font_cfg.EllipsisChar = (ImWchar)0x0085;
    font_cfg.GlyphOffset.y = 1.0f * IM_FLOOR(font_cfg.SizePixels / 13.0f);  // Add +1 offset per 13 units

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    const ImWchar* glyph_ranges = font_cfg.GlyphRanges != NULL ? font_cfg.GlyphRanges : GetGlyphRangesDefault();
    ImFont* font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, font_cfg.SizePixels, &font_cfg, glyph_ranges);
    return font;
}

ImFont* ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    size_t data_size = 0;
    void* data = ImFileLoadToMemory(filename, "rb", &data_size, 0);
    if (!data)
    {
        IM_ASSERT_USER_ERROR(0, "Could not load font file!");
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        // Store a short copy of filename into into the font name for convenience
        const char* p;
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", p, size_pixels);
    }
    return AddFontFromMemoryTTF(data, (int)data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NB: Transfer ownership of 'ttf_data' to ImFontAtlas, unless font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after Build().
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontData = ttf_data;
    font_cfg.FontDataSize = ttf_size;
    font_cfg.SizePixels = size_pixels > 0.0f ? size_pixels : font_cfg.SizePixels;
    if (glyph_ranges)
        font_cfg.GlyphRanges = glyph_ranges;
    return AddFont(&font_cfg);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((const unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char*)IM_ALLOC(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (const unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, &font_cfg, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges)
{
    int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
    void* compressed_ttf = IM_ALLOC((size_t)compressed_ttf_size);
    Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned char*)compressed_ttf);
    ImFont* font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
    IM_FREE(compressed_ttf);
    return font;
}

int ImFontAtlas::AddCustomRectRegular(int width, int height)
{
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    ImFontAtlasCustomRect r;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

int ImFontAtlas::AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset)
{
#ifdef IMGUI_USE_WCHAR32
    IM_ASSERT(id <= IM_UNICODE_CODEPOINT_MAX);
#endif
    IM_ASSERT(font != NULL);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    ImFontAtlasCustomRect r;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    r.GlyphID = id;
    r.GlyphAdvanceX = advance_x;
    r.GlyphOffset = offset;
    r.Font = font;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

void ImFontAtlas::CalcCustomRectUV(const ImFontAtlasCustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max) const
{
    IM_ASSERT(TexWidth > 0 && TexHeight > 0);   // Font atlas needs to be built before we can calculate UV coordinates
    IM_ASSERT(rect->IsPacked());                // Make sure the rectangle has been packed
    *out_uv_min = ImVec2((float)rect->X * TexUvScale.x, (float)rect->Y * TexUvScale.y);
    *out_uv_max = ImVec2((float)(rect->X + rect->Width) * TexUvScale.x, (float)(rect->Y + rect->Height) * TexUvScale.y);
}

bool ImFontAtlas::GetMouseCursorTexData(ImGuiMouseCursor cursor_type, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2])
{
    if (cursor_type <= ImGuiMouseCursor_None || cursor_type >= ImGuiMouseCursor_COUNT)
        return false;
    if (Flags & ImFontAtlasFlags_NoMouseCursors)
        return false;

    IM_ASSERT(PackIdMouseCursors != -1);
    ImFontAtlasCustomRect* r = GetCustomRectByIndex(PackIdMouseCursors);
    ImVec2 pos = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][0] + ImVec2((float)r->X, (float)r->Y);
    ImVec2 size = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][1];
    *out_size = size;
    *out_offset = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][2];
    out_uv_border[0] = (pos) * TexUvScale;
    out_uv_border[1] = (pos + size) * TexUvScale;
    pos.x += FONT_ATLAS_DEFAULT_TEX_DATA_W + 1;
    out_uv_fill[0] = (pos) * TexUvScale;
    out_uv_fill[1] = (pos + size) * TexUvScale;
    return true;
}

bool    ImFontAtlas::Build()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");

    // Default font is none are specified
    if (ConfigData.Size == 0)
        AddFontDefault();

    // Select builder
    // - Note that we do not reassign to atlas->FontBuilderIO, since it is likely to point to static data which
    //   may mess with some hot-reloading schemes. If you need to assign to this (for dynamic selection) AND are
    //   using a hot-reloading scheme that messes up static data, store your own instance of ImFontBuilderIO somewhere
    //   and point to it instead of pointing directly to return value of the GetBuilderXXX functions.
    const ImFontBuilderIO* builder_io = FontBuilderIO;
    if (builder_io == NULL)
    {
#ifdef IMGUI_ENABLE_FREETYPE
        builder_io = ImGuiFreeType::GetBuilderForFreeType();
#elif defined(IMGUI_ENABLE_STB_TRUETYPE)
        builder_io = ImFontAtlasGetBuilderForStbTruetype();
#else
        IM_ASSERT(0); // Invalid Build function
#endif
    }

    // Build
    return builder_io->FontBuilder_Build(this);
}

void    ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_brighten_factor)
{
    for (unsigned int i = 0; i < 256; i++)
    {
        unsigned int value = (unsigned int)(i * in_brighten_factor);
        out_table[i] = value > 255 ? 255 : (value & 0xFF);
    }
}

void    ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride)
{
    unsigned char* data = pixels + x + y * stride;
    for (int j = h; j > 0; j--, data += stride)
        for (int i = 0; i < w; i++)
            data[i] = table[data[i]];
}

#ifdef IMGUI_ENABLE_STB_TRUETYPE
// Temporary data for one source font (multiple source fonts can be merged into one destination ImFont)
// (C++03 doesn't allow instancing ImVector<> with function-local types so we declare the type here.)
struct ImFontBuildSrcData
{
    stbtt_fontinfo      FontInfo;
    stbtt_pack_range    PackRange;          // Hold the list of codepoints to pack (essentially points to Codepoints.Data)
    stbrp_rect*         Rects;              // Rectangle to pack. We first fill in their size and the packer will give us their position.
    stbtt_packedchar*   PackedChars;        // Output glyphs
    const ImWchar*      SrcRanges;          // Ranges as requested by user (user is allowed to request too much, e.g. 0x0020..0xFFFF)
    int                 DstIndex;           // Index into atlas->Fonts[] and dst_tmp_array[]
    int                 GlyphsHighest;      // Highest requested codepoint
    int                 GlyphsCount;        // Glyph count (excluding missing glyphs and glyphs already set by an earlier source font)
    ImBitVector         GlyphsSet;          // Glyph bit map (random access, 1-bit per codepoint. This will be a maximum of 8KB)
    ImVector<int>       GlyphsList;         // Glyph codepoints list (flattened version of GlyphsMap)
};

// Temporary data for one destination ImFont* (multiple source fonts can be merged into one destination ImFont)
struct ImFontBuildDstData
{
    int                 SrcCount;           // Number of source fonts targeting this destination font.
    int                 GlyphsHighest;
    int                 GlyphsCount;
    ImBitVector         GlyphsSet;          // This is used to resolve collision when multiple sources are merged into a same destination font.
};

static void UnpackBitVectorToFlatIndexList(const ImBitVector* in, ImVector<int>* out)
{
    IM_ASSERT(sizeof(in->Storage.Data[0]) == sizeof(int));
    const ImU32* it_begin = in->Storage.begin();
    const ImU32* it_end = in->Storage.end();
    for (const ImU32* it = it_begin; it < it_end; it++)
        if (ImU32 entries_32 = *it)
            for (ImU32 bit_n = 0; bit_n < 32; bit_n++)
                if (entries_32 & ((ImU32)1 << bit_n))
                    out->push_back((int)(((it - it_begin) << 5) + bit_n));
}

static bool ImFontAtlasBuildWithStbTruetype(ImFontAtlas* atlas)
{
    IM_ASSERT(atlas->ConfigData.Size > 0);

    ImFontAtlasBuildInit(atlas);

    // Clear atlas
    atlas->TexID = (ImTextureID)NULL;
    atlas->TexWidth = atlas->TexHeight = 0;
    atlas->TexUvScale = ImVec2(0.0f, 0.0f);
    atlas->TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    atlas->ClearTexData();

    // Temporary storage for building
    ImVector<ImFontBuildSrcData> src_tmp_array;
    ImVector<ImFontBuildDstData> dst_tmp_array;
    src_tmp_array.resize(atlas->ConfigData.Size);
    dst_tmp_array.resize(atlas->Fonts.Size);
    memset(src_tmp_array.Data, 0, (size_t)src_tmp_array.size_in_bytes());
    memset(dst_tmp_array.Data, 0, (size_t)dst_tmp_array.size_in_bytes());

    // 1. Initialize font loading structure, check font data validity
    for (int src_i = 0; src_i < atlas->ConfigData.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == atlas));

        // Find index from cfg.DstFont (we allow the user to set cfg.DstFont. Also it makes casual debugging nicer than when storing indices)
        src_tmp.DstIndex = -1;
        for (int output_i = 0; output_i < atlas->Fonts.Size && src_tmp.DstIndex == -1; output_i++)
            if (cfg.DstFont == atlas->Fonts[output_i])
                src_tmp.DstIndex = output_i;
        if (src_tmp.DstIndex == -1)
        {
            IM_ASSERT(src_tmp.DstIndex != -1); // cfg.DstFont not pointing within atlas->Fonts[] array?
            return false;
        }
        // Initialize helper structure for font loading and verify that the TTF/OTF data is correct
        const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
        IM_ASSERT(font_offset >= 0 && "FontData is incorrect, or FontNo cannot be found.");
        if (!stbtt_InitFont(&src_tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
            return false;

        // Measure highest codepoints
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.SrcRanges = cfg.GlyphRanges ? cfg.GlyphRanges : atlas->GetGlyphRangesDefault();
        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            src_tmp.GlyphsHighest = ImMax(src_tmp.GlyphsHighest, (int)src_range[1]);
        dst_tmp.SrcCount++;
        dst_tmp.GlyphsHighest = ImMax(dst_tmp.GlyphsHighest, src_tmp.GlyphsHighest);
    }

    // 2. For every requested codepoint, check for their presence in the font data, and handle redundancy or overlaps between source fonts to avoid unused glyphs.
    int total_glyphs_count = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.GlyphsSet.Create(src_tmp.GlyphsHighest + 1);
        if (dst_tmp.GlyphsSet.Storage.empty())
            dst_tmp.GlyphsSet.Create(dst_tmp.GlyphsHighest + 1);

        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            for (unsigned int codepoint = src_range[0]; codepoint <= src_range[1]; codepoint++)
            {
                if (dst_tmp.GlyphsSet.TestBit(codepoint))    // Don't overwrite existing glyphs. We could make this an option for MergeMode (e.g. MergeOverwrite==true)
                    continue;
                if (!stbtt_FindGlyphIndex(&src_tmp.FontInfo, codepoint))    // It is actually in the font?
                    continue;

                // Add to avail set/counters
                src_tmp.GlyphsCount++;
                dst_tmp.GlyphsCount++;
                src_tmp.GlyphsSet.SetBit(codepoint);
                dst_tmp.GlyphsSet.SetBit(codepoint);
                total_glyphs_count++;
            }
    }

    // 3. Unpack our bit map into a flat list (we now have all the Unicode points that we know are requested _and_ available _and_ not overlapping another)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        src_tmp.GlyphsList.reserve(src_tmp.GlyphsCount);
        UnpackBitVectorToFlatIndexList(&src_tmp.GlyphsSet, &src_tmp.GlyphsList);
        src_tmp.GlyphsSet.Clear();
        IM_ASSERT(src_tmp.GlyphsList.Size == src_tmp.GlyphsCount);
    }
    for (int dst_i = 0; dst_i < dst_tmp_array.Size; dst_i++)
        dst_tmp_array[dst_i].GlyphsSet.Clear();
    dst_tmp_array.clear();

    // Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
    // (We technically don't need to zero-clear buf_rects, but let's do it for the sake of sanity)
    ImVector<stbrp_rect> buf_rects;
    ImVector<stbtt_packedchar> buf_packedchars;
    buf_rects.resize(total_glyphs_count);
    buf_packedchars.resize(total_glyphs_count);
    memset(buf_rects.Data, 0, (size_t)buf_rects.size_in_bytes());
    memset(buf_packedchars.Data, 0, (size_t)buf_packedchars.size_in_bytes());

    // 4. Gather glyphs sizes so we can pack them in our virtual canvas.
    int total_surface = 0;
    int buf_rects_out_n = 0;
    int buf_packedchars_out_n = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        src_tmp.Rects = &buf_rects[buf_rects_out_n];
        src_tmp.PackedChars = &buf_packedchars[buf_packedchars_out_n];
        buf_rects_out_n += src_tmp.GlyphsCount;
        buf_packedchars_out_n += src_tmp.GlyphsCount;

        // Convert our ranges in the format stb_truetype wants
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        src_tmp.PackRange.font_size = cfg.SizePixels;
        src_tmp.PackRange.first_unicode_codepoint_in_range = 0;
        src_tmp.PackRange.array_of_unicode_codepoints = src_tmp.GlyphsList.Data;
        src_tmp.PackRange.num_chars = src_tmp.GlyphsList.Size;
        src_tmp.PackRange.chardata_for_range = src_tmp.PackedChars;
        src_tmp.PackRange.h_oversample = (unsigned char)cfg.OversampleH;
        src_tmp.PackRange.v_oversample = (unsigned char)cfg.OversampleV;

        // Gather the sizes of all rectangles we will need to pack (this loop is based on stbtt_PackFontRangesGatherRects)
        const float scale = (cfg.SizePixels > 0) ? stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels) : stbtt_ScaleForMappingEmToPixels(&src_tmp.FontInfo, -cfg.SizePixels);
        const int padding = atlas->TexGlyphPadding;
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsList.Size; glyph_i++)
        {
            int x0, y0, x1, y1;
            const int glyph_index_in_font = stbtt_FindGlyphIndex(&src_tmp.FontInfo, src_tmp.GlyphsList[glyph_i]);
            IM_ASSERT(glyph_index_in_font != 0);
            stbtt_GetGlyphBitmapBoxSubpixel(&src_tmp.FontInfo, glyph_index_in_font, scale * cfg.OversampleH, scale * cfg.OversampleV, 0, 0, &x0, &y0, &x1, &y1);
            src_tmp.Rects[glyph_i].w = (stbrp_coord)(x1 - x0 + padding + cfg.OversampleH - 1);
            src_tmp.Rects[glyph_i].h = (stbrp_coord)(y1 - y0 + padding + cfg.OversampleV - 1);
            total_surface += src_tmp.Rects[glyph_i].w * src_tmp.Rects[glyph_i].h;
        }
    }

    // We need a width for the skyline algorithm, any width!
    // The exact width doesn't really matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
    // User can override TexDesiredWidth and TexGlyphPadding if they wish, otherwise we use a simple heuristic to select the width based on expected surface.
    const int surface_sqrt = (int)ImSqrt((float)total_surface) + 1;
    atlas->TexHeight = 0;
    if (atlas->TexDesiredWidth > 0)
        atlas->TexWidth = atlas->TexDesiredWidth;
    else
        atlas->TexWidth = (surface_sqrt >= 4096 * 0.7f) ? 4096 : (surface_sqrt >= 2048 * 0.7f) ? 2048 : (surface_sqrt >= 1024 * 0.7f) ? 1024 : 512;

    // 5. Start packing
    // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
    const int TEX_HEIGHT_MAX = 1024 * 32;
    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, NULL, atlas->TexWidth, TEX_HEIGHT_MAX, 0, atlas->TexGlyphPadding, NULL);
    ImFontAtlasBuildPackCustomRects(atlas, spc.pack_info);

    // 6. Pack each source font. No rendering yet, we are working with rectangles in an infinitely tall texture at this point.
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbrp_pack_rects((stbrp_context*)spc.pack_info, src_tmp.Rects, src_tmp.GlyphsCount);

        // Extend texture height and mark missing glyphs as non-packed so we won't render them.
        // FIXME: We are not handling packing failure here (would happen if we got off TEX_HEIGHT_MAX or if a single if larger than TexWidth?)
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
            if (src_tmp.Rects[glyph_i].was_packed)
                atlas->TexHeight = ImMax(atlas->TexHeight, src_tmp.Rects[glyph_i].y + src_tmp.Rects[glyph_i].h);
    }

    // 7. Allocate texture
    atlas->TexHeight = (atlas->Flags & ImFontAtlasFlags_NoPowerOfTwoHeight) ? (atlas->TexHeight + 1) : ImUpperPowerOfTwo(atlas->TexHeight);
    atlas->TexUvScale = ImVec2(1.0f / atlas->TexWidth, 1.0f / atlas->TexHeight);
    atlas->TexPixelsAlpha8 = (unsigned char*)IM_ALLOC(atlas->TexWidth * atlas->TexHeight);
    memset(atlas->TexPixelsAlpha8, 0, atlas->TexWidth * atlas->TexHeight);
    spc.pixels = atlas->TexPixelsAlpha8;
    spc.height = atlas->TexHeight;

    // 8. Render/rasterize font characters into the texture
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbtt_PackFontRangesRenderIntoRects(&spc, &src_tmp.FontInfo, &src_tmp.PackRange, 1, src_tmp.Rects);

        // Apply multiply operator
        if (cfg.RasterizerMultiply != 1.0f)
        {
            unsigned char multiply_table[256];
            ImFontAtlasBuildMultiplyCalcLookupTable(multiply_table, cfg.RasterizerMultiply);
            stbrp_rect* r = &src_tmp.Rects[0];
            for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++, r++)
                if (r->was_packed)
                    ImFontAtlasBuildMultiplyRectAlpha8(multiply_table, atlas->TexPixelsAlpha8, r->x, r->y, r->w, r->h, atlas->TexWidth * 1);
        }
        src_tmp.Rects = NULL;
    }

    // End packing
    stbtt_PackEnd(&spc);
    buf_rects.clear();

    // 9. Setup ImFont and glyphs for runtime
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        // When merging fonts with MergeMode=true:
        // - We can have multiple input fonts writing into a same destination font.
        // - dst_font->ConfigData is != from cfg which is our source configuration.
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFont* dst_font = cfg.DstFont;

        const float font_scale = stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels);
        int unscaled_ascent, unscaled_descent, unscaled_line_gap;
        stbtt_GetFontVMetrics(&src_tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

        const float ascent = ImFloor(unscaled_ascent * font_scale + ((unscaled_ascent > 0.0f) ? +1 : -1));
        const float descent = ImFloor(unscaled_descent * font_scale + ((unscaled_descent > 0.0f) ? +1 : -1));
        ImFontAtlasBuildSetupFont(atlas, dst_font, &cfg, ascent, descent);
        const float font_off_x = cfg.GlyphOffset.x;
        const float font_off_y = cfg.GlyphOffset.y + IM_ROUND(dst_font->Ascent);

        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
        {
            // Register glyph
            const int codepoint = src_tmp.GlyphsList[glyph_i];
            const stbtt_packedchar& pc = src_tmp.PackedChars[glyph_i];
            stbtt_aligned_quad q;
            float unused_x = 0.0f, unused_y = 0.0f;
            stbtt_GetPackedQuad(src_tmp.PackedChars, atlas->TexWidth, atlas->TexHeight, glyph_i, &unused_x, &unused_y, &q, 0);
            dst_font->AddGlyph(&cfg, (ImWchar)codepoint, q.x0 + font_off_x, q.y0 + font_off_y, q.x1 + font_off_x, q.y1 + font_off_y, q.s0, q.t0, q.s1, q.t1, pc.xadvance);
        }
    }

    // Cleanup
    src_tmp_array.clear_destruct();

    ImFontAtlasBuildFinish(atlas);
    return true;
}

const ImFontBuilderIO* ImFontAtlasGetBuilderForStbTruetype()
{
    static ImFontBuilderIO io;
    io.FontBuilder_Build = ImFontAtlasBuildWithStbTruetype;
    return &io;
}

#endif // IMGUI_ENABLE_STB_TRUETYPE

void ImFontAtlasBuildSetupFont(ImFontAtlas* atlas, ImFont* font, ImFontConfig* font_config, float ascent, float descent)
{
    if (!font_config->MergeMode)
    {
        font->ClearOutputData();
        font->FontSize = font_config->SizePixels;
        font->ConfigData = font_config;
        font->ConfigDataCount = 0;
        font->ContainerAtlas = atlas;
        font->Ascent = ascent;
        font->Descent = descent;
    }
    font->ConfigDataCount++;
}

void ImFontAtlasBuildPackCustomRects(ImFontAtlas* atlas, void* stbrp_context_opaque)
{
    stbrp_context* pack_context = (stbrp_context*)stbrp_context_opaque;
    IM_ASSERT(pack_context != NULL);

    ImVector<ImFontAtlasCustomRect>& user_rects = atlas->CustomRects;
    IM_ASSERT(user_rects.Size >= 1); // We expect at least the default custom rects to be registered, else something went wrong.

    ImVector<stbrp_rect> pack_rects;
    pack_rects.resize(user_rects.Size);
    memset(pack_rects.Data, 0, (size_t)pack_rects.size_in_bytes());
    for (int i = 0; i < user_rects.Size; i++)
    {
        pack_rects[i].w = user_rects[i].Width;
        pack_rects[i].h = user_rects[i].Height;
    }
    stbrp_pack_rects(pack_context, &pack_rects[0], pack_rects.Size);
    for (int i = 0; i < pack_rects.Size; i++)
        if (pack_rects[i].was_packed)
        {
            user_rects[i].X = pack_rects[i].x;
            user_rects[i].Y = pack_rects[i].y;
            IM_ASSERT(pack_rects[i].w == user_rects[i].Width && pack_rects[i].h == user_rects[i].Height);
            atlas->TexHeight = ImMax(atlas->TexHeight, pack_rects[i].y + pack_rects[i].h);
        }
}

void ImFontAtlasBuildRender8bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned char in_marker_pixel_value)
{
    IM_ASSERT(x >= 0 && x + w <= atlas->TexWidth);
    IM_ASSERT(y >= 0 && y + h <= atlas->TexHeight);
    unsigned char* out_pixel = atlas->TexPixelsAlpha8 + x + (y * atlas->TexWidth);
    for (int off_y = 0; off_y < h; off_y++, out_pixel += atlas->TexWidth, in_str += w)
        for (int off_x = 0; off_x < w; off_x++)
            out_pixel[off_x] = (in_str[off_x] == in_marker_char) ? in_marker_pixel_value : 0x00;
}

void ImFontAtlasBuildRender32bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned int in_marker_pixel_value)
{
    IM_ASSERT(x >= 0 && x + w <= atlas->TexWidth);
    IM_ASSERT(y >= 0 && y + h <= atlas->TexHeight);
    unsigned int* out_pixel = atlas->TexPixelsRGBA32 + x + (y * atlas->TexWidth);
    for (int off_y = 0; off_y < h; off_y++, out_pixel += atlas->TexWidth, in_str += w)
        for (int off_x = 0; off_x < w; off_x++)
            out_pixel[off_x] = (in_str[off_x] == in_marker_char) ? in_marker_pixel_value : IM_COL32_BLACK_TRANS;
}

static void ImFontAtlasBuildRenderDefaultTexData(ImFontAtlas* atlas)
{
    ImFontAtlasCustomRect* r = atlas->GetCustomRectByIndex(atlas->PackIdMouseCursors);
    IM_ASSERT(r->IsPacked());

    const int w = atlas->TexWidth;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
    {
        // Render/copy pixels
        IM_ASSERT(r->Width == FONT_ATLAS_DEFAULT_TEX_DATA_W * 2 + 1 && r->Height == FONT_ATLAS_DEFAULT_TEX_DATA_H);
        const int x_for_white = r->X;
        const int x_for_black = r->X + FONT_ATLAS_DEFAULT_TEX_DATA_W + 1;
        if (atlas->TexPixelsAlpha8 != NULL)
        {
            ImFontAtlasBuildRender8bppRectFromString(atlas, x_for_white, r->Y, FONT_ATLAS_DEFAULT_TEX_DATA_W, FONT_ATLAS_DEFAULT_TEX_DATA_H, FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS, '.', 0xFF);
            ImFontAtlasBuildRender8bppRectFromString(atlas, x_for_black, r->Y, FONT_ATLAS_DEFAULT_TEX_DATA_W, FONT_ATLAS_DEFAULT_TEX_DATA_H, FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS, 'X', 0xFF);
        }
        else
        {
            ImFontAtlasBuildRender32bppRectFromString(atlas, x_for_white, r->Y, FONT_ATLAS_DEFAULT_TEX_DATA_W, FONT_ATLAS_DEFAULT_TEX_DATA_H, FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS, '.', IM_COL32_WHITE);
            ImFontAtlasBuildRender32bppRectFromString(atlas, x_for_black, r->Y, FONT_ATLAS_DEFAULT_TEX_DATA_W, FONT_ATLAS_DEFAULT_TEX_DATA_H, FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS, 'X', IM_COL32_WHITE);
        }
    }
    else
    {
        // Render 4 white pixels
        IM_ASSERT(r->Width == 2 && r->Height == 2);
        const int offset = (int)r->X + (int)r->Y * w;
        if (atlas->TexPixelsAlpha8 != NULL)
        {
            atlas->TexPixelsAlpha8[offset] = atlas->TexPixelsAlpha8[offset + 1] = atlas->TexPixelsAlpha8[offset + w] = atlas->TexPixelsAlpha8[offset + w + 1] = 0xFF;
        }
        else
        {
            atlas->TexPixelsRGBA32[offset] = atlas->TexPixelsRGBA32[offset + 1] = atlas->TexPixelsRGBA32[offset + w] = atlas->TexPixelsRGBA32[offset + w + 1] = IM_COL32_WHITE;
        }
    }
    atlas->TexUvWhitePixel = ImVec2((r->X + 0.5f) * atlas->TexUvScale.x, (r->Y + 0.5f) * atlas->TexUvScale.y);
}

static void ImFontAtlasBuildRenderLinesTexData(ImFontAtlas* atlas)
{
    if (atlas->Flags & ImFontAtlasFlags_NoBakedLines)
        return;

    // This generates a triangular shape in the texture, with the various line widths stacked on top of each other to allow interpolation between them
    ImFontAtlasCustomRect* r = atlas->GetCustomRectByIndex(atlas->PackIdLines);
    IM_ASSERT(r->IsPacked());
    for (unsigned int n = 0; n < IM_DRAWLIST_TEX_LINES_WIDTH_MAX + 1; n++) // +1 because of the zero-width row
    {
        // Each line consists of at least two empty pixels at the ends, with a line of solid pixels in the middle
        unsigned int y = n;
        unsigned int line_width = n;
        unsigned int pad_left = (r->Width - line_width) / 2;
        unsigned int pad_right = r->Width - (pad_left + line_width);

        // Write each slice
        IM_ASSERT(pad_left + line_width + pad_right == r->Width && y < r->Height); // Make sure we're inside the texture bounds before we start writing pixels
        if (atlas->TexPixelsAlpha8 != NULL)
        {
            unsigned char* write_ptr = &atlas->TexPixelsAlpha8[r->X + ((r->Y + y) * atlas->TexWidth)];
            for (unsigned int i = 0; i < pad_left; i++)
                *(write_ptr + i) = 0x00;

            for (unsigned int i = 0; i < line_width; i++)
                *(write_ptr + pad_left + i) = 0xFF;

            for (unsigned int i = 0; i < pad_right; i++)
                *(write_ptr + pad_left + line_width + i) = 0x00;
        }
        else
        {
            unsigned int* write_ptr = &atlas->TexPixelsRGBA32[r->X + ((r->Y + y) * atlas->TexWidth)];
            for (unsigned int i = 0; i < pad_left; i++)
                *(write_ptr + i) = IM_COL32_BLACK_TRANS;

            for (unsigned int i = 0; i < line_width; i++)
                *(write_ptr + pad_left + i) = IM_COL32_WHITE;

            for (unsigned int i = 0; i < pad_right; i++)
                *(write_ptr + pad_left + line_width + i) = IM_COL32_BLACK_TRANS;
        }

        // Calculate UVs for this line
        ImVec2 uv0 = ImVec2((float)(r->X + pad_left - 1), (float)(r->Y + y)) * atlas->TexUvScale;
        ImVec2 uv1 = ImVec2((float)(r->X + pad_left + line_width + 1), (float)(r->Y + y + 1)) * atlas->TexUvScale;
        float half_v = (uv0.y + uv1.y) * 0.5f; // Calculate a constant V in the middle of the row to avoid sampling artifacts
        atlas->TexUvLines[n] = ImVec4(uv0.x, half_v, uv1.x, half_v);
    }
}

// Note: this is called / shared by both the stb_truetype and the FreeType builder
void ImFontAtlasBuildInit(ImFontAtlas* atlas)
{
    // Register texture region for mouse cursors or standard white pixels
    if (atlas->PackIdMouseCursors < 0)
    {
        if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
            atlas->PackIdMouseCursors = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_W * 2 + 1, FONT_ATLAS_DEFAULT_TEX_DATA_H);
        else
            atlas->PackIdMouseCursors = atlas->AddCustomRectRegular(2, 2);
    }

    // Register texture region for thick lines
    // The +2 here is to give space for the end caps, whilst height +1 is to accommodate the fact we have a zero-width row
    if (atlas->PackIdLines < 0)
    {
        if (!(atlas->Flags & ImFontAtlasFlags_NoBakedLines))
            atlas->PackIdLines = atlas->AddCustomRectRegular(IM_DRAWLIST_TEX_LINES_WIDTH_MAX + 2, IM_DRAWLIST_TEX_LINES_WIDTH_MAX + 1);
    }
}

// This is called/shared by both the stb_truetype and the FreeType builder.
void ImFontAtlasBuildFinish(ImFontAtlas* atlas)
{
    // Render into our custom data blocks
    IM_ASSERT(atlas->TexPixelsAlpha8 != NULL || atlas->TexPixelsRGBA32 != NULL);
    ImFontAtlasBuildRenderDefaultTexData(atlas);
    ImFontAtlasBuildRenderLinesTexData(atlas);

    // Register custom rectangle glyphs
    for (int i = 0; i < atlas->CustomRects.Size; i++)
    {
        const ImFontAtlasCustomRect* r = &atlas->CustomRects[i];
        if (r->Font == NULL || r->GlyphID == 0)
            continue;

        // Will ignore ImFontConfig settings: GlyphMinAdvanceX, GlyphMinAdvanceY, GlyphExtraSpacing, PixelSnapH
        IM_ASSERT(r->Font->ContainerAtlas == atlas);
        ImVec2 uv0, uv1;
        atlas->CalcCustomRectUV(r, &uv0, &uv1);
        r->Font->AddGlyph(NULL, (ImWchar)r->GlyphID, r->GlyphOffset.x, r->GlyphOffset.y, r->GlyphOffset.x + r->Width, r->GlyphOffset.y + r->Height, uv0.x, uv0.y, uv1.x, uv1.y, r->GlyphAdvanceX);
    }

    // Build all fonts lookup tables
    for (int i = 0; i < atlas->Fonts.Size; i++)
        if (atlas->Fonts[i]->DirtyLookupTables)
            atlas->Fonts[i]->BuildLookupTable();

    atlas->TexReady = true;
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar*   ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesKorean()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3131, 0x3163, // Korean alphabets
        0xAC00, 0xD7A3, // Korean characters
        0xFFFD, 0xFFFD, // Invalid
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesChineseFull()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0xFFFD, 0xFFFD, // Invalid
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}

static void UnpackAccumulativeOffsetsIntoRanges(int base_codepoint, const short* accumulative_offsets, int accumulative_offsets_count, ImWchar* out_ranges)
{
    for (int n = 0; n < accumulative_offsets_count; n++, out_ranges += 2)
    {
        out_ranges[0] = out_ranges[1] = (ImWchar)(base_codepoint + accumulative_offsets[n]);
        base_codepoint += accumulative_offsets[n];
    }
    out_ranges[0] = 0;
}

//-------------------------------------------------------------------------
// [SECTION] ImFontAtlas glyph ranges helpers
//-------------------------------------------------------------------------

const ImWchar*  ImFontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{
    // Store 2500 regularly used characters for Simplified Chinese.
    // Sourced from https://zh.wiktionary.org/wiki/%E9%99%84%E5%BD%95:%E7%8E%B0%E4%BB%A3%E6%B1%89%E8%AF%AD%E5%B8%B8%E7%94%A8%E5%AD%97%E8%A1%A8
    // This table covers 97.97% of all characters used during the month in July, 1987.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,3,2,1,2,2,1,1,1,1,1,5,2,1,2,3,3,3,2,2,4,1,1,1,2,1,5,2,3,1,2,1,2,1,1,2,1,1,2,2,1,4,1,1,1,1,5,10,1,2,19,2,1,2,1,2,1,2,1,2,
        1,5,1,6,3,2,1,2,2,1,1,1,4,8,5,1,1,4,1,1,3,1,2,1,5,1,2,1,1,1,10,1,1,5,2,4,6,1,4,2,2,2,12,2,1,1,6,1,1,1,4,1,1,4,6,5,1,4,2,2,4,10,7,1,1,4,2,4,
        2,1,4,3,6,10,12,5,7,2,14,2,9,1,1,6,7,10,4,7,13,1,5,4,8,4,1,1,2,28,5,6,1,1,5,2,5,20,2,2,9,8,11,2,9,17,1,8,6,8,27,4,6,9,20,11,27,6,68,2,2,1,1,
        1,2,1,2,2,7,6,11,3,3,1,1,3,1,2,1,1,1,1,1,3,1,1,8,3,4,1,5,7,2,1,4,4,8,4,2,1,2,1,1,4,5,6,3,6,2,12,3,1,3,9,2,4,3,4,1,5,3,3,1,3,7,1,5,1,1,1,1,2,
        3,4,5,2,3,2,6,1,1,2,1,7,1,7,3,4,5,15,2,2,1,5,3,22,19,2,1,1,1,1,2,5,1,1,1,6,1,1,12,8,2,9,18,22,4,1,1,5,1,16,1,2,7,10,15,1,1,6,2,4,1,2,4,1,6,
        1,1,3,2,4,1,6,4,5,1,2,1,1,2,1,10,3,1,3,2,1,9,3,2,5,7,2,19,4,3,6,1,1,1,1,1,4,3,2,1,1,1,2,5,3,1,1,1,2,2,1,1,2,1,1,2,1,3,1,1,1,3,7,1,4,1,1,2,1,
        1,2,1,2,4,4,3,8,1,1,1,2,1,3,5,1,3,1,3,4,6,2,2,14,4,6,6,11,9,1,15,3,1,28,5,2,5,5,3,1,3,4,5,4,6,14,3,2,3,5,21,2,7,20,10,1,2,19,2,4,28,28,2,3,
        2,1,14,4,1,26,28,42,12,40,3,52,79,5,14,17,3,2,2,11,3,4,6,3,1,8,2,23,4,5,8,10,4,2,7,3,5,1,1,6,3,1,2,2,2,5,28,1,1,7,7,20,5,3,29,3,17,26,1,8,4,
        27,3,6,11,23,5,3,4,6,13,24,16,6,5,10,25,35,7,3,2,3,3,14,3,6,2,6,1,4,2,3,8,2,1,1,3,3,3,4,1,1,13,2,2,4,5,2,1,14,14,1,2,2,1,4,5,2,3,1,14,3,12,
        3,17,2,16,5,1,2,1,8,9,3,19,4,2,2,4,17,25,21,20,28,75,1,10,29,103,4,1,2,1,1,4,2,4,1,2,3,24,2,2,2,1,1,2,1,3,8,1,1,1,2,1,1,3,1,1,1,6,1,5,3,1,1,
        1,3,4,1,1,5,2,1,5,6,13,9,16,1,1,1,1,3,2,3,2,4,5,2,5,2,2,3,7,13,7,2,2,1,1,1,1,2,3,3,2,1,6,4,9,2,1,14,2,14,2,1,18,3,4,14,4,11,41,15,23,15,23,
        176,1,3,4,1,1,1,1,5,3,1,2,3,7,3,1,1,2,1,2,4,4,6,2,4,1,9,7,1,10,5,8,16,29,1,1,2,2,3,1,3,5,2,4,5,4,1,1,2,2,3,3,7,1,6,10,1,17,1,44,4,6,2,1,1,6,
        5,4,2,10,1,6,9,2,8,1,24,1,2,13,7,8,8,2,1,4,1,3,1,3,3,5,2,5,10,9,4,9,12,2,1,6,1,10,1,1,7,7,4,10,8,3,1,13,4,3,1,6,1,3,5,2,1,2,17,16,5,2,16,6,
        1,4,2,1,3,3,6,8,5,11,11,1,3,3,2,4,6,10,9,5,7,4,7,4,7,1,1,4,2,1,3,6,8,7,1,6,11,5,5,3,24,9,4,2,7,13,5,1,8,82,16,61,1,1,1,4,2,2,16,10,3,8,1,1,
        6,4,2,1,3,1,1,1,4,3,8,4,2,2,1,1,1,1,1,6,3,5,1,1,4,6,9,2,1,1,1,2,1,7,2,1,6,1,5,4,4,3,1,8,1,3,3,1,3,2,2,2,2,3,1,6,1,2,1,2,1,3,7,1,8,2,1,2,1,5,
        2,5,3,5,10,1,2,1,1,3,2,5,11,3,9,3,5,1,1,5,9,1,2,1,5,7,9,9,8,1,3,3,3,6,8,2,3,2,1,1,32,6,1,2,15,9,3,7,13,1,3,10,13,2,14,1,13,10,2,1,3,10,4,15,
        2,15,15,10,1,3,9,6,9,32,25,26,47,7,3,2,3,1,6,3,4,3,2,8,5,4,1,9,4,2,2,19,10,6,2,3,8,1,2,2,4,2,1,9,4,4,4,6,4,8,9,2,3,1,1,1,1,3,5,5,1,3,8,4,6,
        2,1,4,12,1,5,3,7,13,2,5,8,1,6,1,2,5,14,6,1,5,2,4,8,15,5,1,23,6,62,2,10,1,1,8,1,2,2,10,4,2,2,9,2,1,1,3,2,3,1,5,3,3,2,1,3,8,1,1,1,11,3,1,1,4,
        3,7,1,14,1,2,3,12,5,2,5,1,6,7,5,7,14,11,1,3,1,8,9,12,2,1,11,8,4,4,2,6,10,9,13,1,1,3,1,5,1,3,2,4,4,1,18,2,3,14,11,4,29,4,2,7,1,3,13,9,2,2,5,
        3,5,20,7,16,8,5,72,34,6,4,22,12,12,28,45,36,9,7,39,9,191,1,1,1,4,11,8,4,9,2,3,22,1,1,1,1,4,17,1,7,7,1,11,31,10,2,4,8,2,3,2,1,4,2,16,4,32,2,
        3,19,13,4,9,1,5,2,14,8,1,1,3,6,19,6,5,1,16,6,2,10,8,5,1,2,3,1,5,5,1,11,6,6,1,3,3,2,6,3,8,1,1,4,10,7,5,7,7,5,8,9,2,1,3,4,1,1,3,1,3,3,2,6,16,
        1,4,6,3,1,10,6,1,3,15,2,9,2,10,25,13,9,16,6,2,2,10,11,4,3,9,1,2,6,6,5,4,30,40,1,10,7,12,14,33,6,3,6,7,3,1,3,1,11,14,4,9,5,12,11,49,18,51,31,
        140,31,2,2,1,5,1,8,1,10,1,4,4,3,24,1,10,1,3,6,6,16,3,4,5,2,1,4,2,57,10,6,22,2,22,3,7,22,6,10,11,36,18,16,33,36,2,5,5,1,1,1,4,10,1,4,13,2,7,
        5,2,9,3,4,1,7,43,3,7,3,9,14,7,9,1,11,1,1,3,7,4,18,13,1,14,1,3,6,10,73,2,2,30,6,1,11,18,19,13,22,3,46,42,37,89,7,3,16,34,2,2,3,9,1,7,1,1,1,2,
        2,4,10,7,3,10,3,9,5,28,9,2,6,13,7,3,1,3,10,2,7,2,11,3,6,21,54,85,2,1,4,2,2,1,39,3,21,2,2,5,1,1,1,4,1,1,3,4,15,1,3,2,4,4,2,3,8,2,20,1,8,7,13,
        4,1,26,6,2,9,34,4,21,52,10,4,4,1,5,12,2,11,1,7,2,30,12,44,2,30,1,1,3,6,16,9,17,39,82,2,2,24,7,1,7,3,16,9,14,44,2,1,2,1,2,3,5,2,4,1,6,7,5,3,
        2,6,1,11,5,11,2,1,18,19,8,1,3,24,29,2,1,3,5,2,2,1,13,6,5,1,46,11,3,5,1,1,5,8,2,10,6,12,6,3,7,11,2,4,16,13,2,5,1,1,2,2,5,2,28,5,2,23,10,8,4,
        4,22,39,95,38,8,14,9,5,1,13,5,4,3,13,12,11,1,9,1,27,37,2,5,4,4,63,211,95,2,2,2,1,3,5,2,1,1,2,2,1,1,1,3,2,4,1,2,1,1,5,2,2,1,1,2,3,1,3,1,1,1,
        3,1,4,2,1,3,6,1,1,3,7,15,5,3,2,5,3,9,11,4,2,22,1,6,3,8,7,1,4,28,4,16,3,3,25,4,4,27,27,1,4,1,2,2,7,1,3,5,2,28,8,2,14,1,8,6,16,25,3,3,3,14,3,
        3,1,1,2,1,4,6,3,8,4,1,1,1,2,3,6,10,6,2,3,18,3,2,5,5,4,3,1,5,2,5,4,23,7,6,12,6,4,17,11,9,5,1,1,10,5,12,1,1,11,26,33,7,3,6,1,17,7,1,5,12,1,11,
        2,4,1,8,14,17,23,1,2,1,7,8,16,11,9,6,5,2,6,4,16,2,8,14,1,11,8,9,1,1,1,9,25,4,11,19,7,2,15,2,12,8,52,7,5,19,2,16,4,36,8,1,16,8,24,26,4,6,2,9,
        5,4,36,3,28,12,25,15,37,27,17,12,59,38,5,32,127,1,2,9,17,14,4,1,2,1,1,8,11,50,4,14,2,19,16,4,17,5,4,5,26,12,45,2,23,45,104,30,12,8,3,10,2,2,
        3,3,1,4,20,7,2,9,6,15,2,20,1,3,16,4,11,15,6,134,2,5,59,1,2,2,2,1,9,17,3,26,137,10,211,59,1,2,4,1,4,1,1,1,2,6,2,3,1,1,2,3,2,3,1,3,4,4,2,3,3,
        1,4,3,1,7,2,2,3,1,2,1,3,3,3,2,2,3,2,1,3,14,6,1,3,2,9,6,15,27,9,34,145,1,1,2,1,1,1,1,2,1,1,1,1,2,2,2,3,1,2,1,1,1,2,3,5,8,3,5,2,4,1,3,2,2,2,12,
        4,1,1,1,10,4,5,1,20,4,16,1,15,9,5,12,2,9,2,5,4,2,26,19,7,1,26,4,30,12,15,42,1,6,8,172,1,1,4,2,1,1,11,2,2,4,2,1,2,1,10,8,1,2,1,4,5,1,2,5,1,8,
        4,1,3,4,2,1,6,2,1,3,4,1,2,1,1,1,1,12,5,7,2,4,3,1,1,1,3,3,6,1,2,2,3,3,3,2,1,2,12,14,11,6,6,4,12,2,8,1,7,10,1,35,7,4,13,15,4,3,23,21,28,52,5,
        26,5,6,1,7,10,2,7,53,3,2,1,1,1,2,163,532,1,10,11,1,3,3,4,8,2,8,6,2,2,23,22,4,2,2,4,2,1,3,1,3,3,5,9,8,2,1,2,8,1,10,2,12,21,20,15,105,2,3,1,1,
        3,2,3,1,1,2,5,1,4,15,11,19,1,1,1,1,5,4,5,1,1,2,5,3,5,12,1,2,5,1,11,1,1,15,9,1,4,5,3,26,8,2,1,3,1,1,15,19,2,12,1,2,5,2,7,2,19,2,20,6,26,7,5,
        2,2,7,34,21,13,70,2,128,1,1,2,1,1,2,1,1,3,2,2,2,15,1,4,1,3,4,42,10,6,1,49,85,8,1,2,1,1,4,4,2,3,6,1,5,7,4,3,211,4,1,2,1,2,5,1,2,4,2,2,6,5,6,
        10,3,4,48,100,6,2,16,296,5,27,387,2,2,3,7,16,8,5,38,15,39,21,9,10,3,7,59,13,27,21,47,5,21,6
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0xFFFD, 0xFFFD  // Invalid
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesJapanese()
{
    // 2999 ideograms code points for Japanese
    // - 2136 Joyo (meaning "for regular use" or "for common use") Kanji code points
    // - 863 Jinmeiyo (meaning "for personal name") Kanji code points
    // - Sourced from the character information database of the Information-technology Promotion Agency, Japan
    //   - https://mojikiban.ipa.go.jp/mji/
    //   - Available under the terms of the Creative Commons Attribution-ShareAlike 2.1 Japan (CC BY-SA 2.1 JP).
    //     - https://creativecommons.org/licenses/by-sa/2.1/jp/deed.en
    //     - https://creativecommons.org/licenses/by-sa/2.1/jp/legalcode
    //   - You can generate this code by the script at:
    //     - https://github.com/vaiorabbit/everyday_use_kanji
    // - References:
    //   - List of Joyo Kanji
    //     - (Official list by the Agency for Cultural Affairs) https://www.bunka.go.jp/kokugo_nihongo/sisaku/joho/joho/kakuki/14/tosin02/index.html
    //     - (Wikipedia) https://en.wikipedia.org/wiki/List_of_j%C5%8Dy%C5%8D_kanji
    //   - List of Jinmeiyo Kanji
    //     - (Official list by the Ministry of Justice) http://www.moj.go.jp/MINJI/minji86.html
    //     - (Wikipedia) https://en.wikipedia.org/wiki/Jinmeiy%C5%8D_kanji
    // - Missing 1 Joyo Kanji: U+20B9F (Kun'yomi: Shikaru, On'yomi: Shitsu,shichi), see https://github.com/ocornut/imgui/pull/3627 for details.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,3,3,2,2,1,5,3,5,7,5,6,1,2,1,7,2,6,3,1,8,1,1,4,1,1,18,2,11,2,6,2,1,2,1,5,1,2,1,3,1,2,1,2,3,3,1,1,2,3,1,1,1,12,7,9,1,4,5,1,
        1,2,1,10,1,1,9,2,2,4,5,6,9,3,1,1,1,1,9,3,18,5,2,2,2,2,1,6,3,7,1,1,1,1,2,2,4,2,1,23,2,10,4,3,5,2,4,10,2,4,13,1,6,1,9,3,1,1,6,6,7,6,3,1,2,11,3,
        2,2,3,2,15,2,2,5,4,3,6,4,1,2,5,2,12,16,6,13,9,13,2,1,1,7,16,4,7,1,19,1,5,1,2,2,7,7,8,2,6,5,4,9,18,7,4,5,9,13,11,8,15,2,1,1,1,2,1,2,2,1,2,2,8,
        2,9,3,3,1,1,4,4,1,1,1,4,9,1,4,3,5,5,2,7,5,3,4,8,2,1,13,2,3,3,1,14,1,1,4,5,1,3,6,1,5,2,1,1,3,3,3,3,1,1,2,7,6,6,7,1,4,7,6,1,1,1,1,1,12,3,3,9,5,
        2,6,1,5,6,1,2,3,18,2,4,14,4,1,3,6,1,1,6,3,5,5,3,2,2,2,2,12,3,1,4,2,3,2,3,11,1,7,4,1,2,1,3,17,1,9,1,24,1,1,4,2,2,4,1,2,7,1,1,1,3,1,2,2,4,15,1,
        1,2,1,1,2,1,5,2,5,20,2,5,9,1,10,8,7,6,1,1,1,1,1,1,6,2,1,2,8,1,1,1,1,5,1,1,3,1,1,1,1,3,1,1,12,4,1,3,1,1,1,1,1,10,3,1,7,5,13,1,2,3,4,6,1,1,30,
        2,9,9,1,15,38,11,3,1,8,24,7,1,9,8,10,2,1,9,31,2,13,6,2,9,4,49,5,2,15,2,1,10,2,1,1,1,2,2,6,15,30,35,3,14,18,8,1,16,10,28,12,19,45,38,1,3,2,3,
        13,2,1,7,3,6,5,3,4,3,1,5,7,8,1,5,3,18,5,3,6,1,21,4,24,9,24,40,3,14,3,21,3,2,1,2,4,2,3,1,15,15,6,5,1,1,3,1,5,6,1,9,7,3,3,2,1,4,3,8,21,5,16,4,
        5,2,10,11,11,3,6,3,2,9,3,6,13,1,2,1,1,1,1,11,12,6,6,1,4,2,6,5,2,1,1,3,3,6,13,3,1,1,5,1,2,3,3,14,2,1,2,2,2,5,1,9,5,1,1,6,12,3,12,3,4,13,2,14,
        2,8,1,17,5,1,16,4,2,2,21,8,9,6,23,20,12,25,19,9,38,8,3,21,40,25,33,13,4,3,1,4,1,2,4,1,2,5,26,2,1,1,2,1,3,6,2,1,1,1,1,1,1,2,3,1,1,1,9,2,3,1,1,
        1,3,6,3,2,1,1,6,6,1,8,2,2,2,1,4,1,2,3,2,7,3,2,4,1,2,1,2,2,1,1,1,1,1,3,1,2,5,4,10,9,4,9,1,1,1,1,1,1,5,3,2,1,6,4,9,6,1,10,2,31,17,8,3,7,5,40,1,
        7,7,1,6,5,2,10,7,8,4,15,39,25,6,28,47,18,10,7,1,3,1,1,2,1,1,1,3,3,3,1,1,1,3,4,2,1,4,1,3,6,10,7,8,6,2,2,1,3,3,2,5,8,7,9,12,2,15,1,1,4,1,2,1,1,
        1,3,2,1,3,3,5,6,2,3,2,10,1,4,2,8,1,1,1,11,6,1,21,4,16,3,1,3,1,4,2,3,6,5,1,3,1,1,3,3,4,6,1,1,10,4,2,7,10,4,7,4,2,9,4,3,1,1,1,4,1,8,3,4,1,3,1,
        6,1,4,2,1,4,7,2,1,8,1,4,5,1,1,2,2,4,6,2,7,1,10,1,1,3,4,11,10,8,21,4,6,1,3,5,2,1,2,28,5,5,2,3,13,1,2,3,1,4,2,1,5,20,3,8,11,1,3,3,3,1,8,10,9,2,
        10,9,2,3,1,1,2,4,1,8,3,6,1,7,8,6,11,1,4,29,8,4,3,1,2,7,13,1,4,1,6,2,6,12,12,2,20,3,2,3,6,4,8,9,2,7,34,5,1,18,6,1,1,4,4,5,7,9,1,2,2,4,3,4,1,7,
        2,2,2,6,2,3,25,5,3,6,1,4,6,7,4,2,1,4,2,13,6,4,4,3,1,5,3,4,4,3,2,1,1,4,1,2,1,1,3,1,11,1,6,3,1,7,3,6,2,8,8,6,9,3,4,11,3,2,10,12,2,5,11,1,6,4,5,
        3,1,8,5,4,6,6,3,5,1,1,3,2,1,2,2,6,17,12,1,10,1,6,12,1,6,6,19,9,6,16,1,13,4,4,15,7,17,6,11,9,15,12,6,7,2,1,2,2,15,9,3,21,4,6,49,18,7,3,2,3,1,
        6,8,2,2,6,2,9,1,3,6,4,4,1,2,16,2,5,2,1,6,2,3,5,3,1,2,5,1,2,1,9,3,1,8,6,4,8,11,3,1,1,1,1,3,1,13,8,4,1,3,2,2,1,4,1,11,1,5,2,1,5,2,5,8,6,1,1,7,
        4,3,8,3,2,7,2,1,5,1,5,2,4,7,6,2,8,5,1,11,4,5,3,6,18,1,2,13,3,3,1,21,1,1,4,1,4,1,1,1,8,1,2,2,7,1,2,4,2,2,9,2,1,1,1,4,3,6,3,12,5,1,1,1,5,6,3,2,
        4,8,2,2,4,2,7,1,8,9,5,2,3,2,1,3,2,13,7,14,6,5,1,1,2,1,4,2,23,2,1,1,6,3,1,4,1,15,3,1,7,3,9,14,1,3,1,4,1,1,5,8,1,3,8,3,8,15,11,4,14,4,4,2,5,5,
        1,7,1,6,14,7,7,8,5,15,4,8,6,5,6,2,1,13,1,20,15,11,9,2,5,6,2,11,2,6,2,5,1,5,8,4,13,19,25,4,1,1,11,1,34,2,5,9,14,6,2,2,6,1,1,14,1,3,14,13,1,6,
        12,21,14,14,6,32,17,8,32,9,28,1,2,4,11,8,3,1,14,2,5,15,1,1,1,1,3,6,4,1,3,4,11,3,1,1,11,30,1,5,1,4,1,5,8,1,1,3,2,4,3,17,35,2,6,12,17,3,1,6,2,
        1,1,12,2,7,3,3,2,1,16,2,8,3,6,5,4,7,3,3,8,1,9,8,5,1,2,1,3,2,8,1,2,9,12,1,1,2,3,8,3,24,12,4,3,7,5,8,3,3,3,3,3,3,1,23,10,3,1,2,2,6,3,1,16,1,16,
        22,3,10,4,11,6,9,7,7,3,6,2,2,2,4,10,2,1,1,2,8,7,1,6,4,1,3,3,3,5,10,12,12,2,3,12,8,15,1,1,16,6,6,1,5,9,11,4,11,4,2,6,12,1,17,5,13,1,4,9,5,1,11,
        2,1,8,1,5,7,28,8,3,5,10,2,17,3,38,22,1,2,18,12,10,4,38,18,1,4,44,19,4,1,8,4,1,12,1,4,31,12,1,14,7,75,7,5,10,6,6,13,3,2,11,11,3,2,5,28,15,6,18,
        18,5,6,4,3,16,1,7,18,7,36,3,5,3,1,7,1,9,1,10,7,2,4,2,6,2,9,7,4,3,32,12,3,7,10,2,23,16,3,1,12,3,31,4,11,1,3,8,9,5,1,30,15,6,12,3,2,2,11,19,9,
        14,2,6,2,3,19,13,17,5,3,3,25,3,14,1,1,1,36,1,3,2,19,3,13,36,9,13,31,6,4,16,34,2,5,4,2,3,3,5,1,1,1,4,3,1,17,3,2,3,5,3,1,3,2,3,5,6,3,12,11,1,3,
        1,2,26,7,12,7,2,14,3,3,7,7,11,25,25,28,16,4,36,1,2,1,6,2,1,9,3,27,17,4,3,4,13,4,1,3,2,2,1,10,4,2,4,6,3,8,2,1,18,1,1,24,2,2,4,33,2,3,63,7,1,6,
        40,7,3,4,4,2,4,15,18,1,16,1,1,11,2,41,14,1,3,18,13,3,2,4,16,2,17,7,15,24,7,18,13,44,2,2,3,6,1,1,7,5,1,7,1,4,3,3,5,10,8,2,3,1,8,1,1,27,4,2,1,
        12,1,2,1,10,6,1,6,7,5,2,3,7,11,5,11,3,6,6,2,3,15,4,9,1,1,2,1,2,11,2,8,12,8,5,4,2,3,1,5,2,2,1,14,1,12,11,4,1,11,17,17,4,3,2,5,5,7,3,1,5,9,9,8,
        2,5,6,6,13,13,2,1,2,6,1,2,2,49,4,9,1,2,10,16,7,8,4,3,2,23,4,58,3,29,1,14,19,19,11,11,2,7,5,1,3,4,6,2,18,5,12,12,17,17,3,3,2,4,1,6,2,3,4,3,1,
        1,1,1,5,1,1,9,1,3,1,3,6,1,8,1,1,2,6,4,14,3,1,4,11,4,1,3,32,1,2,4,13,4,1,2,4,2,1,3,1,11,1,4,2,1,4,4,6,3,5,1,6,5,7,6,3,23,3,5,3,5,3,3,13,3,9,10,
        1,12,10,2,3,18,13,7,160,52,4,2,2,3,2,14,5,4,12,4,6,4,1,20,4,11,6,2,12,27,1,4,1,2,2,7,4,5,2,28,3,7,25,8,3,19,3,6,10,2,2,1,10,2,5,4,1,3,4,1,5,
        3,2,6,9,3,6,2,16,3,3,16,4,5,5,3,2,1,2,16,15,8,2,6,21,2,4,1,22,5,8,1,1,21,11,2,1,11,11,19,13,12,4,2,3,2,3,6,1,8,11,1,4,2,9,5,2,1,11,2,9,1,1,2,
        14,31,9,3,4,21,14,4,8,1,7,2,2,2,5,1,4,20,3,3,4,10,1,11,9,8,2,1,4,5,14,12,14,2,17,9,6,31,4,14,1,20,13,26,5,2,7,3,6,13,2,4,2,19,6,2,2,18,9,3,5,
        12,12,14,4,6,2,3,6,9,5,22,4,5,25,6,4,8,5,2,6,27,2,35,2,16,3,7,8,8,6,6,5,9,17,2,20,6,19,2,13,3,1,1,1,4,17,12,2,14,7,1,4,18,12,38,33,2,10,1,1,
        2,13,14,17,11,50,6,33,20,26,74,16,23,45,50,13,38,33,6,6,7,4,4,2,1,3,2,5,8,7,8,9,3,11,21,9,13,1,3,10,6,7,1,2,2,18,5,5,1,9,9,2,68,9,19,13,2,5,
        1,4,4,7,4,13,3,9,10,21,17,3,26,2,1,5,2,4,5,4,1,7,4,7,3,4,2,1,6,1,1,20,4,1,9,2,2,1,3,3,2,3,2,1,1,1,20,2,3,1,6,2,3,6,2,4,8,1,3,2,10,3,5,3,4,4,
        3,4,16,1,6,1,10,2,4,2,1,1,2,10,11,2,2,3,1,24,31,4,10,10,2,5,12,16,164,15,4,16,7,9,15,19,17,1,2,1,1,5,1,1,1,1,1,3,1,4,3,1,3,1,3,1,2,1,1,3,3,7,
        2,8,1,2,2,2,1,3,4,3,7,8,12,92,2,10,3,1,3,14,5,25,16,42,4,7,7,4,2,21,5,27,26,27,21,25,30,31,2,1,5,13,3,22,5,6,6,11,9,12,1,5,9,7,5,5,22,60,3,5,
        13,1,1,8,1,1,3,3,2,1,9,3,3,18,4,1,2,3,7,6,3,1,2,3,9,1,3,1,3,2,1,3,1,1,1,2,1,11,3,1,6,9,1,3,2,3,1,2,1,5,1,1,4,3,4,1,2,2,4,4,1,7,2,1,2,2,3,5,13,
        18,3,4,14,9,9,4,16,3,7,5,8,2,6,48,28,3,1,1,4,2,14,8,2,9,2,1,15,2,4,3,2,10,16,12,8,7,1,1,3,1,1,1,2,7,4,1,6,4,38,39,16,23,7,15,15,3,2,12,7,21,
        37,27,6,5,4,8,2,10,8,8,6,5,1,2,1,3,24,1,16,17,9,23,10,17,6,1,51,55,44,13,294,9,3,6,2,4,2,2,15,1,1,1,13,21,17,68,14,8,9,4,1,4,9,3,11,7,1,1,1,
        5,6,3,2,1,1,1,2,3,8,1,2,2,4,1,5,5,2,1,4,3,7,13,4,1,4,1,3,1,1,1,5,5,10,1,6,1,5,2,1,5,2,4,1,4,5,7,3,18,2,9,11,32,4,3,3,2,4,7,11,16,9,11,8,13,38,
        32,8,4,2,1,1,2,1,2,4,4,1,1,1,4,1,21,3,11,1,16,1,1,6,1,3,2,4,9,8,57,7,44,1,3,3,13,3,10,1,1,7,5,2,7,21,47,63,3,15,4,7,1,16,1,1,2,8,2,3,42,15,4,
        1,29,7,22,10,3,78,16,12,20,18,4,67,11,5,1,3,15,6,21,31,32,27,18,13,71,35,5,142,4,10,1,2,50,19,33,16,35,37,16,19,27,7,1,133,19,1,4,8,7,20,1,4,
        4,1,10,3,1,6,1,2,51,5,40,15,24,43,22928,11,1,13,154,70,3,1,1,7,4,10,1,2,1,1,2,1,2,1,2,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,
        3,2,1,1,1,1,2,1,1,
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0xFFFD, 0xFFFD  // Invalid
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00)*2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesCyrillic()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesThai()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x2010, 0x205E, // Punctuations
        0x0E00, 0x0E7F, // Thai
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesVietnamese()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x0102, 0x0103,
        0x0110, 0x0111,
        0x0128, 0x0129,
        0x0168, 0x0169,
        0x01A0, 0x01A1,
        0x01AF, 0x01B0,
        0x1EA0, 0x1EF9,
        0,
    };
    return &ranges[0];
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontGlyphRangesBuilder
//-----------------------------------------------------------------------------

void ImFontGlyphRangesBuilder::AddText(const char* text, const char* text_end)
{
    while (text_end ? (text < text_end) : *text)
    {
        unsigned int c = 0;
        int c_len = ImTextCharFromUtf8(&c, text, text_end);
        text += c_len;
        if (c_len == 0)
            break;
        AddChar((ImWchar)c);
    }
}

void ImFontGlyphRangesBuilder::AddRanges(const ImWchar* ranges)
{
    for (; ranges[0]; ranges += 2)
        for (unsigned int c = ranges[0]; c <= ranges[1] && c <= IM_UNICODE_CODEPOINT_MAX; c++) //-V560
            AddChar((ImWchar)c);
}

void ImFontGlyphRangesBuilder::BuildRanges(ImVector<ImWchar>* out_ranges)
{
    const int max_codepoint = IM_UNICODE_CODEPOINT_MAX;
    for (int n = 0; n <= max_codepoint; n++)
        if (GetBit(n))
        {
            out_ranges->push_back((ImWchar)n);
            while (n < max_codepoint && GetBit(n + 1))
                n++;
            out_ranges->push_back((ImWchar)n);
        }
    out_ranges->push_back(0);
}

//-----------------------------------------------------------------------------
// [SECTION] ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    FallbackChar = (ImWchar)-1;
    EllipsisChar = (ImWchar)-1;
    DotChar = (ImWchar)-1;
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    ConfigData = NULL;
    ConfigDataCount = 0;
    DirtyLookupTables = false;
    Scale = 1.0f;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
    memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
}

ImFont::~ImFont()
{
    ClearOutputData();
}

void    ImFont::ClearOutputData()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    Glyphs.clear();
    IndexAdvanceX.clear();
    IndexLookup.clear();
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    DirtyLookupTables = true;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
}

static ImWchar FindFirstExistingGlyph(ImFont* font, const ImWchar* candidate_chars, int candidate_chars_count)
{
    for (int n = 0; n < candidate_chars_count; n++)
        if (font->FindGlyphNoFallback(candidate_chars[n]) != NULL)
            return candidate_chars[n];
    return (ImWchar)-1;
}

void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for (int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    // Build lookup table
    IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
    IndexAdvanceX.clear();
    IndexLookup.clear();
    DirtyLookupTables = false;
    memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
    GrowIndex(max_codepoint + 1);
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexAdvanceX[codepoint] = Glyphs[i].AdvanceX;
        IndexLookup[codepoint] = (ImWchar)i;

        // Mark 4K page as used
        const int page_n = codepoint / 4096;
        Used4kPagesMap[page_n >> 3] |= 1 << (page_n & 7);
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
    if (FindGlyph((ImWchar)' '))
    {
        if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times (FIXME: Flaky)
            Glyphs.resize(Glyphs.Size + 1);
        ImFontGlyph& tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((ImWchar)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.AdvanceX *= IM_TABSIZE;
        IndexAdvanceX[(int)tab_glyph.Codepoint] = (float)tab_glyph.AdvanceX;
        IndexLookup[(int)tab_glyph.Codepoint] = (ImWchar)(Glyphs.Size - 1);
    }

    // Mark special glyphs as not visible (note that AddGlyph already mark as non-visible glyphs with zero-size polygons)
    SetGlyphVisible((ImWchar)' ', false);
    SetGlyphVisible((ImWchar)'\t', false);

    // Ellipsis character is required for rendering elided text. We prefer using U+2026 (horizontal ellipsis).
    // However some old fonts may contain ellipsis at U+0085. Here we auto-detect most suitable ellipsis character.
    // FIXME: Note that 0x2026 is rarely included in our font ranges. Because of this we are more likely to use three individual dots.
    const ImWchar ellipsis_chars[] = { (ImWchar)0x2026, (ImWchar)0x0085 };
    const ImWchar dots_chars[] = { (ImWchar)'.', (ImWchar)0xFF0E };
    if (EllipsisChar == (ImWchar)-1)
        EllipsisChar = FindFirstExistingGlyph(this, ellipsis_chars, IM_ARRAYSIZE(ellipsis_chars));
    if (DotChar == (ImWchar)-1)
        DotChar = FindFirstExistingGlyph(this, dots_chars, IM_ARRAYSIZE(dots_chars));

    // Setup fallback character
    const ImWchar fallback_chars[] = { (ImWchar)IM_UNICODE_CODEPOINT_INVALID, (ImWchar)'?', (ImWchar)' ' };
    FallbackGlyph = FindGlyphNoFallback(FallbackChar);
    if (FallbackGlyph == NULL)
    {
        FallbackChar = FindFirstExistingGlyph(this, fallback_chars, IM_ARRAYSIZE(fallback_chars));
        FallbackGlyph = FindGlyphNoFallback(FallbackChar);
        if (FallbackGlyph == NULL)
        {
            FallbackGlyph = &Glyphs.back();
            FallbackChar = (ImWchar)FallbackGlyph->Codepoint;
        }
    }

    FallbackAdvanceX = FallbackGlyph->AdvanceX;
    for (int i = 0; i < max_codepoint + 1; i++)
        if (IndexAdvanceX[i] < 0.0f)
            IndexAdvanceX[i] = FallbackAdvanceX;
}

// API is designed this way to avoid exposing the 4K page size
// e.g. use with IsGlyphRangeUnused(0, 255)
bool ImFont::IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last)
{
    unsigned int page_begin = (c_begin / 4096);
    unsigned int page_last = (c_last / 4096);
    for (unsigned int page_n = page_begin; page_n <= page_last; page_n++)
        if ((page_n >> 3) < sizeof(Used4kPagesMap))
            if (Used4kPagesMap[page_n >> 3] & (1 << (page_n & 7)))
                return false;
    return true;
}

void ImFont::SetGlyphVisible(ImWchar c, bool visible)
{
    if (ImFontGlyph* glyph = (ImFontGlyph*)(void*)FindGlyph((ImWchar)c))
        glyph->Visible = visible ? 1 : 0;
}

void ImFont::GrowIndex(int new_size)
{
    IM_ASSERT(IndexAdvanceX.Size == IndexLookup.Size);
    if (new_size <= IndexLookup.Size)
        return;
    IndexAdvanceX.resize(new_size, -1.0f);
    IndexLookup.resize(new_size, (ImWchar)-1);
}

// x0/y0/x1/y1 are offset from the character upper-left layout position, in pixels. Therefore x0/y0 are often fairly close to zero.
// Not to be mistaken with texture coordinates, which are held by u0/v0/u1/v1 in normalized format (0.0..1.0 on each texture axis).
// 'cfg' is not necessarily == 'this->ConfigData' because multiple source fonts+configs can be used to build one target font.
void ImFont::AddGlyph(const ImFontConfig* cfg, ImWchar codepoint, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x)
{
    if (cfg != NULL)
    {
        // Clamp & recenter if needed
        const float advance_x_original = advance_x;
        advance_x = ImClamp(advance_x, cfg->GlyphMinAdvanceX, cfg->GlyphMaxAdvanceX);
        if (advance_x != advance_x_original)
        {
            float char_off_x = cfg->PixelSnapH ? ImFloor((advance_x - advance_x_original) * 0.5f) : (advance_x - advance_x_original) * 0.5f;
            x0 += char_off_x;
            x1 += char_off_x;
        }

        // Snap to pixel
        if (cfg->PixelSnapH)
            advance_x = IM_ROUND(advance_x);

        // Bake spacing
        advance_x += cfg->GlyphExtraSpacing.x;
    }

    Glyphs.resize(Glyphs.Size + 1);
    ImFontGlyph& glyph = Glyphs.back();
    glyph.Codepoint = (unsigned int)codepoint;
    glyph.Visible = (x0 != x1) && (y0 != y1);
    glyph.Colored = false;
    glyph.X0 = x0;
    glyph.Y0 = y0;
    glyph.X1 = x1;
    glyph.Y1 = y1;
    glyph.U0 = u0;
    glyph.V0 = v0;
    glyph.U1 = u1;
    glyph.V1 = v1;
    glyph.AdvanceX = advance_x;

    // Compute rough surface usage metrics (+1 to account for average padding, +0.99 to round)
    // We use (U1-U0)*TexWidth instead of X1-X0 to account for oversampling.
    float pad = ContainerAtlas->TexGlyphPadding + 0.99f;
    DirtyLookupTables = true;
    MetricsTotalSurface += (int)((glyph.U1 - glyph.U0) * ContainerAtlas->TexWidth + pad) * (int)((glyph.V1 - glyph.V0) * ContainerAtlas->TexHeight + pad);
}

void ImFont::AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst)
{
    IM_ASSERT(IndexLookup.Size > 0);    // Currently this can only be called AFTER the font has been built, aka after calling ImFontAtlas::GetTexDataAs*() function.
    unsigned int index_size = (unsigned int)IndexLookup.Size;

    if (dst < index_size && IndexLookup.Data[dst] == (ImWchar)-1 && !overwrite_dst) // 'dst' already exists
        return;
    if (src >= index_size && dst >= index_size) // both 'dst' and 'src' don't exist -> no-op
        return;

    GrowIndex(dst + 1);
    IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (ImWchar)-1;
    IndexAdvanceX[dst] = (src < index_size) ? IndexAdvanceX.Data[src] : 1.0f;
}

const ImFontGlyph* ImFont::FindGlyph(ImWchar c) const
{
    if (c >= (size_t)IndexLookup.Size)
        return FallbackGlyph;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return FallbackGlyph;
    return &Glyphs.Data[i];
}

const ImFontGlyph* ImFont::FindGlyphNoFallback(ImWchar c) const
{
    if (c >= (size_t)IndexLookup.Size)
        return NULL;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return NULL;
    return &Glyphs.Data[i];
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
    // Simple word-wrapping for English, not full-featured. Please submit failing cases!
    // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

    // For references, possible wrap point marked with ^
    //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
    //      ^    ^    ^   ^   ^__    ^    ^

    // List of hardcoded separators: .,;!?'"

    // Skip extra blanks after a line returns (that includes not counting them in width computation)
    // e.g. "Hello    world" --> "Hello" "World"

    // Cut words that cannot possibly fit within one line.
    // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;
    wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)*s;
        const char* next_s;
        if (c < 0x80)
            next_s = s + 1;
        else
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
        if (c == 0)
            break;

        if (c < 32)
        {
            if (c == '\n')
            {
                line_width = word_width = blank_width = 0.0f;
                inside_word = true;
                s = next_s;
                continue;
            }
            if (c == '\r')
            {
                s = next_s;
                continue;
            }
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX);
        if (ImCharIsBlankW(c))
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
                word_end = s;
            }
            blank_width += char_width;
            inside_word = false;
        }
        else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            }
            else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = (c != '.' && c != ',' && c != ';' && c != '!' && c != '?' && c != '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width > wrap_width)
        {
            // Words that cannot possibly fit within an entire line will be cut anywhere.
            if (word_width < wrap_width)
                s = prev_word_end ? prev_word_end : word_end;
            break;
        }

        s = next_s;
    }

    return s;
}

ImVec2 ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

    const float line_height = size;
    const float scale = size / FontSize;

    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    const char* s = text_begin;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                if (text_size.x < line_width)
                    text_size.x = line_width;
                text_size.y += line_height;
                line_width = 0.0f;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        const char* prev_s = s;
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                continue;
            }
            if (c == '\r')
                continue;
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX) * scale;
        if (line_width + char_width >= max_width)
        {
            s = prev_s;
            break;
        }

        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

// Note: as with every ImDrawList drawing function, this expects that the font atlas texture is bound.
void ImFont::RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const
{
    const ImFontGlyph* glyph = FindGlyph(c);
    if (!glyph || !glyph->Visible)
        return;
    if (glyph->Colored)
        col |= ~IM_COL32_A_MASK;
    float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
    pos.x = IM_FLOOR(pos.x);
    pos.y = IM_FLOOR(pos.y);
    draw_list->PrimReserve(6, 4);
    draw_list->PrimRectUV(ImVec2(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale), ImVec2(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale), ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V1), col);
}

// Note: as with every ImDrawList drawing function, this expects that the font atlas texture is bound.
void ImFont::RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // ImGui:: functions generally already provides a valid text_end, so this is merely to handle direct calls.

    // Align to be pixel perfect
    pos.x = IM_FLOOR(pos.x);
    pos.y = IM_FLOOR(pos.y);
    float x = pos.x;
    float y = pos.y;
    if (y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    // Fast-forward to first visible line
    const char* s = text_begin;
    if (y + line_height < clip_rect.y && !word_wrap_enabled)
        while (y + line_height < clip_rect.y && s < text_end)
        {
            s = (const char*)memchr(s, '\n', text_end - s);
            s = s ? s + 1 : text_end;
            y += line_height;
        }

    // For large text, scan for the last visible line in order to avoid over-reserving in the call to PrimReserve()
    // Note that very large horizontal line will still be affected by the issue (e.g. a one megabyte string buffer without a newline will likely crash atm)
    if (text_end - s > 10000 && !word_wrap_enabled)
    {
        const char* s_end = s;
        float y_end = y;
        while (y_end < clip_rect.w && s_end < text_end)
        {
            s_end = (const char*)memchr(s_end, '\n', text_end - s_end);
            s_end = s_end ? s_end + 1 : text_end;
            y_end += line_height;
        }
        text_end = s_end;
    }
    if (s == text_end)
        return;

    // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
    const int vtx_count_max = (int)(text_end - s) * 4;
    const int idx_count_max = (int)(text_end - s) * 6;
    const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
    draw_list->PrimReserve(idx_count_max, vtx_count_max);

    ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

    const ImU32 col_untinted = col | ~IM_COL32_A_MASK;

    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                x = pos.x;
                y += line_height;
                if (y > clip_rect.w)
                    break; // break out of main loop
                continue;
            }
            if (c == '\r')
                continue;
        }

        const ImFontGlyph* glyph = FindGlyph((ImWchar)c);
        if (glyph == NULL)
            continue;

        float char_width = glyph->AdvanceX * scale;
        if (glyph->Visible)
        {
            // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
            float x1 = x + glyph->X0 * scale;
            float x2 = x + glyph->X1 * scale;
            float y1 = y + glyph->Y0 * scale;
            float y2 = y + glyph->Y1 * scale;
            if (x1 <= clip_rect.z && x2 >= clip_rect.x)
            {
                // Render a character
                float u1 = glyph->U0;
                float v1 = glyph->V0;
                float u2 = glyph->U1;
                float v2 = glyph->V1;

                // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
                if (cpu_fine_clip)
                {
                    if (x1 < clip_rect.x)
                    {
                        u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                        x1 = clip_rect.x;
                    }
                    if (y1 < clip_rect.y)
                    {
                        v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                        y1 = clip_rect.y;
                    }
                    if (x2 > clip_rect.z)
                    {
                        u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                        x2 = clip_rect.z;
                    }
                    if (y2 > clip_rect.w)
                    {
                        v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                        y2 = clip_rect.w;
                    }
                    if (y1 >= y2)
                    {
                        x += char_width;
                        continue;
                    }
                }

                // Support for untinted glyphs
                ImU32 glyph_col = glyph->Colored ? col_untinted : col;

                // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug builds. Inlined here:
                {
                    idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx+1); idx_write[2] = (ImDrawIdx)(vtx_current_idx+2);
                    idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx+2); idx_write[5] = (ImDrawIdx)(vtx_current_idx+3);
                    vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = glyph_col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                    vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = glyph_col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                    vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = glyph_col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                    vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = glyph_col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                    vtx_write += 4;
                    vtx_current_idx += 4;
                    idx_write += 6;
                }
            }
        }
        x += char_width;
    }

    // Give back unused vertices (clipped ones, blanks) ~ this is essentially a PrimUnreserve() action.
    draw_list->VtxBuffer.Size = (int)(vtx_write - draw_list->VtxBuffer.Data); // Same as calling shrink()
    draw_list->IdxBuffer.Size = (int)(idx_write - draw_list->IdxBuffer.Data);
    draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_IdxWritePtr = idx_write;
    draw_list->_VtxCurrentIdx = vtx_current_idx;
}

//-----------------------------------------------------------------------------
// [SECTION] ImGui Internal Render Helpers
//-----------------------------------------------------------------------------
// Vaguely redesigned to stop accessing ImGui global state:
// - RenderArrow()
// - RenderBullet()
// - RenderCheckMark()
// - RenderMouseCursor()
// - RenderArrowPointingAt()
// - RenderRectFilledRangeH()
// - RenderRectFilledWithHole()
//-----------------------------------------------------------------------------
// Function in need of a redesign (legacy mess)
// - RenderColorRectWithAlphaCheckerboard()
//-----------------------------------------------------------------------------

// Render an arrow aimed to be aligned with text (p_min is a position in the same space text would be positioned). To e.g. denote expanded/collapsed state
void ImGui::RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);

    ImVec2 a, b, c;
    switch (dir)
    {
    case ImGuiDir_Up:
    case ImGuiDir_Down:
        if (dir == ImGuiDir_Up) r = -r;
        a = ImVec2(+0.000f, +0.750f) * r;
        b = ImVec2(-0.866f, -0.750f) * r;
        c = ImVec2(+0.866f, -0.750f) * r;
        break;
    case ImGuiDir_Left:
    case ImGuiDir_Right:
        if (dir == ImGuiDir_Left) r = -r;
        a = ImVec2(+0.750f, +0.000f) * r;
        b = ImVec2(-0.750f, +0.866f) * r;
        c = ImVec2(-0.750f, -0.866f) * r;
        break;
    case ImGuiDir_None:
    case ImGuiDir_COUNT:
        IM_ASSERT(0);
        break;
    }
    draw_list->AddTriangleFilled(center + a, center + b, center + c, col);
}

void ImGui::RenderBullet(ImDrawList* draw_list, ImVec2 pos, ImU32 col)
{
    draw_list->AddCircleFilled(pos, draw_list->_Data->FontSize * 0.20f, col, 8);
}

void ImGui::RenderCheckMark(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz)
{
    float thickness = ImMax(sz / 5.0f, 1.0f);
    sz -= thickness * 0.5f;
    pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

    float third = sz / 3.0f;
    float bx = pos.x + third;
    float by = pos.y + sz - third * 0.5f;
    draw_list->PathLineTo(ImVec2(bx - third, by - third));
    draw_list->PathLineTo(ImVec2(bx, by));
    draw_list->PathLineTo(ImVec2(bx + third * 2.0f, by - third * 2.0f));
    draw_list->PathStroke(col, 0, thickness);
}

void ImGui::RenderMouseCursor(ImDrawList* draw_list, ImVec2 pos, float scale, ImGuiMouseCursor mouse_cursor, ImU32 col_fill, ImU32 col_border, ImU32 col_shadow)
{
    if (mouse_cursor == ImGuiMouseCursor_None)
        return;
    IM_ASSERT(mouse_cursor > ImGuiMouseCursor_None && mouse_cursor < ImGuiMouseCursor_COUNT);

    ImFontAtlas* font_atlas = draw_list->_Data->Font->ContainerAtlas;
    ImVec2 offset, size, uv[4];
    if (font_atlas->GetMouseCursorTexData(mouse_cursor, &offset, &size, &uv[0], &uv[2]))
    {
        pos -= offset;
        ImTextureID tex_id = font_atlas->TexID;
        draw_list->PushTextureID(tex_id);
        draw_list->AddImage(tex_id, pos + ImVec2(1, 0) * scale, pos + (ImVec2(1, 0) + size) * scale,    uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos + ImVec2(2, 0) * scale, pos + (ImVec2(2, 0) + size) * scale,    uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos,                        pos + size * scale,                     uv[2], uv[3], col_border);
        draw_list->AddImage(tex_id, pos,                        pos + size * scale,                     uv[0], uv[1], col_fill);
        draw_list->PopTextureID();
    }
}

// Render an arrow. 'pos' is position of the arrow tip. half_sz.x is length from base to tip. half_sz.y is length on each side.
void ImGui::RenderArrowPointingAt(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, ImGuiDir direction, ImU32 col)
{
    switch (direction)
    {
    case ImGuiDir_Left:  draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), pos, col); return;
    case ImGuiDir_Right: draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), pos, col); return;
    case ImGuiDir_Up:    draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), pos, col); return;
    case ImGuiDir_Down:  draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), pos, col); return;
    case ImGuiDir_None: case ImGuiDir_COUNT: break; // Fix warnings
    }
}

static inline float ImAcos01(float x)
{
    if (x <= 0.0f) return IM_PI * 0.5f;
    if (x >= 1.0f) return 0.0f;
    return ImAcos(x);
    //return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f; // Cheap approximation, may be enough for what we do.
}

// FIXME: Cleanup and move code to ImDrawList.
void ImGui::RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding)
{
    if (x_end_norm == x_start_norm)
        return;
    if (x_start_norm > x_end_norm)
        ImSwap(x_start_norm, x_end_norm);

    ImVec2 p0 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_start_norm), rect.Min.y);
    ImVec2 p1 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_end_norm), rect.Max.y);
    if (rounding == 0.0f)
    {
        draw_list->AddRectFilled(p0, p1, col, 0.0f);
        return;
    }

    rounding = ImClamp(ImMin((rect.Max.x - rect.Min.x) * 0.5f, (rect.Max.y - rect.Min.y) * 0.5f) - 1.0f, 0.0f, rounding);
    const float inv_rounding = 1.0f / rounding;
    const float arc0_b = ImAcos01(1.0f - (p0.x - rect.Min.x) * inv_rounding);
    const float arc0_e = ImAcos01(1.0f - (p1.x - rect.Min.x) * inv_rounding);
    const float half_pi = IM_PI * 0.5f; // We will == compare to this because we know this is the exact value ImAcos01 can return.
    const float x0 = ImMax(p0.x, rect.Min.x + rounding);
    if (arc0_b == arc0_e)
    {
        draw_list->PathLineTo(ImVec2(x0, p1.y));
        draw_list->PathLineTo(ImVec2(x0, p0.y));
    }
    else if (arc0_b == 0.0f && arc0_e == half_pi)
    {
        draw_list->PathArcToFast(ImVec2(x0, p1.y - rounding), rounding, 3, 6); // BL
        draw_list->PathArcToFast(ImVec2(x0, p0.y + rounding), rounding, 6, 9); // TR
    }
    else
    {
        draw_list->PathArcTo(ImVec2(x0, p1.y - rounding), rounding, IM_PI - arc0_e, IM_PI - arc0_b, 3); // BL
        draw_list->PathArcTo(ImVec2(x0, p0.y + rounding), rounding, IM_PI + arc0_b, IM_PI + arc0_e, 3); // TR
    }
    if (p1.x > rect.Min.x + rounding)
    {
        const float arc1_b = ImAcos01(1.0f - (rect.Max.x - p1.x) * inv_rounding);
        const float arc1_e = ImAcos01(1.0f - (rect.Max.x - p0.x) * inv_rounding);
        const float x1 = ImMin(p1.x, rect.Max.x - rounding);
        if (arc1_b == arc1_e)
        {
            draw_list->PathLineTo(ImVec2(x1, p0.y));
            draw_list->PathLineTo(ImVec2(x1, p1.y));
        }
        else if (arc1_b == 0.0f && arc1_e == half_pi)
        {
            draw_list->PathArcToFast(ImVec2(x1, p0.y + rounding), rounding, 9, 12); // TR
            draw_list->PathArcToFast(ImVec2(x1, p1.y - rounding), rounding, 0, 3);  // BR
        }
        else
        {
            draw_list->PathArcTo(ImVec2(x1, p0.y + rounding), rounding, -arc1_e, -arc1_b, 3); // TR
            draw_list->PathArcTo(ImVec2(x1, p1.y - rounding), rounding, +arc1_b, +arc1_e, 3); // BR
        }
    }
    draw_list->PathFillConvex(col);
}

void ImGui::RenderRectFilledWithHole(ImDrawList* draw_list, ImRect outer, ImRect inner, ImU32 col, float rounding)
{
    const bool fill_L = (inner.Min.x > outer.Min.x);
    const bool fill_R = (inner.Max.x < outer.Max.x);
    const bool fill_U = (inner.Min.y > outer.Min.y);
    const bool fill_D = (inner.Max.y < outer.Max.y);
    if (fill_L) draw_list->AddRectFilled(ImVec2(outer.Min.x, inner.Min.y), ImVec2(inner.Min.x, inner.Max.y), col, rounding, ImDrawFlags_RoundCornersNone | (fill_U ? 0 : ImDrawFlags_RoundCornersTopLeft)    | (fill_D ? 0 : ImDrawFlags_RoundCornersBottomLeft));
    if (fill_R) draw_list->AddRectFilled(ImVec2(inner.Max.x, inner.Min.y), ImVec2(outer.Max.x, inner.Max.y), col, rounding, ImDrawFlags_RoundCornersNone | (fill_U ? 0 : ImDrawFlags_RoundCornersTopRight)   | (fill_D ? 0 : ImDrawFlags_RoundCornersBottomRight));
    if (fill_U) draw_list->AddRectFilled(ImVec2(inner.Min.x, outer.Min.y), ImVec2(inner.Max.x, inner.Min.y), col, rounding, ImDrawFlags_RoundCornersNone | (fill_L ? 0 : ImDrawFlags_RoundCornersTopLeft)    | (fill_R ? 0 : ImDrawFlags_RoundCornersTopRight));
    if (fill_D) draw_list->AddRectFilled(ImVec2(inner.Min.x, inner.Max.y), ImVec2(inner.Max.x, outer.Max.y), col, rounding, ImDrawFlags_RoundCornersNone | (fill_L ? 0 : ImDrawFlags_RoundCornersBottomLeft) | (fill_R ? 0 : ImDrawFlags_RoundCornersBottomRight));
    if (fill_L && fill_U) draw_list->AddRectFilled(ImVec2(outer.Min.x, outer.Min.y), ImVec2(inner.Min.x, inner.Min.y), col, rounding, ImDrawFlags_RoundCornersTopLeft);
    if (fill_R && fill_U) draw_list->AddRectFilled(ImVec2(inner.Max.x, outer.Min.y), ImVec2(outer.Max.x, inner.Min.y), col, rounding, ImDrawFlags_RoundCornersTopRight);
    if (fill_L && fill_D) draw_list->AddRectFilled(ImVec2(outer.Min.x, inner.Max.y), ImVec2(inner.Min.x, outer.Max.y), col, rounding, ImDrawFlags_RoundCornersBottomLeft);
    if (fill_R && fill_D) draw_list->AddRectFilled(ImVec2(inner.Max.x, inner.Max.y), ImVec2(outer.Max.x, outer.Max.y), col, rounding, ImDrawFlags_RoundCornersBottomRight);
}

// Helper for ColorPicker4()
// NB: This is rather brittle and will show artifact when rounding this enabled if rounded corners overlap multiple cells. Caller currently responsible for avoiding that.
// Spent a non reasonable amount of time trying to getting this right for ColorButton with rounding+anti-aliasing+ImGuiColorEditFlags_HalfAlphaPreview flag + various grid sizes and offsets, and eventually gave up... probably more reasonable to disable rounding altogether.
// FIXME: uses ImGui::GetColorU32
void ImGui::RenderColorRectWithAlphaCheckerboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 col, float grid_step, ImVec2 grid_off, float rounding, ImDrawFlags flags)
{
    if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
        flags = ImDrawFlags_RoundCornersDefault_;
    if (((col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) < 0xFF)
    {
        ImU32 col_bg1 = GetColorU32(ImAlphaBlendColors(IM_COL32(204, 204, 204, 255), col));
        ImU32 col_bg2 = GetColorU32(ImAlphaBlendColors(IM_COL32(128, 128, 128, 255), col));
        draw_list->AddRectFilled(p_min, p_max, col_bg1, rounding, flags);

        int yi = 0;
        for (float y = p_min.y + grid_off.y; y < p_max.y; y += grid_step, yi++)
        {
            float y1 = ImClamp(y, p_min.y, p_max.y), y2 = ImMin(y + grid_step, p_max.y);
            if (y2 <= y1)
                continue;
            for (float x = p_min.x + grid_off.x + (yi & 1) * grid_step; x < p_max.x; x += grid_step * 2.0f)
            {
                float x1 = ImClamp(x, p_min.x, p_max.x), x2 = ImMin(x + grid_step, p_max.x);
                if (x2 <= x1)
                    continue;
                ImDrawFlags cell_flags = ImDrawFlags_RoundCornersNone;
                if (y1 <= p_min.y) { if (x1 <= p_min.x) cell_flags |= ImDrawFlags_RoundCornersTopLeft; if (x2 >= p_max.x) cell_flags |= ImDrawFlags_RoundCornersTopRight; }
                if (y2 >= p_max.y) { if (x1 <= p_min.x) cell_flags |= ImDrawFlags_RoundCornersBottomLeft; if (x2 >= p_max.x) cell_flags |= ImDrawFlags_RoundCornersBottomRight; }

                // Combine flags
                cell_flags = (flags == ImDrawFlags_RoundCornersNone || cell_flags == ImDrawFlags_RoundCornersNone) ? ImDrawFlags_RoundCornersNone : (cell_flags & flags);
                draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col_bg2, rounding, cell_flags);
            }
        }
    }
    else
    {
        draw_list->AddRectFilled(p_min, p_max, col, rounding, flags);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(const unsigned char *input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier_out_e, *stb__barrier_out_b;
static const unsigned char *stb__barrier_in_b;
static unsigned char *stb__dout;
static void stb__match(const unsigned char *data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_out_b) { stb__dout = stb__barrier_out_e+1; return; }
    while (length--) *stb__dout++ = *data++;
}

static void stb__lit(const unsigned char *data, unsigned int length)
{
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_in_b) { stb__dout = stb__barrier_out_e+1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static const unsigned char *stb_decompress_token(const unsigned char *i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
        else if (*i >= 0x08)  stb__lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i+3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen = buflen % 5552;

    unsigned long i;
    while (buflen) {
        for (i=0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output, const unsigned char *i, unsigned int /*length*/)
{
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    const unsigned int olen = stb_decompress_length(i);
    stb__barrier_in_b = i;
    stb__barrier_out_e = output + olen;
    stb__barrier_out_b = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        const unsigned char *old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                IM_ASSERT(stb__dout == output + olen);
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int) stb__in4(2))
                    return 0;
                return olen;
            } else {
                IM_ASSERT(0); /* NOTREACHED */
                return 0;
            }
        }
        IM_ASSERT(stb__dout <= output + olen);
        if (stb__dout > output + olen)
            return 0;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Default font data (ProggyClean.ttf)
//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using misc/fonts/binary_to_compressed_c.cpp (with compression + base85 string encoding).
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
//-----------------------------------------------------------------------------
static const char proggy_clean_ttf_compressed_data_base85[44050 + 1] =
    "7])#######F;`vs'/###I),##bw9hLYqH##pQdL<cKEtn2<vLgP?'o/lExF>S@t8S(guH2@6YY#&]n42GXNZJw.>>#4LD21l%lj;j)8RGpPUV$#+3v56PV=B(g9*_(3NP&Id68%11XGH"
    "1ml/$e.CG);nh'#xIk7D7(88Umqbf(GWus/(a(*HUYI%#.aDE-%H7u/+-0%J;LQfLcW+41@5a1^'/d<B)rqd&W'l?-W<oY-TT$=(Fq(($K*m<-1rEn/<_[FHtYcs3bduH2u@uu#T3JuB"
    "j]0s)&IFrdr4kn/;K[^IB?lu#>gG<-$W+41+>00FAG[xk8['SeaMfn0f%S+HD?7dg#u#PfOV+_#xbFVC?8(W^7LC9MHb?+#`6Cl1@Z%32KIraAeI1=#E<5)MmLrCa3h]F%U`a7exASVQ"
    "nc7R*PPYY#Z0Yh#[si^#i/;,#OX1xLOmW8$[cW3M<DT;-t/Xn/F5B2#=A+.#6k3n/-`($#&2>>#O.,d2FQFVHU*WV$<51G`Zo'##<,LF%']T:#)TIFMx3Js$.=Im/RP'##i;%##@*oJM"
    "Sds_?_bHS7A+Hu#eJ/;-,p>wTSUN&#0N&F.XgFf_A0Eb.W=rKG&,2F%3+5F%x_'B#'jRvPXK+p%G%KlfAru-$,ltgLlCB4R9lL%M^C->#?9h'#6:Y`MnMpnLXw)$#/>r*MH46Y#_M?IM"
    "+*C<.YZ4A=Twt-$OY.R3'J=gL6#T,M7`T;-h]goLa>*G#Uc9B#XwK=Q2VhlLQao(WSt4Vd)?gQa06'.$x'rr-q+DG2qDHJr1tS_&,.6;#]S#<-gi4sLY*9VmjO0#,x't.LY:2G`'0,:2"
    "n8bi9P:u[tY+=R*Zp=;$>m;MB*]@5BmDHJrio8kO:W%>YYhJJrA>UF%a?+_SsmUfL'E[1g6Ml-$Aoq9;Sa%-#X7a'M^.I3#TQj-$JY)d*jtDpJ9ISlJ?g%j:EwNYdxW:m9?(-F.g2Q9i"
    "Ax4;6Pj(,)W-u-$%4aV$`tm-$GPNG)xu;w^5A(^>ETK_&Yn(F7l`98.@S=%tPG,;n-P<KrPV0Yu_:;/16Ml-$$e.292-m59LWk:mC'PJrS@Uw9t#-_J[]l-$]P]1gRQs-$`kr-$e$s-$"
    "HW,Da1m9_S2,KP8vW6^#0@W:/'8Oxu]DG)MK-;.M+aLD3wAVx^Wq](MLK'=#=`l[0B@_J#*1FA#ejxcN^g::#.aO.#Sc#]%[lhi0m'/T.0sE9#U<:H/?`s)#UJA_M7&0nLLKhE#h#_B#"
    "A-wiLFcbjLm]Y&#sXajL(cYs-eo4wLaotjL0,O4#]&.8#0sF6#'W_@-dn,D-QM#<--).m/CQQ$v5ZL7#34`T.]`Q(#8@Rm/_<&=#kC*1#>4`T.Kig:#x9B,23]/fq1?pl&FO+,)v/hf("
    "AIY]u?66R*#CGk4]EKk=ij$4vX*M$#Zv$8#H-[a..1?;#?2.b%qDPxkCY#E=Et$]tU#r-$1+>AXH1v-$CeY+#nC//.sj%nLUu'kLX1CkLOI-LM_O95#I####Sun+Me/ll&$D:vu2%78%"
    "+xvo%)/pXu6=no%-e.T&`)Ke$]UNP&3946',2T=ufA^e$bq/2'8Nb2(d5Ke$f3gi':s#k(/59xtGKGJ(6a#K)hAKe$7W^e$ug(,)5?HD*jGKe$s)`c);8V)+lMKe$'B@D*8a`]+nSKe$"
    ">m^e$&^w%+LFk^,68=%t^uW]+Bxj>-r`Ke$.89>,D4Kv-tfKe$CvKe$jSTS.:>x_si_5;-HXc8/woKe$?xlr-Bxll/#vKe$=:MS.JqhP0UTwm0>>A(svT.5/OBwM1',Le$Fnel/W#902"
    "AA&cr(0FM0QZ&g2VpS,31oD_&OK'/1]Pld3DDaFr1d^f1X>lD4/DLe$W&?G2XD:&5siZjq85Zc2PdUY52MLe$_M;D3eCE?6IG*fq@fr%4a1Ev66YLe$b+`e$n+S]4UGjo78`Le$lC4>5"
    "f_xS8:fLe$p[ku5l3(69OMI.qH@`e$uwKV6qHU2:>rLe$):-87]=_f:@xLe$'Rdo7oZmJ;B(Me$nIMe$4]v(<TPiLpb#al8s)/d<E1Me$8<AM9c*8A=G7Me$6Tx.:uA4&>$WbA>OtE_&"
    "ApXf:gTku>KCMe$?2:G;&&$Z?MIMe$GJq(<jv,8@+AvV@U0F_&HfQ`<1x79A_VQ4o*(3A=-f7pAS[Me$P@jx=-l[PBidJWn1O/>>qlw.CVeMe$Whfu>3F0jCXkMe$b*GV?t79GDZqMe$"
    "6Uae$aE(8@DTDHEg]U7nB^_o@:0D)F_'Ne$iv?PA@ZIaFj`:rmsjae$n;w1BEpw]Gc3Ne$xSWiB'F*;He9Ne$vl8JCC,9vHg?Ne$D*be$+2p+D+q]oIiENe$)JPcDHYlSJkKNe$1c1DE"
    ".<u1KMuhPKs2G_&2(i%FSU*3Ltf#Ylj?I]FOC*jLq^Ne$:X*>GOINJMaqq&lqgEYG52j(NtgNe$A*';HU$#dNvmNe$KB^rH8S+AOxsNe$Xgbe$J^>SIg27BP&m']k,vu4J]d6#Q&*Oe$"
    "R8VlJc8<ZQ)pb@k?&ce$WS7MKhMjVR*6Oe$bln.LAbr4S,<Oe$`.OfLf`+pS.BOe$g;ce$kI0GME6OiT0HOe$ibg(Nk7_MU2NOe$s$H`NHWg+V4TOe$nPce$r?)AO&Fs,W3vJ(jSW`xO"
    "rwrcW8aOe$$q@YPxKxDX6#0ciTfce$)6x:Q'bOAY<mOe$-NXrQ)0h#Z9&kFief9SR%tgYZ@#Pe$&&de$<,q4SU:5S[B)Pe$:DQlS*KD8]D/Pe$D]2MTX[Ll]F5Pe$-;de$Cxi.U;YXm^"
    "@)oIh%:JfU15XM_JAPe$KR+GV3G9/`LGPe$2DQe$4OAc`D/S.h0$(DW7lPGaOPPe$]<_%XcsX%bQVPe$ZT?]X9.V`bDhe&cH/sLg=pv=Y>Ue]cUcPe$d2WuY@hE>dWiPe$lJ8VZjiMrd"
    "E-B;e`OI_&mfo7[KdYseN5<lfN(Po[GQYSf^%Qe$u@1P]GW(5gN#29fUOLl]q_Biga.Qe$&i-M^SV3NhS8[4f^*e._OD3/ie:Qe$POee$/FEf__@g,jV;@oeg^&G`TrfcjiFQe$7w](a"
    "ZFlDkY>%Se7eee$<<>`a`[CAlmRQe$FTu@b'9KuloXQe$DmUxb^nZYmq_Qe$]nRe$TWc7n_ADrd)>Rucb<srnthQe$OV3Vdjs4TobD)Vd1oj7edTx4pijOPp(UJ_&_4Koe1PW.q$%Re$"
    "]L,Pfk8hiq&+Re$eec1g4roFrpSdfr.hJ_&f*Digv4&HsiG-YcGB%Jhrx%)t,=Re$safe$o^[+i+uX&ulJh=cPv<ciwOX]u0IRe$w8tCj#]b&#U++GMla5;dZ2c'&dvrx+^>l'&qd''#"
    "d[k,MR9ieMQUcdMJ=-##UE31#'PF&##KBL5/9A##:&/5#ib.-#YKa)#u9v3#]d/*#u+2,#EK;4#X[*9#(]U7#U0:/#M+c6#C=x-#]%),#dYN1#]Mh/#fj8*#R[u##9l:$#B:r$#JF.%#"
    "Rke%#uQlf%l5TY,lBel/wpEM0%3'/1-pu(35>V`3:`R]4a2]i9[Tdo7c[%8ILE@M9w6FV?I.-AFlDq1K)YJcM1(,DN;k$>PRf-JU(gj4]AI@c`Y)m:dcPMrdi%f4f`5x.:ZW)585V#<?"
    "/Mc##<xL$#gQk&#7&*)#F,3)#OoA*#`bY+#B6o-#B0f-#)A*1#);w0#E@T2#NF^2#T'Z3#X%.8#Xu$8#/Oi,#3=(7#(Q>+#8BU/#.2;,#u3<)#+[V&.HMNjLrZ2xLJ:$##Wr%VM+V>s-"
    "/_ovL;bO)#iv%5#GL)uLCwXrLNQF&#m5u4J9##v#1(RS%lnPV-#Om(E9uo:Z*TRrZ11'5AT_Pg1&ld(#E7D,#qf60#Y#(/#xPD4#*-85#w<G##C6u6#D4i$#PXI%#Te[%#@93jLLK5s."
    "+W#,2a$u1B76tCaQ*Wc;8LHl]I95MK`gS`<wIn(W#VM&5Z#wE7F(t9)awQY#CiAE#/$]H#r(eK#,Cno#%'Q7$,i9O-<Bx=-Nsk>-er?@-8Ig^-F*ft-pTa_tRle+MQKG&#NKWY.dt(G#"
    "`&@A-''@A-q'@A-5e8;/96f=-YDOJM@h+GOW45GM)8?uuBl6mLbR`CaX`P=l9Jm[b^l6B5FE*##3nt6*kktL8SZn-$O64(V`r(iAx-x9)3wlx4nY+##32t4]c=2F%@_$@'/2$.$jDGq;"
    "[HCi#%FI#M4dvu`1?A##9ol+#88D,#u<':#7Vr,#jj>3#&`W1#V]+6#aOi,#Y/?;#B4<)#N3m3#oic+#HrG3#'a^:#9$-;#(IY##8=G##0AP##Qf<9#?4i$#@(V$#9e`78,n4ZGk5=&#"
    "kEX&#nd0'#$SK:#%K6(#+?$(#*/k9#pBU/#E&*)#MoA*#JPj)#]bY+#B$S-#B0f-#)/e0#);w0#7/92#M@T2#S'Z3#l[%-#0%.8#Voq7#$6H;#'Je8#'I9:#/Qp2#.np:#7<Q;#`[T=8"
    "t]]8Sb-9MK0G@N0)DV,#xO>+#j_0;(l$t(We%_._E(W0;5/Ka</JM>#p;xQ#bd^6#>-d6#9;WPAhWZ6#;5Y>-r3GZ[imb$'Jv/kLJY$[-Bj8&$gcVZ[9uXH=ns`D-LZ$Y:#UgP9r/;9C"
    ",f&KMCZ^40F2Q52G&Q]=mINe$m_t`-3I.g+>RtMC='^lLh=5,MLskkLVB-f?ri7,Eu8.F%`vN/2%&IW%m7W0;sKN5/v2cC?_6rd+]P[^?Kpb>-*=)u<1&8+9hpo`?F6@v$4vC']DEqk*"
    "/Ba;%+?Nv$e3S>-usL90kA6g7H]IV?evMk=qnTA.UrbNO+O3uLTOD6Mc5Z6#N^;X-ohA>(pHG=(Wk%v#CBi=/I_'6_xv&oLqcl8.3c$V[Qep;.aAxiL4_3v8Otms%FmtC?k-h%1v?k'#"
    "]T`i04_D>##oT^#`3l6#(],<-G2p,2l7p*#1qB'#<####lerCa@IA5BO.Lj_*P:;$-)'kLCdG598]&@0?d2>5E=.F%$gFV-B/g._PZ8>GtL[w'`(no%1KJi^hl%)*@-w.::&0REdouu#"
    "$J^]+5fYY#WIQlL]RG,2e6+gL*ecgL,qugLDt$29eWCG2''d6#?g*s6@mr6M`X61MgBJFOg-w1Mnm4GOi932Mi932MgGv6#P6P0.7;OGM3W.(#rh__%]:SC#8alk0%e75/$L8[#'K8f3"
    "Y-fI*'kRP/]=@8%UA@d)PJ))3=0w)4QD1@0hl:[0A4vr-g7+gLbS8f3-u9C4ZFWH*'#mP&)jv?$#w'<-2m.T&H^ev,<x_V$5x6s$#dYP&ETOcMlt=u%UExo%rp.T%9_N?#cZm5/CDh(a"
    "7+.<$T,=s67%rV$>BGYuW:IL(I1mYu';G>#8VcY#+LWb%AZY+%o?]p%)DlY#=dc##eu&58e<=F+rD@lf*c68%jr6onwQho7g/]p&,`fr6@?Fs-[xefLMHj$#(eDB%1q^3_ku]&?Ow'02"
    "tT35/ow;<-U1v<-(rLq7T/B,3pcB,3QI1j_3xqr$=#t/`?(?>#Sw;;$VxS8/[B:WANqb/2_V8f34^N.)lp?70Scf^+D8vm8<ANj;5Jxk$DO_cDvUTp7^aP&#auN$%UM22'fCq/2b0JiT"
    "JMF_#'G:;$gB=8%93CG)9Vk2_U@D0_Hn[&M=Y@k_k9IP/b(Sh(0D^]+]^I21_j[21)B7ZR^&[12-2w%4v6<<-e1v<-lFv<-l1v<-sFv<-n1v<-QQ6i$au>>#%CxBJNYd'&bCw'&.Nc'&"
    "5dc'&n:SC#nGUv-[5MG)*)TF4Pm(u$wAOZ6m('J3n9<78LiiR'$M0+*$]gb*j+es.o%[s.=vh2D6B1m&p%]L((J9+*Wh=n0Yo%Q0AH*ig.#m88#vW>-EI.<-8tJ0%Uq%##jhVVHU+PV-"
    "6[OiKJJ`22I?;mLi*gfj2%###XYDX(fTY6#Loq-$/dd6#7:)=-,4)=->4)=-+M#<-<M#<-bV>W-E`@dOVEU02#-gi0L>oj2IYb/MeY^s75e^_,)SUV$a^93_h'aK_/M>>#Q71m0;&dG*"
    "guYL21n6l1t3vr-;`tw5AB:a#Y%Xx'q]b1)2L-L,Z%.L,UgAb.w35N'dL0+*qM%d)21-@,@jcB&@9tR/9>r_&e%_w6.YQIbK[P:=^Kr0([*YJ(n+2K_81GM(u*TK()-g@#?CoT%jNrs6"
    "1^'#=Cg/2B;*wX&54cI)@iCv#h6i@,]i###g%'58rtbuY19:A4xUw6#%s[fL.&0j0wE<m'bh;m'rRc;%3UZ;%tJE2_=0A3k&#eEn+Ec'&3MnEn1DXI),gVEed1taeuk9'f.Hrc)M7%s$"
    "ikkj1:3O7/YA[L(/x]fLKeCE4x]DD3h:F`WLaRB5jaO=AiY2v-Oj_:%UV:v#^49+*-4ZG*Nt1hL@2A.*.G]p&K4Mv#.j0s$&rXi(a_258I68C+V+Mv#HLsP&`m(0('nA[u<S+',^v:0("
    "@@D6jeBO,*R<j&>o;$7#2rpk.mk.j0I/fu-`_kgLnom6#Ske%#*r+@5NYL0(Rbmtff^bvIB/x.M,r5DOq9,,MXF?##q,^o%Bc*=(l@cvItJ#4M&^YkMqDH/(;[u##'[b6#BY5<-V`5<-"
    ":Y5<-o-^g1H5sG)x3vr-&wA%6eB7lLI7NT/Cu6=7UPBs73%O&-dCXI)C7r?#jUKF*9M487Y+'9&,Xm_+ahiZ#2-Mj;]XFw$Ru-%-sB=g1nc>:&nIBJ)[^k9%x$vM9kaQ/2JRj5&qa4x#"
    "bihV$j`1'#tR0^#8J1&&tu[o@+:E/2*/wiLSG^S87=0?-Y(b`*htbP8Dio2DD4o.2iF^]+Op=<-A1v<-sxE_$2``K_);>>#;fec)=3Os-tqLP8Hc5g)xUB%%SKTZ7^JsR's,2H24.nR/"
    "Y7)ZulOc$,i2$'=4EmX$oe>,Mf(6m#gLb&#RiYY,5H4Ze?&]iLJSNn8TDvV%fH%W%xE-^#.((^#BlrM09),##mk.j0gB=gL(/VHM$*^fLD2oiLg5eh2K^9Z-kUFb3rfUC,LXqm8fpJe$"
    "[uxfL+<T@#>lfI_+()/:2s/xt=Ls^MMh`V$,cLP/E4<6&)_-TeGsOn*G&>uu[@1_]#B6_#.`kgLgad##cn]N-A',)N#d%)*80HcMAR,lLK,+JM>^-lLHbC`jg%5F%7gnD_=xGhMOl'0M"
    "Aq.>-LV>W-K%Hk2R5+/M/x>DO3jI.MG#+fME(1kLLX?LMI-fiL6f'KM2a.iL^$wLMBN)J)T2^m(k$)U:O3PPT<H51:Pg+&u]wgp7Yt`D+oq)<-]^AS'1O=G2L*#7#]RV=-Ul[Q-@#Kg1"
    "Ye%j0md^3(v3Pof$0&/:fApAZ:(Ar%rtMs-[-L8:BM^v-K1HP/O9pDcg/p%#JF3w7TmL]$si&f)cl'1&j^t>-aZrM%eF19'Ghu-&XO_hL2Fe+#*R4m#a(+&#<Qif(njkG*%m5j([r+Z$"
    "1,dp.A3pL_uL)jr:Hr-MSHnq7GG;s%o4qP'n$tD+ZPAb7mC@b2/]`F(*)4I)93]58,1oJj6e5:&+3n<&1'uMi<^veqHF(gLAIC&v1aqn#2'*)#D^SY5Ffou5uIO&#f3^gLhk*mfgEO,M"
    "5fG<-7fG<->C%q.`4h'#kb;9BPa%jL%@+jL:<&C%51[<_fG$9gE+,s6pJW]4jHeJ2GS#T&hC[2V#.gfLur1'#miNs.XY)6hJXHM9gSWv6Bc``3,1h#5WQSeAAbO=A'+P=.MdU:%V`U;$"
    "/,$w-&QOk%TWFB,]kD@&*18NT,CDBH46oX-r]dj09-3-*X+36/7A###K9g20#CL/#_6i8_q)sjtCjk-$32Kb_WQE-vi+Zj)];d6#>_@U.oCa6#5LM=-`5=k%t#Yg:p>FQ'''d6#Mp@n$"
    "vb9'#Zk.j0@8_s79h0wgT0fs-,p?79BkL?[i)T@#Ek=:&2:A6&[VafL6O@q.G$pi'HptVfFc0xt/@$Z$16%B#eRI3bm<S9`oSSY,oVg;-Tln$8@d.(S3#gb4l9,OFfECF@W:.T.#)>>#"
    "c[c</DD6_#M_rn8K#L#v[=(7#]LOgL?^n7$eR@%#7ZD1`%7=dt@.a:%IV6o8IksJ2&-BbcI,'COIx:.9Fk;s%hF+3M3rJfLEFx_sVSmiq'OV_]/^=_]oYI+M%%6wLZ)<$#=_AN-O=3Z'"
    "*5?n8wx:_]Qj^g:Yl5g)0sF<%Wl[qVhh:hL#Z-##0Q]X#PAL/#SHSs7O5`D+2b001,3pL_+Muu#u7f6#=8v9%0Dh8.Fc7C#xE(E#FjE.3DfWF3Us1T%sJ))3_TPK+4;=q0KGn;%TP(Z#"
    "^&DK(Rst^,#i=:&CN7H+Q0'7/IZ-d)cRpB+9-3-*`1@6&JZsJjs[8+MM3D'#iOho7hHb2_N+D0_;R>j`(=5,Mv1aJ:x9?v$2L?v$xo+?.g5eh22DXI)$--$$H7Gm''c><-G(^0%PGEF3"
    ",.U_4]CYI);N:j0+.jP&%h-k$(eE)<xVI&4$bGMpov/w#Hjxi'74IW$[QAW$l#2KN*)bIho5vx47x_;$iIcY#otc9'(GLS.3N70:=P0dkf;d6#FC4o&^vO/:+VE/2x3vcM1'pbi>iQd+"
    ">%'v5gYSV-C[]6#-^Z6#Fc1p.dTY6#0oq-$5dd6#w#%Grq)ChL:3DhLWD4jLXN'cju5UhLR)>M((,e6#]9S>-O3S>-sh6m%FNXb+'R^p7O_[A,Ir[A,eMTK21Lo8%=;.$$>iim/AS<F3"
    ",50J3X&Ht$dvlu>B$Xt$2Bp5&Wrpj17wt2$aE=gL;n5:&>#p>#`gjhu[r8c0/pHo@$=.Z#MxOwLI`@/95ar)+_W^6#<O/N-;G5s-U[;$M@.MP-rh)M-04)=-%F:S-9er=-9er=-abHm%"
    "N?wcM2l)V(>Kf58Dn[kt$9Ks-M&G)OXPg@#t&B%6mAT@#@ISt%t@qV$bgwB2fe82'f#CH2FU[W$)ql%+Z%cZ$uT'Poj+]?R1<R7$ld''#u9-5.m;k$M`P)7#;pKWfi#K%#c*V8_8a0q`"
    "I&l-$(XXe+MotjL[.fiLkB=/2U*[DNagb/2q978.u)(-;(0q#$t1]6#`FJV%?,d'&X`>>#1IJV%],19.w6rI3^dKq.wLg$,fN0:B8vgR/1kTT9vLB$-(2q*+>LnR/o?smLwcZI%sA'[6"
    "KfKwg*GKq#fYI%#&K`@_mjq^fl7m-$)kiwg;93wgfb1wgj$Vwg2&[`3m[jV70#.E<AGvE5Ml'0M8X;EO=6k(&wv]/:88RKu=Ssx4bMIV6:`.H;3O[A,>;Ls-1YajL*Sr'#'[b6#xTC2:"
    "qEhM:jc%g)-=<:.#VQi&V-J,;[m>g)ApT*YiT/[#Kiv[-B7]v7d.JS@x?\?L,o&h207D[lJs_I7nXD,_d^^=T%O2EA+/QK5)?n=6&Bf%N0^[mp.l^401H]SgL4[TjKx@fL(:xCvuTA@^$"
    "/B6_#j9F&#*Mc##eGY##kQBDN-,&)*<FGS7Dhx6#H/5##3l.j0<6Dt-jEqhL8O_:-eLZ@nM[*(&;C'01&j;5(N#x%#'[b6#VY5<-)r3W+JPY58AKqfD+[&=-p55i,DT9X-`H6TTfKUX%"
    "#Ns22?V[#8H7(r&(#7A4P6#7#>/5##)l.j0<6Dt-`_kgL]5Z6#b-,w$0bMW%`DXS%,Cgw'noYb+OF1:8'[&Q_sp/O&5WCG2TJif([;*O8GYw],D3Oj9I@4K3*A`m9RmOPTI2vl9mZ+?."
    "t8R>%(.[0#Z^''#vB%9_,<u0;AD:&5U?bp.%UY6#Roq-$1dd6#rr[fL^@+jLduuP9*FHT0ZJ3v%h]Yb+OXH3:(WrS&pq)<-4)^7.D@gD=^ce%%=Pr`3>U^:/*s*Q'hqc$,/fWF3BMj0E"
    "B4&5J?ePL2jG,B,GWFB,/@;8.GdT7/U(D;$3PY>#MOCE'TaalET5I9%6gPS/v8TM#snYm8j(U^#CeWT%HHeF'WhKW/XDK886*dGa;/uF`@gZ,2iBL/#X^''#e[NP/3ejl/3W,g)UC[p."
    "iTY6#Goq-$5dd6#8$%Gr(hPm88X'3(''d6#GI=8&L++=(b<]w@OJha<-8O2(q?Em8n@(0Edc]+4G,m]#fsT,;Jv]G3f0Ud2$*=x/o'[(+9P^mKqb4:&X_bki+'wJ&VOI8+.i*9/H>V.;"
    "JwVEe7RpCWfH,/L4L$29N0#7#JU.:.fk.j0g,%Z>LdF59.uF&#>V=2_BqH9ivTI9i5W,F%w6Z?gOmMliE,0(%Fv+68P9=2_2`2-Od:;69>P@&,]M:X-$@94r`N?p-gbYWSd>6N'x-,Q'"
    ".vKW$i>=j1N]/D$7Zx?&LXRx,hkmp.bUUw,QVuj)>mFf*@nWT%>Top&d.&[>%4mp^u$?K),<$7&C0CH2)Ngr.NMub+XF)v#%n-<&$99W$cQ4K1jRYm:AOMm$#BG2r%=Yp#R)V$#%%(C_"
    ")BwmsA/3`-dB],(:8<A4lvJM0iHw<1i,M*M2%vW-;ONF%n]Gs-nm'hLE^&c$@,dG*QidG*:O@R*[j4j9EwdM3xQD#&%T>N9]<;H*''d6#3>jF&AdK=-IN#M*8J'f)4X75/EsLT/S]Od="
    "R4r='EVt#5)'ME#mT3?#`miMKqb4:&JS_=A3fKb*/jF@&>+,b#5rU;$@=;v#jZr;6K@MB+8b'U%.5G>#Iugo.a/>MTAuBM9j#_6#]WjU-lc1p.3UY6#Woq-$&%Y<<A.C2:CvX6'8s/20"
    "(k`H_wHLk+Ma=_/K=_r0[5Z6#OfG<-rX`=-*.;.MhNEmL)tXs8`Y1E4''d6#j/l@&lIEj0wx38_#[aK_$,>>#.+R2(G=T;.q[`M2BS+Q'eL0+*qSID*ig'u$J><j1#7[=7kiE.32=C#$"
    "/x%Q/Yq[P/iB7f3Uj-U%TbSGj81GM(AMHT84ao/=)Rlr/lD-_#.PuY#qtPC+H>-o/2[3:..9=?6gqQj<5aDJ<^v0H2aeQM95tv,*L@5i(@DGu@J?S_+U+Mv#?JuY#[gTq%96NH*E<j`+"
    "<P%Q0%.W$@Q^98/@_R@#6pwiLmj?>#;l###g^SVH&`bo7<]Cv>QLx/3c%'w%OINp.wx38_&*aRUcPZ6#fCa6#eujh2[[aqg?9pg1t-iI37BT@#&x+B&?9m,MLKEJ:**[<&d<@L&buI9&"
    "C&5LWEN;m$1Hsu,KX-`m*M1`jHS)rTgYd6#($%GrAa+/(GCM(=Lbvu8c]^,.Aul'=1:2($jMl]#`A,W-Gf-F%>ELj0#5NK(F68x,97PM(/qxR1,9NK&sdII*qJ_*<2U6X/q'+O:u-NqK"
    "3N'j0pM6A4%`w6#J/5##qk.j0erc;-9iiv-gMls-PbdN:MAMkD,I#E:VooDXBOlc<:U%G:L,cAu@Q?&@R_r;r3+@5/:PtH:<[,:r-+&/15:x6#*33U-%93U-1Y5<-1#AT@hb0^#*nd/:"
    "]SDN;T/&N%d52/(W(b#>#sBX8XXhI:G,5DkYTbN`3fLd1-xmo%1KJi^_g$&+19k^6)<#,2MR)##5B6B=vMILaZgdh2Q+8+/Ri[:/`PN3XOS<F3Kl-F%*.YI)@%);$]6av#nrKH2po1g$"
    "@f9H2=:w8%:/xd#5F39%$&g=u5MG>#R3n0#ab+/(cWerQqXHM0(hceFb6eA/w?#29:2+8@VG3=(-DGHM>)N?#;.Xa+kiE.3)Mdh-2))C&6[6H2;FeS%+D4GM*NHT.^NEL#lTNZ#hDQ;$"
    "5XSM'_Ik(Nn'-E3?IET%G&7oun%MZ#M3;i1Jgd%+mr9?#@TH`*BSEgLQC-##=^v%+)BvV[q_di0?^X=/Jp:-/43pL_ElaIF<,#6Bl#1H2PcW%$CiC%6ZM.)*t?(E#KmE.3VO`v#?R4m#"
    "umEZ#K2Rs$78JW6ajBaSvT:v%3S4U%nfL8%V6FSR?.eS%ucXQ%'`;T%SOml/YAvv$x&mip&^+q$2lL;$ff55)4KI*N(,g%#VN3thM`qr$._jm8Jp&K2e&;_=eE)fQYS^-QZ7E5&b[ol#"
    "A_4QJ,4EuIs#kZ#._fRnQ)*&+o#X;$5f-S%1Ei6NH5>##FC_$$Hf1$#mP?(#];'&+;H,Q/`k<2(W)?lfMBE/26;FM_Zw(_#?4'&+Njpl&Xsk&=(]r(vB+,s6AUto%1<I#$<RC2_EjNw9"
    ">.#HP?7UnfV4GI)?]d8/K)'J3c&KCO?JhlSQ]t9+%x>c*]aZFW&P^b*&c,(+o'w1/BP4(+G0Rh,JH[.Mep][uO+%I1FEX&#[#Bi#c4=&#ngU-/93pL_M_hWJL7[^&xX9tqot60u.q>:9"
    "=J$9%qct1)eCC;0NIw[-0qB:%V*bY$dgS@#e]:rZ&KSt#pC@6&n,kZ#m>`/qIqWT%K]F;-;_&6&LlYW-:<9C&I5YY#jKeER:2%Abp(fr6_[6am%&;'#CbUd-=gO#oet&d<-7Z;%_QVp/"
    "Np52'<Ko5'9Fl+Mp;hH%Sh5R*iQ6R*umAw@`3*2McE=)#1Qkn%=S*=(-S1P2mMv6#xlRfL^%0j0dv+W-Xw?0uS#90uP`d'&om8tqxBFA#4(XD#WcJD*04vr-o`qk),fG<-MlP<-sT9S/"
    "Yc7C#(36N'nPCgCY@u)4vcMD3,CS@#]@(4+S2?o/#chl8emXT%i`d7&S6IB5lK4MMkAru5O(,2(;VuY#<$,o/0wX=AuGV)+qK1^><6>6&5PBJ)TD=K3RJ/<&#PVG+],.)*T1lZ,GV]S:"
    "eu&58HN?_#fe''#[####C`*GkxRO&#DmpO0__ho.)]0S[*sZiL`9x(#;PG$MU@6##//cp7Y$U^#Yd`Y#BeWr$[Q9T.+UY6#VrL*%F5-F%_de--g3u9)t*F_&T0_5D7+d42J86A=,f@H;"
    "f$u&#$/]6#rBf>-Mft_$0:@>#F>NqMip2EQv`5oL,Ehp9Sc]/Ng/Ag<f'wf;M2M[-5%A-Y4?cw7hjfZp'A^+ra'[]Fe&q%4=b($#Te%j02XD>#dk%v#%gRv>;j&:'Bn3R/Ij::%C7[=7"
    "?FpD3E'/w#mB*t&wSft#FO_GD8+%W$HS1<-092U%GRmp'G*>%vvHfP#M0`$#iRq/#JD=8%r-7G;d#60`+Pd'&&<u'&HHg'&5dc'&SO)##Md=<-NocW-Rg?:)g*2r0>a472b^RcD'NB<-"
    "K2v<-DuV-D0fpT9k,K3MC-Gw7C(8)=T#Ps-+#eiLJ]tJMi&h6(E_'<J?-lrLipA%#'[b6#XPf3%8a,F%]vUg8p+`a4=UKu.+',)<x6K%6Mu#J*H2Cv-tX_[,m1pb4stC.3q%hc$_]d8/"
    "u2Y`<:`Uv[7UkD#b,Av$Ik*X-j<(L577?c42Pr_,6<X`<NKSu-$p^@6hd)&+90<-*8(,2(=djm82i)P&80EH*0dMk'V_VM0Njn^5Ot:7'e[Mv#/-T30?3,89<@TD&e7`/)&FmAmDQ3_5"
    "UaaL)S:;v#SkGd3(Pdb+$HV)+*L1^>kk9DN87PM(8*3-*u&(-2.Nf.*dj?d+[vhc)U4lZ,lp,R&3.Uf)c]=K3bj;;&I^b^,@[)n8&RI&,@O[Q/rI^'+r9k]-WB$e2Jqn%#:xK?`9rYY#"
    "U'<;$E=F[''_p%4+YM<-@:*7%ElRE+r<BA+9Ecf1r'gS8jefS8LaPq.13pL_bEkW82n(-M?W<BOlqQ6%XfTjBAU0R`S4M&&8&@D33Xfi'fxA2_34W9LY5Z6#pu19.RtqFrPb<n815-Au"
    "6;$<-p?-G2^pKb?;0810J,'58wG$s?dQ,,)b,bxF78`6#'_Z6#;6T;-C/pg%KEx;-I5)=-n1C[%4#%@',3av<>JJ886ZgJ)#*<,<qg;,<K5&j:*,hY?*,hY?3&dp.o3pL_*K/TK$:RE("
    "$I#01>c.-#3)]-#aPZ6#Jk)O;Cs481d^j$#'[b6#4[c6#Rmq&41[q,3/]NT/)3qB#[5MG)*AWD#6&(-4%(YI)Bcor6+@GI)M):=-#12iL[d#9%/feT.KG>c4wDoH=-o?dDQ6FA#(cK+*"
    "d'eK#(?f^5-^]Q(I<Ax,MiC[,&dgw6p1F^44P)6/EVke)LpU0(l72$-KLUL(*S+M(+j5X7mUOd,.SJp&u*qf)D:IW$#-+D%YfsE*)_+=$ueJ5J-M;x-$ZKW7Cl<X(,c5P]<Iui(Kv'<7"
    "8>XW65]FY%xS%L(#J9f)U-/G*3v3.)RA@L(Ofcv6kqHj<DQ3P(NXes$`Cl?,u7kI)mJSX%;<WH*D#(,+XoIfL9t)j9urH(YI2<A+Qx`r?>V%7#G/5##9l.j06fsjLNuBKMPA-^=%?`D+"
    "TC.<-GOZ&%wx_Wf0?jb$:o4;63)#E=T76A=p>cp.)4pL_trB<AR.L0M?LN-Ext9/`:+b_7,RGgL69iHM1PEmLAvRiL]5Z6#]qRm%&7f'&9X@>#2Br%%ljK=-ic^:1=ZGq.TddD*,(X[A"
    "PTER8ESUj(s/Dk'V.RT.R^Qe)#j0(%a&#B,g(@w-=)4wI%ZmN'_mq</VWO&#OCdl/,qZo[a60/(-0ZlA6H*]t@o@-#7;x-#dcv6#tY5<-3`5<-=D&g%$1f'&3Ev9)w678.cll;-vr:T."
    "nCa6#eQk,%JA-F%#JCq'xi2>(+SZb+dmNW8Fn5,EA*6,Elbjk9LjfwSYeLq7JJ?$$wCx6#w),##l8XJ-x7jU%EoAtLB7oOrDP',NMu'kL:[`=-SM#<-X'oB&fSi34)uU;oa@`m9i?<Z6"
    "Q16J*,rF$>q$S?n1LXD#$9qs.stC.34^0I$<2'J3mNi2%Rg(Z>Y_,g)+dls-C_,M;E%[C#Ra,J*+3q6&*4mMT5A;6&?W(C&5c4:&#HXh#53oU7f?66';`:v#BR8<$fovV%WW4?-JlZl$"
    "D5Xi(&]b.)aQ^M'ZaGS/?;;6&=WS@YNov[bwxnr6NI5s.6j[R9U'fx,omtc2]'Nv#JW,R&mFn92OK2uL$3rF&TqCC+uu,U;I5YY#'&1I$`U'/L7#DV?gqa6#f^Z6#(ZlS.+oj6#U7`a%"
    ",#<X(ljF0_ZN]&M22%O_Jj)4+/j$@'&+`+$I^A0:sPUA5hkF59DcZp.s3pL_@Px]m%vc4M2`wIOwWj3&PRk>>VsT#$g_v9'0?cw7Pm<#-qss;-$M#<-Y'rH-to^K-8M#<-N(m<-VY#<-"
    "mxS,M/UNmLw%LFE<fsJ2-RnVo?k'E#N:ZDOTu1e$K/#q7%'e,*?+Lp7J+SEb97nS%-Q+',;Tw<&3lRV.u-Df)>#X,'?-+?#eNs;-(O,U,-_J8%A<d1%1o#/CC?Sf_.p98%?#QM'7(%1M"
    "wBh0MOGGA#:k;RUeif+4V7Gm's1v)4#&AA41Zb*.oLrI36N.)*r3lD#DiC%6urGA#1SEF3)$fJMN-%&%3wZEnHKtV$aMLpLMi_6&k3'U%4p[)$BEC,)E:6C%3w2`#@]1Z#4mb;$h036/"
    "E@iv#8sFv#coAIEPUZC%-bti$tSP5&M..p%7nJ,2u$p&$P[)Z#ZPZ$$bE=,2`,/?$:P^P8W>/&GZJA&Gii_$'CB3/Mm@h0Mk4U0Mi:$##MHxnLi92f#QsC$#bMi;;'d[a+d#60`R<q2`"
    "cB3mLuYGOM4A2pLZb6lLaAZ6#NTY6#10-e%Kw]-;hvA,3r`Ls-Pk%d<HnA,3Z,p;-4r:T.pCa6#CM#<-T77Q/A[c6#StqFr2Gh;-Jvav%&(4w7B/Y:.af6<.#AC8.TekD#vcMD3NTk-$"
    "ARa8.jx/+*SbSp.x)Qv$2Pnw'p%d8.DX)P(w0f--<x]fL,Fl>%l:d?-_`L?-$SY&%nCdY#l?%],m[?3)C%g1(bq@w#Zf@l'X>dn&vt2'6D=D99'_Lk'k=KN2vFMU83AVb%]?-5A_VlB,"
    "Jf%T%I$#B#37668FCk11e-bp%BwoM',$r**Ps=W4.%g_u6(Yp';)%M1SY^Y,%)h*+SAI2h&9###J94<-8%<^%f86(Ms-[Y#kN%##@AUVHu+/;6%W_6#H^Z6#]5T;-78m`%:$GOWbJd6#"
    "%$%GrW9JRhX5Z6#@n7Y-ZONF%T`-F%W.ippK2@`G4ci'6.+:W&:5GB?YaTHk7c#>@-H%a+6Hb2(h;<xB.nHwBgEw][qpj7%KWLD3vu.&4%r.[#s*Rp7T/G)4;>dX$jWD.3TQp8.ZxalA"
    "w+ET%FZ58/?T4',k$0M1]Od_+kfQV%e%/9&o/>9%R)VK(,Kep0ca^I;+AHa3FXFB,4gPS/q2I51XEuJ(-AS9&ExP?&(1F3'+#.i1DI@>#to+##)VmrH`<AJ1515##Cu)Y$_B@rgI@[kC"
    ";TY8/ss.a$BNg@#2J$t-'Z/xt(4Hv$VXDe$[GbE$uUBp7,-=r/$(,>Yb?KJ(/RSY>lg662@Jtrfexl12l'*)#([b6#?;w&$@t/2'nMZ#8*e#tL^%buLQ1oo@RMu2(q9Qaj2^WG<@eU#$"
    "t3&c%dM$c%m#@D*YaV#$Tw[b%luED*A39$.Lp`uL_1oo@og[caKtGb%a;_b%gJif(P0&6'd'VV-R9I21F3o;->Ww1%#_,x'vn.F%IK^d+%uP42H&U`<;$=<-32v<-wf+U@ajE/2mGF/2"
    "M,R^=%mv$$wCx6#H),##&l.j0s+mU99CpV.2^t;-6P%:.3tqFrr$j;-ihKF@C&Ne-75V6CxxYoL&7252gCj30P/NG)Q(P)4ESLe$c.giB%R/'HSq9a#ae_F**YwM0I>WD#'YYfC:5H1:"
    "MZm?$vd6<&bQTa<oiGP9Q3lF++jKE+2pw-)SP3.)bnR:Rv1FW6aC=d2:s.5'?-96&hi20(fEiX6OI$tJ;2;t.eu&5'ci[1(N9K6&$,>>#nUPi9ZPB`jruHJ(>b($#5.`$#a%_._Ow60u"
    "6=<:.d.<9/hlwU/,INh#_Y1E4&,]]4<Ze>%&@2<%?1[s$5BFQ'`qh[,cr7T@x@qB#2_6Y6-#@r%4iofL;4I1*505o/G4=S&krvV%#1.x,(+$g)nkCW-QgvG&'V7R9^a/p&Ai1v#]6Nv#"
    "K^5R&C=I^9>,E8&`XjP&*9K;.#Jt>-W_W5&HeLN(77fiLp-S8%?HcPB;6,pArd0?6UDv6#8NYO-RSjb0YF[1($LuofgB=gLOjfqfMxY5'gVEs.DhvxL;8Uv-6Nhv6kS@l1i5Em.ae_F*"
    "_4Xj.XXR@#CEW'1elU,+JQhG)q5;o&b]AT/:oCW-`R6T&2&$kk2%mG*T]7+Mob-R&@.`&9B8W8&v0%B2jkU<-x,,7/t6A<$I*N>Z?]Cm'idm5.J9[iLRb;H.igYn&ZQSBfO),##v%gE#"
    "Z_T:#X]@Q-RY@Q-Hu<N.Ilec)R?+<-P>o3%V_k[$EXS)</s>g)+)D_&a%dY#HUp,MV%#=&2(Qp[jRFGMg%/9&#j^N&vVjxBajjMa$bIq$6stNFDXrP'C=nwBAK]58Yf__&$f9j:gdNh#"
    "cdVwB)&O#$M]DqLvp-##O:/X#Psk.#(wp3_'j_3_qPk-$N@s'MS1;*%1&[w'8;[w'*gZw'd*t'M)vSk9wasJ2nBI5`n8^e>%6YD44QCW-$7n62$7YN1a@uv,Laf=-D=`Z#)GAq/-3:6&"
    "3$f],^v'hleL5k:=WW`EScmi'tV=$M9vd##b7?f_2cuu#EFe@Bm3YD#,CS@##k@B$;MDI$gmA@6](.j),$lB6OHB'5GM@JLp.`c2r;-aZV:JW?+.oP'4orM9QftlVquV0(sasuHc$iP0"
    ".`*0:JpM+Pm1sk'&/%N9%E8N0x]DD3=j$5JLaRB5Q:W#ANX180nG-_#8UsP&x@+M(8(,2(80EH*A]@tHK_8]?DG-C?$>&p&A_R@#UJ71(+aGS/*/n_=<tVEe-b$JqPF?AFLg(,)GH.%#"
    ",6G>>/q&K2n@:gC(0k#-J^D.3T6cg$j0KU%H9M($2^Pg10h]c20#AT&wfFU$CGc;$qqx]$Buh*%%xoE@MA8D*lF[xb$`,P]YO=42XDonL;eRajLme%#([b6#*oq-$M4n;-.fG<-2rG<-"
    "0fG<-44`T.:UY6#Z9pg13jm6#m^''#7r+@5(_6WA;J>RC4j=%&NdXb+B0`$#]Z<T@7v)@]7x+s6`k,[$]o>>#)q;Q0(iou5I=T;.k$cP9$>-<-m*ed$ej/88XXHdFi^B7&KLTfLMY)0."
    "21N,+&*m<Im-ZK)%ZmR&=O$+>vnFm0X=+b32lh;$=kIT.%d6t-_x?w7KI.(#':f1vsxv(#E-4&#oX)##60c20JZI%#AhFJ(HB4F571-of=DFl9bFe;:^;d6##,0_+0t]s$IXj;-4VX3."
    "c?:;I]m=&>0Hl[M'U*,#pFMk#S)V$#UF^99gk(9/d#60`xa@2&U*F0_RhD'Sxs+F%C8QF%OP-F%gM^G3sxi6/CTjP24JrU8^b(*4DDXI)I:>JCx`3dXk[Zx6[iWv%Ws]'/1s6T%=-Tv-"
    ";GEF3tcMD3a8GQ%pr`O'p(&5'=bP2Bjx&'+eC#(+[3(O1fYXjL0H/m(q7d200chV$m/$R&a+O.);`L;$m'90(S#4FN3(GM(LX7s$$.s;$,);6&w17W$7<Fh:Y441;S*0Q&,02u7ZF6T."
    "j^?)*[Uu_+T%5/&R,Ce;#f?>QZ.Ms-k8#GEF/[ddjb^iB5H<X?,x-(#^#mx4Zp4MTFxQP&l#Yc2JMc;-BrvT%8ZnD_GlC0_[@49#g>(39Q(4m'q`kJ2P?^k&QHvF#d@C3,0[r%,1k7A,"
    "EeNj0>h7A,/b%&,Zb_[,[0CC,@,>>#wOi4Jo+*8@]4pu,OKi0>[0l6#0AP##vH4uf6W[*Nqb/[#CY@C#?D,c4MSoGPBN<$6[TBOT&G/p&6x_Z#X2?7&>?t)<P^[#6$nY/(0]Cv#?F#2("
    "'m:6&kS74;*MV'#]nl+#l.VV-x9`8&3B*/`+x'^#`kn;-c@;=-vb1I&08Ya.%v6*&'DI<-VZlO-eU*B-Gp8_%eKbK_Se?>#hHho7kwas-k>LI;JdQH+W1CkL.P.#%icMD3-6Jf3dI]7&"
    "9)tI3;4K)<$=s20^f&m$?dfE+xN+'6>452(KO)Z#[7g'+RRoQ0(/+M(7YC;$I)'s]3ADI$nFf.2mP<E07s0cNivD+lK2w)*gl9H4as=T'Xe.<$D'B2'4aB$-4]1R'vuB`jt<JE5)/mr8"
    "Z8lv6YArL1V%vr-#vKe*#$d;/%/5##TI+m#bb+6#hJ6(#5k$)3>s?D3;J[A,;a0<-pi1K*cMd6#$$%Gr1&,>>ViL_&IxDs-4#t)<2m__&UooD_f<mvI3kpnJ^eis%wCx6#o>MX%Ex`3r"
    "UcX0&f>_e;.-s209jE.3:>`:%jpB:%oY)w$A`E<%[=8C#OM>c4PdMoel'X]-nf^*+HS6F%<0T[u$V@6:DF@2CU/1I$a52t8l$^,2&]0f)ZwRo[l`T*+lt<A-6@8^5hK1kL:e489'V#F-"
    "PTaZ5].g>.]o#2CC####^uv(#f`Gj#XM7%#Fx3T<:3W;79a@^$BUgD<53N;7%<c,*e$`$0Lk`H__U+wpaD>_/E1Tq)1mb&d2(00%[Kkp.?3pL_M,tEcJSX/MRf9gM'k5I<sApV.ZxpDN"
    "x:#gLR]-lLbqZ12=P>RAG]p;.q8c`<R)WqK5]2Y$V<X`<cQZ*3emXT%S/`K(u#S#6U&vO04j-F<6YjJ2=x3g(_6>6&sQ#R&8,;l0q-ID.U&r:%f(sS&0Or*%m>+;-.gprHP3>`W<Ad5/"
    "dk.j01dEd;6k%9&r>MJ(]'o;-9rG<-freQ-U:#-Mju'kLg1Z98C=`D+R'[8@Hva=(lDOe-qH7/=YX61MgBJFOfX=9@-(W[$t1]6#qZc6#ssqFrs5UhL([FIDkJXV/1BGA#'*lk0):X%$"
    "D-;o0q>Tv-&wA%6Cd0YAC*[#6llE.3)8@)&$[sS%T'xs$VKtT%&b/p%&V`k#pj+01E=VZ#,%uA#_#5R*,E?4oPsQD*@*([-Oa;4+Jmf5'Z3n0#1sao7x[bA>88$adoVG&#aPZ6#Fq$:&"
    "6jV'6tAE8A6iL_&BB(kb<?_3_'iG<-CfG<-GrG<-VfG<-e:#-M;%[lLeHs.N;m0o%@5GB?4qA02wp/20bO;<-Y1v<-aFv<-j1v<-qFv<-o1v<-^oFq@?ZvZ$wCx6#u),##6TYO-)T5,%"
    "se.F%`TCwK-i^-uY5Z6#%+0k0Hvjh23mqB#On?g)uDFp7k1&NM.?GA#-'6B,WPId)os#*&JL/p%Jkj5&*^#2T91VZ#/F?v$[$sq7u0sw$sucj#Wi;3'S-JA&?mVuLQ4U0Mru41<WEIhL"
    "k-jv#N8X$BT/5##`Pn8#dAw0#Kqn%#^F33&0]qd4?pIiL-BVhL%`..2.B@g:C'0auCXcs6B_95&h.BP85e7e*En%v#IC@b2_p4f*#/9c3MHXI)x=OI)_Lj9/oLEt.YPe]4Pq1Kadq[<."
    "cb4n0D####Nwmg#N[?g#7g1$#8D&6_#^_3_mDk-$tkD'S+,h;-Qc=?-2qchL8ecgLdPs$#xuKk)j+?20g8l^n5dLD3*%rH',5ZqJ5Z(>c;RNP&MDW]+G+H$MY@6##[%$`%(7$OE4x#of"
    "M4p&#*]qr$H1:/`j`c'&<)v'&3^c'&0Tc'&$+'u.tp41:c[Ps.&um`OZkJcV6RHG*xAYA#j-:n#wIACM?Ba@,)e;,*6R6,*=tRQ2FUDC&:?4-vS54l:Q6+Da<d52'YdE]F47x6#G/5##"
    "Avb;-UpN^%>*g'&,vW-Z-TG0_W5nD_#P#<-LM#<-N361%jtbP9-a4&>I@ev?W&Y6#_X?']/XD.3x?7f3aGUv-=M`=.kmqc)n&PA#*v#J*=<7.M)56N'H,]j%Lljj1g_(5%..sE4;E6C#"
    "/W8f3'[U&$%9#t-Rm,+<lD3j1]Vo]%0nPV%%Sf1(-N(:%0Dl?,u=tI))eb,2OHkN34JXi(]>)8&F9=:.)I&N0/fhV$./[r%i6rD+9rlm'37YgLI?/A,%/W4''N_V%=E3e)=cbs6*Ngr."
    "Of;P0u<_V&3eQg)/2oh(e,qm&c%`O38`[s$T+1W-do7p&BNL,)d*SW$A@`V$f'S8%.muN';8E1;NEo>-L5-T/>:es$j&L:%aL[O(S=)$#D>eS#FD?_#FsC$#18N+&U0l6#VnEs-HAf`*"
    "nVfZ5-q6G*04-G*xLG;-(f'J)1u5c*7####Ch%nL0;1M#>h1$#jBYK(EeX9.Hj=:&=pk:&4kpF&//5##SQ]X#(rq7#m%*)#ktf2&.HAdbOP3AHAMDH*(k`H_);r?[*oAK%<_De-D2PWo"
    "_vQ12,)[`3^7;<-_q3R/:g*s6F-QM'Nm0<-6Mql%s=QtTWjHl$R6x;-.IQA+dc``3EG>k%_uWb<@shP0BEc20&FTL;(Pdb+>v*$SuQ/s$&rXi(l2B1,qJ:c+VDdxu/fMY$n.Lf#VsI-#"
    ")GY##)4o*%$PBDN#d%)*EmO`<:ZJ$M86Np$>Ncf1kC#29a^93_XID0_#SZ&Mak)*%6sC_&]kt9)a@`)La<mGGRct0&YSd20$>05`IF?>#*01I?nt8W(fTNmL<[<1D[_W0=:xlX&Ci.7C"
    ">pUx7750CQ`G0i^u9O?-5;l`%V-Z&H?V[#8-Qxp&1I?M9H/K$M'vH[$)IRS.a>iu5di2q.fTY6#b90Q3JC5p-m]Bp7gwK.;PgT/`?(?>#c,:h$T$T,?[nHlLbr,t7FXDr9p8&lB4(jAd"
    "7O)0CO:c`OuBk?-_fJ/&o3'[-qt<vKr5K%#LUG+#=%U`<],q%=ndVG<<Jdp.<UY6#[oq-$_:*<-sRx#%.B,F%X0v3+e/v20p9E)#fDlR(v3o92p_N;72^WG<n?Lm9P]=G;QSv2VE),8("
    "<Zp4&+*KM0Zm7j%d]@DegZWmLm;,&#VNv<CuXDM07bh0>kwK.;:.xP'jR>O%t[-2&KvK/)WH=W-43vLsj@U%6D,jhL:[o8%mAic)xU,@,mOO^'Dkl4KlVZ4(G9Ax,94GM(6AS(-+_+v#"
    "A,$u@7:J9A70e-36S=B,Z96I6$Nu;-dg?<%ET5o/++tH?PpXCjtK@5;rFB;0OQq80EI&?AE<TW7@$]k9muA9r@8^CsLR+/L/an+DvWs)#0f1$#aPZ6#LTY6#kCa6#dM#<-[TS_%cPkcQ"
    "Hj./&'pls-AR,lLM%[lLJmm6#n+3)#`r+@5X@R1(ZmBB>l42(v^B/[#rqMp7N,hI;NU3R2VGBs7l1*$o.m)7:]Tg-%i09>.$j#33dx#:AEXrA5bYMH%],$w-GQ+',3aGS/?8;6&'8c5;"
    "#E0$-w`mN':VuY#:rf2'un0U@R8`Q9876:7eBO=A&G/p&Ae[@#`*[=-lb[,*[Xf34.&/9&8'*-*<mo,4aPnH)$####7@%%#WUl>#2f1$#U+sqg>$NJ:/f,K)TwtYH^mv$$Rlk7.n%$Ak"
    "w[@6&.:h)Nj9CloxpVB#TVr:6S=s=cc#s^#t=;SIYg-2BNg#H*6ReLGllec)Gc-<-owCH-hL`T.U)0Zu0`#`-M[YF%5C=6&H.FdVS6SBfN/5##w*j<#70trLl*^%#s:'&+?j0j(uV[Y%"
    "gJC@e1/M-29&3B=cQCW-'BqB#UA%&4qJ))3xmk7,2k@A,9^gr.;UEp%uSCH2YE-r^<=s(&4$BP&xIvHH<)'58dvY%XPM62'mp&-3B$M$#E?q+ZvqJfL,UucM%c[`Eqw9WJEXPs6GBMJ("
    "IC@b2W$VhLVh*P(2FoHH+00>GpB>tR(]fd);U3T%j*;[,d)GW1]&?6&*bT+&qQ./:#6qTVUjRiL)Z-##L,>>#(.[0#%Pg3EA-D)+M/A2_;DkX&:^.iL]c#98>M)c<C'x[@:+,s6ChTP&"
    "6i.T.md^3(=Zwm/'[b6#[Ca6#<?f6%-?,F%m#O^2)/Us-9_Cs?t/N*es1mJCpN[F4KjE.3jpB:%TTOT%BZj$6Z5n,M:8CS%^cwI2CR]@#$<+?#1CsZ@V'ms.@1&^-,tq2s%@GGMSpMxL"
    "cnr'#NuJ*#_pou5guao7W:b2=`Gv6#^@;=-IwAo%aOFx[>G-5&Z5Yb+X&)8Acj+p8q',p8;_vV%4_vV%TN=<-^1v<-(Pie$s[?>#2]T?$_b`Y#O#QM'P:L216ZgJ)qH1q8+.lr6IC@b2"
    "okh6;LD#d$We;gCDtNcsjQTN0dP^w6MdH)*;_qhLKgGx:Mxwa+%1vC+cTx@#%Kou%8*:6&&pUG+?TZZ$CQ:0(,GCjC#-Nl((5oh(Vo###%0GY>0)-Da(*qx4I=^E70`1`jN#x%#XYDX("
    "ol<s.s.bs.>m$)3kjea*([g<HQ^i^6qcY&#aPZ6#]8wh%RT.N%x*'u.#)Yv)ec``3baK.;O9H+<=I3$6,R(f)>#t/=wsv%(YaV?%RQPE&l_X:&2:A6&w_a2WeW$Ht.oSH)kU(@,E[m,*"
    ",<$7&rf:6&$1PvcrtjX8sa),WNpFd390REKfj0i:_5>)47Sa>-7Sa>-2&.a+#>[;o@2oiLs@)U8t;(0*i=9w%[k]T.$2+8_+eTE#t.w6#6),##jk.j0s2?4(:Ul##'[b6#:^^s$4T,F%"
    "?q,4iw`8pANj;N;m(,G4Qh%i)VPHH2&a,Q84>g,3=*$E&<UoGl9+09&04Z038/JfLi2=%viAa^#<d/*#%-sQWsX`i0v+DT.bCa6#>B-&'/NF&OlSY,%8k*LWDbI8B?*$Z$#L'a+`k.<-"
    "Zc..%W1VtoUU$lL(7YS*I#mP3<$(E#*^B.*tB:a#sWSQ8'f?#6s<6a$io6XKoAeL2C4A@#<c:Z#p#,o/#1_nDrgIE+juO9'HlcY#3.PJ(HIxtHg%/9&tN]`9AeS8%T>mv$%gPS/ui?QM"
    "Y<)c-_lE%$.85##%/5##G,>>##CL/#YvK'#KIRS.$+FT.fTY6#_G]3%tb?2&E/?p%)njl/9I/<-o&l?-O&l?-1M#<-3p]R.NDn;%WOJp.Yglk0ZSt*cCG:Z-^n4D#*T'i)QFf20r(Vv#"
    "f^]#c)l?0*Lx0xt<Fsl&IWQgL<X<N0]nln$r%8?5?W)UM1OFD#sg(Zurm]r0BM=%t]]=m0Dp*'%WxUV$gK=<-:7#4+:#gb4v>Tv-<J0YIqxlW.O,)S&,Ml>#)Gp;-0Ce208]JMB3oLZ#"
    "T3n0#>'mx4]n<]bAv52'W'QfCHu%7#Ws[fL+-0j0:0qQ;sI9/`dMc'&aO7DI&.gfL]5Z6#D4ju$]PC&5S3m<-gr:T.K^Z6#iq.>-,Z:a%nNiE[Y49x.',v4M4l3JOX.L0M1M`'#&>05`"
    "P@Uu&A&]iL0l7QMZa4nKEWZJ;=&-0)'h`H_cE%RE-F%RE;q%RE3kF_&boAL2mHRF4=4/+%M]WF3lJ3<?9Ac)4.R(f)>]DT.XC[x6MS-=.xH:a#`eMgt5h9a#L^Z[$)Fh&4#0Tv-6&Ad)"
    "=xKP8FBa)+dqKv-.XIG>b0U?5$)X/2mor.2qm5n&j)_U%hec(+*).M1cXCN(pxKc$5v>X7k99U%8n+J.FsKv-&KuA#OtIJ5C0*39In*H;B2OK3DX(j(AW/e*$)8-)`rFZ,runL(Q>8#,"
    ";BbB,c,I51&4IX-m8:6&R4[P1Q-1,)q9a$#L:dl/&tVl]N?O]=p5_6#`^Z6#)D*j0enj6#)$%Grx?cofBmb2(2%dfXY;d6#]RM=-[4)=-b)Qt$p8q)<XM&FGkAZ6#$2+8_nq_D(H`ms-"
    "EkPlLoJA(#aPZ6#.[c6#?tqFriK@K)]::8.[6r`$4R7lLNqkj1*SYmBxjNj1q:Rv$.g_eD_7@fPw(jc)nY]H*9RbT%$24E*$xcG*;jAc$JdPS/YmD;&::XW8Rk4J2F^'<7`MQ<.3lLv#"
    "YEt$:A^42C'E+U&S;hV.gW,R&Ji6n9+/s20C<K7/gd;22eG:6&wu:6&DPOW6'GZb+'?DW/WccF+/]:v#&iF`4['XKluHZ+rd/X]Fe&q%4515##mG$i$JUP)',2Lc*h#(X-PX2o(EQ+)<"
    "3WF&#)=-C#GqBD*Usu:d_:pu,=h8,MJRwa$)A49#<rGc%XU>L2/=Js$j/q5/=29f3?ql&>WG_s-UES@#QUaP$:eJ2'KSxu,h(mXl;=@w#/Jc>#8e:,^oT$.MfZGi$,+KAXKVZg)@p;-&"
    "65N34^uv(#x_;W#RT@%##BO:_9J`3_8+vwn5$@a$/5XsphPd/($qMbjVM/'F++4m'1-4m'vfF/2`meM1O1]A,D=qA7O+:kLRJ=jL/P'NBUaAFc>^.iLls[.MVAVhLZI.&4x9%J3-_pi'"
    "SiWI)@IY;)U+wo%FJ_c`._`W%n$3W$slJ(Mo.]O$2ux(N4pg]&B2I^4l0oP&m(CF$4eGgDeW/w#$,HhL?XZY#$E3GD?ro9rT*I;rQ(h'`5(rr$]oRh((3Xb$u?[5'/HEv$@Bt;-W=WU%"
    ">Fho7daBj_t#BJ1Owx34oo$<-bpX?-Fa,t$i)IpIs()d*<ZhE+.+'u.v;Tv-;0d4*22.&41Y7%-:u?:Ro8mf(rW')NJ9jv#s9@DjUxY-2`Un^Nd4&X%D#.W$l_;N0hBa#Peb?>#o#3GD"
    "G(<]bTYW]+4k:p.We%j07*Q9ilIG(H4>,*8FMe5L-rJfLm[WmLA_IIMS)$HN04=s'(n@vn5.g)*rncil>0:j0@AUVHw_RlJ83g028p5g:lAeM1L[Bj0xcd6#+$%Grt'>of34[58X<sqg"
    "P'102u^NP/+Hcg:]lK/)PJL/)0]`5/jZc6#]lRfLS/)h*mqk#>k[Gmqm)ug$9)TF4k7soIc(&dWf4R^=HYaJ2wT1K1iR#(+eaUS0Z-96&fNkW8>pVFr#No#5Hq<9%rKT>.%/5##);/X#"
    "hsk.#Lww%#?m7i$3[;3t$/j^oRe@3k*NC_&Ut%<-.Om=M0KrM%_2^'/QjV[ET[gJ)_(iJ)1Is;-Dx(t-^n_l8-tnP'E,13(r2=1)eC];q>mneM2v?A4m2hh)q*2k%Z@qo.]3aZ#,M+gL"
    "fN&W$SVP>#++iv#H(@T%rtSkN$rJfLQ[-##g6u6#M(5>#S4Rh_3U?Z$LX+e<%Ru`4DYd;-[s.B.EL@D&=UeAcrh=:&o23U-5W$x-ir,tLEtJ%#.qpl&&>NE<1gk&#$/]6#KTf7(a&_Q/"
    "WZc6#(tqFrDQxjBv5;8C#%l.)pHj;-MA9c$B[Yq^N=Q:vu<6kOJx_]FWXt1B/X)<%#pXxF&o9^#TdSe$<G_]F[Clr-*.5b-M#g(%HJa41DD/PiX2(*<`89Y]Gr`FV(h-#v<KY)Mi_3s#"
    "'&*M-$v)M-#[`=-X8Af.G_R%#[`-Q%Ub`Y#M%ET.70xr$o.NGY6lo[+[_rm'''d6#4_XTq/qh]RGbZf=x_hx9MWfS8xeT*,EZ8$MFjQuu#R@%#;/[0#[EX&#sTwa$oVc_d=X$29Tr:;-"
    "RJ[>-O3'0:S16Z$Y>k0,gZc6#jk01jqiZI%i>$T..QB%6oP25'k?nH*E6OaE]rP*RAUg)3D%8D,#TZZ$BK10(sYuA#t*-68ac?C#kN%##HYUVHYk9MBhLY(F'>0XU`.Gv>B9g*[+=Gi="
    "'&VlMAV@C#+TVU)c49d2N^?d)Oq>YYvT:v%f#.W$SW8WAjsJW$Vi@^4Z*w,$:.Dv#og%5%K#P(9U<h^u1juuY7%`;$SaCv#f:X_$=CLo*g@)=@=+L^#C))$#GCX,20>2O#^#x%#Xh`Y#"
    "<+vu#VB(K1dCa6#m1_._pA_3_e,k-$%c<O:09d;%5Nck-V'&9(),MT/1v4c49jE.3/;;k#fT7P%Pj#Z$:.V;$F=oUdh;;>51cLv#ZrYe$NK>i$-4g]X2iU;$H<<P]tX_2^+5###Ke60#"
    "E.@S#wBP##j2.^$MJ:;$7$F$M+xSfLn[0j0x(.<.%G3]-n-fBuY`:$[V]4t_ArrM0uxKf_wo:2'OdrmL:_XjLR)102L:VuPC,fB#Om*)*F^R##<nP<-D@pV-t.o3+FR(4+IWcofca$#Q"
    "gFXw0N/qw0p-$PfhW$x0Z.*SVHk<^ML#5$M[3&Gr*uQ2(0P$<-LcPW-1;&:)-&mM9-g?7;i,e12.;<A4FF@<-m1v<-?F:f/:5GB?Piwp_**1T9<Q@&,E/k$K9fou5cx?#6<nTx*=Yv=-"
    "Elc)%s/$JE+O?_#Pj=:&/@Lj(-r=M(>dKj(Hn1hL=Qop.H3>j'.<=^,<f'f)Tfje).p=Z6R#6t$P:YM(8tdg)*FA6&ta_hL+RD[,FJRGXlI+q&br/b*$VK+*<8V11-;$##^*;YPLL#/C"
    "#`@3DMXHM9e$1B#_/pI&vX0^#n:@T%sAw4vQs=u#PWt&#,%Y6#1j@>%>wn;-QJcveuY+iC>$wU7jkBN_*>>>#7AMJ($=Cp$Zo7g1u,AS'dL0+*KG>c4Eq,,Mjxt_5C&9KW3s8U':]d8/"
    ".Vg/)uMc;$&MLA?_Cgg<Qd0^#H0TQ&D2PN)h(3U8M-DTT-#dt-h51mCD,`ZuB0W8&(2a%$D&p5/Bsu]uDC':#2tll$IciO#^xK'#7+gi0`iO217qN/'$./L*C%f3'Q4Dm8xO5s.-P=s."
    "_FE?-90D,3XUxB->709&I?#R&IM$<.E2)M:$P(<-4Z>6&nBT6&7,;l00MZY#^L^CseE,/L1n7G;hs^6#_?EU-m5T;->VjU%c>=3tsRpD_N:&O_1YYY#JBRS.4$8p&D%r;-m&l?-sj[aE"
    "Z%cP9[LkP9T'(9Im8?0`6c>>#dQ<;$MtDM9pD6t-)mRfLquTb*4=Qa**)t8@80dG*;14n/AP/v5AB:a#@JNQhT:_0M_b6lLAMm]#oD?W(F0s[,8+52(8'*-*FWFB,(W]j<qNlr/UFEp."
    "WcNe)BIGR*n-#N'(q7a+x/I51]+&c,Rt$#SC.lh<;2EKrI68C+W.Mv#>EY>>W?CW-vI^6&9/Dl0sc84;RB'[6G####kUci9tIE`W]`B5/Co%=(bJqCa-'^,2Qb%/1,oH8%WwJJ(9;pCj"
    "?LWp.74pL_Lq,61HHL8MU2aMOa`E:MhIYOOdra:Mk[uOOntH6%-xNa<05R>65FS>6*GDp.$2+8_J0,=(VRlY/qV5&#J^''#aPZ6#:xpd%,HnD_$q</4'@,gL6?VhL?E`hLDVOjLc<a.N"
    "qSY0N<g>oL7E`hL9G;pL:T:vfe>``3C-;o0VBs?#giWI)&9G=7oO<]'F:p6*6JWX(V)>)4+cv`4)`_Y(q%ZD4@WLF.g]@[@fl@d)U?d*QJ@q[/L7[P&u#ur-,R2<%6ET5&sH5-2uF<5&"
    ")37w%`dxo%)@ff1siB,3XZ*N1<xPLMQ-?>%UKXP&jC%q.Le9)32)C@0_*1@0;RN*4MNFVH/(wo%(8afL;)KvPYt?K#3k<n/L+$p.>9hJ%,S,<-K_g`OF6]Y-@(KR*,$UYZH*;^#09m-6"
    "657`&>L-L,c*.jB.Q;h)P1]o@1_&020SWjLYl>T8&u(9/^L##,JnL5/maTP&]>*O8)-tH=0Ys/%<I5F#h%4a*NL*<-*U.EE-k#cE#s*^,T7i5)LN*i<V4?u/dp8lBQqJGN)$l20mHI5;"
    "gH)j*FT?\?.IuN//h+C5/6?<*H5G`'#7j^7`&xT^#>7;;$hC+j$M#9*HM>+jLUg;kBX=Ma4''d6#'F_9&][+=(j;aZ%8OIoB2ZaJ2)OAb%NK]j)<.O<B4Wgb%w]j.&&Hrc)Eime+j&Yp7"
    "#6h-O2fFv%cWWkB$$s22po7:&WFfP#RN7%#W#S-#^&;;-UO]V%/Qc'&E6=;$OBVn*^E$)*3242_B4nX$,k'L3-1m42KAQ]=cR;<-52v<-r&n?>?bJQCf7G4%_Sd<-[#bZ%)+qqhe>Z/("
    "TgPr@igA,3j'(m998%a+2AU>?`tM7)(n6l1rCn;%GhOvKaW^;%kh@8%kpC[.h6t(&('PA#cAUE%&:6Y),W*M(oD^O9k4W&b%'+ZuTqMM0+^^q'[Xjl0h)u=$$9b:.f:W#A?*4H*Qn]2'"
    "C1xQ01w/q796MIN87PM(8D@<$u&(-2o:*D#Pwji*+VWT%qpLK(p.b**E1A:;`p[-)]c&b*r9k]-^+Dul>,QY>9'cf(R;v6#(3g+%Ca_6/ETjP2195CFd0,H3m&l]#0YNj)BhBH)Q,;L1"
    "c`rO'hgeL&K#Te*2'bb*K4DZ#<xOfCrExp&xMu(36NJ$MQR6##`k.j0$N$iLXjkjLl8N<3RYS(v9td5(RqJ<-H28Y%JMd'&Pg^04;091*SL3R8EX4U(u_U2)H####qwjrLV)Qm#iXt&#"
    "fCAZ$jKu_-(304&F&;Q/$Da6##vjh293>T.r37+4/L)NVWirr?Xm+u[d97L&S:[T%s6>B$F#<&+u[RH%H*Xn/gwmp.5.B]X?h.Y-7-Vk+V_S5&dI@6&%/5##(>WD#c-[0#juv(#RBY0&"
    "s0Mb_HIZY#&R)##Xa.u-Z20AFK6XM_nefq)2teXJFw&x:LFLlMIikjLEbPE=,X9dkf)[12-2w%4$QGc%4-4-v%7rr-R?3L#/_93_^i0X:C&O&`RGw%+PJRh(J/w%4[to/1MJUv-[5MG)"
    "RJ))3hukL<-IYQ'6$r@%jx[]4GF;8.&3coL)m?'7K]B.*pQYS@nr@1(+AJp&/[UB+lBWU.UBdH345Kd#jpW^,j_`;$N#?;%?dfE+ee&DaTv,R&+_r%,at/n&pr/f)O[R5/:7R],vtDZ#"
    "EUj5&FFhT7[/5##%ddC#4gbNFNK$6/ocf],XZ]],[0xa5vBSE5YT,p%XAPB,F;J60OKgv70'Pu73%DP8&=_B#L*V$#xP?(#dgFb$*uLkL1`8Y%&kc]AUU$lLh.Nej4^@k9&whv@mUH##"
    "'[b6#EWts-;;kXIMn%.*t?(E#nGUv-G0'u$;-rhL?h9a#lEn'(8J65/8(,2(?rUv#Rd@W$NuC8%3H,o/0kTT9b^aD#Y]J2'8*3-*<_QV%uEEW%h]kY#6C3p%9-3-*4HKN9K1=2CaBfs-"
    "R1RfLTh6##5@bA#.5l6#u7E)#<@#S8CZ`p/*gjl/8.wQ%2s$@'Aw9ZfRYFo$`tt9)e#b]/O7Rt$.&Zj97EEUK;:pu%FtZx%5AW]4/P-<-)H?e%=+Es-d0xfL)gpX9))dH3hCh8.lwkj1"
    "hg6r@HsnA,DP3*GW=GZIeN141ETZV%j?+9%/<1d;de',H.cGK3pHuJ(FTn_slh9DN]#%h1$),##1I'^#a@L/#MImN</1T;.I=R<-^)FCRw[K@%$rr9(LpJp7))oP'l^aGM-x=2(I)&jK"
    "#gpU@Umpk1Bc``35IL,3`&)8&v[u/Ch8t$30d<Q1A+pF#0X.L&Ch/6&qg0kb./f9&sZ#Z.LM/?&G&M6BmgeY#w)wu,Y)1-O?td29_f;r7$P7+43=[x6QFVC,M^5N'Fp?d)L<<r7R%<r7"
    "Cr;r7[q0^#^j.K<SWiR[tujZ%x@'DN]HF/Mf;$##g<6eMAOP,2nYPwB0J1gL'h+G4(/[78.u5Q8ae_F*Q%dY#diCg*7o_;$$8=E*$o5g)OPsa*Wlxp&'kx=-YPdZ$BHuj'tiL^#n]&p&"
    "Al1v##Cvv,[QL*#Sb`i07LReaSR32'h<*20NQ[i9m6'7#_97I-4:7I-<6Dt-cwTHM+F5gLRPbJM,>>6Bgm5g)JU0K%SO)##$2Zp7U3p,3''d6#rSl@&ah+=(bsSq1V5+/M'V3,<fqMF5"
    "V#^e*tF%6/Z`''#V4FGM,Y%iLb6wlL]5Z6#^58D$RLrg*$p(<-JRi-'5D*A'X^^<-IE2E'RT<M,JXho*s[gF*>*5@l.%:B#f-pQ0PApV-+O6Kk/dXVN-4Q'(tTV0Ct7]1(.qfC&]fHE/"
    "buu2C.d%T]1Y(?#CV_s$8$V7*A84$gvS*fqg8P]=5/=A%PSd>>XD;s%>7>h%P8hW-(r]^/Vc9C%aHI_8Sc($#r*cXTeS+V&*clf)..CJ)=G8;62=UJ)q##&#Uh`i0#ufxXtWXm/L%T*#"
    "2-oH#ldL]$RQfER<O'=(s-E<-aF;=-R-0o%2[f'&L@0:)GDd'&+Ff'&ncec))xOV6oEie$&BC_&lhE_&#@ME>@`Qt$Oc[@[5#Z,%4p+E=''d6#tq%?&YR+=(S5w2;Hl)63g-w1M)n4GO"
    "%@<2Mq)PGO0>;5M0>;5MgGv6#W-]v&s)O$^e$EZ@bUkIFpmB#$L6C#-7#^8&xSp;-Yiu8.avjh28+*88JFT;.TgB68&Q[x0+(st-W&n68MdIP^>'gY-)),KWpa#J(=(^0%RgpY-PtFp^"
    "1]i'#hxaT#[Mb&#/<k#&c^TP&XuED*fac20und._lGZ0`m]C7)L8xiLRcKnfe(2NCO@7m0eExX$;-Ow&?nXD#>**Mg>^>6&OnY#Y4.Gb%-:8^kFePhLQAtFN_h$##Le60#[5[o#b5>##"
    "/GS2`kCWt(lgb;-^Lm)%+9,F%$249#U4?p/$####dcv6#gugK-T*Wg:sOV8&0E*Z$Kj8p&8w([-FcYV-<BF:.RK4L#Yk'>c$ESJ:$%&m&F#6_uU)_H2aE*/:./OY2:Z2:1&@bA#t4l6#"
    "wCW)#@NA)ZVxKdQPAkb$;(R29l=%a+Y<<a*8FsdDp)*eFI1E,Yk+54%aGQd;7l&g2Xp+<-_8G`%kv]dMZGs7).XH4ERha=.a35N'&AlMCan$iNP]DH2H-#N'[Ge-)egZp0B;`11ehUN("
    "_lZ/:0?[_ui2D?#RQ#R&It<9%s[&^-x*.p0cGk'%S5YY#^.+fqa9,/LqC'58nBt5/wk.j0s#:hLe+:kL]5Z6#=(918s9pV.<?cs-$a?iLo,EQ.Qq:;-nCRp.33pL_Z>h-O%]RmM0ecgL"
    "c`@(#'K4a*:.#-2#,m]#]q.[#Ps$d)wCXI)+(fs-Z4fIE*+?v$eAcW-V-e(%crV<%'/h*+D8uB,vS?7&gU]X%;/5',>/8L(P2-o/1ElX$U_VM0Lde^5QXPL%NKNJ:s^5v.a/^&,f('+*"
    "ahnS%MacN'WPBM)2K';.BWFB,p;(Y$AFV;$&rXi(xK;)#O8WD#O:I8#:8E)#kj_c2]pP]4d#60`(Gd'&/Qc'&ObxP/-$%GrnX]nfMBE/2YekLW@?MM%bCMs-+X#mD$4Z;%E0#_5w([`3"
    "IC@b2$w[9_TTv)4H_[1MgH`)ER;FQ';@i?#xsbg$kHuD#MaGx#QW[J&;(:S[JZ[^5t#P=Ajl)P0^eE5&lqJ2'V4xI2=L9U/sirg(@]C;$KVOI).POI)Y8Y;$'7LJ)C[Kr8vR#c*h-Fp%"
    "aI,(+B$*)+Mhkn0P,V6&M%Q>#]sIA,0e7A,)Laf1O#_B#<;At#UsC$#CdwqJ.R@&,=/(<-RAV.M8#SiLPN'cj=MBI;4^'^#/3L#G,0dG*Ktb6BE<Kq%%;W^4LchS%>kaj0=?xFi$5dj#"
    "$KBPS5U$(#FBkxuTw+>#Df[%#pRai*eZ5q.fCa6#K5$H577`7BBlL500JwR8dsG,*cG-<-@9]>-mPU@-3M#<-+6*2%%QO^2cqhb*St7<-x0%HbD;0(OGe9a#,HC8Ko`v7&4lLZ#eA$n&"
    "+nq;&9mYC+>=3p%Npc3'vaFU%k^%B5sZ>j'0]Cv#C8e7&LTZa3g#og)>+Dv#m^<u-6cat(PkwjkB$m7e#S3DWxx2>5:WG^5Ffou5XPq]5jWbs-g<4gLepchLY56djRRcV$8_CJ,bMm6#"
    "<####X]F'%FVrLNO1CkL;^.iL-i?lLNq'+*qcHJ8^H[qh8h9a#YCpGPd86N'j.2@Gp0*Z6LvVk+63=L#@mR#$Sn1a1qoDS&^daL#D=M?#:-F0lHV30(kL#$$e>gi#Eu<N.r<49%x9E)4"
    "3,Vh)?7%s$M<B6&];?V%S-dhLXouA#v3cu#Icf/.Pk8-8V8rXSn]p>G&3p&Z/bl,M]cm#>389v-NI9v-W[pV.P[pV.oCsR8na*1>U*um/ITY6#iha6#tU$.'J;f,=Lu'kLKH&qf/*+78"
    "m)kd*MrCv#s=K58<rT3(at%w#?kA2'mh.?7HIB;6e^.0)<u1v#(DI60/7u:&i>a&5#9am1&=:/)>w$(#$),##nD[8#MWD4#vbuc*2H1<-04'L(3geJsj_.()pJ=IM5$vA#I)wg#bf[%#"
    "pDA^$U$o@@#47g-x]tp7v32N;A`I<(<_7q7xT9tUM),##wTN1#+CCpL7w>DOm6[f(V.7S8W9,N`Pk2u0s]*9#j`$0#R%I-`#f9^#WBHekN^an8/xQpgqgf+%,)Hf_F>HY>M<cjBq#c*?"
    "T(_1&%<F&O=/D-2F9T*I5sx]?97xM0qsqFr]Gmlf#^H.M[s?L271<:.[GUv-i)9_IJrsu5mAT@#jd_,)5Q+',+]X)3k-V`+wZwp.asOB&$Z58/FV=fCb1#-3lGU:%^kumsl,sx+gwK`N"
    "DF/_JhLGS7U-7ci;4TkL/0'j0WSk&#,$5F%$>[c2``hA5NX.F%E0&/:+pR5'i#6T.t]Z6#hT`IP]c^._%iw<(;Qk-$@LnE5`Z,=(jinZpM+Gj`4OUA(KSGp7fcvf;F5xf;XCM>?/2L>?"
    "E7q/`b9@>#np<;$TS'N1OjCnW`fgHQR41F_KwoaPB1Xb3B4vr-C(Am/eXlI)]]WI)-QD]$BY@C#Nlf$7#x'f)C$2C>vakL:S?<Z6CH:a#=C0T%eWCO'xI:<-li;s%R&*&+F0Xt$F[7s$"
    "[6TQ&>ovD,*i]:9B1eOM3?ND+m[]?$-G?F+6gZp.--?h(#;jg(PW:0(@o(Z#]9*Z#mUSx$oA:6&klnA,lkgk;SE9I$3T*Q1k`p<7>jPb%6ZTw6I####&.JP#eTYb#-n:$#1^v4_`h[n9"
    "*6`D+T'+<-)Qde%]3xr$+nrR8DMt2(d#60`]?Hh%HXQ2,.:`d*XHB9.pUx21u5.Q'wUfQ0F,>>#Hc]CsqU[xbTk*JUQU7<->4+,%[J%O_(eBwK>=LS7;?AT&9,gi0*jS2BU8=2_2QkL:"
    "f*AT&gBRS.Y2tS&iiGd36([`3srwV.[Tt;-`Zgo%CZk8(U>l;-%a2n:l:dH.4&QM'Apuc%1aQv$$`^F*x]:%GXGavSb]]+4]u;%G[jMj9vl-L2H&OQ1OM`'-i2=-*Vk2v#Gghg(0Oiw,"
    "dBN91T19%>8IM&5:=n5//iqV$/vuA#jb4#-ks_g(pDbV7jbI],IBC,):o8g1av&M))ZPo/7Rwn(@k:HMOdAx,%5YY#.x8D*gK,/LQQaxF8w@t*ph.<-`)+k%86,[GOFkM_.PYY#ig9H*"
    "wjuu,APq;-m?cc%[`c;-6F`.MTb:m;+WpfD<nPGE8nPGE,t]p.24pL_Leb@.v$-&%@##^?LN@9(FH.%#/AAF-RY5<-iu19.3nQd3G*Bp.jUKF*X3%p_ow1s-I6u%no1WO'E5CI;^uq[G"
    "h(^,3A.ikL97$c*YqoT.x?7f3prbP8Tu;Q/M7%s$ikkj1o@@PJAZMJ<gZ&r.x1AZ5;w_,)v8Q/*D@fJ;;R#R&:;i)+;e>31%(5g15IaP&HIIW$C^s-*Q-V-Mxvr&5smG3'.](p.&9;W/"
    "mXaC=2ou>>s)ZPTT,:7/Y]vo/S=RW$9p(B#knBI*1Df1(Z#h-+vB]w#(#250v/7p02kex,LTqc)N/`0(d_c$,JK9V.'vIh(%.t=.;xx1(T(5_+qs*?6FYr[GCs>g)s3n0#-(x+27Z0MT"
    "UT?A+7ICP8VN#N0gk.j0Aa+/(YX8b*DR/q.UTY6#&@O+<Nb>W.B5SY,*pSa*>US>-=g<;&4DfZ(;YXI)0c0+*GH]kVVnAa)W0]Y-9c^E[Wg?S@3^+J_k(lo.'tWmV*M1`j&8tWUEMl58"
    "2rO3bW<1Msn2L-%lVhp.23pL_Y(Go1-*<d*%Fjs-`2n`*dRWQ/.QYx6Cc``3aVNT%+AHw)0$@d3Ji=:&Lcq$-T9i/)2i)P&,9w<&C#T<7k&c:.)lSD&m<$E4O1ue)eY(C,/xmp.ZNk6)"
    "M^q*#bLx+;WT=G;i6$j:qq)<-8cSv$U[#?%Ulc;-R)Qt$PS-F%#in92LQ]'/i@nv*A8Nv-<I$h*</(<-'ihl%GDd'&*x?>#+*]V$LE5v6],Zv6.+'u.#WkJCII5H3jAqB#bIGCf8#7e%"
    "Oj'T.wR,@,-'.i(SCs>AYuoJjY04,%`aT7/^[IW$p0IX-hpn>6C*t=*2sr0(/vjP03'G9.a)F_?F,r;%kv6-)5*Ne-^)D3'nrl_FxRbA#)v37'o77kLvOW=IOq&K2q6T;I+&g*%YL3T%"
    "E6$30,%wo%1KJi^M:m[P;=K%6t2'G`(J:;$S)=ciD:?J:XN#7#J6*^%^Y692wqJfLt?8v7XR;l*KF['&qxYb+eP6(>ZP[A,G;WW1uX*mJVu]G3*hgNaU>Grf@fgI;B.9v-Z&mV7Go[A,"
    "n<:Q9i4=1)?E@l$2AfvRU4xo@j,bh<nGGA#?hec;?%N&dl[Zx6RM$Im-&V@Q.Wis/c2XH*Yt2v#Hp6-)=)i-2gTw<&8ZXU&?J9as;mh7@5lWj1,?)8&'+_B#4cNm14bI],<d#V`b8nP1"
    "f-Y208j,G*Nn&6&,eNh#7gr^4MO9[fe#LM'oVf=uewHP/HHVZ/a?]f$[c<*ejECW/.ij/%)_;QA*Yje+Tq)$#0(T#%894-vDol2aYT:o0/XkD#F,pn$Il[I*pxJ+*^`4$,RDw7&5+<D%"
    "eL7'ML4rU%dI2?#W8b?69V@6&fLR5&h+EM-@[1l%R,>>#`R^CsUn+/L1gn+D]7`8.nk.j0n2g;-1_'-%&&2:`-YZb+i>^T9IuB#$/1C#$ngj;-;IKC-1OFU%MvqKcV3bK_7ud'&Umt(<"
    "mEel/S%:SKDn(IP)YR_#;sKW-bHkZ0[B/[#<)ga*vIV9.KBS(-ix?m//fhV$qwYC+?YWX%Y8;6&oT4',4jco/.(-C+S)6B=*P9$-2a/12uVMU<1*Md%uGrk't#eg1+Y(@,15X**+0_q%"
    "N2?B=dK_s-`5Mn/8$M,)=G$##;LH`ES@T`N]fgl/ZPm6#;R@%#RYHY>2)%gs%4Y2(k%24&7ADs-HP-+<N>^;.bLS7&hJE.3x@^Y,sq-x6Lf^I*fK'N'/:2[,,>=i(Za^6&7CE5&$DEj1"
    "q4=e)'IYgLMNct6ps3Z#f%J1(cKKq%@4I8%0SuY#0q:0(G6gm&[lIfLO:K@#JKOe#_XI%#ho+n_4CNP&&x5j_Xr8>,R?[213]-F%U$Y6#+%-42TNq02JX[[>L*-g)H8O,M]YlrfC4K[>"
    "Y9-g)D*<#&(DC8.GH5<.P:`a*x<7gL$CitQ]a96&m/Te$9t`/h2[Z?$Nt7<$Pk.W$@QG-2m1W[$`KO,2D@`Z#%Ud&O9TOU&?<-G&3/Q9%*oV20UBJ3MX;C&=2N7`$#k<7%14,/LvVf%F"
    "DwclL$3nlL/(``jWYt&#&CeNk'c@hlOQdKjLcD':cB%a+`;^G3l`^G3u4`p.'3pL_GOgDl=Z7.Mt)K%#x+OS_5`>>#n>n&#m#KM0`JpK2Pdp_,Tg'u$++di9)55H3?sGA#Y4Ve$h1f]4"
    "R%tU%Eq]W7_;6<.wW(g2clC;$DjfI_H6i&5lX&Y.7gPS/Ux;p/AVuY#l7])*S]D^#$C'[61t0+,=3J=-oRT9&g):6&pv]J3Hkwt-FI_+*BRr<-TKhc)V=1w,8[N9`<NIV6i;p1T^s/20"
    ";C]6#,Dh[SVXTiBmDvV%DAl;-No93%@T$O_mX*2''``T.$2+8_@[+S&q*RKuRD>>#DbYY,<`;aEEEdv$C]R_#,Krg%EWLD3)7p['bYfvLr1,9%?1^G3ro^*+x1NK(>C_F*Pt1hLi/:6&"
    "xFSH)fiZEn`tJ6&iff-:2p-@'E)P:vJ2]^#fe''#l,$P.[8c.sxX$,ZDlq1TU9()*(&G]F<4TkLse[N:Ul9^#>4e'&8]SY5VP/^#cu/^#crl;-6fG<-?S4Vh5=ih&&%x<-.WjU-6^jU-"
    "=8gbD/)D:q<>Uv-*?ET',e``HMAwuQ^&uIDDd4Au^uT:/l?7K3?V[#8I%cq&(cVoIZ`''#D3=&#i$(fF4@[kCi#6m'6bC^=pG:*6qx`0(6xO3)`(?t7vvgI;VPk,4I%SXH=ujudvk4YH"
    "_UG^?o_$##lW(fqprS+i%pPP&-/>$Mq%4a*T[$3BDBXM_5N4l9UfsZ6?-YD#L$/L&/gNh#4D@6&(lOgL3Y?>#/G###+JLf_#RgRJ*@+<%=;.$$CMh;-pXT5sa`;<%KtiM0.@fl#iT8s$"
    ".J3L#kdjZ#5o$s$&C.Z#8'nO]+&6T%0?vTM^'(,)vWa`<+^T)4^=ew7HvI3%U,CQ8lA$tL=+drDDhRq.83pL_u4C,H)fnv$CT(T/GN><.l>l'&VouhMrel]#YNU&$px_eNC17l)ShR@#"
    "vH6T.pJ6I2.UWu-#(IT.Z9*L#%)ej'ldx[#*JfL('>*0(pp+]###sk'maN?#u+o;6eJs7LY1*?##:G>#,vBVKKw^K>N#UrK0_Vs%V1=GMETs$#'n,tLqV&e*G2FuHO.8p&[R;p77>`_Z"
    "Zgdh2F*_i>6p$&4Ju86*giZ_#]CYI)IXR@#:XV^4C;uV$C(?_%CVFGM?XDB4DG:s$B%6_%=;]fLA7wS%Rn'gLT*NZ#quDZc2'ThV)+fh$OIfuc.),##wX[@#sIl0.,0c2=N]O&#Z$)t-"
    "c%`rHop^N_SkZY#1aTP&0Gcf1r$XT.BZc6#Mkr=-F9qd%S_T,&jsT)<OsG*nND2U2Xc>>#2N;v-5=Ml%'WO^2]hin8'EYd3x9%J34C3qKATi2%E%?29623s@?vU&&`)hZ-:152(H9pQ&"
    "fCTb*g#@51rb2J'1gGx@m>Mg1rttM(JR=:ILq<9%0+Z29N/P&#6%sY$Qa%-#pM+K0hC&:`l^[Y#Q^57MTI-LM$rJfL6RcGM0ecgL63DhLaAZ6#qTY6#)g%V%Z/Yb+Msn%#-8.e%R:Za*"
    "V/Xa*oTOt->JOl9=0=2_k1Z&@Y5Z6#71&Nk]Beq77MPYda-11(bbZ#@W#@>8'PN.L,j*41Q2h'#N+'q#:D,+#V+_D*j7q1&A=b/1$>05`oa@>#)M:;$=F[q$>)d'&`1R.;oD$j:P-o;-"
    "Gr:T.?Da6#<M#<-dCI',?#LW-*'@'$+nvc*ts=<-WU.X$oDwU&6**Mg3<>6&Z`.KUXtoGNtoYj#9jvo&^9pcNQ8qs.<g;@&S1dh#5':6&bxmp.<x$s$F[rV$K@/n35XJ7n`C=gLR0J>-"
    "#lhl/J^''#SE<L#n0ZQ<YV>x)=S(i3B;-ZuhrMigP%^I&ot0V&^lhP&m/96&3JY>#f8+,M82jv#H(c<M66$##u[OlX-6w##331n#VA%%#A0vU9xPr)+''d6#X<Ql$;5GB?&pd.MOc#DO"
    "xwrdcIEf>-Z7B^$9+C[%nO;;$n2=1)B(b3lg5bW#I#*a*)$%Z-gO]IVmfl&#J'6lf[gZYQkQJn'b57YYmwv(#_XbgLF@Z6#MTY6#uqQQ%@2Qg):Lsx4itE$'kdIq@]3I1E7<`-M6?[q7"
    "viu_FZq[A,ZO]A,D.*T.-),##[q$*.<4TkL9:rRBS?.60'h`H_O/DwK#D`K_C[&@'T:SC#?T0i:D;<Z6qL>c4*j6N0a=7[#'K8f34x+M;NG#cEWfC_&L=w[-I6CD3u5OSR$rwD&,^hr?"
    "C#jH&Go?E<hWjZ#AG>?#`o_E8xpjV$EuYYu01[S%sXo%HnLs5&jeD<$km1?#<Jta$Y6B4TM5[ihlqK&4@qn%#IcNp#a(+&#tER5`GFZY#=PDa6[UV*%dqep.H3pL_436B$`0qmA6w]G3"
    "kQ+<-EY5<-+W/7.4dIk9/e7p&O<eM1_XCs?Up`mL>q9a#T#)u$Q4fN'O(8j0/I-AO:3<-*fNIW?TU*nK:7PM(.YQIb4jjJM[NQ:=%d9.Z0m(-2lZxN(32wR&30G?#q=*=(,VV8AJ4n0#"
    "MAuu#(OUrZ<0xG3j;>U(wCv?Gc/bJ2SlA]J$DE5*h8ZJ(qJ))3dLmG36%V+v$vZJ(iHL,3GEdG&C5$q%NaB#$:X/m&wQY'$]T<X%R&?v$5IW5&sBG'$&#_:&+YQ<$SLjP&1cU;$A8qo."
    "2Y?&M^q`;$rs2J3dwUFr7GP:v?G]CsA-fM0wpai0?AT<%_]mOo7n`Y#e]hB&#>AS@(S^w%m@m[PsaijrD<FbjC6i$#([b6#]Kx>-v`bk%^`Mp.$>05`<A+&%]Gv6#6:.6/mk.j0HAf`*"
    "t?_6/RR(f)3Pto75J(Z[p3&v7f]HC#XhPS/#:=6&S[FB,PRnXA2a.-&;t5:&dSD5*,6q6&&Rc$,p3ncAT3I51W#3H&ExP?&+<+$##)>>#4,>>#/u&kL>qdLMoNX%vK,O$MT>+jL?;_.$"
    "]X`=-*j*c3W+?;#715##Z2MG#K<u6#-Mc##3#c,4]5Z6#q'd4%#w+F%ro39#Pf'W.Qe%j0RfF?-,.r5/Wh`8.u?.lL_Mic)UPlo.ppWrLteW*@)9XF=eT_+rfdcc)B'Ei^tA*72s6[qL"
    "N?`q:S#8T&G,M*MA4)=-L[#hOi'B)=n-42_%WpvG]61UqR)#rfA@ns?Po__&,0v9)Cus9)*7/F%'ZT.40b`52S4+8@Mhem8H1q#$wCx6#SJwe%SO',&hXq[I`AZ6#H[c6#lVjj$W<d01"
    "vCXI)N^?d)t(]L([Xf.%anW@G`*wY6PM9;']K[+4gDf;%cn;bR0$&],]nH6%s-r(EFP2:%SaGS/>T4',m@Q;$'a&QA9#aj*ks_T%bx_;$uo$W$5oL;$Ma/b*HW=b+D%Dv#m=f)*?GLs7"
    "#ekX$mwxA$G.jxb87PM(vNRoh<PZS/*ZK@MhCF@$bXPc*0Q]$@`21e*>>NT%b-4w#kqxM'0]Cv#k%@s$l4g^+Yshc)U:1w,)MOM(jJQ$$),>>#to+##s0WVHM0IPjLZNR8BG6?$''d6#"
    "Xt7f$qv1K(,1EW-<rJD7x^+W8OCW<%DrBH2>%`;$h?eE.)oui9.s9dkTSmu>'9>e80>?\?$8xCv#L:X,kcb?>#^Bpf(.'758;?_c)+38f.Re%j0t8>(&n:SC#GG2^#K/:n#ZE6I2ejNoR"
    "Ga%8[h,PP&tV=$MWOP,MPV);8)`HC#`W^:%Zk_a4Eg,]#Ud%jo%Y0^#g.Q<-Su6A-'F(a3kGU/#@Vs)#nR^/`2^]Y#u/=;$Ipgm<+?D)+Bu0j_D(S]4?Z@2_]v.(m[5Z6#B+D9..Da6#"
    "Qx]-&:1DD($>Zb+[9I;8KR-uQU_)s@E=*s@NOaSA6r`SA?.A5B8.A5BFmK/)?mK/)*jfa*)j9d*bKUd;A/<+NZZ'NCi7^l8m+O&#Z);N0ZZc6#(tqFros0hLc*8qLN[7tfc,))38DNb&"
    "(?0W-/jq[G^0>)4h5;j0]AqB#/W8f3BgdC#i6X<-FwU,b>;Cx&?+[=7oBUT%36?E3I5fL(F34#-0nWs$WQ#12kM5',wi`4'E#NA+rvj:.s)4/G=mTp77:I,`#U($%W[D20,DlY#BDjlA"
    "@LRs$)/YfCniV11`2)o&3Ih+*I9'u$jME*4>9]&,'ddn&7rC;$S^g6&-+fC=4]O]uP1C<.?F-p.t6#6/,?YY>I[kRe/5###qn:J:cTRVQRInl&2YNY5Eh0)*:J`,#+3:0E==Ab.3^m6#"
    "6E-(#`PZ6#4B1&*=I#UIseY2Vh,Hw$ZAhp.(4pL_l7]^$l,Hw$Hx-v?Bx-v?782g*@Po5J['C#$pGu;-nj2m.-*,##dlls-rDrd*?o2t-2kTm;MlYQ:wnXI)T-xe*NkxQ/Q(P)4uwC.3"
    "rZj;-SP?h':bVa4YVc>#%b;mJpwIg1Tv,v$+C%?#SOvv$'Y4Au%7r7/#r`v#(ehV$T-Js$LXNh#UW->mhar4%5[qs&...##os4Y#d@L/##%t.`E@ZY#2i:;$`6u6#bE)2%Jl`8.^ujh2"
    "?fd=-Hwok&,4_Y,P#Ys$xrLv#VnZ>#:C@[#F7W2''nU?%2.Rs$DSYYuW2-$$;%2v#/Z.H#9H?0%)####.frgXi,=AFVPb1^4_Zv$H@g+:W:Lm9=UF`W)GVg)QO.MTGWI:IYmQ7A5$.B5"
    "''d6#&%qG&a8;>-c)HT%CD>x.TOr_,#LgRJ2#bJ2_V&h1kIIW$SNYj'%f^b*nXa`FQi./lNqC58&Tbi&O?5N'xM:l0[(KfLO_H##Q>uu#voL^8$M'SKah&7#a/5##Nl.j0=Ic7(bgJvf"
    "DwclLbD=s:/[4ZePG:82lccuG3>5_.+*,##d2H-.Z>fnL#XxV8#p0$HP'ZiB)$A*`L,Yw0sBUh(V:WU82q.Z$EWjVTiJE.3?Dsu5u'7]6%-q)=>6R@'uN3^Iv%Jd)Zk3x#ew#X%iGXJ3"
    "5wi)+R<bx#MV&b*;mYo/Dls<%D]HS/;c0+*MouY#EB0>$0hm,*7@IA6N:LP8aeDfsH/nj'@VY>#Qv6h(7u,(+g<0u$,5t&+=I1-M('K8%,dZa3I$8I*oTMH*$,>>#H`J(sj:elJjRIY>"
    "aD_5/kk.j0j-nXHQKF&#a+3v6rhi/:wQ9/`L<$I6x7i;-GY5<-oiq@-+c/v$FU-58c_18.<D>p72MlD4O)>^?oDoH#s.1M)4cAP'E1H>#7efM'/=D@,nFfi1&ZK_,R_UJ)e3Jw#D)j]+"
    "wG#8/n+Qx6MNlA#HeDV&N=2?#7i(v#00AE+*8wv$+hn&,)R$0)BR82'AWh,)QH;M2$a>]#I#&iLH*&(#E^9^#&L6(#Ek$)3>s?D3&(q#$.nuu,BG=,Q<EI4([s^s-osKHM;D*J%FKk<-"
    "Por-%f:Nq2PlD_&?(*R<HsNq@fR9/`<vV6NIX,hOtcY&#%7lh2[eK>2YQM84OG4i(KEX>-9<'9%fpYL2sicB,&,8P'F2/#,v>^r.DSIN4Sw?O(g`I-).?Uu$KkRs$5Pr(,E<j`+kq@w#"
    "<([8%P@MvuDSg(%7,C5/D3*##>pN'%_Q29/Run+M-P8R*X)NR*I2v'Mjw.p%EM3S[o^7##)P7GVS_JYc#rWMK&#oA+GIsfU@aY5JZg*#u:p1Z>8A*aWdFq/:3;_Ak'+f#>jb.<HpT'n&"
    "ECSgU2Z>01J-S6/.h4tHXM6U%-R(hLtCcasv5dHV0`t<6Rax$#Gc,U[qpr<Zc$Wn]+RrhCt8Au6bt/oSnk?iCD:C]5u1wI`-IP]>Y)s=dH2ID=wNeV7Z_-d*6=icN0JlPB3EbDO3Qc]u"
    "P5`8SCCGjUg7J&lXO#9J#_dd3M<<EXl4C9AE8Y^Y,*_9/Q5(X7((Jq84M,F49MJe<qv/@6JhN-sUu$@Zo6[X@k]Qkhwe+r/uZrqJv*+`,j[@@Z.2k:&0DTr&<J>:ot=L.W(*ir/316fa"
    "b;C`Y%torA4X^S9?'sS'%uq(cvRM;AX#&aG@s`S^4CSg*NNm`u=krs&`T[/a`:Fmhn:7<AJ/as]W^9Tph_BNi.o.O)GJEb,cd0[@O0n0<7&7C6NR&+5_;%CZ>sRU9,X,OrM]3CZg:QUT"
    "H&BIk+Y5%Rw,VCZg$f=JA)I7q%bW]%m?ci*^x;VK0wj1aJ%%^%I7HV^7oY8:4ALJki<+##OR/YcXv+F%ojrc<:b__&S4NP&gc[R*NVUV$fmk_&4g*87k&l_&]tFJ(T:n_&[efi'fnn_&"
    "u4@k=?Fo_&xokr-KZ&v#1:<p%oV^JVQ2q_&_B$)*Gfm_&S.35&^Rn_&tJ2MKhqn_&nckr-;=j_&kASY,^Nk_&nV4;-&Tl_&QM^l8@DpmBuYRdF>M_oD2<XG-Q]#,4da$^42vnHF3>;MF"
    "'>ZhF*LeF-rr4VC_0iTCf(,^GO#=SMIvX>-^meF-3wI%H9CM*H,,lVCi/SdF*v#pDU5U*>3[0O=p>.>Brrg*70opbG24%$?`(7$6gqX>-++]#68'rk1kk&%6@E@L2F^&+4;fC`5M2ki2"
    "3g_<.pkn+H$o]>-+O4.-aqhM1I;8_^/k9'#ZqB'#1c/*#Xst.#e)1/#I<x-#-#krL4]7(#XD>8.03h'#_Gu`4$kqM:/uOk+Rq@_/k2w1B8bpfDj2n'&MoOk+YlxE7s]8JC%Jo3+0I/F%"
    ";Ch]G7.l`F6/xX1goUw0HdJ`E<`W>-[E.#HM,f]Go@,#H=Ww]GwpNe$-G`f1'TwDFRt@VHT5rER8Mix=%$[J;q%:&><7cDF0`8R*SWEqDq*r?9)'v9)<n/F%pYdu>[>NDF6H>#6B9d>-"
    "nW^$.g2TkLMC_kLQ,T,M71x(#^@;=-[Gg;-TTIu/6pm(#<^Q(#WSGs-)ax-Wt5;PM])crL<)m.#81uDP@r0*#wbo3k'ilM1ECd,F5;$##T(+GMAqKxXQD&(&m3N;n&;cY#W>O%$MH?MB"
    "4XYiBL*no.Xtp.CsWel/hwj@de]i7ewpEM0%3'/1iuIoem7+Pf)K^f1[>d5]/K6(#gQ?(#Ob39#]=x-#3;K2#[XH(#7dZ(#;pm(#0#X9#bC+.#3(*)#C2<)#q6u6#Oi8*#MTF.#sZO.#"
    ".O=.#Efb.#qm@-#SKb&#+pA*#ma.-#7h7-#LPj)#1TE1#Kr#;#r`^:#+<':#O(V$#mC17#B4n0#j;w0#k%Y6#]RK:#C-^*#[)1/#^#(/#?+85#4Wx5#G;G##f2g*#QaR%#6e`4#L<(7#"
    "L_W1#2@O&#x&/5#LPC7#Nfg:#*C,+#dAU/#f;L/#jO>+#<4=&#oflF#i],3#;xw%#LHY##DSq/#qf60#amp:#]G.%#`3m3#Bm?0#TXN1#:l:$#==.@#C#.8#pD^2#DFX&#vBP##Hd46#"
    ":pF6#Joq7#-$),#6ZT:#UL<1#[ke%#N:r$#sG31#;S@%#4ss1#;6D,#x([0#$#R0#j<M,#.I`,#:1l6#mUr,#ZQk&#HMm;#@0?;#RZ)<#HHd;#G*6;#Y#W<#dA/=#P6H;#_G8=#`/j<#"
    "Rp7p$LXvlEe5S=B,SvLF$42eGoN;tB&jXVC`[`-#SxefLLN6##4,Guu&iG<--4)=-hoYD/hOQ`E?^7rL:4pfL,rJfL7xSfL^k^iBOgIrLW1$##+iO&#*Y#<-*M#<-]q9S-hpdT-iNP</"
    "_om(#b#F$M)%S(#w6t9#(9_W.^D-(#8$J>/-wox=d`piLjY2E-M>:@-fbQiMhf6##bu.>Bc<5cEUOD-#H(`5/<V+;C[^6R9G3_^#&8YY#[/,f$Ol68%Xgq>-3>+XC#X@M9xDtr$=k$Z$"
    "1fDW@nu.GMAr+eZH,>>#McUF%8N&.-Uh-S[f@^.hRr*r)jTOF%lE4OMNEH>#mvP)/7ix.Cj9n-$&?c3=uQj-$6`fq)FIw(<s=$)<Kj&:)7]A:)$FQS7nBl_/ST0/1.Sx.6=Df`*;wT:)"
    "prTS.,oQS%,Od?$?/^Y#OYB>#%(tX(H#@X(%,$v#4U?v$<Sw],Zg@v$.uO&#IM40MJtJfLxa?##,JY##]+,##Kfdp/n,j<#H+,###hrt-6qf(MGR<$#PC,+#1`($#Oke%#QL/(M$LD/#"
    "NY]_M;f_[M>MU<NgGm6#jt[fL5rugLOD?##$RM]ll[7L,Cmb2(<PTw94((Pf3:l'&6o%(&o:dl/(1ik4j'9L>B6m+MDH4v-h:eqL0&@3#.Mx>-K>:@-+.;t-,1tZMnn>WM@9CSM9f_[M"
    "6SC[M9_4rLVw)$#(aErLpK12#u%Y6#t7b$M1g3^Ms**MM_CTVMnBdwL#.gfL/#<7#I+,##^uv(#eQUV$LDaR'$%###";



static const char* GetDefaultCompressedFontDataTTFBase85()
{
    return proggy_clean_ttf_compressed_data_base85;
}

#endif // #ifndef IMGUI_DISABLE
