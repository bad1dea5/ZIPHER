//
//	2021-03-07T15:54:00Z
//

#include "Application.hpp"

//
//
//
INT_PTR DecipherProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	Application* application = reinterpret_cast<Application*>(::GetWindowLongPtrW(hwnd, DWLP_USER));

	switch (message)
	{
	case WM_INITDIALOG:
		::SetWindowLongPtrW(hwnd, DWLP_USER, LONG_PTR(lparam));
		break;

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
		return (LRESULT)::GetStockObject(WHITE_BRUSH);

	default:
		return FALSE;
	}

	return TRUE;
}