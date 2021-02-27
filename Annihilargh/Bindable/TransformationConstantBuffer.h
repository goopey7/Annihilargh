#pragma once
#include "Bindable.h"
#include "ConstantBuffer.h"
#include "../Drawable/Drawable.h"

class TransformationConstantBuffer : public Bindable
{
public:
	TransformationConstantBuffer(Graphics &gfx, const Drawable &parent);
	void Bind(Graphics& gfx) noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVertexConstantBuffer;
	const Drawable &parent;
};
