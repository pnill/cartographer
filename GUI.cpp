//#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <OSHGui.hpp>
#include <Drawing/Direct3D9/Direct3D9Renderer.hpp>
#include <Input/WindowsMessage.hpp>
#include "GUI.h"
#include "H2MOD.h"
#include "xliveless.h"
#include "cartographer_main.hpp"

extern void InitInstance();


typedef struct _XLIVE_INITIALIZE_INFO {
	UINT cbSize;
	DWORD dwFlags;
	IUnknown *pD3D;
	VOID *pD3DPP;
	LANGID langID;
	WORD wReserved1;
	PCHAR pszAdapterName;
	WORD wLivePortOverride;
	WORD wReserved2;
} XLIVE_INITIALIZE_INFO;

typedef struct XLIVE_INPUT_INFO  {
	UINT cbSize;
	HWND hWnd;
	UINT uMSG;
	WPARAM wParam;
	LPARAM lParam;
	BOOL fHandled;
	LRESULT lRet;
} XLIVE_INPUT_INFO;

LPDIRECT3DDEVICE9 pDevice;
D3DPRESENT_PARAMETERS *pD3DPP;
LPD3DXFONT pFont;
IDirect3DTexture9* Primitive = NULL;

using namespace OSHGui;
using namespace OSHGui::Drawing;
Input::WindowsMessage input;

char* BuildText;
const char CompileDate[] = __DATE__;
const char CompileTime[] = __TIME__;

int verticalRes = 0;
int horizontalRes = 0;

int normalSizeCurrentFontHeight = 0;
int largeSizeCurrentFontHeight = 0;
int largeSizeFontHeight = 0;
int normalSizeFontHeight = 0;

LPD3DXFONT normalSizeFont = 0;
LPD3DXFONT largeSizeFont = 0;
LPD3DXFONT smallFont = 0;

class CVertexList
{
public:
	FLOAT X, Y, Z;
	DWORD dColor;
};

BOOL bIsCreated, bNeedsFlush;

DWORD dwOldFVF;
LPD3DXSPRITE pSprite;


void GUI::Initialize()
{
	initFontsIfRequired();



	/*auto renderer = std::unique_ptr<Direct3D9Renderer>(new Direct3D9Renderer(pDevice));
	
	Application::Initialize(std::move(renderer));

	auto &app = Application::Instance();

	auto font = FontManager::LoadFont("Arial", 8.0f, false);
	app.SetDefaultFont(font);
	
	auto form = std::make_shared<cartographer_main>();

	app.Run(form);

	app.Disable();

	app.RegisterHotkey(Hotkey(Key::Home, []
	{
		Application::Instance().Toggle();
	}));
	*/


}

// #5001

int WINAPI XLiveInput(XLIVE_INPUT_INFO* pPii)
{
	static int print = 0;
	if (print < 15)
	{
		//	TRACE("XLiveInput  (a1 = %X) (00 = %X, 04 = %X, 08 = %X, 0C = %X, 10 = %X, 14 = %X, 18 = %X",
		//		a1,
		//		a1[0], a1[1], a1[2], a1[3], a1[4], a1[5], a1[6] );

		print++;
	}





	return 1;
}

// #5030: XLivePreTranslateMessage
int WINAPI XLivePreTranslateMessage(const LPMSG lpMsg)
{

	//if (input.ProcessMessage(lpMsg))
	//{
	//	return true;
	//}

	return 0;
}
// #5000: XLiveInitialize
int WINAPI XLiveInitialize(XLIVE_INITIALIZE_INFO* pPii)
{
		InitInstance();
		TRACE("XLiveInitialize()");

		if (!h2mod->Server)
		{
			//TRACE("XLiveInitialize  (pPii = %X)", pPii);
			pDevice = (LPDIRECT3DDEVICE9)pPii->pD3D;
			pD3DPP = (D3DPRESENT_PARAMETERS*)pPii->pD3DPP;
			BuildText = new char[255];
			sprintf(BuildText, "Project Cartographer (v0.1a) - Build Time: %s %s", CompileDate, CompileTime);


			D3DVIEWPORT9 pViewPort;
			pDevice->GetViewport(&pViewPort);

			verticalRes = pViewPort.Height;
			horizontalRes = pViewPort.Width;

			GUI::Initialize();
		}
		
#if 0
	while (1)
		Sleep(1);
#endif
	
	return 0;
}

// #5005: XLiveOnCreateDevice
int WINAPI XLiveOnCreateDevice(IDirect3DDevice9 *pD3D, VOID* vD3DPP)
{
	pDevice = pD3D;
	pD3DPP = (D3DPRESENT_PARAMETERS*)vD3DPP;
	//TRACE("XLiveOnCreateDevice  (pD3D = %X, pD3DPP = %X)", pD3D, vD3DPP);
	return 0;
}

// #5007: XLiveOnResetDevice
int WINAPI XLiveOnResetDevice(D3DPRESENT_PARAMETERS* vD3DPP)
{
	//pFont->OnLostDevice();
	//pFont->OnResetDevice();
	largeSizeFont->OnLostDevice();
	largeSizeFont->OnResetDevice();
	normalSizeFont->OnLostDevice();
	normalSizeFont->OnResetDevice();
	smallFont->OnLostDevice();
	smallFont->OnResetDevice();

	pD3DPP = vD3DPP;
	//TRACE("XLiveOnResetDevice");
	return 0;
}

// #5006 XLiveOnDestroyDevice
HRESULT WINAPI XLiveOnDestroyDevice()
{
	//TRACE("XLiveOnDestroyDevice");
	return S_OK;
}


void initFontsIfRequired()
{
	normalSizeFontHeight = 0.017 * verticalRes;
	largeSizeFontHeight = 0.034 * verticalRes;
	D3DXCreateFont(pDevice, 10, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Lucida Console", &smallFont);

	if (!normalSizeFont || normalSizeFontHeight != normalSizeCurrentFontHeight) {
		if (normalSizeFont)
		{
			normalSizeFont->Release();
		}

		D3DXCreateFont(pDevice, normalSizeFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &normalSizeFont);
		normalSizeCurrentFontHeight = normalSizeFontHeight;
	}

	if (!largeSizeFont || largeSizeFontHeight != largeSizeCurrentFontHeight) {
		if (largeSizeFont)
		{
			largeSizeFont->Release();
		}

		D3DXCreateFont(pDevice, largeSizeFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &largeSizeFont);
		largeSizeCurrentFontHeight = largeSizeFontHeight;
	}

}

int getTextWidth(const char *szText, LPD3DXFONT pFont)
{
	RECT rcRect = { 0, 0, 0, 0 };
	if (pFont)
	{
		pFont->DrawTextA(NULL, szText, strlen(szText), &rcRect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
	}
	int width = rcRect.right - rcRect.left;
	std::string text(szText);
	std::reverse(text.begin(), text.end());

	text = text.substr(0, text.find_first_not_of(' ') != std::string::npos ? text.find_first_not_of(' ') : 0);
	for (char c : text)
	{
		width += getSpaceCharacterWidth(pFont);
	}
	return width;
}

int getSpaceCharacterWidth(LPD3DXFONT pFont)
{
	return getTextWidth("i i", pFont) - ((getTextWidth("i", pFont)) * 2);
}

void drawRect(int x, int y, int width, int height, DWORD Color)
{
	D3DRECT rec = { x, y, x + width, y + height };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, Color, 0, 0);
}

void drawHorizontalLine(int x, int y, int width, D3DCOLOR Color)
{
	drawRect(x, y, width, 1, Color);
}

void drawVerticalLine(int x, int y, int height, D3DCOLOR Color)
{
	drawRect(x, y, 1, height, Color);
}

void drawBox(int x, int y, int width, int height, D3DCOLOR BorderColor, D3DCOLOR FillColor)
{
	drawRect(x, y, width, height, FillColor);
	drawHorizontalLine(x, y, width, BorderColor);
	drawVerticalLine(x, y, height, BorderColor);
	drawVerticalLine(x + width, y, height, BorderColor);
	drawHorizontalLine(x, y + height, width, BorderColor);
}

int centerTextHorizontally(const char* text, int x, int width, LPD3DXFONT pFont)
{
	return x + (width - getTextWidth(text, pFont)) / 2;
}

void drawText(int x, int y, DWORD color, const char* text, LPD3DXFONT pFont)
{
	RECT shadow1;
	RECT shadow2;
	RECT shadow3;
	RECT shadow4;
	SetRect(&shadow1, x + 1, y + 1, x + 1, y + 1);
	SetRect(&shadow2, x - 1, y + 1, x - 1, y + 1);
	SetRect(&shadow3, x + 1, y - 1, x + 1, y - 1);
	SetRect(&shadow4, x - 1, y - 1, x - 1, y - 1);
	pFont->DrawTextA(NULL, text, -1, &shadow1, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	pFont->DrawTextA(NULL, text, -1, &shadow2, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	pFont->DrawTextA(NULL, text, -1, &shadow3, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	pFont->DrawTextA(NULL, text, -1, &shadow4, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

	RECT rect;
	SetRect(&rect, x, y, x, y);
	pFont->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
}


// #5002: XLiveRender
int WINAPI XLiveRender()
{
	
	if (pDevice)
	{	
		if (pDevice->TestCooperativeLevel() == D3D_OK)
		{
			drawText(0, 0, COLOR_WHITE, BuildText, smallFont);
		}
			

	}

	return 0;
}

