#include <string>
#include <sstream>
#include <Windows.h>

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CLOSE:
			PostQuitMessage(1);
			break;
		case WM_KEYDOWN:
			if(wParam=='F')
				SetWindowText(hWnd,"Respects have been paid");
			break;
		case WM_KEYUP:
			if(wParam=='F')
			{
				SetWindowText(hWnd,"Press F to pay respects");
			}
			break;
		case WM_CHAR:
			{
				static std::string title;
				title.push_back((char)wParam);
				SetWindowText(hWnd,title.c_str());
			}
			break;
		case WM_LBUTTONDOWN:
			{
				POINTS pt = MAKEPOINTS(lParam);
				std::ostringstream oss;
				oss << "(" << pt.x << "," << pt.y << ")";
				SetWindowText(hWnd,oss.str().c_str());
			}
			break;
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	const auto pClassName = "hw3d";
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	RegisterClassEx(&wc);
	HWND hWnd = CreateWindowExA(
		0, pClassName, "Annihilargh",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, 640, 480,
		nullptr, nullptr, hInstance, nullptr
	);
	ShowWindow(hWnd, SW_SHOW);

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
