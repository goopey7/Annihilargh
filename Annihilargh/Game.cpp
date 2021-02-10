#include "Game.h"
#include <sstream>

Game::Game(): window(800, 600, "Annihilargh")
{
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
	if (r >= 1.f)
		r = 0.f;
	else r += 0.02f;
	if (g >= 1.f)
		g = 0.f;
	else g += 0.1f;
	if (b <= 0.f)
		b = 1.f;
	else b -= 0.04f;
	window.GetGraphics().ClearBuffer(r, g, b);
	window.GetGraphics().EndFrame();
	if (window.mouse.MiddleIsPressed())
	{
		MessageBox(nullptr, "MIDDLE MOUSE PRESSED", "MIDDLE MOUSE PRESSED",MB_OK | MB_ICONINFORMATION);
	}
	if (window.mouse.Read().IsMove())
	{
		std::ostringstream oss;
		oss << "(" << window.mouse.GetXPos() << "," << window.mouse.GetYPos() << ")";
		window.SetTitle(oss.str());
	}
}
