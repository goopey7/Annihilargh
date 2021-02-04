#pragma once
#include "WindowsWithoutTheCrap.h"
#include "BaseAnomaly.h"

class Window
{
public:
class Anomaly : public BaseAnomaly
{
public:
	Anomaly(int line, const char* file, HRESULT hr) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept override;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorString() const noexcept;
private:
	HRESULT hr; // windows error code
};
private:
	// we create a singleton because we are only going to need one window
	// hence all the static stuff
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass(); // when creating a destructor, it's could practice to deal with copy assignment.
		WindowClass(const WindowClass& ) = delete;
		WindowClass& operator=(const WindowClass&)=delete;
		static constexpr const char* wndClassName = "Annihilargh";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width,int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT WINAPI HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI HandleMessageAfterCreation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	int width;
	int height;
	HWND hWnd;
};

// cheeky macro for getting the line and file from the hresult
#define WND_ANOMALY(hr) Window::Anomaly(__LINE__,__FILE__,hr)

// some windows functions don't return an hresult, so here's another version that gets the last error
#define WND_ANOMALY_LAST_ERROR() Window::Anomaly(__LINE__,__FILE__,GetLastError())
