#include "pch-il2cpp.h"
#include "dx11-hook.h"

#include <cstdio>
#include <iostream>

#include <common/HookManager.h>
#include <common/Logger.h>

#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")

// D3X HOOK DEFINITIONS
typedef HRESULT(__stdcall* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

// Definition of WndProc Hook. Its here to avoid dragging dependencies on <windows.h> types.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static IDXGISwapChainPresent fnIDXGISwapChainPresent;

static HRESULT __stdcall Present(IDXGISwapChain* pChain, const UINT SyncInterval, const UINT Flags)
{
	static BOOL g_bInitialised = false;

	// Main D3D11 Objects
	static ID3D11DeviceContext* pContext = nullptr;
	static ID3D11Device* pDevice = nullptr;

	if (!g_bInitialised) {
		pChain->GetDevice(__uuidof(pDevice), reinterpret_cast<void**>(&pDevice));
		pDevice->GetImmediateContext(&pContext);

		DXGI_SWAP_CHAIN_DESC sd;
		pChain->GetDesc(&sd);

		DX11Events::InitializeEvent(sd.OutputWindow, pDevice, pContext, pChain);

		g_bInitialised = true;
	}

	// render function
	DX11Events::RenderEvent(pContext);

	return callOrigin(Present, pChain, SyncInterval, Flags);
}


static IDXGISwapChainPresent findDirect11Present()
{
	const HWND hWnd = GetForegroundWindow();
	IDXGISwapChain* pSwapChain;

	constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;//((GetWindowLong(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Main D3D11 Objects
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11Device* pDevice = nullptr;
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext)))
	{
		return nullptr;
	}

	const DWORD_PTR* pSwapChainVtable = reinterpret_cast<DWORD_PTR*>(pSwapChain);
	pSwapChainVtable = reinterpret_cast<DWORD_PTR*>(pSwapChainVtable[0]);

	auto swapChainPresent = reinterpret_cast<IDXGISwapChainPresent>(pSwapChainVtable[8]);

	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();

	return swapChainPresent;
}

void InitializeDX11Hooks() 
{
	LOG_DEBUG("Initializing D3D11 hook: started.");
	fnIDXGISwapChainPresent = findDirect11Present();
	if (fnIDXGISwapChainPresent == nullptr)
	{
		LOG_ERROR("Failed to find 'Present' function for D3D11.");
		return;
	}
	LOG_DEBUG("SwapChain Present: %p", fnIDXGISwapChainPresent);

	HookManager::install(fnIDXGISwapChainPresent, Present);
	LOG_DEBUG("Initializing D3D11 hook: done.");
}


