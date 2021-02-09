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
};
