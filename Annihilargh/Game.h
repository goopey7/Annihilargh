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
	void ShowModelWindow();
	void ShowPerformanceWindow();
	ImguiManager imgui;
	Window window;
	Timer timer;
	Camera camera;
	PointLight light;
	Model ironMan{window.GetGraphics(),"3DAssets\\ironMan.obj"};

	struct
	{
		float x=0.f,y=0.f,z=0.f,pitch=0.f,yaw=0.f,roll=0.f;
	} pos;
};
