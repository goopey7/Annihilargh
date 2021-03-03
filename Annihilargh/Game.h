#pragma once
#include "Camera.h"
#include "ImguiManager.h"
#include "Timer.h"
#include "Window.h"
#include "Drawable/Cube.h"
#include "Drawable/Melon.h"


class Game
{
public:
	Game();
	int BeginPlay();
private:
	void Tick();
	ImguiManager imgui;
	Window window;
	Timer timer;
	Camera camera;
	std::vector<std::unique_ptr<Drawable>> drawables;
	bool showDemoWindow=true;
	float simulationSpeedFactor = 1.f;
};
