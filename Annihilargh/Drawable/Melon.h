#pragma once
#include <random>

#include "../Graphics.h"
#include "DrawableBase.h"

class Melon : public DrawableBase<Melon>
{
public:
	Melon(Graphics &gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist);
	void Tick(float deltaTime) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// position
	float offsetX,pitch,yaw,roll,theta,phi,chi;
	// speed
	float dPitch,dYaw,dRoll,dTheta,dPhi,dChi;
};
