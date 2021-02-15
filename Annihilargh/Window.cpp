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
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON, 0, 0, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = wc.hIcon;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
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
	: width(width), height(height)
{
	// the specified height and width should be the client width and height, not the window width and height
	// 
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	// Calculates Window Size based on desired client region
	if (!AdjustWindowRect(&wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE))
	{
		throw WND_ANOMALY_LAST_ERROR();
	}

	hWnd = CreateWindow(WindowClass::GetName(), name,
	                    WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,
	                    CW_USEDEFAULT, CW_USEDEFAULT, wr.right-wr.left, wr.bottom-wr.top,
	                    nullptr, nullptr, WindowClass::GetInstance(), this);

	if (hWnd == nullptr)
		throw WND_ANOMALY_LAST_ERROR();

	ShowWindow(hWnd,SW_SHOWDEFAULT);

	// create graphics object
	pGraphics = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// while there are messages, remove them and process them
	// but if there aren't any messages, keep going. We don't want the game to freeze because
	// there aren't any messages
	while (PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
	{
		// check for quit since PeekMessage returns whether or not it has a message, not if it is a quit.
		if (msg.message == WM_QUIT)
			return msg.wParam; // arg to PostQuitMessage is in wParam, so we return it.

		TranslateMessage(&msg); // Posts WM_CHAR messages from key messages
		DispatchMessage(&msg);
	}
	// return an empty optional if not quitting the game.
	return {};
}

Graphics& Window::GetGraphics()
{
	if(!pGraphics)
		throw WND_NOGFX_ANOMALY();
	return *pGraphics;
}

// messages come in here
LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) // happens while window is being created
	{
		// lparam contains creation data we want
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// link our C++ Window class with the Windows API window. We can store this ptr in userdata
		SetWindowLongPtr(hWnd,GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// now that we have installed our ptr, we now need to install our message handler function
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageAfterCreation));
		// forward the message to our C++ window message handler
		return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
	}
	// anything before WM_NCCREATE should be handled by the default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// we need this because you cannot register a C++ member function directly with the Windows API
// so this function basically just forwards the message to the handler in our C++ Window class
LRESULT WINAPI Window::HandleMessageAfterCreation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// get ptr to our C++ window class which was installed during WM_NCCREATE
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd,GWLP_USERDATA));
	// forward message to our C++ member function
	return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// **** KEYBOARD MESSAGES ****
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // we also want to handle syskeys like ALT
		// filter auto repeat key events. Bit 30 is auto repeat
		if (keyboard.IsAutoRepeat() || !(lParam & 0x40000000))
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
		// **** ----------------- ****
		// ****   MOUSE MESSAGES  ****
	case WM_MOUSEMOVE:
		{
			const POINTS p = MAKEPOINTS(lParam);

			// check if we are in the client region
			if (p.x >= 0 && p.y >= 0 && p.x < width && p.y < height)
			{
				mouse.OnMouseMove(p.x, p.y);

				// if we previously were not in the client region
				if (!mouse.IsInWindow())
				{
					SetCapture(hWnd);
					mouse.OnMouseEnter();
				}
			}
			else // not in the client region
			{
				// if any of these buttons are down we keep tracking the mouse
				// since this means that the user is dragging something and their mouse has left the client region
				// so we don't want to loose whatever is being dragged, we only want to stop dragging when the user
				// has released a mouse button
				/*TODO keep track of the buttons that were being pressed until we return to the client region
				 * So if I left the region holding middle mouse, but then hold left mb and released middle,
				 * That stop capture..... maybe idk depends. Maybe I make this a setting in the engine.
				 */
				if (mouse.LeftIsPressed() || mouse.RightIsPressed() || mouse.MiddleIsPressed())
				{
					mouse.OnMouseMove(p.x, p.y);
				}
				else // released button, so we don't care about the mouse anymore
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(p.x, p.y);
		}
		break;
	case WM_LBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(p.x, p.y);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnRightPressed(p.x, p.y);
		}
		break;
	case WM_RBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnRightReleased(p.x, p.y);
		}
		break;
	case WM_MBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnMiddlePressed(p.x, p.y);
		}
		break;
	case WM_MBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnMiddleReleased(p.x, p.y);
		}
		break;
	case WM_MOUSEWHEEL:
		{
			const POINTS p = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(p.x, p.y, delta);
		}
		break;
		// **** ----------------- ****
	case WM_KILLFOCUS:
		keyboard.ClearState(); // reset states to released when we loose focus
		// so when a messagebox pops up all keys are released
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Window Anomaly
Window::Anomaly::Anomaly(int line, const char* file, HRESULT hr) noexcept
	: BaseAnomaly(line, file), hr(hr)
{
}

const char* Window::Anomaly::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "ERROR CODE: " << GetErrorCode() << std::endl
		<< "DESCRIPTION: " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}


const char* Window::Anomaly::GetType() const noexcept
{
	return "Window Anomaly";
}

std::string Window::Anomaly::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// basically takes hresult (error code) and gives us a description
	DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	                              FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	                              nullptr, hr,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	                              reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
	if (nMsgLen == 0) // if length is 0 something went wrong
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

const char* Window::NoGraphicsAnomaly::GetType() const noexcept
{
	return "No Graphics Anomaly";
}
