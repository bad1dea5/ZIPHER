//
//	2021-02-19T21:16:00Z
//

#include "Window.hpp"

//
//
//
Window::Window(HINSTANCE instance)
	: m_window(NULL), m_instance(instance)
{
	WNDCLASSEXW wc;

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.cbWndExtra = 0;
	wc.hbrBackground = HBRUSH(COLOR_WINDOWFRAME);
	wc.hCursor = ::LoadCursorW(NULL, IDC_ARROW);
	wc.hIcon = ::LoadIconW(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hInstance = m_instance;
	wc.lpfnWndProc = WNDPROC(WndProc);
	wc.lpszClassName = L"ZIPHER";
	wc.lpszMenuName = 0;
	wc.style = 0;

	if (!::RegisterClassExW(&wc)) {
		throw std::runtime_error("Register class failed");
	}

	if (!(m_window = ::CreateWindowExW(
		WS_EX_CLIENTEDGE,
		wc.lpszClassName,
		wc.lpszClassName,
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,
		480, 380,
		NULL,
		NULL,
		m_instance,
		this
	)))
	{
		throw std::runtime_error("Create window failed");
	}

	::ShowWindow(m_window, TRUE);
	::UpdateWindow(m_window);
}

//
//
//
LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Window* window = reinterpret_cast<Window*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_CREATE:
		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, LONG_PTR(lparam));
		return 0;

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}

	return ::DefWindowProcW(hwnd, msg, wparam, lparam);
}
