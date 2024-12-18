#pragma once
#include <Windows.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include "../CheatSDK/Data/DrawData.h"
#include "../CheatSDK/KeyHandler.h"
#include "../UI/Pages/MainMenu.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hooks {
	typedef long(*PresentFn)(IDXGISwapChain*, UINT, UINT);
	inline PresentFn oPresent;

	LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	long hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);


	inline bool HookDX11Old() {
		HWND hWnd = GetForegroundWindow();
		IDXGISwapChain* pSwapChain;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
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

		ID3D11Device* pDevice;
		ID3D11DeviceContext* pContext;

		auto hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1
			, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext);
		if (FAILED(hr))
		{
			MessageBox(hWnd, std::format("Failed to create DirectX device and swapchain!\nHRESULT: 0x{:X}", (uintptr_t)hr).c_str(), "Dota2Cheat", MB_ICONERROR);
			return false;
		}


		auto Present = (*(void***)pSwapChain)[8];
		auto res = HOOKFUNC(Present);

		pDevice->Release();
		pContext->Release();
		pSwapChain->Release();

		return res;
	}
	inline bool HookDirectX() {
		//HookDX11Old();

		// xref: "Hooking vtable for swap chain\n"
		auto Present = SignatureDB::FindSignature("IDXGISwapChain::Present");
		return HOOKFUNC(Present);
	}
}