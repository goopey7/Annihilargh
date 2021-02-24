#include "PixelShader.h"
#include <d3dcompiler.h>
#include "../GraphicsAnomalyMacros.h"
namespace wrl = Microsoft::WRL;

PixelShader::PixelShader(Graphics& gfx, const std::wstring& path)
{
	HRESULT hr;
	// create pixel shader
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(path.c_str(),&pBlob));
	GFX_THROW_FAILED(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer()
        ,pBlob->GetBufferSize(),nullptr,&pPixelShader));
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
	// bind pixel shader
	GetContext(gfx)->PSSetShader(pPixelShader.Get(),nullptr,0u);
}
