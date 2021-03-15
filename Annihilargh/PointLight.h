#pragma once
#include "Graphics.h"
#include "Bindable/ConstantBuffer.h"
#include "Drawable/SolidSphere.h"

class PointLight
{
public:
	PointLight(Graphics &gfx, float radius = 0.5f);
	void DisplayControlGUI() noexcept;
	void ResetLocation() noexcept;
	void Draw(Graphics &gfx) const noexcept;
	void Bind(Graphics &gfx) const noexcept;
private:
	struct PointLightCB
	{
		DirectX::XMFLOAT3 pos;
		float padding; // Gotta add up to 16 bytes
	};

	DirectX::XMFLOAT3 pos = {0.f,0.f,0.f};
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCB> cb;
};
