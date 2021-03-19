#pragma once
#include <random>
#include "DrawableBase.h"

template <class T>
class Object : public DrawableBase<T>
{
public:
	Object(std::mt19937 &rng, std::uniform_real_distribution<float> &adist,
	       std::uniform_real_distribution<float> &ddist,
	       std::uniform_real_distribution<float> &odist,
	       std::uniform_real_distribution<float> &rdist) :
		offsetX(rdist(rng)),
		dPitch(ddist(rng)),
		dYaw(ddist(rng)),
		dRoll(ddist(rng)),
		dTheta(odist(rng)),
		dPhi(odist(rng)),
		dChi(odist(rng)),
		theta(adist(rng)),
		phi(adist(rng)),
		chi(adist(rng))
	{}
	void Tick(float deltaTime) noexcept override
	{
		pitch += dPitch * deltaTime;
		yaw += dYaw * deltaTime;
		roll += dRoll * deltaTime;
		theta += dTheta * deltaTime;
		phi += dPhi * deltaTime;
		chi += dChi * deltaTime;
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
        DirectX::XMMatrixTranslation(offsetX, 0.f, 0.f) *
        DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}

private:
	// position
	float offsetX, pitch, yaw, roll, theta, phi, chi;
	// speed
	float dPitch, dYaw, dRoll, dTheta, dPhi, dChi;
};
