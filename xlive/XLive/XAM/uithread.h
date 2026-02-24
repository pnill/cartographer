#pragma once

/* structures */

typedef struct XLIVE_INPUT_INFO {
	UINT cbSize;
	HWND hWnd;
	UINT uMSG;
	WPARAM wParam;
	LPARAM lParam;
	BOOL fHandled;
	LRESULT lRet;
} XLIVE_INPUT_INFO;

/* prototypes */

void InitializeD3D9(D3DPRESENT_PARAMETERS* presentParameters);

void D3D9ReleaseResources();

int WINAPI XLiveOnCreateDevice(IUnknown* pD3D, VOID* vD3DPP);

int WINAPI XLiveOnResetDevice(D3DPRESENT_PARAMETERS* pD3DPP);

HRESULT WINAPI XLiveOnDestroyDevice();


/* globals */

extern IDirect3DDevice9Ex* g_xlive_d3d_device;

extern CRITICAL_SECTION g_render_section;
