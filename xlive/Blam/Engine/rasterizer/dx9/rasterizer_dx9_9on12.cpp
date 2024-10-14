#include "stdafx.h"
#include "rasterizer_dx9_9on12.h"

#ifdef D3D9_ON_12_ENABLED

#include "rasterizer/rasterizer_globals.h"

#include <d3d9on12.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

HRESULT rasterizer_dx9_create_through_d3d9on12(IDirect3D9Ex** d3d, ID3D12Device** d3d12, bool use_warp)
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

    LOG_CRITICAL_GAME("D3D12 device created successfully using WARP!");

    // Create D3D9On12
    D3D9ON12_ARGS Args = {};
    Args.Enable9On12 = TRUE;
    Args.pD3D12Device = d3d12_device.Get();
    if (rasterizer_globals_get()->use_d3d9_ex)
    {
        hr = Direct3DCreate9On12Ex(D3D_SDK_VERSION, &Args, 1, d3d);
    }
    else
    {
        *d3d = (IDirect3D9Ex*)Direct3DCreate9On12(D3D_SDK_VERSION, &Args, 1);
    }

    if (FAILED(hr))
    {
        LOG_CRITICAL_GAME("Failed to initialize D3D9On12.");
        return E_FAIL;
    }

    d3d12_device->AddRef();
    *d3d12 = d3d12_device.Get();

    LOG_CRITICAL_GAME("D3D9On12 initialized successfully!");

    return 0;
}

#endif