#include "pch-il2cpp.h"

#include "dx11hook.h"

#include <cstdio>
#include <iostream>

// DX11 imports
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")

//ImGUI imports
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#include "HookManager.h"

// D3X HOOK DEFINITIONS
typedef HRESULT(__stdcall* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

// Definition of WndProc Hook. Its here to avoid dragging dependencies on <windows.h> types.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND window = nullptr;
static WNDPROC OriginalWndProcHandler = nullptr;

static IDXGISwapChainPresent fnIDXGISwapChainPresent;

static short ioFlag;

// Callbacks
static RenderCallback renderCallback;
static OnKeyUpCallback onKeyUpCallback;

// Font
static LPBYTE pFont;
static DWORD dFontSize;
static ImFont* fontUnicode;

LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	POINT mPos;
	GetCursorPos(&mPos);
	ScreenToClient(window, &mPos);
	ImGui::GetIO().MousePos.x = mPos.x;
	ImGui::GetIO().MousePos.y = mPos.y;

	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (uMsg == WM_KEYUP) {
		onKeyUpCallback(wParam, ioFlag);
	}

	if (ioFlag & BType_SendImGUI)
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (ioFlag & BType_Blocked)
		return true;

	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall Present(IDXGISwapChain* pChain, const UINT SyncInterval, const UINT Flags)
{
	static BOOL g_bInitialised = false;

	// Main D3D11 Objects
	static ID3D11DeviceContext* pContext = nullptr;
	static ID3D11Device* pDevice = nullptr;
	static ID3D11RenderTargetView* mainRenderTargetView;

	if (!g_bInitialised) {
		pChain->GetDevice(__uuidof(pDevice), reinterpret_cast<void**>(&pDevice));
		pDevice->GetImmediateContext(&pContext);

		DXGI_SWAP_CHAIN_DESC sd;
		pChain->GetDesc(&sd);

		window = sd.OutputWindow;

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		if (pFont != nullptr)
			fontUnicode = io.Fonts->AddFontFromMemoryTTF(pFont, dFontSize, 16, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		else
			fontUnicode = io.FontDefault;

		ImGui::StyleColorsDark();

		//Set OriginalWndProcHandler to the Address of the Original WndProc function
		OriginalWndProcHandler = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(hWndProc)));

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);
		ImGui::GetIO().ImeWindowHandle = window;

		ID3D11Texture2D* pBackBuffer;

		pChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
		pBackBuffer->Release();

		g_bInitialised = true;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	ImGui::PushFont(fontUnicode);
	ImGui::StyleColorsDark();

	// render function
	renderCallback();

	ImGui::PopFont();
	ImGui::EndFrame();
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return callOrigin(Present, pChain, SyncInterval, Flags);
}


IDXGISwapChainPresent findDirect11Present()
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

void createOverlay(const RenderCallback renderFunction, const OnKeyUpCallback onKeyUpFunction, const LPBYTE fontData, const DWORD fontSize)
{
	renderCallback = renderFunction;
	onKeyUpCallback = onKeyUpFunction;
	pFont = fontData;
	dFontSize = fontSize;

	printf("Initializing D3D11 hook: started.\n");
	fnIDXGISwapChainPresent = findDirect11Present();
	if (fnIDXGISwapChainPresent == nullptr)
	{
		printf("[ERROR] Failed to find 'Present' function for D3D11.\n");
	}
	printf("SwapChain Present: %p\n", fnIDXGISwapChainPresent);

	HookManager::set(fnIDXGISwapChainPresent, Present);
	printf("Initializing D3D11 hook: done.\n");
}
