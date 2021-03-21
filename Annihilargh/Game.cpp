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

// let's see if this actually compiles and works when stepping through in the debugger. It Does!
void test()
{
	VertexBuffer vertexBuffer(std::move(
		VertexLayout{}
		.Append<VertexLayout::Location3D>()
		.Append<VertexLayout::Normal>()
		.Append<VertexLayout::TextureCoord2D>()
	));
	vertexBuffer.EmplaceBack(
		dx::XMFLOAT3{1.0f, 2.0f, 3.0f},
		dx::XMFLOAT3{3.0f, 2.0f, 1.0f},
		dx::XMFLOAT2{1.0f, 6.0f}
	);
	vertexBuffer.EmplaceBack(
		dx::XMFLOAT3{9.0f, 1.0f, 6.0f},
		dx::XMFLOAT3{9.0f, 1.0f, 6.0f},
		dx::XMFLOAT2{9.1f, 6.0f}
	);
	auto location = vertexBuffer[0].Retrieve<VertexLayout::Location3D>();
	auto norm = vertexBuffer[0].Retrieve<VertexLayout::Normal>();
	auto texture2d = vertexBuffer[1].Retrieve<VertexLayout::TextureCoord2D>();
	vertexBuffer.Back().Retrieve<VertexLayout::Location3D>().z = 666.0f;
	location = vertexBuffer.Back().Retrieve<VertexLayout::Location3D>();
}

Game::Game(): window(800, 600, "Annihilargh"), light(window.GetGraphics(), 0.5f)
{
	test();
	class Factory
	{
	public:
		Factory(Graphics &gfx)
			:
			gfx(gfx)
		{
		}

		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 material = {matDist(rng), matDist(rng), matDist(rng)};
			return std::make_unique<IronMan>(
				gfx, rng, adist, ddist,
				odist, rdist, material, 0.5f
			);
		}

	private:
		Graphics &gfx;
		std::mt19937 rng{std::random_device{}()};
		std::uniform_real_distribution<float> adist{0.0f, PI * 2.0f};
		std::uniform_real_distribution<float> ddist{0.0f, PI * 0.5f};
		std::uniform_real_distribution<float> odist{0.0f, PI * 0.08f};
		std::uniform_real_distribution<float> rdist{6.0f, 20.0f};
		std::uniform_real_distribution<float> matDist{0.0f, 1.0f};
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
	window.GetGraphics().BeginFrame(0.01f, 0.0f, 0.05f);
	window.GetGraphics().SetCamera(camera.GetMatrix());
	light.Bind(window.GetGraphics(), camera.GetMatrix());
	for(auto &drawable : drawables)
	{
		if(window.keyboard.KeyIsPressed(VK_SPACE)) deltaTime = 0.f;
		drawable->Tick(deltaTime);
		drawable->Draw(window.GetGraphics());
	}
	light.Draw(window.GetGraphics());
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

		// spawns imgui windows
		camera.DisplayControlGUI();
		light.DisplayControlGUI();
	}

	window.GetGraphics().EndFrame();
}
