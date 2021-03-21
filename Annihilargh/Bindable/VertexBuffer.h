#pragma once
#include <vector>


#include "Bindable.h"
#include "../GraphicsAnomalyMacros.h"
#include "../VertexSystem.h"

class VertexBuffer : public Bindable
{
public:
	// template the constructor because we define the vertex structure
	// we don't care about the actual struct but the data and size are what matter
	// so template works just fine
	VertexBuffer(Graphics &gfx, const alrg::VertexBuffer &vb) : stride((UINT)vb.GetLayout().Size())
	{
		HRESULT hr;
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0u;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.ByteWidth = UINT(vb.SizeBytes());
		bufferDesc.StructureByteStride = stride;
		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = vb.GetData();
		GFX_THROW_FAILED(GetDevice(gfx)->CreateBuffer(&bufferDesc, &subresourceData, &pVertexBuffer));
	}

	void Bind(Graphics &gfx) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};
