#include <string>
#include <sstream>
#include "Game.h"
#include "Window.h"
#include "WindowsWithoutTheCrap.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	try
	{
		return Game{}.BeginPlay();
	}
	catch (const BaseAnomaly& a)
	{
		MessageBox(nullptr, a.what(), a.GetType(),MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, "No idea lol", "Unknown Anomaly", MB_OK | MB_ICONERROR);
	}
	return -1;
}
