#pragma once
#include "Camera.h"
#include "ImguiManager.h"
#include "Timer.h"
#include "Window.h"
#include "PointLight.h"
#include "Drawable/Model.h"

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
	PointLight light;
	Model ironMan{window.GetGraphics(),"3DAssets\\ironMan.obj"};
};
