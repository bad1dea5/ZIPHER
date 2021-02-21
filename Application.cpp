//
//	2021-02-19T22:14:00Z
//

#include <cwctype>
#include <iomanip>
#include <sstream>

#include "Application.hpp"
#include "Resource.hpp"

#define WM_INITIALIZE (WM_USER + 1)

//
//
//
Application::Application(HINSTANCE instance)
	: m_instance(instance), m_window(new Window(instance)), m_vector(), m_method(TRUE)
{
	HWND hwnd;

	if (!(hwnd = ::CreateWindowExW(
		NULL,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		m_window->GetHandle(),
		HMENU(IDC_TOOLBAR),
		m_instance,
		NULL
	)))
	{
		throw std::runtime_error("TOOLBARCLASSNAME failed");
	}

	TBBUTTON tbButton[] = {
		{ MAKELONG(STD_FILENEW, 0), IDM_NEW, TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, 0 },
		{ MAKELONG(STD_FILEOPEN, 0), IDM_OPEN, TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, 0 },
		{ MAKELONG(STD_FILESAVE, 0), IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, 0 }
	};

	::SendMessageW(hwnd, TB_LOADIMAGES, WPARAM(IDB_STD_SMALL_COLOR), LPARAM(HINST_COMMCTRL));
	::SendMessageW(hwnd, TB_BUTTONSTRUCTSIZE, WPARAM(sizeof(TBBUTTON)), LPARAM(0));
	::SendMessageW(hwnd, TB_ADDBUTTONSW, WPARAM(sizeof(tbButton) / sizeof(TBBUTTON)), LPARAM(&tbButton));
	::SendMessageW(hwnd, TB_AUTOSIZE, WPARAM(0), LPARAM(0));

	if (!(hwnd = ::CreateWindowExW(
		NULL,
		STATUSCLASSNAMEW,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		m_window->GetHandle(),
		HMENU(IDC_STATUSBAR),
		m_instance,
		NULL
	)))
	{
		throw std::runtime_error("STATUSCLASSNAME failed");
	}

	INT sbWidth[] = { 100,200,300,-1 };

	::SendMessageW(hwnd, SB_SETPARTS, WPARAM(sizeof(sbWidth) / sizeof(int)), LPARAM(&sbWidth));
	::SendMessageW(hwnd, SB_SETTEXTW, WPARAM(0), LPARAM(L"1.0.0.0"));
	::SendMessageW(hwnd, SB_SETTEXTW, WPARAM(1), LPARAM(L""));

	if (!(m_dialog = ::CreateDialogParamW(
		m_instance,
		MAKEINTRESOURCEW(IDD_DIALOG),
		m_window->GetHandle(),
		DLGPROC(DlgProc),
		LPARAM(this)
	)))
	{
		throw std::runtime_error("Dialog failed");
	}

	::ShowWindow(m_dialog, TRUE);
}

//
//
//
LONG Application::initialize()
{
	//
	//
	//
	::SendMessageW(::GetDlgItem(m_dialog, IDC_ENCIPHER), BM_SETCHECK, WPARAM(BST_CHECKED), 0);

	//
	//
	//
	WCHAR types[4][12] = {
		L"Normal", L"ASM (AT&T)", L"ASM (Intel)", L"C++"
	};
	WCHAR str[16] = { 0 };

	for (auto i = 0; i <= 3; i += 1)
	{
		::wcscpy_s(str, sizeof(str) / sizeof(WCHAR), (WCHAR*)types[i]);
		::SendMessage(::GetDlgItem(m_dialog, IDC_TYPE), (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)str);
	}

	::SendMessage(::GetDlgItem(m_dialog, IDC_TYPE), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);	

	return 0;
}

//
//
//
std::wstring Application::GetWindowString(INT handle)
{
	std::wstring ws;
	INT length;
	WCHAR* ptr;

	if (!(length = ::GetWindowTextLengthW(::GetDlgItem(m_dialog, handle)))) {
		return std::wstring();
	}

	if (!(ptr = (WCHAR*)::operator new(sizeof(WCHAR) * (static_cast<std::size_t>(length) + 1)))) {
		return std::wstring();
	}

	::GetDlgItemTextW(m_dialog, handle, ptr, INT(length + 1));

	ws.assign(ptr, length);
	::operator delete(ptr);

	return ws;
}

//
//
//
LRESULT Application::DlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Application* application = reinterpret_cast<Application*>(::GetWindowLongPtrW(hwnd, DWLP_USER));

	switch (msg)
	{
	case WM_INITDIALOG:
		::SetWindowLongPtrW(hwnd, DWLP_USER, LONG_PTR(lparam));
		break;

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
		return (LRESULT)::GetStockObject(WHITE_BRUSH);

	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
		{
			std::wstring str;
			std::wstring data;

			application->m_method = (INT)::SendMessageW(::GetDlgItem(hwnd, IDC_ENCIPHER), BM_GETCHECK, 0, 0);

			if (application->m_method)
			{
				//	encipher
				str = application->GetWindowString(IDC_TEXTBOX);
				data = application->Encipher<WCHAR>(str.c_str());
			}
			else
			{
				//	decipher
				data = application->GetWindowString(IDC_EDITOR);
				str = application->Decipher<WCHAR>(str.c_str());
			}

			switch (::SendMessageW(::GetDlgItem(hwnd, IDC_TYPE), (UINT)CB_GETCURSEL, WPARAM(0), LPARAM(0)))
			{
			case 0:
				application->m_vector.push_back(std::make_pair<std::wstring, std::wstring>(
					std::wstring(str),
					std::wstring(data)
				));
				break;

			case 2:
			{
				std::wstringstream ss;

				for (auto iter = data.cbegin(); iter != data.cend(); iter++) 
				{
					std::wstringstream ws;

					ws << std::hex << INT(*iter);

					if (std::iswalpha(ws.str().front())) {
						ss << "0";
					}

					ss << std::hex << INT(*iter) << "h, ";
				}

				ss << "0";

				application->m_vector.push_back(std::make_pair<std::wstring, std::wstring>(
					std::wstring(str),
					std::wstring(ss.str())
				));

				break;
			}


			case 3:
			{
				std::wstringstream ss;

				for (auto iter = data.cbegin(); iter != data.cend(); iter++) {
					ss << std::hex << "0x" << std::setfill<WCHAR>('0') << INT(*iter) << ", ";
				}

				ss << "0";

				application->m_vector.push_back(std::make_pair<std::wstring, std::wstring>(
					std::wstring(str),
					std::wstring(ss.str())
				));

				break;
			}

			}

			if (application->m_method)
			{
				//	encipher
				::SetWindowTextW(::GetDlgItem(hwnd, IDC_EDITOR), application->m_vector.back().second.c_str());
			}
			else
			{
				//	decipher
				::SetWindowTextW(::GetDlgItem(hwnd, IDC_TEXTBOX), application->m_vector.back().second.c_str());
			}

			::SendMessageW(::GetDlgItem(hwnd, IDC_LISTBOX), LB_ADDSTRING, 0, (LPARAM)application->m_vector.back().first.c_str());
			
			break;
		}

		case IDC_LISTBOX:
			switch (HIWORD(wparam))
			{
			case LBN_SELCHANGE:
			{
				::CreateDialogParamW(
					application->m_instance,
					MAKEINTRESOURCEW(IDD_DETAIL),
					application->m_window->GetHandle(),
					DLGPROC(DetailProc),
					LPARAM(application)
				);

				break;
			}

			}
			break;

		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

LRESULT Application::DetailProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Application* application = reinterpret_cast<Application*>(::GetWindowLongPtrW(hwnd, DWLP_USER));

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		::SetWindowLongPtrW(hwnd, DWLP_USER, LONG_PTR(lparam));
		::ShowWindow(hwnd, TRUE);
		::SendMessageW(hwnd, WM_INITIALIZE, 0, 0);
		break;
	}

	case WM_INITIALIZE:
	{
		INT pos = (INT)::SendMessageW(::GetDlgItem(application->m_dialog, IDC_LISTBOX), LB_GETCURSEL, 0, 0);

		if (pos > application->m_vector.size()) {
			break;
		}

		::SetWindowTextW(::GetDlgItem(hwnd, IDC_DETAIL_NAME), application->m_vector.at(pos).first.c_str());
		::SetWindowTextW(::GetDlgItem(hwnd, IDC_DETAIL_EDITBOX), application->m_vector.at(pos).second.c_str());

		break;
	}

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
		return (LRESULT)::GetStockObject(WHITE_BRUSH);

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		break;

	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDC_DETAIL_BUTTON:
			::DestroyWindow(hwnd);
			break;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}
