#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void DisplayControlGUI() noexcept;
	void Reset() noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	void Rotate(float dx,float dy) noexcept;
private:

	DirectX::XMFLOAT3 location;
	
	float pitch=0.f,yaw=0.f;
	static constexpr float travelSpeed = 12.f; // speed when pressing wasd
	static constexpr float rotationSpeed = 0.004f; // how fast mouse rotates
};
