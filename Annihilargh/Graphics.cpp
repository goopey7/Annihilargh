#include "Graphics.h"

#include <ostream>
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Window.h"

// these namespaces will just make things look nicer
namespace wrl = Microsoft::WRL;
namespace dx=DirectX;

// this way you don't have to worry about configuring your project correctly.
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

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

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float colour[] = {r, g, b, 1.f};
	pDeviceContext->ClearRenderTargetView(pTargetView.Get(), colour);
}

void Graphics::DrawTestTriangle(float angle,float x,float y)
{
	HRESULT hr;
	namespace wrl = Microsoft::WRL;
	struct Vertex
	{
		struct
		{
			float x, y;
		} pos;

		struct
		{
			unsigned char r,g,b,a;
		} colour;
		
	};
	const Vertex vertices[] =
	{
		{0.f, 0.5f,255,0,255,0},
		{0.5f, -0.5f,0,255,0,0},
		{-0.5f, -0.5f,0,0,255,0},
		{-.3f,.3f,0,0,255,0},
		{.3f,.3f,0,0,255,0},
		{0.f,-.8f,255,0,0,0},
	};
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices;
	GFX_THROW_FAILED(pDevice->CreateBuffer(&bufferDesc, &subresourceData, &pVertexBuffer));

	// index buffer for indexed drawing
	const unsigned short indices[] =
	{
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5,
	};

	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_FAILED(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// bind index buffer to pipeline
	pDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(),DXGI_FORMAT_R16_UINT,0u);

	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		// 4x4 floating point matrix. Optimised for SIMD, so can't directly access elements.
		// only interfaced with using directxmath
		dx::XMMATRIX transform;
	};
	const ConstantBuffer cb=
	{
		//rotation around z-axis
		//we have to scale the x-axis by 3/4
		//these matrices are row-major, and the shader is more efficient with column-major matrices,
		// so we transpose the result to make it column-major. Which makes sense because 1 operation on the CPU
		// optimises thousands of operations on the GPU.
{
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixScaling(.75f,1.f,1.f)*
				dx::XMMatrixTranslation(x,y,0.f))
		}
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC; //updates every frame
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_FAILED(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// bind constant buffer to the vertex shader
	pDeviceContext->VSSetConstantBuffers(0u,1u,pConstantBuffer.GetAddressOf());
	
	const UINT stride = sizeof(Vertex);
	const UINT offset=0u;
	pDeviceContext->IASetVertexBuffers(0u,1u,pVertexBuffer.GetAddressOf(),&stride,
		&offset);

	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	
	// create and bind pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_FAILED(D3DReadFileToBlob(L"PixelShader.cso",&pBlob));
	GFX_THROW_FAILED(pDevice->CreatePixelShader(pBlob->GetBufferPointer()
        ,pBlob->GetBufferSize(),nullptr,&pPixelShader));
	pDeviceContext->PSSetShader(pPixelShader.Get(),nullptr,0u);
	
	// create and bind vertex shader
	GFX_THROW_FAILED(D3DReadFileToBlob(L"VertexShader.cso",&pBlob));
	GFX_THROW_FAILED(pDevice->CreateVertexShader(pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),nullptr,&pVertexShader));
	pDeviceContext->VSSetShader(pVertexShader.Get(),nullptr,0);

	// input layout
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[]=
	{
		{"Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,
            D3D11_INPUT_PER_VERTEX_DATA,0},
		//UNORM will not only convert our byte to float for the shader, but will also change the value so it works.
		//So 255 becomes 1.f
		{"Colour",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,
    D3D11_INPUT_PER_VERTEX_DATA,0},
    };
	GFX_THROW_FAILED(pDevice->CreateInputLayout(ied,(UINT)std::size(ied),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout));

	pDeviceContext->IASetInputLayout(pInputLayout.Get());

	// bind render target
	pDeviceContext->OMSetRenderTargets(1u,pTargetView.GetAddressOf(),nullptr);

	// configure the viewport
	D3D11_VIEWPORT viewport;
	viewport.Width=800;
	viewport.Height=600;
	viewport.MinDepth=0;
	viewport.MaxDepth=1;
	viewport.TopLeftX=0;
	viewport.TopLeftY=0;
	pDeviceContext->RSSetViewports(1u,&viewport);
	
	// create triangle list
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	pDeviceContext->DrawIndexed((UINT)std::size(indices),0u,0u);
}
