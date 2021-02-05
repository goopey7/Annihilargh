#include <string>
#include <sstream>

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
		Window window(640,480,"Big good");
		// message loop
		MSG msg;
		BOOL result;
		while(result=GetMessage(&msg,nullptr,0,0)>0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(window.keyboard.KeyIsPressed(VK_MENU))
			{
				MessageBox(nullptr,"AAAAAAAAAAAHHHHHHHHHH","YOU HIT THE ALT KEY",MB_OK|MB_ICONINFORMATION);
			}
		}
		if(result==-1)
			return -1;
		return msg.wParam;
	}
	catch(const BaseAnomaly &a)
	{
		MessageBox(nullptr,a.what(),a.GetType(),MB_OK|MB_ICONERROR);
	}
	catch(const std::exception &e)
	{
		MessageBox(nullptr,e.what(),"Standard Exception", MB_OK|MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr,"No idea lol","Unknown Anomaly", MB_OK|MB_ICONERROR);
	}
	return -1;
}
