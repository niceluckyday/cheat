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
#include <util/Logger.h>

void SetupImGuiStyle();

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

		//Set OriginalWndProcHandler to the Address of the Original WndProc function
		OriginalWndProcHandler = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(hWndProc)));
		
		SetupImGuiStyle();

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);
		io.SetPlatformImeDataFn = nullptr; // F**king bug take 4 hours of my live

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

void SetupImGuiStyle()
{
	ImGui::GetStyle().FrameRounding = 4.0f;
	ImGui::GetStyle().GrabRounding = 4.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_TextDisabled] = ImVec4(0.10f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.05f, 0.06f, 0.07f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.04f, 0.06f, 0.07f, 1.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.11f, 0.12f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.06f, 0.10f, 0.14f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.06f, 0.08f, 0.09f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.10f, 0.12f, 0.65f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.54f, 0.64f, 0.78f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.73f, 0.83f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.68f, 0.80f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.12f, 0.16f, 0.18f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.37f, 0.47f, 0.62f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.37f, 0.51f, 0.64f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.21f, 0.27f, 0.31f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.43f, 0.55f, 0.71f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.34f, 0.44f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.31f, 0.45f, 0.60f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.42f, 0.57f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.64f, 0.79f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.65f, 0.75f, 0.87f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.43f, 0.55f, 0.70f, 0.95f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.44f, 0.56f, 0.71f, 0.80f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 1.00f, 0.35f);

	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}
