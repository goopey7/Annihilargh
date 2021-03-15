#include "Game.h"
#include <random>
#include "Image.h"
#include "GDIPlusManager.h"
#include "Drawable/Melon.h"
#include "Drawable/TexturedCube.h"
#include "imgui/imgui.h"
#include "Geometry/Math.h"

GDIPlusManager gdiPM;

Game::Game(): window(800, 600, "Annihilargh")
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{
		}

		std::unique_ptr<Drawable> operator()()
		{
			return std::make_unique<Cube>(
				gfx, rng, adist, ddist,
				odist, rdist
			);
		}

	private:
		Graphics& gfx;
		std::mt19937 rng{std::random_device{}()};
		std::uniform_real_distribution<float> adist{0.0f, PI * 2.0f};
		std::uniform_real_distribution<float> ddist{0.0f, PI * 0.5f};
		std::uniform_real_distribution<float> odist{0.0f, PI * 0.08f};
		std::uniform_real_distribution<float> rdist{6.0f, 20.0f};
	};
	drawables.reserve(numDrawables);
	std::generate_n(std::back_inserter(drawables), numDrawables, Factory{window.GetGraphics()});
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
	auto deltaTime = timer.Reset() * simulationSpeedFactor;
	window.GetGraphics().BeginFrame(0.1f, 0.0f, 0.0f);
	window.GetGraphics().SetCamera(camera.GetMatrix());
	for(auto& drawable : drawables)
	{
		if(window.keyboard.KeyIsPressed(VK_SPACE)) deltaTime = 0.f;
		drawable->Tick(deltaTime);
		drawable->Draw(window.GetGraphics());
	}
	if(window.keyboard.KeyIsPressed(VK_ESCAPE))
		showDemoWindow = true;
	if(showDemoWindow)
	{
		static char buffer[1024];
		if(ImGui::Begin("Simulation Speed"))
		{
			ImGui::SliderFloat("Simulation Speed Factor", &simulationSpeedFactor, 0.f, 4.f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate,
			            ImGui::GetIO().Framerate);
		}
		ImGui::End();

		// spawns imgui window for camera ctrls
		camera.SpawnControlWindow();
	}

	window.GetGraphics().EndFrame();
}
