#include "stdafx.h"
#include "uithread.h"

#include "imgui.h"

#include "backends/imgui_impl_dx9.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"

#ifndef IMGUI_DISABLE
extern LRESULT IMGUI_IMPL_API ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

static HWND H2hWnd;
static D3DPRESENT_PARAMETERS g_d3dPresentParameters;

IDirect3DDevice9Ex* g_xlive_d3d_device;
CRITICAL_SECTION g_render_section;

void InitializeD3D9(D3DPRESENT_PARAMETERS* presentParameters)
{
	InitializeCriticalSection(&g_render_section);

	g_d3dPresentParameters = *presentParameters;

#ifndef IMGUI_DISABLE
	ImGuiHandler::Initalize(presentParameters->hDeviceWindow);
#endif
}

void D3D9ReleaseResources()
{
#ifndef IMGUI_DISABLE
	ImGuiHandler::release_motd_texture();
	ImGui_ImplDX9_InvalidateDeviceObjects();
#endif
}

// #5005: XLiveOnCreateDevice
int WINAPI XLiveOnCreateDevice(IUnknown* pD3D, VOID* vD3DPP)
{
	//LOG_TRACE_XLIVE("XLiveOnCreateDevice  (pD3D = %X, pD3DPP = %X)", pD3D, vD3DPP);
	return 0;
}

// #5007: XLiveOnResetDevice
int WINAPI XLiveOnResetDevice(D3DPRESENT_PARAMETERS* pD3DPP)
{
	g_d3dPresentParameters = *pD3DPP;

	//Have to invalidate ImGUI on device reset, otherwise it hangs the device in a reset loop.
	//https://github.com/ocornut/imgui/issues/1464#issuecomment-347469716

	D3D9ReleaseResources();

	//LOG_TRACE_XLIVE("XLiveOnResetDevice");
	return 0;
}

// #5006 XLiveOnDestroyDevice
HRESULT WINAPI XLiveOnDestroyDevice()
{
	D3D9ReleaseResources();

	//LOG_TRACE_XLIVE("XLiveOnDestroyDevice");
	return S_OK;
}

// #5001
int WINAPI XLiveInput(XLIVE_INPUT_INFO* pPii)
{
	static bool has_initialised_input = false;
	if (!has_initialised_input)
	{
		H2hWnd = pPii->hWnd;
		has_initialised_input = true;
	}

	if ((pPii->uMSG == WM_KEYDOWN || pPii->uMSG == WM_SYSKEYDOWN)
		&& (GetKeyState(pPii->wParam) & 0x8000))
	{
		// hotkeys
		KeyboardInput::ExecuteHotkey(pPii->wParam);
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
	EnterCriticalSection(&g_render_section);

	if (!g_xlive_d3d_device)
	{
		LeaveCriticalSection(&g_render_section);
		return E_UNEXPECTED;
	}

	if (FAILED(g_xlive_d3d_device->TestCooperativeLevel()))
	{
		LeaveCriticalSection(&g_render_section);
		return E_UNEXPECTED;
	}

#ifndef IMGUI_DISABLE
	ImGuiHandler::DrawImgui();
#endif

	LeaveCriticalSection(&g_render_section);
	return S_OK;
}



