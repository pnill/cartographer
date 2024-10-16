#include "stdafx.h"
#include "rasterizer_dx9_9on12.h"

#include "rasterizer/dx12/rasterizer_dx12_main.h"

using Microsoft::WRL::ComPtr;

/* globals */

bool g_rasterizer_dx9on12_enabled = false;
IDirect3DDevice9On12* g_d3d9on12_device = NULL;

/* public code */

HRESULT rasterizer_dx9_create_through_d3d9on12(IDirect3D9Ex** d3d, bool use_warp)
{
    // Create the WARP adapter
    ComPtr<IDXGIFactory4> dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr))
    {
        LOG_CRITICAL_GAME("Failed to create DXGIFactory.");
        return E_FAIL;
    }

    ComPtr<IDXGIAdapter> dxgi_adapter;
    if (use_warp)
    {
        if (FAILED(hr = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgi_adapter))))
        {
            LOG_CRITICAL_GAME("Failed to get WARP adapter.");
            return E_FAIL;
        }
    }
    else
    {
        if (FAILED(hr = dxgiFactory->EnumAdapters(0, &dxgi_adapter)))
        {
            LOG_CRITICAL_GAME("Failed to get adapter.");
            return E_FAIL;
        }
    }

    // Create D3D12 device using WARP
    ComPtr<ID3D12Device> d3d12_device;
    hr = D3D12CreateDevice(
        dxgi_adapter.Get(),           // Adapter
        D3D_FEATURE_LEVEL_11_0,      // Minimum feature level
        IID_PPV_ARGS(&d3d12_device)); // D3D12 device

    if (FAILED(hr))
    {
        LOG_CRITICAL_GAME("Failed to create D3D12 device.");
        return E_FAIL;
    }

    LOG_INFO_GAME("D3D12 device created successfully using WARP!");

    // Create D3D9On12
    D3D9ON12_ARGS Args = {};
    Args.Enable9On12 = TRUE;
    Args.pD3D12Device = d3d12_device.Get();
    *d3d = (IDirect3D9Ex*)Direct3DCreate9On12(D3D_SDK_VERSION, &Args, 1);

    if (FAILED(hr))
    {
        LOG_CRITICAL_GAME("Failed to initialize D3D9On12.");
        return E_FAIL;
    }

    d3d12_device->AddRef();
    g_d3d12_device = d3d12_device.Get();

    LOG_INFO_GAME("D3D9On12 initialized successfully!");

    return 0;
}
