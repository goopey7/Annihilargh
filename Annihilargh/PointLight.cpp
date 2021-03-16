#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics &gfx, float radius)
	: mesh(gfx,radius),cb(gfx)
{
	Reset(); // Initialise the constant buffer structure
}

void PointLight::DisplayControlGUI() noexcept
{
	if(ImGui::Begin("Point Light"))
	{
		ImGui::Text("Location");
		ImGui::SliderFloat("X",&cbData.pos.x,-60.f,60.f,"%.1f");
		ImGui::SliderFloat("Y",&cbData.pos.y,-60.f,60.f,"%.1f");
		ImGui::SliderFloat("Z",&cbData.pos.z,-60.f,60.f,"%.1f");
		if(ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData.pos = {0.f,0.f,0.f};
	cbData.materialColour = {0.7f,0.7f,0.9f};
	cbData.diffuseColour = {1.f,1.f,1.f};
	cbData.diffuseIntensity = 1.f;
	cbData.ambient = {.0f,.0f,.0f};
	cbData.attenuationConstant = 1.f;
	cbData.attenuationLinear = .027f;
	cbData.attenuationQuadratic = .0028f;
}

void PointLight::Draw(Graphics &gfx) const noexcept
{
	mesh.SetLocation(cbData.pos);
	mesh.Draw(gfx);
}

void PointLight::Bind(Graphics &gfx) const noexcept
{
	cb.Tick(gfx,cbData);
	cb.Bind(gfx);
}
