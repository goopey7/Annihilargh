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
	ironMan.Draw(window.GetGraphics());
	light.Draw(window.GetGraphics());
	if(ImGui::Begin("Performance"))
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate,
		            ImGui::GetIO().Framerate);
	}
	ImGui::End();
	
	// spawns imgui windows
	camera.DisplayControlGUI();
	light.DisplayControlGUI();
	window.GetGraphics().EndFrame();
}
