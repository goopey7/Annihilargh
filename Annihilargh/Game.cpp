#include "Game.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

#include "VertexSystem.h"

GDIPlusManager gdiPM;

namespace dx = DirectX;

Game::Game(): window(1600, 900, "Annihilargh"), light(window.GetGraphics(), 0.5f)
{
	window.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.f, 9.f / 16.f, 0.5f, 40.f));
	window.DisableMousePointer();
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
	window.GetGraphics().BeginFrame(0.05f, 0.05f, 0.05f);
	window.GetGraphics().SetCamera(camera.GetMatrix());
	light.Bind(window.GetGraphics(), camera.GetMatrix());
	
	nanosuit.Draw(window.GetGraphics());
	light.Draw(window.GetGraphics());
	
	
	// spawns imgui windows
	camera.DisplayControlGUI();
	light.DisplayControlGUI();
	nanosuit.ShowWindow();
	ShowPerformanceWindow();
	window.GetGraphics().EndFrame();
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
