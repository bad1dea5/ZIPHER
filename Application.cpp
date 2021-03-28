//
//	2021-03-07T12:04:00Z
//

#include "Application.hpp"
#include "Resource.hpp"

INT_PTR EncipherProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
INT_PTR DecipherProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

//
//
//
Application::Application(HINSTANCE instance)
	: m_datalist(), m_dialog(), m_instance(instance), m_window()
{
	if (!(m_window = ::CreateDialogParamW(
		m_instance,
		MAKEINTRESOURCEW(IDD_WINDOW),
		NULL,
		DLGPROC(DlgProc),
		LPARAM(this)
	)))
	{
		throw std::runtime_error("Failed to create window!");
	}

	LPWSTR buffer;

	::SendMessageW(m_window, WM_SETICON, ICON_SMALL, LPARAM(::LoadIconW(NULL, IDI_APPLICATION)));
	::SendMessageW(m_window, WM_SETICON, ICON_BIG, LPARAM(::LoadIconW(NULL, IDI_APPLICATION)));

	::LoadStringW(m_instance, IDS_META_NAME, reinterpret_cast<LPWSTR>(&buffer), 0);
	::SendMessageW(m_window, WM_SETTEXT, 0, LPARAM(buffer));

	::ShowWindow(m_window, TRUE);
}

//
//
//
Application::~Application()
{
}

//
//
//
LRESULT Application::initialize()
{
	HWND hwnd;

	//
	//
	//
	if (!(hwnd = ::CreateWindowExW(
		NULL,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		m_window,
		HMENU(IDC_TOOLBAR),
		m_instance,
		NULL
	)))
	{
		return -1;
	}

	TBBUTTON toolbar[] = {
		{ MAKELONG(STD_FILENEW, 0), IDM_NEW, TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, 0 },
		{ MAKELONG(STD_FILEOPEN, 0), IDM_OPEN, TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, 0 },
		{ MAKELONG(STD_FILESAVE, 0), IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, 0 }
	};

	::SendMessageW(hwnd, TB_LOADIMAGES, WPARAM(IDB_STD_SMALL_COLOR), LPARAM(HINST_COMMCTRL));
	::SendMessageW(hwnd, TB_BUTTONSTRUCTSIZE, WPARAM(sizeof(TBBUTTON)), LPARAM(0));
	::SendMessageW(hwnd, TB_ADDBUTTONSW, WPARAM(ARRAYSIZE(toolbar)), LPARAM(&toolbar));
	::SendMessageW(hwnd, TB_AUTOSIZE, WPARAM(0), LPARAM(0));


	//
	//	statusbar
	//
	if (!(hwnd = ::CreateWindowExW(
		NULL,
		STATUSCLASSNAMEW,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		m_window,
		HMENU(IDC_STATUSBAR),
		m_instance,
		NULL
	)))
	{
		return -1;
	}

	INT widths[] = { 100, 200, 400, -1 };

	::SendMessageW(hwnd, SB_SETPARTS, WPARAM(ARRAYSIZE(widths)), LPARAM(&widths));
	::SendMessageW(hwnd, SB_SETTEXTW, WPARAM(0), LPARAM(L""));
	::SendMessageW(hwnd, SB_SETTEXTW, WPARAM(1), LPARAM(L""));

	//
	//
	//
	TABPAGE tabs[] = {
		{{ TCIF_IMAGE | TCIF_TEXT, 0, 0, LPWSTR(L"Encipher"), 0, -1, 0 }, IDD_ENCIPHER, EncipherProc },
		{{ TCIF_IMAGE | TCIF_TEXT, 0, 0, LPWSTR(L"Decipher"), 0, -1, 0 }, IDD_DECIPHER, DecipherProc }
	};

	for (auto i = 0; i < _ARRAYSIZE(tabs); i++)
	{
		if (!(hwnd = ::CreateDialogParamW(
			m_instance,
			MAKEINTRESOURCEW(tabs[i].resource),
			::GetDlgItem(m_window, IDC_TABCONTROL),
			DLGPROC(tabs[i].proc),
			LPARAM(this)
		)))
		{
			return -1;
		}

		::SendMessageW(::GetDlgItem(m_window, IDC_TABCONTROL), TCM_INSERTITEMW, i, LPARAM(&tabs[i].name));
		m_dialog.push_back(std::pair<HWND, TABPAGE>(hwnd, tabs[i]));
	}

	::ShowWindow(m_dialog.front().first, TRUE);

	return 0;
}

//
// 
// 
std::wstring Application::GetWindowText(HWND hwnd, INT control)
{
	INT length;

	if (!(length = ::GetWindowTextLengthW(::GetDlgItem(hwnd, control)))) {
		return std::wstring();
	}

	std::unique_ptr<WCHAR, VOID(*)(LPWSTR)> buffer(
		static_cast<LPWSTR>(::operator new(
			(length + 1) * sizeof(WCHAR)
		)),
		[](LPWSTR ptr) { ::operator delete(ptr); }
	);

	::GetDlgItemTextW(hwnd, control, LPWSTR(buffer.get()), length + 1);

	return std::wstring(LPWSTR(buffer.get()));
}

//
//
//
INT_PTR Application::DlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
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

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDC_LISTBOX:
			switch (HIWORD(wparam))
			{
			case LBN_SELCHANGE:
			{
				::CreateDialogParamW(
					application->m_instance,
					MAKEINTRESOURCEW(IDD_DETAIL),
					hwnd,
					DLGPROC(DetailProc),
					LPARAM(application)
				);
				break;
			}
			}
			break;
		}
		break;

	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lparam)->code)
		{
		case TCN_SELCHANGING:
		{
			INT index = INT(::SendMessageW(::GetDlgItem(application->m_window, IDC_TABCONTROL), TCM_GETCURSEL, 0, 0));
			::ShowWindow(application->m_dialog.at(index).first, FALSE);
			return FALSE;
		}

		case TCN_SELCHANGE:
		{
			INT index = INT(::SendMessageW(::GetDlgItem(application->m_window, IDC_TABCONTROL), TCM_GETCURSEL, 0, 0));
			::ShowWindow(application->m_dialog.at(index).first, TRUE);
			break;
		}

		default:
			return FALSE;
		}
		break;
	}

	default:
		return FALSE;
	}

	return TRUE;
}

//
//
//
INT_PTR Application::DetailProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	Application* application = reinterpret_cast<Application*>(::GetWindowLongPtrW(hwnd, DWLP_USER));

	switch (message)
	{
	case WM_INITDIALOG:
		::SetWindowLongPtrW(hwnd, DWLP_USER, LONG_PTR(lparam));
		::ShowWindow(hwnd, TRUE);
		::SendMessageW(hwnd, WM_USER, 0, 0);
		break;

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
		return (LRESULT)::GetStockObject(WHITE_BRUSH);

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
			::SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		}
		break;

	case WM_USER:
	{
		INT pos = (INT)::SendMessageW(::GetDlgItem(application->m_window, IDC_LISTBOX), LB_GETCURSEL, 0, 0);

		if (pos > application->m_datalist.size()) {
			return FALSE;
		}

		DATALIST data = application->m_datalist.at(pos);
		std::wstringstream ws;

		::SetWindowTextW(::GetDlgItem(hwnd, IDX_NAME), data.in.c_str());
		::SetWindowTextW(::GetDlgItem(hwnd, IDX_EDIT), data.out.c_str());

		switch (data.type)
		{
		case 0:
			::SetWindowTextW(::GetDlgItem(hwnd, IDX_TYPE), LPCWSTR(L"ASM (AT&T)"));
			break;
		case 1:
			::SetWindowTextW(::GetDlgItem(hwnd, IDX_TYPE), LPCWSTR(L"ASM (Intel)"));
			break;
		case 2:
			::SetWindowTextW(::GetDlgItem(hwnd, IDX_TYPE), LPCWSTR(L"C++"));
			break;
		case 3:
			::SetWindowTextW(::GetDlgItem(hwnd, IDX_TYPE), LPCWSTR(L"Normal"));
			break;
		}

		//::SetWindowTextW(::GetDlgItem(hwnd, IDX_TYPE), data.type);

		ws << std::hex << "0x" << std::setfill<WCHAR>('0') << data.min;
		::SetWindowTextW(::GetDlgItem(hwnd, IDX_MIN), ws.str().c_str());

		ws = std::wstringstream();

		ws << std::hex << "0x" << std::setfill<WCHAR>('0') << data.max;
		::SetWindowTextW(::GetDlgItem(hwnd, IDX_MAX), ws.str().c_str());

		break;
	}

	default:
		return FALSE;
	}

	return TRUE;
}
