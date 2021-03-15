#pragma once
#include "DrawableBase.h"

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere(Graphics &gfx, float radius);
	void Tick(float deltaTime) noexcept override;
	void SetLocation(DirectX::XMFLOAT3 _pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = {1.f,1.f,1.f};
};
