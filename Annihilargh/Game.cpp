#include "Game.h"
#include <random>
#include "Image.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "Geometry/Math.h"
#include "Drawable/IronMan.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VertexSystem.h"

GDIPlusManager gdiPM;

namespace dx = DirectX;

Game::Game(): window(800, 600, "Annihilargh"), light(window.GetGraphics(), 0.5f)
{
	window.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.f, 3.f / 4.f, 0.5f, 40.f));
}

int Game::BeginPlay()
{
	while(true)
	{
		// process any pending messages
		if(const auto exitCode = Window::ProcessMessages())
		{
			// if the optional returned has an int value, we are exiting.
			return *exitCode;
		}
		Tick();
	}
}

void Game::Tick()
{
	auto deltaTime = timer.Reset();
	window.GetGraphics().BeginFrame(0.01f, 0.0f, 0.05f);
	window.GetGraphics().SetCamera(camera.GetMatrix());
	light.Bind(window.GetGraphics(), camera.GetMatrix());

	// I have no idea why it's called Roll Pitch Yaw when params are pitch yaw roll. Mega confusing lol.
	const auto transform = dx::XMMatrixRotationRollPitchYaw(pos.pitch,pos.yaw,pos.roll)
	* dx::XMMatrixTranslation(pos.x,pos.y,pos.z);
	
	ironMan.Draw(window.GetGraphics(),transform);
	light.Draw(window.GetGraphics());
	
	
	// spawns imgui windows
	camera.DisplayControlGUI();
	light.DisplayControlGUI();
	ShowModelWindow();
	ShowPerformanceWindow();
	window.GetGraphics().EndFrame();
}

void Game::ShowModelWindow()
{
	if(ImGui::Begin("Model"))
	{
		ImGui::Text("Location");
		ImGui::SliderFloat("X",&pos.x,-20.f,20.f);
		ImGui::SliderFloat("Y",&pos.y,-20.f,20.f);
		ImGui::SliderFloat("Z",&pos.z,-20.f,20.f);

		ImGui::Text("Rotation");
		ImGui::SliderAngle("Pitch",&pos.pitch,-180.f,180.f);
		ImGui::SliderAngle("Yaw",&pos.yaw,-180.f,180.f);
		ImGui::SliderAngle("Roll",&pos.roll,-180.f,180.f);
	}
	ImGui::End();
}

void Game::ShowPerformanceWindow()
{
	if(ImGui::Begin("Performance"))
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
	}
	ImGui::End();
}
