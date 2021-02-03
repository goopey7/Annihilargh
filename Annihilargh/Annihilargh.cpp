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
	Window window(640,480,"Big good");
	// message loop
	MSG msg;
	BOOL result;
	while(result=GetMessage(&msg,nullptr,0,0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if(result==-1)
		return -1;
	return msg.wParam;
}
