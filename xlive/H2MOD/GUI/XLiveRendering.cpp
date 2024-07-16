#include "stdafx.h"

#include "XLiveRendering.h"
#include "imgui.h"
#include "backends/imgui_impl_dx9.h"

#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"



extern void InitInstance();
extern LRESULT IMGUI_IMPL_API ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool displayXyz = false;
bool doDrawIMGUI = false;

HWND H2hWnd;
LPDIRECT3DDEVICE9 g_pD3DDevice9;
D3DPRESENT_PARAMETERS g_d3dPresentParameters;

IDirect3DTexture9* Primitive = NULL;

char* buildText = nullptr;

const char CompileDate[] = __DATE__;
const char CompileTime[] = __TIME__;

int verticalRes = 0;
int horizontalRes = 0;

extern HMODULE hThis;
extern std::wstring ModulePathW(HMODULE);

std::wstring GetDirectoryFile(wchar_t *filename)
{
	std::wstring dirFile(ModulePathW(hThis));
	dirFile.append(filename);
	return dirFile;
}

void XLiveRendering::InitializeD3D9(LPDIRECT3DDEVICE9 pD3D9Device, D3DPRESENT_PARAMETERS* presentParameters)
{
	g_pD3DDevice9 = pD3D9Device;
	// increase the Ref count
	g_pD3DDevice9->AddRef();
	memcpy(&g_d3dPresentParameters, presentParameters, sizeof(D3DPRESENT_PARAMETERS));

	ImGuiHandler::Initalize(g_pD3DDevice9, presentParameters->hDeviceWindow);
}

void XLiveRendering::D3D9ReleaseResources()
{
	ImGuiHandler::ReleaseTextures();
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

// #5297: XLiveInitializeEx
int WINAPI XLiveInitializeEx(XLIVE_INITIALIZE_INFO* pXii, DWORD dwVersion)
{
	LOG_TRACE_XLIVE("XLiveInitializeEx()");

	InitInstance();

	if (pXii->pD3D) {
		XLiveRendering::InitializeD3D9((LPDIRECT3DDEVICE9)pXii->pD3D, (D3DPRESENT_PARAMETERS*)pXii->pD3DPP);

		buildText = new char[256];
		snprintf(buildText, 256, "Project Cartographer (v%s) - Build Time: %s %s", DLL_VERSION_STR, CompileDate, CompileTime);
	}

	UpdateMasterStatus(-1, "Status: Initializing");

	LOG_TRACE_XLIVE("XLiveInitializeEx() - dwVersion = {0:x}", dwVersion);
	return 0;
}

// #5000: XLiveInitialize
int WINAPI XLiveInitialize(XLIVE_INITIALIZE_INFO* pXii)
{
	return XLiveInitializeEx(pXii, 0);
}

// #5003: XLiveUninitialize
int WINAPI XLiveUninitialize()
{
	LOG_TRACE_XLIVE("XLiveUninitialize");
	
	XLiveRendering::D3D9ReleaseResources();

	if (g_pD3DDevice9)
	{
		g_pD3DDevice9->Release();
		g_pD3DDevice9 = NULL;
	}

	return 0;
}

// #5005: XLiveOnCreateDevice
int WINAPI XLiveOnCreateDevice(IUnknown* pD3D, VOID* vD3DPP)
{
	g_pD3DDevice9 = (LPDIRECT3DDEVICE9)pD3D;
	
	//pPresent = (HRESULT(WINAPI*)(LPDIRECT3DDEVICE9 pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)) *(DWORD_PTR*)(pDevice+17);
	//VirtualProtect((LPVOID)(pDevice + 17), sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &dwPresent);
	//*(DWORD_PTR*)(pDevice + 17) = (DWORD_PTR)hkPresent;

	//LOG_TRACE_XLIVE("XLiveOnCreateDevice  (pD3D = %X, pD3DPP = %X)", pD3D, vD3DPP);
	return 0;
}

// #5007: XLiveOnResetDevice
int WINAPI XLiveOnResetDevice(D3DPRESENT_PARAMETERS* pD3DPP)
{
	memcpy(&g_d3dPresentParameters, pD3DPP, sizeof(D3DPRESENT_PARAMETERS));

	//Have to invalidate ImGUI on device reset, otherwise it hangs the device in a reset loop.
	//https://github.com/ocornut/imgui/issues/1464#issuecomment-347469716

	ImGuiHandler::ReleaseTextures();
	ImGui_ImplDX9_InvalidateDeviceObjects();
	//LOG_TRACE_XLIVE("XLiveOnResetDevice");
	return 0;
}

// #5006 XLiveOnDestroyDevice
HRESULT WINAPI XLiveOnDestroyDevice()
{
	ImGuiHandler::ReleaseTextures();
	ImGui_ImplDX9_InvalidateDeviceObjects();
	XLiveRendering::D3D9ReleaseResources();
	
	//LOG_TRACE_XLIVE("XLiveOnDestroyDevice");
	return S_OK;
}

void InitalizeFont(std::wstring strFontName, const std::wstring& strFontPath, int size, IDirect3DDevice9* pD3Ddev, bool OnOff)
{
}


void InitFontsIfRequired()
{
}

int achievement_height = 0;
bool achievement_freeze = false;
int achievement_timer = 0;

char* autoUpdateText = 0;

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

// #5002: XLiveRender
HRESULT WINAPI XLiveRender()
{
	static std::mutex renderMtx;
	std::lock_guard lg(renderMtx);

	if (!g_pD3DDevice9) {
		return E_UNEXPECTED;
	}

	if (FAILED(g_pD3DDevice9->TestCooperativeLevel())) {
		return E_UNEXPECTED;
	}

	D3DVIEWPORT9 pViewport;
	g_pD3DDevice9->GetViewport(&pViewport);

	D3DDEVICE_CREATION_PARAMETERS cparams;
	g_pD3DDevice9->GetCreationParameters(&cparams);
	RECT gameWindowRect, gameWindowInnerRect;
	GetWindowRect(cparams.hFocusWindow, &gameWindowRect);
	GetClientRect(cparams.hFocusWindow, &gameWindowInnerRect);

	int gameWindowWidth = gameWindowRect.right - gameWindowRect.left - GetSystemMetrics(SM_CXSIZEFRAME);
	int gameWindowHeight = gameWindowRect.bottom - gameWindowRect.top;

	//if (displayXyz && (NetworkSession::LocalPeerIsSessionHost() || game_is_campaign())) {
	//	int text_y_coord = 60;
	//	player_iterator player_it;
	//	while (player_it.get_next_active_player())
	//	{
	//		real_point3d* player_position = s_player::get_unit_coords(player_it.get_current_player_index());
	//		object_datum* biped_unit = (object_datum*)s_player::get_player_unit_data(player_it.get_current_player_index());
	//		if (player_position != nullptr) {
	//			std::wstring playerNameWide(player_it.get_current_player_name());
	//			std::string playerName(playerNameWide.begin(), playerNameWide.end());
	//			std::string xyzText =
	//				"Player name: " + playerName +
	//				", xyz = " + std::to_string(player_position->x) + " "
	//				+ std::to_string(player_position->y) + " "
	//				+ std::to_string(player_position->z) + " "
	//				+ "Velocity: " + std::to_string(magnitude3d(&biped_unit->translational_velocity));
	//			drawText(0, text_y_coord, COLOR_GOLD, xyzText.c_str(), normalSizeFont);
	//			text_y_coord += 15;
	//		}
	//	}
	//}

	ImGuiHandler::DrawImgui();

	// limit framerate if needed
	// UPDATE: frame limiting in XLiveRender adds input lag
	// shell_windows_throttle_framerate(H2Config_fps_limit);

	return S_OK;
}

