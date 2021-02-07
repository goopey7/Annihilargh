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
				switch(event.GetType())
				{
					case Mouse::Event::Type::Leave:
						window.SetTitle("Outside Client Region!");
						break;
					case Mouse::Event::Type::Move:
						{
							std::ostringstream oss;
							oss << "Mouse Pos ("<< window.mouse.GetXPos() << "," << window.mouse.GetYPos() << ")";
							window.SetTitle(oss.str());
						}
					break;
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
