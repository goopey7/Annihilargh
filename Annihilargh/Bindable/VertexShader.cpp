#include "VertexShader.h"

#include <d3dcompiler.h>


#include "../GraphicsAnomalyMacros.h"

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{
	HRESULT hr;
	// create vertex shader
	GFX_THROW_FAILED(D3DReadFileToBlob(path.c_str(),&pBlob));
	GFX_THROW_FAILED(GetDevice(gfx)->CreateVertexShader(pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),nullptr,&pVertexShader));
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(),nullptr,0);
}

ID3DBlob* VertexShader::GetBlob() const noexcept
{
	return pBlob.Get();
}
