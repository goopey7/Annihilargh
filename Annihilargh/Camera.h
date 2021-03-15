#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void DisplayControlGUI() noexcept;
	void Reset() noexcept;
private:

	// location
	float r=20.f; // distance from the origin
	float theta=0.f; // rotation around equator
	float phi = 0.f; // rotation orthogonal to the equator (from pole to pole)

	// rotation
	float pitch=0.f,yaw=0.f,roll=0.f;
};
