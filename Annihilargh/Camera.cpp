#include "Camera.h"

#include "imgui/imgui.h"

namespace dx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// start at origin and move away by r then rotate around that position using phi and theta
	const auto location = dx::XMVector3Transform(
		dx::XMVectorSet(0.f,0.f,-r,0.f),
		dx::XMMatrixRotationRollPitchYaw(phi,-theta,0.f));

	const auto rotation = dx::XMMatrixRotationRollPitchYaw(pitch,-yaw,roll);

	// look at allows us to look at the origin. Takes eyePos, focusPos, and upDir
	return dx::XMMatrixLookAtLH(location,dx::XMVectorZero(),
		dx::XMVectorSet(0.f,1.f,0.f,0.f))*rotation;
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera Control"))
	{
		ImGui::Text("Location");
		ImGui::SliderFloat("R", &r, 1.f, 80.f, "%.1f");
		// slider angle automatically converts degrees to radians. Nice!
		ImGui::SliderAngle("Theta", &theta, -180.f, 180.f);
		ImGui::SliderAngle("Phi", &phi,-89.f,89.f);
		ImGui::Text("Rotation");
		ImGui::SliderAngle("Pitch",&pitch,-180.f,180.f);
		ImGui::SliderAngle("Yaw",&yaw,-180.f,180.f);
		ImGui::SliderAngle("Roll",&roll,-180.f,180.f);
		if(ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	// location
	r = 20.f; // distance from the origin
	theta = 0.f; // rotation around equator
	phi = 0.f; // rotation orthogonal to the equator (from pole to pole)

	// rotation
	pitch = 0.f, yaw = 0.f, roll = 0.f;
}
