#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics &gfx, float radius)
	: mesh(gfx,radius),cb(gfx)
{}

void PointLight::DisplayControlGUI() noexcept
{
	if(ImGui::Begin("Point Light"))
	{
		ImGui::Text("Location");
		ImGui::SliderFloat("X",&pos.x,-60.f,60.f,"%.1f");
		ImGui::SliderFloat("Y",&pos.y,-60.f,60.f,"%.1f");
		ImGui::SliderFloat("Z",&pos.z,-60.f,60.f,"%.1f");
		if(ImGui::Button("Reset"))
		{
			ResetLocation();
		}
	}
	ImGui::End();
}

void PointLight::ResetLocation() noexcept
{
	pos = {0.f,0.f,0.f};
}

void PointLight::Draw(Graphics &gfx) const noexcept
{
	mesh.SetLocation(pos);
	mesh.Draw(gfx);
}

void PointLight::Bind(Graphics &gfx) const noexcept
{
	cb.Tick(gfx,PointLightCB{pos});
	cb.Bind(gfx);
}
