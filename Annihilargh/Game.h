#pragma once
#include "Timer.h"
#include "Window.h"
#include "Drawable/Cube.h"



class Game
{
public:
	Game();
	int BeginPlay();
private:
	void Tick();
	Window window;
	Timer timer;
	std::vector<std::unique_ptr<Cube>> cubes;
};
