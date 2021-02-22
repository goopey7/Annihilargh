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
	const float c = sin(abs(timer.GetElapsed())/2.f+.5f);
	window.GetGraphics().ClearBuffer(c,c/3.f,0.f);
	window.GetGraphics().DrawTestTriangle(
		timer.GetElapsed(),window.mouse.GetXPos()/400.f-1,-(window.mouse.GetYPos()/300.f-1)
		);
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
