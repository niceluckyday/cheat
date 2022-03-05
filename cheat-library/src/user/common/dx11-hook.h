#pragma once
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <common/Event.h>

void InitializeDX11Hooks();

class DX11Events {
public:
	inline static TEvent<ID3D11DeviceContext*> RenderEvent{};
	inline static TEvent<HWND, ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*> InitializeEvent{};
};