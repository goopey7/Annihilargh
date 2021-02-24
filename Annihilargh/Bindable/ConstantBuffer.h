#pragma once
#include "Bindable.h"
#include "../Graphics.h"
#include "../GraphicsAnomalyMacros.h"

// just like the vertex buffer, we define the constant buffer ourselves, so let's template that
template<typename C>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(Graphics &gfx)
	{
		HRESULT hr;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC; //updates every frame
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;
		GFX_THROW_FAILED(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	};
	ConstantBuffer(Graphics &gfx, const C &constants)
	{
		HRESULT hr;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC; //updates every frame
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(constants);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &constants;
		GFX_THROW_FAILED(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}

	// Updated every frame
	// Before we were creating and destroying the constant buffer every frame
	// now if we want to change anything in between frames we can use Update
	void Tick(Graphics &gfx, const C &constants)
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE msr;
		// to update a constant buffer you call map from the context
		// this function locks the ptr and fills a ptr with writable memory
		// when done we call unmap
		GFX_THROW_FAILED(GetContext(gfx)->Map(
            pConstantBuffer.Get(),0u,
            D3D11_MAP_WRITE_DISCARD,0u,&msr));

		// copy data into subresource
		memcpy(msr.pData, &constants,sizeof(constants));
		GetContext(gfx)->Unmap(pConstantBuffer.Get(),0u);
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		// bind to vertex shader
		Bindable::GetContext(gfx)->VSSetConstantBuffers(0u,1u,
			ConstantBuffer<C>::pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics &gfx) noexcept override
	{
		// bind to pixel shader
		Bindable::GetContext(gfx)->PSSetConstantBuffers(0u,1u,
			ConstantBuffer<C>::pConstantBuffer.GetAddressOf());
	}
};