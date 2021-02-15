#include "Graphics.h"

#include <ostream>
#include <sstream>

#include "Window.h"
namespace wrl = Microsoft::WRL;
#pragma comment(lib,"d3d11.lib")

// checks the return value of an expression that returns an hresult to see if it is a failure code
// if it is we throw a gfx anomaly with the hresult
// expects the hr variable to be in the local scope
#define GFX_THROW_FAILED(hrcall) if(FAILED(hr=(hrcall)))throw Graphics::AnomalyHresult(__LINE__,__FILE__,hr)

// creates a device removed anomaly
#define GFX_DEVICE_REMOVED_ANOMALY(hr) Graphics::DeviceRemovedAnomaly(__LINE__,__FILE__,(hr))

// ERROR HANDLING - Similar to how window works
Graphics::AnomalyHresult::AnomalyHresult(int line, const char* file, HRESULT hr) noexcept
:
Anomaly(line,file),
hr(hr)
{
}

const char* Graphics::AnomalyHresult::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
	<< "Error Code: 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec
	<< " (" << (unsigned long)GetErrorCode() << ")" << std::endl
	<< "Error String: " << GetErrorString() << std::endl
	<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::AnomalyHresult::GetType() const noexcept
{
	return "Graphics Anomaly";
}

HRESULT Graphics::AnomalyHresult::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::AnomalyHresult::GetErrorString() const noexcept
{
	/*TODO I don't really like that I'm having to deal with window to get this function
	 * There must be a better way
	 */
	return Window::Anomaly::TranslateErrorCode(hr);
}


const char* Graphics::DeviceRemovedAnomaly::GetType() const noexcept
{
	return "Graphics Anomaly: Device Removed";
}

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

	// for checking the hresults returned by d3d functions
	HRESULT hr;
	UINT swapFlags = 0u;
#ifndef NDEBUG
	swapFlags=D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr, // choose default gfx card
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // if we wanted to load a software driver
		swapFlags,
		nullptr, // feature level for the gfx card
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pDeviceContext));
	// the wrl comptr overloads the addressof operator. How convenient! Keep in mind when using the operator it does
	// also call release which is mostly convenient because we usually use the operator to modify the ptr anyway
	// use .addressOf() if you don't want to call release

	// gain access to back buffer
	wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	// 0 gives us the index, our back buffer, the uuid of the interface, the ptr to fill.
	GFX_THROW_FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTargetView));

	pBackBuffer->Release();
}

void Graphics::EndFrame()
{
	HRESULT hr;
	// SyncInterval according to msdn:
	// 0: Presentation occurs immediately. No synchronization
	// 1-4: Synchronize presentation for at least n vertical blocks
	if(FAILED(hr = pSwapChain->Present(1u, 0u)))
	{
		// occurs usually due to some kind of gfx driver failure
		if(hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_ANOMALY(pDevice->GetDeviceRemovedReason());
		}
		GFX_THROW_FAILED(hr);
	}
	
	
}
