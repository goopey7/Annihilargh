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
			while (!window.mouse.IsEmpty())
			{
				const auto event = window.mouse.Read();
				if(event.IsMove())
				{
					std::ostringstream oss;
					oss << "Mouse Pos: (" << event.GetXPos() << "," << event.GetYPos() << ")";
					window.SetTitle(oss.str());
				}
				if(window.mouse.MiddleIsPressed())
				{
					MessageBox(nullptr,"MIDDLE MOUSE IS PRESSED", "AAAAAAAHHHHHHHHH",MB_OK|MB_ICONINFORMATION);
				}
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
