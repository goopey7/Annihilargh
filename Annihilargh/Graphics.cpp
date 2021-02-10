#include "Graphics.h"

#pragma comment(lib,"d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	// setting these to zero means that it will fit to the size of the window
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	// pick whatever refresh rate
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1; // one front buffer and one back buffer (double buffering)
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr, // choose default gfx card
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // if we wanted to load a software driver
		0,
		nullptr, // feature level for the gfx card
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pDeviceContext);

	// gain access to back buffer
	ID3D11Resource* pBackBuffer = nullptr;
	// 0 gives us the index, our back buffer, the uuid of the interface, the ptr to fill.
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));

	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTargetView);

	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pTargetView != nullptr)
		pTargetView->Release();
	if (pDevice != nullptr)
		pDeviceContext->Release();
	if (pSwapChain != nullptr)
		pSwapChain->Release();
	if (pDevice != nullptr)
		pDevice->Release();
}

void Graphics::EndFrame()
{
	pSwapChain->Present(1u, 0u);
}
