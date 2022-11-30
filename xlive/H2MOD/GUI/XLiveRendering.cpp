#include "stdafx.h"

#include "XLiveRendering.h"

#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"
#include "imgui_integration/imgui_handler.h"
#include "XLiveRendering.h"

#include "H2MOD.h"
#include "Blam/Engine/Game/units/bipeds.h"
#include "H2MOD/Modules/Achievements/Achievements.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "H2MOD/Modules/Networking/Networking.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/Shell.h"

#include "Util/Hooks/Hook.h"

extern void InitInstance();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool displayXyz = false;
bool doDrawIMGUI = false;

HWND H2hWnd;
LPDIRECT3DDEVICE9 pDevice;

IDirect3DTexture9* Primitive = NULL;

int masterState = 0;
char* buildText = nullptr;
char* serverStatus = nullptr;

const char CompileDate[] = __DATE__;
const char CompileTime[] = __TIME__;

int verticalRes = 0;
int horizontalRes = 0;

double normalSizeCurrentFontHeight = 0;
double largeSizeCurrentFontHeight = 0;
double largeSizeFontHeight = 0;
double normalSizeFontHeight = 0;

LPD3DXFONT normalSizeFont = 0;
LPD3DXFONT largeSizeFont = 0;
LPD3DXFONT smallFont = 0;
LPD3DXFONT haloFont = 0;

struct vertex
{
	FLOAT x, y, z, rhw;
	DWORD color;
};

struct CVertexList
{
	D3DXVECTOR4 xyzrhw;
	DWORD dColor;
};

extern HMODULE hThis;
extern std::wstring ModulePathW(HMODULE);

std::wstring GetDirectoryFile(wchar_t *filename)
{
	std::wstring dirFile(ModulePathW(hThis));
	dirFile.append(filename);
	return dirFile;
}

inline void BuildVertex(D3DXVECTOR4 xyzrhw, D3DCOLOR color, CVertexList* vertexList, int index)
{
	vertexList[index].xyzrhw = xyzrhw;
	vertexList[index].dColor = color;
}

LPD3DXSPRITE pSprite;

LPD3DXSPRITE Sprite_Interface;
LPDIRECT3DTEXTURE9 Texture_Interface;


// #5297: XLiveInitializeEx
int WINAPI XLiveInitializeEx(XLIVE_INITIALIZE_INFO* pXii, DWORD dwVersion)
{
	InitInstance();

	LOG_TRACE_XLIVE("XLiveInitializeEx()");

	if (pXii->pD3D)
	{
		pDevice = (LPDIRECT3DDEVICE9)pXii->pD3D;
		auto d3dpp = (D3DPRESENT_PARAMETERS*)pXii->pD3DPP;
		XLiveRendering::Initialize(d3dpp->hDeviceWindow);
	}
	LOG_TRACE_XLIVE("XLiveInitializeEx() - dwVersion = {0:x}", dwVersion);
	return 0;
}

// #5000: XLiveInitialize
int WINAPI XLiveInitialize(XLIVE_INITIALIZE_INFO* pXii)
{
	return XLiveInitializeEx(pXii, 0);
}

// #5005: XLiveOnCreateDevice
int WINAPI XLiveOnCreateDevice(IDirect3DDevice9 *pD3D, VOID* vD3DPP)
{
	pDevice = pD3D;
	
	//pPresent = (HRESULT(WINAPI*)(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)) *(DWORD_PTR*)(pDevice+17);
	//VirtualProtect((LPVOID)(pDevice + 17), sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &dwPresent);
	//*(DWORD_PTR*)(pDevice + 17) = (DWORD_PTR)hkPresent;

	//LOG_TRACE_XLIVE("XLiveOnCreateDevice  (pD3D = %X, pD3DPP = %X)", pD3D, vD3DPP);
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
	Sprite_Interface->OnLostDevice();
	Sprite_Interface->OnResetDevice();

	//Have to invalidate ImGUI on device reset, otherwise it hangs the device in a reset loop.
	//https://github.com/ocornut/imgui/issues/1464#issuecomment-347469716

	ImGuiHandler::ReleaseTextures();
	ImGui_ImplDX9_InvalidateDeviceObjects();
	//pDevice->Reset(pD3DPP);
	//LOG_TRACE_XLIVE("XLiveOnResetDevice");
	return 0;
}

// #5006 XLiveOnDestroyDevice
HRESULT WINAPI XLiveOnDestroyDevice()
{
	largeSizeFont->Release();
	normalSizeFont->Release();
	smallFont->Release();
	Sprite_Interface->Release();
	
	//LOG_TRACE_XLIVE("XLiveOnDestroyDevice");
	return S_OK;
}

void InitalizeFont(std::wstring strFontName, const std::wstring& strFontPath, int size, IDirect3DDevice9* pD3Ddev, bool OnOff)
{
	if (OnOff)
	{
		addDebugText("Adding font: %ws", strFontPath.c_str());
		if (AddFontResource(strFontPath.c_str()) > 0)
		{
			addDebugText("Font successfully added.");
		}
		else
		{
			addDebugText("Adding font failed.");
		}
	}

	D3DXCreateFont(pD3Ddev, size, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName.c_str(), &haloFont);
}


void InitFontsIfRequired()
{
	normalSizeFontHeight = 0.017 * verticalRes;
	largeSizeFontHeight = 0.034 * verticalRes;

	InitalizeFont(L"Conduit ITC Medium", GetDirectoryFile(L"maps\\fonts\\conduit_itc_medium1.ttf"), largeSizeFontHeight, pDevice, true);
	D3DXCreateFont(pDevice, 10, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Lucida Console", &smallFont);

	if (!normalSizeFont || normalSizeFontHeight != normalSizeCurrentFontHeight) {
		if (normalSizeFont)
		{
			normalSizeFont->Release();
		}

		D3DXCreateFont(pDevice, normalSizeFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Verdana", &normalSizeFont);
		normalSizeCurrentFontHeight = normalSizeFontHeight;
	}

	if (!largeSizeFont || largeSizeFontHeight != largeSizeCurrentFontHeight) {
		if (largeSizeFont)
		{
			largeSizeFont->Release();
		}

		D3DXCreateFont(pDevice, largeSizeFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Tahoma", &largeSizeFont);
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

void drawPrimitiveRect(FLOAT x, FLOAT y, FLOAT w, FLOAT h, D3DCOLOR color) {
	CVertexList vertexList[4];

	BuildVertex(D3DXVECTOR4(x, y + h, 0, 1), color, vertexList, 0);
	BuildVertex(D3DXVECTOR4(x, y, 0, 1), color, vertexList, 1);
	BuildVertex(D3DXVECTOR4(x + w, y + h, 0, 1), color, vertexList, 2);
	BuildVertex(D3DXVECTOR4(x + w, y, 0, 1), color, vertexList, 3);

	LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
	pDevice->CreateStateBlock(D3DSBT_ALL, &pStateBlock);
	pStateBlock->Capture();
	pDevice->SetTexture(0, Primitive);
	pDevice->SetPixelShader(NULL);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	//pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertexList, sizeof(CVertexList));

	pStateBlock->Apply();
	pStateBlock->Release();

}

IDirect3DVertexBuffer9* g_pVB = NULL;
IDirect3DIndexBuffer9* g_pIB = NULL;

void DrawRect(IDirect3DDevice9* pDevice, float x, float y, float w, float h, D3DCOLOR Color)
{
	struct vertex
	{
		float x, y, z, rhw;
		DWORD color;
	};

	vertex qV[4] = {
		{ (float)x , (float)(y + h), 0.0f, 1.0f, Color },
		{ (float)x , (float)y , 0.0f, 1.0f, Color },
		{ (float)(x + w), (float)(y + h), 0.0f, 1.0f, Color },
		{ (float)(x + w), (float)y , 0.0f, 1.0f, Color }
	};
	const DWORD D3DFVF_TL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	pDevice->SetFVF(D3DFVF_TL);
	pDevice->SetTexture(0, Primitive);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(vertex));
}

void drawFilledBox(float x, float y, float w, float h, DWORD color)
{
	vertex V[4];

	V[0].color = V[1].color = V[2].color = V[3].color = color;

	V[0].z = V[1].z = V[2].z = V[3].z = 0;
	V[0].rhw = V[1].rhw = V[2].rhw = V[3].rhw = 0;

	V[0].x = x;
	V[0].y = y;
	V[1].x = x + w;
	V[1].y = y;
	V[2].x = x + w;
	V[2].y = y + h;
	V[3].x = x;
	V[3].y = y + h;

	unsigned short indexes[] = { 0, 1, 3, 1, 2, 3 };

	pDevice->CreateVertexBuffer(4 * sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);
	pDevice->CreateIndexBuffer(2 * sizeof(short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL);

	VOID* pVertices;
	g_pVB->Lock(0, sizeof(V), (void**)&pVertices, 0);
	memcpy(pVertices, V, sizeof(V));
	g_pVB->Unlock();

	VOID* pIndex;
	g_pIB->Lock(0, sizeof(indexes), (void**)&pIndex, 0);
	memcpy(pIndex, indexes, sizeof(indexes));
	g_pIB->Unlock();

	pDevice->SetTexture(0, NULL);
	pDevice->SetPixelShader(NULL);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetStreamSource(0, g_pVB, 0, sizeof(vertex));
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetIndices(g_pIB);

	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	g_pVB->Release();
	g_pIB->Release();
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


int achievement_height = 0;
bool achievement_freeze = false;
int achievement_timer = 0;

char* autoUpdateText = 0;


void XLiveRendering::Initialize(HWND hWnd)
{
	InitFontsIfRequired();

	serverStatus = new char[256];
	snprintf(serverStatus, 256, "Status: Initializing....");

	buildText = new char[256];
	snprintf(buildText, 256, "Project Cartographer (v%s) - Build Time: %s %s", DLL_VERSION_STR, CompileDate, CompileTime);

	if (FAILED(D3DXCreateTextureFromFile(pDevice, L"sounds/h2pc_logo.png", &Texture_Interface)))
	{
		addDebugText("ERROR: Failed to create logo texture (for achievements).");
	}

	D3DXCreateSprite(pDevice, &Sprite_Interface);
	ImGuiHandler::Initalize(pDevice, hWnd);
}

// #5001
int WINAPI XLiveInput(XLIVE_INPUT_INFO* pPii)
{
	static bool has_initialised_input = false;
	if (!has_initialised_input) {
		extern RECT rectScreenOriginal;
		H2hWnd = pPii->hWnd;
		GetWindowRect(H2hWnd, &rectScreenOriginal);
		has_initialised_input = true;
	}

	if ((pPii->uMSG == WM_KEYDOWN || pPii->uMSG == WM_SYSKEYDOWN)
		&& (GetKeyState(pPii->wParam) & 0x8000))
	{
		// hotkeys
		KeyboardInput::ExecuteHotkey(pPii->wParam);
	}
	
	if (ImGuiHandler::ImGuiShouldHandleInput())
	{
		ImGui_ImplWin32_WndProcHandler(pPii->hWnd, pPii->uMSG, pPii->wParam, pPii->lParam);
	}

	return S_OK;
}

// #5030: XLivePreTranslateMessage
BOOL WINAPI XLivePreTranslateMessage(const LPMSG lpMsg)
{
	return false;
}

static void set_max_system_timer_resolution(bool enabled)
{
	ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
	_Shell::NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);
	_Shell::NtSetTimerResolutionHelper(ulMaximumResolution, enabled, &ulCurrentResolution);
}

// TODO: move to _Shell or somewhere else?
void XLiveThrottleFramerate(int maxFramerate) 
{
	static LARGE_INTEGER lastCounter;
	static int lastFrameSetting = -1;
	static bool frameLimiterInitialized = false;

	const int threadWaitTimePercentage = 90;
	static HANDLE hFrameLimitTimer = NULL;

	if (maxFramerate <= 0)
	{
		lastFrameSetting = maxFramerate;
		frameLimiterInitialized = false;
		return;
	}

	if (lastFrameSetting != maxFramerate)
	{
		lastFrameSetting = maxFramerate;
		frameLimiterInitialized = false;
	}

	if (!frameLimiterInitialized)
	{
		QueryPerformanceCounter(&lastCounter);
		frameLimiterInitialized = true;

		if (NULL == hFrameLimitTimer)
		{
			hFrameLimitTimer = CreateWaitableTimer(NULL, FALSE, NULL);

			atexit([]() {
				if (NULL != hFrameLimitTimer)
					CloseHandle(hFrameLimitTimer);

				// reset timer resolution back to default on exit
				set_max_system_timer_resolution(false);
			});
		}

		// skip the first frame after init
		return;
	}

	LARGE_INTEGER frequency;
	LARGE_INTEGER deltaCounter;
	QueryPerformanceFrequency(&frequency); 

	auto minFrameTimeUs = (long long)(1000000.0 / (double)maxFramerate);

	QueryPerformanceCounter(&deltaCounter);
	deltaCounter.QuadPart = deltaCounter.QuadPart - lastCounter.QuadPart;
	auto deltaTimeUs = _Shell::QPCToTime(std::micro::den, deltaCounter, frequency);

	if (deltaTimeUs < minFrameTimeUs)
	{
		auto sleepTimeUs = minFrameTimeUs - deltaTimeUs;

		// sleep threadWaitTimePercentage out of the target render time using thread sleep or timer wait
		long long timeToWaitSleepUs = (threadWaitTimePercentage * sleepTimeUs) / 100;

		// sleep just the milliseconds part
		// timeToWaitSleepUs = timeToWaitSleepUs - (timeToWaitSleepUs % 1000);

		// skip if time to wait is lower than 3ms
		if (timeToWaitSleepUs > 3000)
		{
			if (NULL != hFrameLimitTimer)
			{
				// Create an unnamed waitable timer.
				ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
				_Shell::NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);

				if (10ll * timeToWaitSleepUs > ulMaximumResolution)
				{
					LARGE_INTEGER liDueTime;
					_Shell::NtSetTimerResolutionHelper(ulMaximumResolution, TRUE, &ulCurrentResolution);

					liDueTime.QuadPart = -10ll * timeToWaitSleepUs;
					if (SetWaitableTimer(hFrameLimitTimer, &liDueTime, 0, NULL, NULL, TRUE))
					{
						// Wait for the timer.
						_Shell::NtWaitForSingleObjectHelper(hFrameLimitTimer, FALSE, &liDueTime);
					}
				}
			}

			/*int sleepTimeMs = timeToWaitSleepUs / 1000ll;
			if (sleepTimeMs >= 0)
				Sleep(sleepTimeMs);*/
		}

		while (true)
		{
			QueryPerformanceCounter(&deltaCounter);
			deltaCounter.QuadPart = deltaCounter.QuadPart - lastCounter.QuadPart;
			deltaTimeUs = _Shell::QPCToTime(std::micro::den, deltaCounter, frequency);
			if (deltaTimeUs >= minFrameTimeUs)
				break;
		}
	}

	QueryPerformanceCounter(&lastCounter);
}

// #5002: XLiveRender
int WINAPI XLiveRender()
{
	static std::mutex renderMtx;
	std::lock_guard lg(renderMtx);

	if (pDevice)
	{
		if (!FAILED(pDevice->TestCooperativeLevel()))
		{
			D3DVIEWPORT9 pViewport;
			pDevice->GetViewport(&pViewport);

			D3DDEVICE_CREATION_PARAMETERS cparams;
			pDevice->GetCreationParameters(&cparams);
			RECT gameWindowRect, gameWindowInnerRect;
			GetWindowRect(cparams.hFocusWindow, &gameWindowRect);
			GetClientRect(cparams.hFocusWindow, &gameWindowInnerRect);

			int gameWindowWidth = gameWindowRect.right - gameWindowRect.left - GetSystemMetrics(SM_CXSIZEFRAME);
			int gameWindowHeight = gameWindowRect.bottom - gameWindowRect.top;

			DWORD gameGlobals = *Memory::GetAddress<DWORD*>(0x482D3C, 0x4CB520);
			DWORD gameEngine = *(DWORD*)(gameGlobals + 0x8);
			bool paused_or_in_menus = (*Memory::GetAddress<BYTE*>(0x47A568) != 0);

			if (gameEngine == 3 || (gameEngine != 3 && paused_or_in_menus)) {
				drawText(0, 0, COLOR_WHITE, buildText, smallFont);
				if (masterState == 0)
					drawText(0, 15, COLOR_WHITE, serverStatus, smallFont);
				else if (masterState == 1)
					drawText(0, 15, COLOR_GREY, serverStatus, smallFont);
				else if (masterState == 2)
					drawText(0, 15, COLOR_RED, serverStatus, smallFont);
				else if (masterState == 10)
					drawText(0, 15, COLOR_GREEN, serverStatus, smallFont);

				if(H2Config_anti_cheat_enabled)
					drawText(0, 30, COLOR_GREEN, "Anti-Cheat: Enabled", smallFont);
				else
					drawText(0, 30, COLOR_RED, "Anti-Cheat: Disabled", smallFont);
			}
			
			//drawPrimitiveRect(gameWindowWidth / 1.15, gameWindowHeight - 150, 250, 100, D3DCOLOR_ARGB(155, 41, 65, 129));
			//drawText(gameWindowWidth / 1.13, gameWindowHeight - 145, COLOR_WHITE, "Points: 10,000", haloFont);
	
#pragma region Achievement Rendering		
			if (!AchievementMap.empty())
			{
				auto it = AchievementMap.begin();
				
				if (it->second == false)
				{
					h2mod->custom_sound_play(L"sounds/AchievementUnlocked.wav", 0);
					it->second = true;
				}
				
				if (achievement_height >= 150)
				{
					achievement_freeze = true;
				}
				else
					achievement_height = achievement_height + 2;		

				float scalar = 11.0f;
				D3DXVECTOR3 position;
				position.x = (gameWindowWidth / 2 - 250 + 3) * scalar;
				position.y = (gameWindowHeight - achievement_height + 3) * scalar;


				Sprite_Interface->Begin(D3DXSPRITE_ALPHABLEND);
				D3DXVECTOR2 vCenter(0.0f, 0.0f);
				D3DXVECTOR2 vScale((1 / scalar), (1 / scalar));
				D3DXVECTOR2 vPosition(150.0f, 200.0f);
				D3DXVECTOR2 vRotationCenter(0.0f, 0.0f);

				D3DXMATRIX mat;

				drawPrimitiveRect(gameWindowWidth / 2 - 250, gameWindowHeight - achievement_height, 500, 100, D3DCOLOR_ARGB(155, 000, 000, 000));

				size_t delim = it->first.find("|");
				std::string achievement_title = it->first.substr(0, delim);
				std::string achievement_desc = it->first.substr(delim+1, it->first.size() - delim);

				drawText(gameWindowWidth / 2 - 100, gameWindowHeight - (achievement_height - 25), COLOR_WHITE, achievement_title.c_str(), normalSizeFont);
				drawText(gameWindowWidth / 2 - 100, gameWindowHeight - (achievement_height - 50), COLOR_WHITE, achievement_desc.c_str(), normalSizeFont);

				D3DXMatrixTransformation2D(&mat, &vCenter, NULL, &vScale, NULL, NULL, NULL);
				Sprite_Interface->SetTransform(&mat);

				Sprite_Interface->Draw(Texture_Interface, NULL, NULL, &position, 0xFFFFFFFF);
				Sprite_Interface->End();

				if (achievement_freeze == true)
				{
					if (achievement_timer >= 400)
					{
						achievement_freeze = false;
						achievement_timer = 0;
						achievement_height = 0;
						AchievementMap.erase(it);
					}

					achievement_timer++;
				}

			}
#pragma endregion achievement rendering

			if (displayXyz && (NetworkSession::LocalPeerIsSessionHost() || s_game_globals::game_is_campaign)) {
				int text_y_coord = 60;
				PlayerIterator playerIt;
				while (playerIt.get_next_active_player()) 
				{
					real_point3d* player_position = units::carto_get_player_unit_coords(playerIt.get_current_player_index());
					s_biped_data_definition* biped_unit = (s_biped_data_definition*)units::carto_get_player_unit_from_player_index(playerIt.get_current_player_index());
					if (player_position != nullptr) {
						std::wstring playerNameWide(playerIt.get_current_player_name());
						std::string playerName(playerNameWide.begin(), playerNameWide.end());
						std::string xyzText = 
							"Player name: " + playerName + 
							", xyz = " + std::to_string(player_position->x) + " " 
							+ std::to_string(player_position->y) + " " 
							+ std::to_string(player_position->z) + " "
							+ "Velocity: " + std::to_string(real_math::magnitude3d(&biped_unit->translational_velocity));
						drawText(0, text_y_coord, COLOR_GOLD, xyzText.c_str(), normalSizeFont);
						text_y_coord += 15;
					}
				}
			}

			if (autoUpdateText) {
				drawText(10, 60, COLOR_WHITE, autoUpdateText, normalSizeFont);
			}
			extern long long sizeOfDownload;
			extern long long sizeOfDownloaded;
			if (sizeOfDownload > 0) {
				drawBox(10, 52, 200, 6, COLOR_RED, COLOR_RED);
				drawBox(10, 52, ((sizeOfDownloaded * 100) / sizeOfDownload) * 2, 6, COLOR_GREEN, COLOR_GREEN);
			}

			ImGuiHandler::DrawImgui();
		}
	}

	// limit framerate if needed
	// UPDATE: frame limiting in XLiveRender adds input lag
	// XLiveThrottleFramerate(H2Config_fps_limit);
	return 0;
}

