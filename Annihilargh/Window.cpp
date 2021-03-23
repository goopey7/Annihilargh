#include "Window.h"
#include "resource.h"
#include <sstream>
#include "imgui/imgui_impl_win32.h"

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
	if(!AdjustWindowRect(&wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE))
	{
		throw WND_ANOMALY_LAST_ERROR();
	}

	hWnd = CreateWindow(WindowClass::GetName(), name,
	                    WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,
	                    CW_USEDEFAULT, CW_USEDEFAULT, wr.right-wr.left, wr.bottom-wr.top,
	                    nullptr, nullptr, WindowClass::GetInstance(), this);

	if(hWnd == nullptr)
		throw WND_ANOMALY_LAST_ERROR();

	ShowWindow(hWnd,SW_SHOWDEFAULT);

	// initialise ImGui Win32
	ImGui_ImplWin32_Init(hWnd);

	// create graphics object
	pGraphics = std::make_unique<Graphics>(hWnd, width, height);

	// register mouse device for raw input
	RAWINPUTDEVICE rawInputDevice;

	// the combination of these two values determines the device we are using
	rawInputDevice.usUsagePage = 0x01; // mouse page
	rawInputDevice.usUsage = 0x02; // mouse usage

	rawInputDevice.dwFlags = 0;
	rawInputDevice.hwndTarget = nullptr;
	if(RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)) == FALSE)
	{
		throw WND_ANOMALY_LAST_ERROR();
	}
}

Window::~Window()
{
	DestroyWindow(hWnd);
	ImGui_ImplWin32_Shutdown();
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// while there are messages, remove them and process them
	// but if there aren't any messages, keep going. We don't want the game to freeze because
	// there aren't any messages
	while(PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
	{
		// check for quit since PeekMessage returns whether or not it has a message, not if it is a quit.
		if(msg.message == WM_QUIT)
			return (int)msg.wParam; // arg to PostQuitMessage is in wParam, so we return it.

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

void Window::EnableMousePointer()
{
	bPointerEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableMousePointer()
{
	bPointerEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	TrapCursor();
}

bool Window::IsPointerEnabled() const noexcept
{
	return bPointerEnabled;
}

// messages come in here
LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if(msg == WM_NCCREATE) // happens while window is being created
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto &imio = ImGui::GetIO();
	// if imgui is handling it then we don't need to do anything
	if(ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	switch(msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// **** KEYBOARD MESSAGES ****
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // we also want to handle syskeys like ALT
		// if imgui wants to capture keyboard, then we let it swallow up our inputs.
		if(imio.WantCaptureKeyboard)
			break;
		// filter auto repeat key events. Bit 30 is auto repeat
		if(keyboard.IsAutoRepeat() || !(lParam & 0x40000000))
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// if imgui wants to capture keyboard, then we let it swallow up our inputs.
		if(imio.WantCaptureKeyboard)
			break;
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		// if imgui wants to capture keyboard, then we let it swallow up our inputs.
		if(imio.WantCaptureKeyboard)
			break;
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
		// **** ----------------- ****
		// ****   MOUSE MESSAGES  ****
	case WM_MOUSEMOVE:
		{
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);

			// check if we are in the client region
			if(p.x >= 0 && p.y >= 0 && p.x < width && p.y < height)
			{
				mouse.OnMouseMove(p.x, p.y);

				// if we previously were not in the client region
				if(!mouse.IsInWindow())
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
				if(mouse.LeftIsPressed() || mouse.RightIsPressed() || mouse.MiddleIsPressed())
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
			if(!bPointerEnabled)
			{
				TrapCursor();
				HideCursor();
			}
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(p.x, p.y);
		}
		break;
	case WM_LBUTTONUP:
		{
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(p.x, p.y);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnRightPressed(p.x, p.y);
		}
		break;
	case WM_RBUTTONUP:
		{
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnRightReleased(p.x, p.y);
		}
		break;
	case WM_MBUTTONDOWN:
		{
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnMiddlePressed(p.x, p.y);
		}
		break;
	case WM_MBUTTONUP:
		{
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);
			mouse.OnMiddleReleased(p.x, p.y);
		}
		break;
	case WM_MOUSEWHEEL:
		{
			// if imgui wants to capture mouse, then we let it swallow up our inputs.
			if(imio.WantCaptureMouse)
				break;
			const POINTS p = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(p.x, p.y, delta);
		}
		break;
		// **** ----------------- ****
		// **** RAW MOUSE MESSAGES ****
	case WM_INPUT:
		{
			if(!mouse.bIsRawEnabled)
				break;
			UINT size;
			// First determine the size of the data in bytes
			// if we pass in a nullptr to data, it will fill in the size pointer, with the size required
			if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			                   RID_INPUT,
			                   nullptr,
			                   &size,
			                   sizeof(RAWINPUTHEADER)) == -1)
			{
				// if something went wrong we shouldn't try and read input
				break;
			}
			rawBuffer.resize(size);
			// read input data
			if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			                   RID_INPUT,
			                   rawBuffer.data(),
			                   &size,
			                   sizeof(RAWINPUTHEADER)) != size)
			{
				// break out if anything goes wrong
				break;
			}
			// process the data read in
			auto &rawInput = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			// we only want mouse messages. If mouse dx or dy were 0, then chances are it's a click
			// and we don't care about clicks in raw input. Just movement
			if(rawInput.header.dwType == RIM_TYPEMOUSE && (rawInput.data.mouse.lLastX != 0 ||
				rawInput.data.mouse.lLastY != 0))
					mouse.OnRawDelta(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
			break;
		}
	case WM_KILLFOCUS:
		keyboard.ClearState(); // reset states to released when we loose focus
		// so when a messagebox pops up all keys are released
		// reset mouse states as well
		mouse.Clear();
		break;
	case WM_ACTIVATE:
		if(!bPointerEnabled)
		{
			TrapCursor();
			HideCursor();
		}
		else
		{
			FreeCursor();
			ShowCursor();
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::TrapCursor() noexcept
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	// map from window-space to screen-space
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	// rect is a structure of 2 points. Maps the two points to screen-space
	rect.top+=1; // prevent pointer from accidentally exiting window
	ClipCursor(&rect); // clip cursor traps a cursor to a rectangular region
	// so we create a rectangle the size of the window
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::HideCursor() noexcept
{
	// WINAPI keeps an internal counter. When it is above 0 the pointer is shown.
	// so we will while loop here.
	while(::ShowCursor(FALSE) >= 0);
}

void Window::ShowCursor() noexcept
{
	// same for showing the pointer
	while(::ShowCursor(TRUE) < 0);
}

void Window::EnableImGuiMouse() noexcept
{
	// clear the no mouse bit
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse() noexcept
{
	// set the no mouse bit
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
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
	if(nMsgLen == 0) // if length is 0 something went wrong
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
