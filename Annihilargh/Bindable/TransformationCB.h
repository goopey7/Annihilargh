#pragma once
#include "Bindable.h"
#include "ConstantBuffer.h"
#include "../Drawable/Drawable.h"

class TransformationCB : public Bindable
{
public:
	TransformationCB(Graphics &gfx, const Drawable &parent);
	void Bind(Graphics& gfx) noexcept override;
private:
	struct Transforms
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};
	
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVertexConstantBuffer;
	const Drawable &parent;
};
