#include "Texture.h"

#include "../GraphicsAnomalyMacros.h"

namespace wrl = Microsoft::WRL;
Texture::Texture(Graphics& gfx, const Image& img)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = img.GetWidth();
	texDesc.Height = img.GetHeight();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count=1;
	texDesc.SampleDesc.Quality=0;
	texDesc.Usage=D3D11_USAGE_DEFAULT;
	texDesc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = img.GetBufferPtr();
	sd.SysMemPitch = img.GetWidth()*sizeof(Image::Colour);
	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_FAILED(GetDevice(gfx)->CreateTexture2D(&texDesc,&sd,&pTexture));
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(0u,1u,pTextureView.GetAddressOf());
}
