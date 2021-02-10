#pragma once
#include "Window.h"


class Game
{
public:
	Game();
	int BeginPlay();
private:
	void Tick();
	Window window;
	float r = 0.f, g = 0.5f, b = 1.f, a = 1.f;
};
