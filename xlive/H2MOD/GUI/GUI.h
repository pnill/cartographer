#pragma once

static void drawText(int x, int y, DWORD color, const char* text, LPD3DXFONT pFont);
static void drawRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor);
static void drawHorizontalLine(int x, int y, int width, D3DCOLOR Color);
static void drawVerticalLine(int x, int y, int height, D3DCOLOR Color);
static void drawBox(int x, int y, int width, int height, D3DCOLOR BorderColor, D3DCOLOR FillColor);
static void drawChatInterface();
static int getTextWidth(const char *szText, LPD3DXFONT pFont);
static int getSpaceCharacterWidth(LPD3DXFONT pFont);
static int centerTextHorizontally(const char* text, int x, int width, LPD3DXFONT pFont);
void initFontsIfRequired();

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


namespace GUI
{
	extern void Initialize();
};

