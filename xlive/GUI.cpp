//#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <memory>

//#include <OSHGui.hpp>
//#include <Drawing/Direct3D9/Direct3D9Renderer.hpp>
//#include <Input/WindowsMessage.hpp>
#include "GUI.h"
#include "H2MOD.h"
#include "xliveless.h"
#include "cartographer_main.hpp"
#include "H2MOD_MapManager.h"
#include "H2OnscreenDebugLog.h"
#include "H2ConsoleCommands.h"
#include "H2Config.h"
#include <time.h>

extern ConsoleCommands* commands;


extern void InitInstance();
extern bool overrideUnicodeMessage;
extern MapManager* mapManager;


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

typedef struct XLIVE_INPUT_INFO {
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

IDirect3DTexture9* Primitive = NULL;

char* BuildText;
extern char* ServerStatus;
extern int MasterState;

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

inline void BuildVertex(D3DXVECTOR4 xyzrhw, D3DCOLOR color, CVertexList* vertexList, int index)
{
	vertexList[index].xyzrhw = xyzrhw;
	vertexList[index].dColor = color;
}

BOOL bIsCreated, bNeedsFlush;

DWORD dwOldFVF;
LPD3DXSPRITE pSprite;

bool lowFPSmode;
float lastPresentTime;
float lastRenderTime;
static LARGE_INTEGER timerFreq;
static LARGE_INTEGER counterAtStart;

float getElapsedTime(void) {
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	return (float)((c.QuadPart - counterAtStart.QuadPart) * 1000.0f / (float)timerFreq.QuadPart);
}

float desiredRenderTime = 17.041379f;
void frameTimeManagement() {
	float renderTime = getElapsedTime() - lastPresentTime;

	while (renderTime < desiredRenderTime) {
		SwitchToThread();
		renderTime = getElapsedTime() - lastPresentTime;
	}

	lastPresentTime = getElapsedTime();
}

DWORD dwPresent;
typedef HRESULT(WINAPI* tPresent)(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
tPresent pPresent;

HRESULT hkPresent(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	//DSFix FPS Hax - https://github.com/PeterTh/dsfix/blob/d10fc7ad0a72da0585b5f5f71b03daddc37ef890/RenderstateManager.cpp
	frameTimeManagement();

	return pPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}


LPDIRECT3DTEXTURE9 Texture_Interface;
LPD3DXSPRITE Sprite_Interface;

void GUI::Initialize()
{
	desiredRenderTime = (1000.f / H2Config_fps_limit);
	initFontsIfRequired();
	
	if (FAILED(D3DXCreateTextureFromFile(pDevice, "sounds/h2pc_logo.png", &Texture_Interface) ) )
	{
		addDebugText("ERROR: Failed to create logo texture (for achievements).");
	}

	D3DXCreateSprite(pDevice, &Sprite_Interface);

}

bool once1 = false;
// #5001
int WINAPI XLiveInput(XLIVE_INPUT_INFO* pPii)
{
	if (!once1) {
		extern HWND H2hWnd;
		extern RECT rectScreenOriginal;
		H2hWnd = pPii->hWnd;
		GetWindowRect(H2hWnd, &rectScreenOriginal);
		once1 = true;
	}
	return 1;
}

// #5030: XLivePreTranslateMessage
int WINAPI XLivePreTranslateMessage(const LPMSG lpMsg)
{
	return 0;
}
// #5000: XLiveInitialize
int WINAPI XLiveInitialize(XLIVE_INITIALIZE_INFO* pPii)
{
		InitInstance();
		TRACE("XLiveInitialize()");
		lastRenderTime = 0.0f;
		QueryPerformanceFrequency(&timerFreq);
		QueryPerformanceCounter(&counterAtStart);

		if (!h2mod->Server)
		{
			//TRACE("XLiveInitialize  (pPii = %X)", pPii);
			pDevice = (LPDIRECT3DDEVICE9)pPii->pD3D;
			pD3DPP = (D3DPRESENT_PARAMETERS*)pPii->pD3DPP;

			//pPresent = (HRESULT(WINAPI*)(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)) *(DWORD_PTR*)(pDevice + 17);
			//VirtualProtect((LPVOID)(pDevice + 17), sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &dwPresent);

			//*(DWORD_PTR*)(pDevice + 17) = (DWORD_PTR)hkPresent;

			BuildText = new char[250];
			snprintf(BuildText, 250, "Project Cartographer (v%s) - Build Time: %s %s", DLL_VERSION_STR, CompileDate, CompileTime);
	
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

	//pPresent = (HRESULT(WINAPI*)(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)) *(DWORD_PTR*)(pDevice+17);
	//VirtualProtect((LPVOID)(pDevice + 17), sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &dwPresent);
	//*(DWORD_PTR*)(pDevice + 17) = (DWORD_PTR)hkPresent;

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
	Sprite_Interface->OnLostDevice();
	Sprite_Interface->OnResetDevice();
	
	pD3DPP = vD3DPP;
	//TRACE("XLiveOnResetDevice");
	return 0;
}

// #5006 XLiveOnDestroyDevice
HRESULT WINAPI XLiveOnDestroyDevice()
{
	largeSizeFont->Release();
	normalSizeFont->Release();
	smallFont->Release();
	Sprite_Interface->Release();
	
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

typedef struct {
	int x;
	int y;
	bool xp;
	bool yp;
} exit_countdown_label;

static std::vector<exit_countdown_label*> exit_countdown_labels;

static void create_exit_countdown_label() {
	D3DVIEWPORT9 pViewport;
	pDevice->GetViewport(&pViewport);
	int width = pViewport.Width;
	int height = pViewport.Height;

	exit_countdown_label* exit_cnd_lbl = (exit_countdown_label*)malloc(sizeof(exit_countdown_label));
	exit_cnd_lbl->x = rand() % width;
	exit_cnd_lbl->y = rand() % height;
	exit_cnd_lbl->xp = rand() % 2 ? true : false;
	exit_cnd_lbl->yp = rand() % 2 ? true : false;

	exit_countdown_labels.push_back(exit_cnd_lbl);
}

unsigned long time_end = 0;
static int time_sec = 0;
static unsigned char add_exit_countdown_label = 4;
static char exit_countdown_timer_label[10] = "30:00";

bool StatusCheater = false;
int achievement_height = 0;
bool achievement_freeze = false;
int achievement_timer = 0;

char* Auto_Update_Text = 0;

// #5002: XLiveRender
int WINAPI XLiveRender()
{

	if (pDevice)
	{
		if (pDevice->TestCooperativeLevel() == D3D_OK)
		{
			D3DVIEWPORT9 pViewport;
			pDevice->GetViewport(&pViewport);
			//pViewport
			/*char textttt[255];
			sprintf(textttt, "x:%d, y:%d", pViewport.Width, pViewport.Height);
			drawText(100, 50, COLOR_WHITE, textttt, smallFont);*/

			D3DDEVICE_CREATION_PARAMETERS cparams;
			pDevice->GetCreationParameters(&cparams);
			RECT gameWindowRect;
			GetWindowRect(cparams.hFocusWindow, &gameWindowRect);
			RECT gameWindowInnerRect;
			GetClientRect(cparams.hFocusWindow, &gameWindowInnerRect);

			int gameWindowWidth = gameWindowRect.right - gameWindowRect.left - GetSystemMetrics(SM_CXSIZEFRAME);
			int gameWindowHeight = gameWindowRect.bottom - gameWindowRect.top;
			//TODO: move into chatbox commands
			//drawPrimitiveRect(0, 0, gameWindowWidth, 200, D3DCOLOR_ARGB(255, 000, 000, 0));
			if (commands->console) {
				int x = 0, y = 0;
				int height = 400;
				float startingPosY = height - 15.0f;
				drawPrimitiveRect(x, y, gameWindowWidth, height, D3DCOLOR_ARGB(155, 000, 000, 000));
				//drawFilledBox(x, y, gameWindowWidth, height, D3DCOLOR_ARGB(155, 000, 000, 000));
				drawText(0, startingPosY, COLOR_WHITE, ">>", normalSizeFont);
				drawText(35, startingPosY, COLOR_WHITE, commands->command.c_str(), normalSizeFont);

				startingPosY -= 12.0;
				std::vector<std::string>::iterator it;
				int i = 0;
				for (it = commands->prevCommands.begin(); it < commands->prevCommands.end(); it++, i++) {
					startingPosY -= 15.0;
					drawText(0, startingPosY, COLOR_WHITE, (*it).c_str(), normalSizeFont);
				}
			}
			DWORD GameGlobals = *(DWORD*)((BYTE*)h2mod->GetBase() + ((h2mod->Server) ? 0x4CB520 : 0x482D3C));
			DWORD& GameEngine = *(DWORD*)(GameGlobals + 0x8);
			bool paused_or_in_menus = *((BYTE*)h2mod->GetBase() + 0x47A568) != 0;
			if (GameEngine == 3 || StatusCheater || (GameEngine != 3 && paused_or_in_menus)) {
				drawText(0, 0, COLOR_WHITE, BuildText, smallFont);
				if (MasterState == 0)
					drawText(0, 15, COLOR_WHITE, ServerStatus, smallFont);
				else if (MasterState == 1)
					drawText(0, 15, COLOR_GREY, ServerStatus, smallFont);
				else if (MasterState == 2)
					drawText(0, 15, COLOR_RED, ServerStatus, smallFont);
				else if (MasterState == 10)
					drawText(0, 15, StatusCheater ? COLOR_YELLOW : COLOR_GREEN, ServerStatus, smallFont);
			}

#pragma region Achievement Rendering		
			if (h2mod->AchievementMap.size() > 0)
			{
				auto it = h2mod->AchievementMap.begin();
				
				if (it->second == false)
				{
					std::unique_lock<std::mutex> lck(h2mod->sound_mutex);

					h2mod->SoundMap[L"sounds/AchievementUnlocked.wav"] = 0;

					h2mod->sound_cv.notify_one();

					it->second = true;
				}
				
				if (achievement_height >= 150)
				{
					achievement_freeze = true;
				}
				else
					achievement_height = achievement_height + 2;

		

				float scalar = 11.0f;
				D3DXVECTOR3 Position;
				Position.x = (gameWindowWidth / 2 - 250 + 3) * scalar;
				Position.y = (gameWindowHeight - achievement_height + 3) * scalar;


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

				Sprite_Interface->Draw(Texture_Interface, NULL, NULL, &Position, 0xFFFFFFFF);
				Sprite_Interface->End();

				if (achievement_freeze == true)
				{

					if (achievement_timer >= 200)
					{
						achievement_freeze = false;
						achievement_timer = 0;
						achievement_height = 0;
						h2mod->AchievementMap.erase(it);
					}

					achievement_timer++;
				}

			}
#pragma endregion achievement rendering

			if (GameEngine == 3 && mapManager->getCustomLobbyMessage() != NULL) {
				drawText(0, 30, COLOR_GOLD, mapManager->getCustomLobbyMessage(), normalSizeFont);
			}

			time_t ltime;
			time(&ltime);//seconds since epoch.
			unsigned long time = (unsigned long)ltime;

			int diff;
			if (time_end && (diff = time_end - time) != time_sec) {
				time_sec = diff;
				int time_min = time_sec / 60;
				snprintf(exit_countdown_timer_label, 10, "%02d:%02d", time_min, time_sec % 60);
				if (time_sec <= 0) {
					BYTE& Quit_Exit_Game = *((BYTE*)h2mod->GetBase() + 0x48220b);
					Quit_Exit_Game = 1;
				}
				if (++add_exit_countdown_label >= 4) {
					add_exit_countdown_label = 0;
					create_exit_countdown_label();
				}
			}

			int width = pViewport.Width;
			int height = pViewport.Height;

			for (auto const &ent1 : exit_countdown_labels) {
				ent1->x += ent1->xp ? 1 : -1;
				ent1->y += ent1->yp ? 1 : -1;
				if (ent1->x > width) {
					ent1->xp = false;
				}
				else if (ent1->x < 0) {
					ent1->xp = true;
				}
				if (ent1->y > height) {
					ent1->yp = false;
				}
				else if (ent1->y < 0) {
					ent1->yp = true;
				}
				drawText(ent1->x - 25, ent1->y - 10, COLOR_WHITE, exit_countdown_timer_label, normalSizeFont);
			}
			
			if (getDebugTextDisplay()) {
				for (int i = 0; i < getDebugTextArrayMaxLen(); i++) {
					const char* text = getDebugText(i);
					//int yOffset = 40 + (i * 14);
					int yOffset = gameWindowHeight - 55 - (i * 14);
					if (yOffset < 35) {
						break;
					}
					if (strlen(text) > 0) {
						drawText(10, yOffset, COLOR_WHITE, text, smallFont);
					}
				}
			}

			if (Auto_Update_Text) {
				drawText(10, 60, COLOR_WHITE, Auto_Update_Text, normalSizeFont);
			}
			extern long long Size_Of_Download;
			extern long long Size_Of_Downloaded;
			if (Size_Of_Download > 0) {
				drawBox(10, 52, 200, 6, COLOR_RED, COLOR_RED);
				drawBox(10, 52, ((Size_Of_Downloaded * 100) / Size_Of_Download) * 2, 6, COLOR_GREEN, COLOR_GREEN);
			}

		}

		if (H2Config_fps_limit > 0) {
			frameTimeManagement();
		}
	}

	return 0;
}

