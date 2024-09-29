#include "stdafx.h"
#include "rasterizer_dx9_9on12.h"

#ifdef D3D9_ON_12_ENABLED

#include "rasterizer/rasterizer_globals.h"

#include <d3d9on12.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3d9.lib")

HRESULT rasterizer_dx9_create_through_d3d9on12(IDirect3D9Ex** d3d)
{
    // Initialize COM library
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    // Create the WARP adapter
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr)) {
        LOG_CRITICAL_GAME("Failed to create DXGIFactory.");
        return E_FAIL;
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
    hr = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
    if (FAILED(hr)) {
        LOG_CRITICAL_GAME("Failed to get WARP adapter.");
        return E_FAIL;
    }

    // Create D3D12 device using WARP
    Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device;
    hr = D3D12CreateDevice(
        warpAdapter.Get(),                // Adapter
        D3D_FEATURE_LEVEL_11_0,           // Minimum feature level
        IID_PPV_ARGS(&d3d12Device));      // D3D12 device

    if (FAILED(hr)) {
        LOG_CRITICAL_GAME("Failed to create D3D12 device.");
        return E_FAIL;
    }

    LOG_CRITICAL_GAME("D3D12 device created successfully using WARP!");

    // Create D3D9On12
    if (rasterizer_globals_get()->use_d3d9_ex)
    {
        hr = Direct3DCreate9On12Ex(D3D_SDK_VERSION, NULL, 0, d3d);
    }
    else
    {
        *d3d = (IDirect3D9Ex*)Direct3DCreate9On12(D3D_SDK_VERSION, NULL, 0);
    }

    if (FAILED(hr)) {
        LOG_CRITICAL_GAME("Failed to initialize D3D9On12.");
        return E_FAIL;
    }

    LOG_CRITICAL_GAME("D3D9On12 initialized successfully!");

    // Clean up
    CoUninitialize();

    return 0;
}

#endif