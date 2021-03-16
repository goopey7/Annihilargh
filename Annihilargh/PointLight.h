#pragma once
#include "Graphics.h"
#include "Bindable/ConstantBuffer.h"
#include "Drawable/SolidSphere.h"

class PointLight
{
public:
	PointLight(Graphics &gfx, float radius = 0.5f);
	void DisplayControlGUI() noexcept;
	void Reset() noexcept;
	void Draw(Graphics &gfx) const noexcept;
	void Bind(Graphics &gfx) const noexcept;
private:
	struct PointLightCB
	{
		alignas(16)DirectX::XMFLOAT3 pos;
		alignas(16)DirectX::XMFLOAT3 diffuseColour;
		float diffuseIntensity;
		alignas(16)DirectX::XMFLOAT3 ambient;
		float attenuationConstant;
		float attenuationLinear;
		float attenuationQuadratic;
	};

	PointLightCB cbData;
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCB> cb;
};
