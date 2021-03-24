#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics &gfx, float radius)
	: mesh(gfx, radius), cb(gfx)
{
	Reset(); // Initialise the constant buffer structure
}

void PointLight::DisplayControlGUI() noexcept
{
	if(ImGui::Begin("Point Light"))
	{
		ImGui::Text("Location");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.f, 60.f, "%.1f");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.f, 60.f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.f, 60.f, "%.1f");
		ImGui::Text("Colour");
		ImGui::ColorEdit3("Diffuse", &cbData.diffuseColour.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);
		ImGui::Text("Intesnity");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, .01f, 2.f, "%.2f");
		ImGui::Text("Falloff");
		ImGui::SliderFloat("Quadratic", &cbData.attenuationQuadratic, 0.0000001f, 1.f, "%.7f");
		ImGui::SliderFloat("Linear", &cbData.attenuationLinear, .00001f, 4.f, "%.4f");
		ImGui::SliderFloat("Constant", &cbData.attenuationConstant, .05f, 10.f, "%.2f");
		if(ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData.pos = {-2.7f, 18.6f, 6.2f};
	cbData.diffuseColour = {1.f, 1.f, 1.f};
	cbData.diffuseIntensity = 1.f;
	cbData.ambient = {.05f, .05f, .05f};
	cbData.attenuationConstant = 1.f;
	cbData.attenuationLinear = .045f;
	cbData.attenuationQuadratic = .0075f;
}

void PointLight::Draw(Graphics &gfx) const noexcept
{
	mesh.SetLocation(cbData.pos);
	mesh.Draw(gfx);
}

void PointLight::Bind(Graphics &gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto cp = cbData;
	const auto location = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&cp.pos,DirectX::XMVector3Transform(location,view));
	cb.Tick(gfx, cp);
	cb.Bind(gfx);
}
