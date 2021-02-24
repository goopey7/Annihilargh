#include "Game.h"
#include <random>




Game::Game(): window(800, 600, "Annihilargh")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> ddist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> odist( 0.0f,3.1415f * 0.3f );
	std::uniform_real_distribution<float> rdist( 6.0f,20.0f );
	for(auto i=0;i<80;i++)
	{
		cubes.push_back(std::make_unique<Cube>(window.GetGraphics(),rng,adist,ddist,odist,rdist));
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
	window.GetGraphics().ClearBuffer(0.07f,0.0f,0.12f);
	for(auto &cube : cubes)
	{
		cube->Tick(deltaTime);
		cube->Draw(window.GetGraphics());
	}
	window.GetGraphics().EndFrame();
}
