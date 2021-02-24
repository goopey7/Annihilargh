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
	VertexConstantBuffer<DirectX::XMMATRIX> vertexConstantBuffer;
	const Drawable &parent;
};
