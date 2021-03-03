#include "Game.h"
#include <random>
#include "Image.h"
#include "GDIPlusManager.h"
#include "Drawable/Melon.h"
#include "Drawable/TexturedCube.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

GDIPlusManager gdiPM;

Game::Game(): window(800, 600, "Annihilargh")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> ddist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> odist( 0.0f,3.1415f * 0.3f );
	std::uniform_real_distribution<float> rdist( 6.0f,20.0f );
	for(auto i=0;i<100;i++)
	{
		//drawables.push_back(std::make_unique<Melon>(window.GetGraphics(),rng,adist,ddist,odist,rdist));
		drawables.push_back(std::make_unique<TexturedCube>(window.GetGraphics(),rng,adist,ddist,odist,rdist));
	}
	window.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.f,3.f/4.f,0.5f,40.f));
}

int Game::BeginPlay()
{
	while (true)
	{
		// process any pending messages
		if (const auto exitCode = Window::ProcessMessages())
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
	window.GetGraphics().ClearBuffer(0.1f,0.0f,0.0f);
	for(auto &drawable : drawables)
	{
		if(window.keyboard.KeyIsPressed(VK_SPACE)) deltaTime = 0.f;
		drawable->Tick(deltaTime);
		drawable->Draw(window.GetGraphics());
	}
	// imgui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	
	ImGui::NewFrame();

	if(window.keyboard.KeyIsPressed(VK_ESCAPE))
		showDemoWindow=!showDemoWindow;
	if(showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	window.GetGraphics().EndFrame();
}
