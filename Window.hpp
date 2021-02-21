//
//	2021-02-19T21:16:00Z
//

#pragma once

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, \
	"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" \
)

#include <stdexcept>
#include <Windows.h>
#include <CommCtrl.h>

class Window
{
private:
	HWND m_window;
	HINSTANCE m_instance;
public:
	Window(HINSTANCE instance);
	~Window() {}

	CONST HWND GetHandle() { return m_window; }

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

