#include "Sampler.h"

#include "../GraphicsAnomalyMacros.h"

Sampler::Sampler(Graphics& gfx)
{
	HRESULT hr;
	
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_FAILED(GetDevice(gfx)->CreateSamplerState(&sampDesc,&pSampler));
}

void Sampler::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetSamplers(0u,1u,pSampler.GetAddressOf());
}
