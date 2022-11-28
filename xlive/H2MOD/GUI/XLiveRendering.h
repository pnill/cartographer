#pragma once

typedef struct _XLIVE_INITIALIZE_INFO {
	UINT cbSize;
	DWORD dwFlags;
	IUnknown* pD3D;
	VOID* pD3DPP;
	LANGID langID;
	WORD wReserved1;
	PCHAR pszAdapterName;
	WORD wLivePortOverride;
	WORD wReserved2;
} XLIVE_INITIALIZE_INFO;

typedef struct XLIVE_INPUT_INFO {
	UINT cbSize;
	HWND hWnd;
	UINT uMSG;
	WPARAM wParam;
	LPARAM lParam;
	BOOL fHandled;
	LRESULT lRet;
} XLIVE_INPUT_INFO;

#define DISPLAY_DEV_TESTING_MENU 0

void InitFontsIfRequired();
void drawText(int x, int y, DWORD color, const char* text, LPD3DXFONT pFont);
void drawRect(int x, int y, int width, int height, DWORD Color);
void drawHorizontalLine(int x, int y, int width, D3DCOLOR Color);
void drawVerticalLine(int x, int y, int height, D3DCOLOR Color);
void drawBox(int x, int y, int width, int height, D3DCOLOR BorderColor, D3DCOLOR FillColor);
int getTextWidth(const char* szText, LPD3DXFONT pFont);
int getSpaceCharacterWidth(LPD3DXFONT pFont);
int centerTextHorizontally(const char* text, int x, int width, LPD3DXFONT pFont);

void XLiveThrottleFramerate(int maxFramerate);

static CONST D3DCOLOR COLOR_RED = D3DCOLOR_ARGB(255, 255, 000, 000);
static CONST D3DCOLOR COLOR_GREEN = D3DCOLOR_ARGB(255, 127, 255, 000);
static CONST D3DCOLOR COLOR_ORANGE = D3DCOLOR_ARGB(255, 255, 140, 000);
static CONST D3DCOLOR COLOR_BLUE = D3DCOLOR_ARGB(255, 000, 000, 255);
static CONST D3DCOLOR COLOR_YELLOW = D3DCOLOR_ARGB(255, 255, 255, 51);
static CONST D3DCOLOR COLOR_BLACK = D3DCOLOR_ARGB(255, 000, 000, 000);
static CONST D3DCOLOR COLOR_GREY = D3DCOLOR_ARGB(255, 112, 112, 112);
static CONST D3DCOLOR COLOR_GOLD = D3DCOLOR_ARGB(255, 255, 215, 000);
static CONST D3DCOLOR COLOR_PINK = D3DCOLOR_ARGB(255, 255, 192, 203);
static CONST D3DCOLOR COLOR_PURPLE = D3DCOLOR_ARGB(255, 128, 000, 128);
static CONST D3DCOLOR COLOR_CYAN = D3DCOLOR_ARGB(255, 000, 255, 255);
static CONST D3DCOLOR COLOR_MAGNETA = D3DCOLOR_ARGB(255, 255, 000, 255);
static CONST D3DCOLOR COLOR_WHITE = D3DCOLOR_ARGB(255, 255, 255, 249);
static CONST D3DCOLOR COLOR_GREY_TRANSPARENT = D3DCOLOR_ARGB(150, 112, 112, 112);
static CONST D3DCOLOR COLOR_BLACK_TRANSPARENT = D3DCOLOR_ARGB(150, 0, 0, 0);

namespace XLiveRendering
{
	void Initialize(HWND hWnd);
};

extern HWND H2hWnd;

