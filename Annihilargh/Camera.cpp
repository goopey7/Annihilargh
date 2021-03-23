#include "Camera.h"

#include <algorithm>



#include "Bindable/TransformationCB.h"
#include "Geometry/Math.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const dx::XMVECTOR startingForwardVector = dx::XMVectorSet(0.f,0.f,1.f,0.f);
	// apply rotations to the forward vector
	const auto lookVector = dx::XMVector3Transform(startingForwardVector,
		dx::XMMatrixRotationRollPitchYaw(pitch,yaw,0.f));

	// calculate camera transform from location and forward direction
	// since we never want any roll to occur as a result of manipulating pitch and yaw
	// we need to ensure that the top of the camera is always in the direction of (0,1,0)
	const auto camLocation = dx::XMLoadFloat3(&location);
	using namespace dx;
	const auto camTarget = camLocation + lookVector;
	return dx::XMMatrixLookAtLH(camLocation,camTarget,dx::XMVectorSet(0.f,1.f,0.f,0.f));
}

void Camera::DisplayControlGUI() noexcept
{
	if (ImGui::Begin("Camera Control"))
	{
		ImGui::Text("Location");
		ImGui::SliderFloat("X",&location.x,-100.f,100.f,"%.1f");
		ImGui::SliderFloat("Y",&location.y,-100.f,100.f,"%.1f");
		ImGui::SliderFloat("Z",&location.z,-100.f,100.f,"%.1f");
		ImGui::Text("Rotation");
		// slider angle automatically converts degrees to radians. Nice!
		ImGui::SliderAngle("Pitch",&pitch,0.995f*-90.f,0.995f*90.f);
		ImGui::SliderAngle("Yaw",&yaw,-180.f,180.f);
		if(ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	location = {0.f,11.5f,18.f};
	pitch = 10.f*PI/180.f, yaw = PI;
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	// when using wasd, you want to move relative to the orientation of the camera
	// so when I hit w I always move forwards
	// load up a transform that holds location, and orientation which is scaled by travelSpeed
	// since bigger things will appear to move faster
	dx::XMStoreFloat3(&translation,dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(pitch,yaw,0.f)*
		dx::XMMatrixScaling(travelSpeed,travelSpeed,travelSpeed)));
	
	location = {location.x+translation.x,location.y+translation.y,location.z+translation.z};
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = wrapAngle(yaw+dx*rotationSpeed);

	// clamp pitch. So if the mouse keeps going up it will lock at a view of the sky
	// camera bugs out if we actually reach 90, because roll is unclamped when looking directly up
	pitch = std::clamp(pitch + dy * rotationSpeed,0.995f*-PI/2.f,0.995f*PI/2.f);
}
