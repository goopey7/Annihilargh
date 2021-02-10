#pragma once
#include "WindowsWithoutTheCrap.h"
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void EndFrame();

	void ClearBuffer(float r, float g, float b) noexcept
	{
		const float colour[] = {r, g, b, 1.f};
		pDeviceContext->ClearRenderTargetView(pTargetView, colour);
	}

private:
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11RenderTargetView* pTargetView = nullptr;
};
