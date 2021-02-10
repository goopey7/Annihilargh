#pragma once
#include "WindowsWithoutTheCrap.h"
#include "BaseAnomaly.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>


// cheeky macro for getting the line and file from the hresult
#define WND_ANOMALY(hr) Window::Anomaly(__LINE__,__FILE__,hr)

// some windows functions don't return an hresult, so here's another version that gets the last error
#define WND_ANOMALY_LAST_ERROR() Window::Anomaly(__LINE__,__FILE__,GetLastError())

class Window
{
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
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Annihilargh";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void SetTitle(const std::string& title)
	{
		if (!SetWindowText(hWnd, title.c_str()))
			throw WND_ANOMALY_LAST_ERROR();
	}

	// static because it should process messages for ALL windows
	// using optional from C++17. Allows us to either return an int or an empty optional, so we'll know if something
	// goes wrong and what it is.
	static std::optional<int> ProcessMessages();
	Graphics& GetGraphics();
private:
	static LRESULT WINAPI HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI HandleMessageAfterCreation(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	int width;
	int height;
	HWND hWnd;

	// smart ptr so we don't have to worry about deleting it
	std::unique_ptr<Graphics> pGraphics;
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

public:
	Keyboard keyboard;
	Mouse mouse;
};
