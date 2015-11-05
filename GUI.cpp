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

using namespace OSHGui;
using namespace OSHGui::Drawing;
Input::WindowsMessage input;

void GUI::Initialize()
{

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
	TRACE("XLiveInitialize()");

	if (!h2mod->Server)
	{
		//TRACE("XLiveInitialize  (pPii = %X)", pPii);
		pDevice = (LPDIRECT3DDEVICE9)pPii->pD3D;
		pD3DPP = (D3DPRESENT_PARAMETERS*)pPii->pD3DPP;

		GUI::Initialize();
	}
		InitInstance();

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
	pFont->OnLostDevice();
	pFont->OnResetDevice();
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

	pFont->DrawTextA(NULL, text, -1, &shadow1, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0, 0));
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
				if (pFont)
					pFont->Release();


				D3DXCreateFont(pDevice, 10, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Lucida Console", &pFont);

				drawText(0, 0, COLOR_WHITE, "Project Cartographer (v0.1a)", pFont);
		}
			

	}

	return 0;
}

