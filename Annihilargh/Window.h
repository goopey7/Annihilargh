#pragma once
#include "WindowsWithoutTheCrap.h"

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
		WindowClass(const WindowClass& ) = delete;
		WindowClass& operator=(const WindowClass&)=delete;
		static constexpr const char* wndClassName = "Annihilargh";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width,int height, const char* name) noexcept;
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
