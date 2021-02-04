#include "Window.h"
#include "resource.h"
#include <sstream>

// ************WindowClass************
Window::WindowClass Window::WindowClass::wndClass;
// constructor registers Window Class
Window::WindowClass::WindowClass() noexcept
: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = {0};
	wc.cbSize=sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,0,0,0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm=wc.hIcon;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName,GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

//************Window************
// constructor creates and shows the window
Window::Window(int width, int height, const char* name)
{
	// the specified height and width should be the client width and height, not the window width and height
	// 
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom=height+wr.top;
	// Calculates Window Size based on desired client region
	if(FAILED(AdjustWindowRect(&wr,WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,FALSE)))
	{
		throw WND_ANOMALY_LAST_ERROR();
	}
	
	hWnd = CreateWindow(WindowClass::GetName(),name,
        WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,
        CW_USEDEFAULT,CW_USEDEFAULT,wr.right-wr.left,wr.bottom-wr.top,
        nullptr,nullptr,WindowClass::GetInstance(),this);

	if(hWnd == nullptr)
		throw WND_ANOMALY_LAST_ERROR();
	
	ShowWindow(hWnd,SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

// messages come in here
LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if(msg==WM_NCCREATE) // happens while window is being created
	{
		// lparam contains creation data we want
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// link our C++ Window class with the Windows API window. We can store this ptr in userdata
		SetWindowLongPtr(hWnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(pWnd));
		// now that we have installed our ptr, we now need to install our message handler function
		SetWindowLongPtr(hWnd, GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(&Window::HandleMessageAfterCreation));
		// forward the message to our C++ window message handler
		return pWnd->HandleMessage(hWnd,msg,wParam,lParam);
	}
	// anything before WM_NCCREATE should be handled by the default handler
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

// we need this because you cannot register a C++ member function directly with the Windows API
// so this function basically just forwards the message to the handler in our C++ Window class
LRESULT WINAPI Window::HandleMessageAfterCreation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// get ptr to our C++ window class which was installed during WM_NCCREATE
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd,GWLP_USERDATA));
	// forward message to our C++ member function
	return pWnd->HandleMessage(hWnd,msg,wParam,lParam);
}

LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch(msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		// we return 0 here instead of going to the defaultWindowProc because we only want to call the destructor once!
		// we already have a destructor that calls destroy window, so no need to have the windowproc call it.
		return 0; 
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

// Window Anomaly
Window::Anomaly::Anomaly(int line, const char* file, HRESULT hr) noexcept
: BaseAnomaly(line,file),hr(hr)
{
}

const char* Window::Anomaly::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
	<< "ERROR CODE: " << GetErrorCode() << std::endl
	<< "DESCRIPTION: " << GetErrorString() << std::endl
	<< GetOriginString();
	whatBuffer=oss.str();
	return  whatBuffer.c_str();
}


const char* Window::Anomaly::GetType() const noexcept
{
	return "Window Anomaly";
}

std::string Window::Anomaly::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// basically takes hresult (error code) and gives us a description
	DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,hr,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),0,nullptr);
	if(nMsgLen==0) // if length is 0 something went wrong
		return "Unidentified Anomaly";
	// otherwise we allocate it into a string and then free the buffer
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Anomaly::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Anomaly::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}

