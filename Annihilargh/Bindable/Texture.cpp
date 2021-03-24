#include "Texture.h"

#include "../GraphicsAnomalyMacros.h"

namespace wrl = Microsoft::WRL;
Texture::Texture(Graphics& gfx, const Image& img, unsigned int slot) : slot(slot)
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
	// pitch is the distance in bytes between the first pixel in row 0 and the first pixel in row 2.
	sd.SysMemPitch = img.GetWidth()*sizeof(Image::Colour);
	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_FAILED(GetDevice(gfx)->CreateTexture2D(&texDesc,&sd,&pTexture));

	// create the resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels=1;
	GFX_THROW_FAILED(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(),&srvDesc,&pTextureView));
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot,1u,pTextureView.GetAddressOf());
}
