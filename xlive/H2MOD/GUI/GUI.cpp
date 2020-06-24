#include "stdafx.h"
#include "Globals.h"

#include "GUI.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\Networking\NetworkStats\NetworkStats.h"
#include "H2MOD\Modules\Config\Config.h"


extern void InitInstance();

extern bool displayXyz;

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

int MasterState = 0;
char* BuildText = nullptr;
char* ServerStatus = nullptr;

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

using namespace std::chrono;
high_resolution_clock::time_point nextFrame;
high_resolution_clock::duration desiredRenderTime = duration_cast<high_resolution_clock::duration>(duration<double>(1.0 / (double)H2Config_fps_limit));

void frameTimeManagement() {

	static bool bInitTime = false;
	if (!bInitTime)
	{
		nextFrame = high_resolution_clock::now();
		bInitTime = true;
	}
	
	if (H2Config_fps_limit > 0) {
		std::this_thread::sleep_until(nextFrame);

		auto frameCount = duration<long long, std::micro>(
			(1 + (duration_cast<duration<long long, std::micro>>(high_resolution_clock::now() - nextFrame) / duration_cast<duration<long long, std::micro>>(desiredRenderTime)))
			);

		nextFrame += (desiredRenderTime * frameCount.count());
	}
}

LPDIRECT3DTEXTURE9 Texture_Interface;
LPD3DXSPRITE Sprite_Interface;

void GUI::Initialize()
{
	initFontsIfRequired();
	
	if (FAILED(D3DXCreateTextureFromFile(pDevice, L"sounds/h2pc_logo.png", &Texture_Interface) ) )
	{
		addDebugText("ERROR: Failed to create logo texture (for achievements).");
	}

	D3DXCreateSprite(pDevice, &Sprite_Interface);

}

// #5001
int WINAPI XLiveInput(XLIVE_INPUT_INFO* pPii)
{
	static bool has_initialised_input = false;
	if (!has_initialised_input) {
		extern HWND H2hWnd;
		extern RECT rectScreenOriginal;
		H2hWnd = pPii->hWnd;
		GetWindowRect(H2hWnd, &rectScreenOriginal);
		has_initialised_input = true;
	}

	return S_OK;
}

extern void handleHotkeyInput(WPARAM lpMsg);

// #5030: XLivePreTranslateMessage
BOOL WINAPI XLivePreTranslateMessage(const LPMSG lpMsg)
{
	if ((GetKeyState(lpMsg->wParam) & 0x8000) && (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN))
	{
		// hotkeys
		handleHotkeyInput(lpMsg->wParam);
		// console
		commands->handleInput(lpMsg->wParam);
	}

	return false;
}

// #5297: XLiveInitializeEx
int WINAPI XLiveInitializeEx(XLIVE_INITIALIZE_INFO* pXii, DWORD dwVersion)
{
	InitInstance();

	LOG_TRACE_XLIVE("XLiveInitializeEx()");

	if (!h2mod->Server)
	{
		//LOG_TRACE_XLIVE("XLiveInitialize  (pPii = %X)", pPii);
		pDevice = (LPDIRECT3DDEVICE9)pXii->pD3D;
		pD3DPP = (D3DPRESENT_PARAMETERS*)pXii->pD3DPP;

		ServerStatus = new char[250];
		snprintf(ServerStatus, 250, "Status: Initializing....");

		BuildText = new char[250];
		snprintf(BuildText, 250, "Project Cartographer (v%s) - Build Time: %s %s", DLL_VERSION_STR, CompileDate, CompileTime);

		GUI::Initialize();
	}
	LOG_TRACE_XLIVE("XLiveInitializeEx - dwVersion = {0:x}", dwVersion);
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
	pD3DPP = (D3DPRESENT_PARAMETERS*)vD3DPP;

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
	
	pD3DPP = vD3DPP;
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

void InitalizeFont(std::wstring strFontName, std::wstring& strFontPath, int size, IDirect3DDevice9* pD3Ddev, bool OnOff)
{
	if (OnOff)
	{
		addDebugText("Adding font: ");
		addDebugText(strFontPath.c_str());
		if(AddFontResource(strFontPath.c_str()) > 0)
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


void initFontsIfRequired()
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

				startingPosY -= 12.0f;
				for (std::string& it : commands->prevOutput) {
					startingPosY -= 15.0f;
					drawText(0, startingPosY, COLOR_WHITE, it.c_str(), normalSizeFont);
				}
			}

			DWORD GameGlobals = *h2mod->GetAddress<DWORD*>(0x482D3C, 0x4CB520);
			DWORD GameEngine = *(DWORD*)(GameGlobals + 0x8);
			bool paused_or_in_menus = (*h2mod->GetAddress<BYTE*>(0x47A568) != 0);

			if (GameEngine == 3 || (GameEngine != 3 && paused_or_in_menus)) {
				drawText(0, 0, COLOR_WHITE, BuildText, smallFont);
				if (MasterState == 0)
					drawText(0, 15, COLOR_WHITE, ServerStatus, smallFont);
				else if (MasterState == 1)
					drawText(0, 15, COLOR_GREY, ServerStatus, smallFont);
				else if (MasterState == 2)
					drawText(0, 15, COLOR_RED, ServerStatus, smallFont);
				else if (MasterState == 10)
					drawText(0, 15, COLOR_GREEN, ServerStatus, smallFont);
			}

			//drawPrimitiveRect(gameWindowWidth / 1.15, gameWindowHeight - 150, 250, 100, D3DCOLOR_ARGB(155, 41, 65, 129));
			//drawText(gameWindowWidth / 1.13, gameWindowHeight - 145, COLOR_WHITE, "Points: 10,000", haloFont);
	
#pragma region Achievement Rendering		
			if (h2mod->AchievementMap.size() > 0)
			{
				auto it = h2mod->AchievementMap.begin();
				
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
					if (achievement_timer >= 400)
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

			if (displayXyz && (NetworkSession::localPeerIsSessionHost() || h2mod->GetMapType() == scnr_type::SinglePlayer)) {
				int text_y_coord = 60;
				PlayerIterator playerIt;
				while (playerIt.get_next_player()) 
				{
					real_point3d* player_position = h2mod->get_player_unit_coords(playerIt.get_current_player_index());
					if (player_position != nullptr) {
						std::wstring playerNameWide(playerIt.get_current_player_name());
						std::string playerName(playerNameWide.begin(), playerNameWide.end());
						std::string xyzText = "Player name: " + playerName + ", xyz = " + std::to_string(player_position->x) + " " + std::to_string(player_position->y) + " " + std::to_string(player_position->z);
						drawText(0, text_y_coord, COLOR_GOLD, xyzText.c_str(), normalSizeFont);
						text_y_coord += 15;
					}
				}
			}
			
			if (getDebugTextDisplay()) {
				for (int i = 0; i < getDebugTextDisplayCount(); i++) {
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

			if (NetworkStatistics) {
				if (!NetworkSession::getCurrentNetworkSession(NULL)) {
					ElapsedTime = 0;
					TotalPacketsSent = 0;
				}
				sprintf(packet_info_str, "[ pck/second %d, pck size average: %d ]", ElapsedTime > 0 ? Packets * 1000 / ElapsedTime : 0, TotalPacketsSent > 0 ? TotalBytesSent / TotalPacketsSent : 0);
				drawText(30, 30, COLOR_WHITE, packet_info_str, normalSizeFont);
			}
		}
	}

	return 0;
}

