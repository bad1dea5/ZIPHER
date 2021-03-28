//
//	2021-03-07T15:53:00Z
//

#include <random>

#include "Application.hpp"
#include "Resource.hpp"

//
//
//
template<typename T>
std::basic_string<T> Encipher(std::basic_string<T>& str, INT min = 0x200, INT max = 0x1ef0)
{
	std::random_device device;
	std::mt19937_64 generator(device());
	std::uniform_int_distribution<DWORD> distribution(min, max);
	std::basic_string<T> buffer;

	if (str.empty()) {
		return std::basic_string<T>();
	}

	for (auto i = 0; i < str.size(); i++)
	{
		INT key = distribution(generator);
		buffer.push_back(T(key));
		buffer.push_back(T(key + str.at(i)));
	}

	return buffer;
}

//
//
// 
LRESULT doEncipher(Application* application, HWND hwnd)
{
	DWORD min = 0, max = 0;
	DATALIST data;
	std::wstring buffer;
	std::wstringstream ws;

	data.in = application->GetWindowText(hwnd, IDE_EDIT);

	if (data.in.empty()) {
		return -1;
	}

	buffer = application->GetWindowText(hwnd, IDE_MIN);
	data.min = std::stoul(buffer, nullptr, 16);

	buffer = application->GetWindowText(hwnd, IDE_MAX);
	data.max = std::stoul(buffer, nullptr, 16);

	data.out = Encipher<WCHAR>(data.in, data.min, data.max);
	data.type = ::SendMessage(::GetDlgItem(hwnd, IDE_TYPE), CB_GETCURSEL, 0, 0);

	switch (data.type)
	{

	case 1:
	{
		for (auto iter = data.out.cbegin(); iter != data.out.cend(); iter++)
		{
			std::wstringstream ss;

			ss << std::hex << INT(*iter);

			if (std::isalpha(ss.str().front())) {
				ws << "0";
			}

			ws << std::hex << INT(*iter) << "h,";
		}

		ws << "0";
		data.out = ws.str();

		break;
	}

	case 0:
	case 2:
	{
		for (auto iter = data.out.cbegin(); iter != data.out.cend(); iter++) {
			ws << std::hex << "0x" << std::setfill<WCHAR>('0') << INT(*iter) << ", ";
		}

		ws << "0";
		data.out = ws.str();

		break;
	}

	case 3:
		break;

	default:
		break;
	}

	application->m_datalist.push_back(data);

	::SetWindowTextW(::GetDlgItem(hwnd, IDE_DATA), application->m_datalist.back().out.c_str());
	::SendMessageW(::GetDlgItem(application->GetWindow(), IDC_LISTBOX), LB_ADDSTRING, 0, LPARAM(application->m_datalist.back().in.c_str()));

	return 0;
}

//
//
//
LRESULT initialize(HWND hwnd)
{
	//
	//
	//
	WCHAR types[4][16] = { L"Normal", L"ASM (AT&T)", L"ASM (Intel)", L"C++" };

	for (auto i = 0; i < ((sizeof(types) / 16) / 2); i++) {
		::SendMessage(::GetDlgItem(hwnd, IDE_TYPE), CB_ADDSTRING, 0, LPARAM(types[i]));
	}

	::SendMessage(::GetDlgItem(hwnd, IDE_TYPE), CB_SETCURSEL, WPARAM(3), 0);

	//
	//
	//
	::SendMessageW(::GetDlgItem(hwnd, IDE_MIN_SPIN), UDM_SETBASE, 16, 0);
	::SendMessageW(::GetDlgItem(hwnd, IDE_MIN_SPIN), UDM_SETRANGE, 0, MAKELONG(0, 0x2000));

	::SendMessageW(::GetDlgItem(hwnd, IDE_MAX_SPIN), UDM_SETBASE, 16, 0);
	::SendMessageW(::GetDlgItem(hwnd, IDE_MAX_SPIN), UDM_SETRANGE, 0, MAKELONG(0, 0x2000));

	::SetWindowTextW(::GetDlgItem(hwnd, IDE_MIN), LPCWSTR(L"0x0200"));
	::SetWindowTextW(::GetDlgItem(hwnd, IDE_MAX), LPCWSTR(L"0x1EF3"));

	return 0;
}

//
//
//
INT_PTR EncipherProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	Application* application = reinterpret_cast<Application*>(::GetWindowLongPtrW(hwnd, DWLP_USER));

	switch (message)
	{
	case WM_INITDIALOG:
	{
		::SetWindowLongPtrW(hwnd, DWLP_USER, LONG_PTR(lparam));

		if (FAILED(initialize(hwnd))) {
			return FALSE;
		}

		break;
	}

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
		return (LRESULT)::GetStockObject(WHITE_BRUSH);

	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
		case IDE_OPTION1:
		{
			if (::IsDlgButtonChecked(hwnd, IDE_OPTION1))
			{
				::SendMessageW(::GetDlgItem(hwnd, IDE_MIN_SPIN), UDM_SETRANGE, 0, MAKELONG(0x0021, 0x007E));
				::SendMessageW(::GetDlgItem(hwnd, IDE_MAX_SPIN), UDM_SETRANGE, 0, MAKELONG(0x0021, 0x0081));

				::SetWindowTextW(::GetDlgItem(hwnd, IDE_MIN), LPCWSTR(L"0x0021"));
				::SetWindowTextW(::GetDlgItem(hwnd, IDE_MAX), LPCWSTR(L"0x0081"));
			}
			else
			{
				::SendMessageW(::GetDlgItem(hwnd, IDE_MIN_SPIN), UDM_SETRANGE, 0, MAKELONG(0, 0x2000));
				::SendMessageW(::GetDlgItem(hwnd, IDE_MAX_SPIN), UDM_SETRANGE, 0, MAKELONG(0, 0x2000));

				::SetWindowTextW(::GetDlgItem(hwnd, IDE_MIN), LPCWSTR(L"0x0200"));
				::SetWindowTextW(::GetDlgItem(hwnd, IDE_MAX), LPCWSTR(L"0x1EF3"));
			}
			break;
		}

		case IDE_OPTION4:
		{
			if (::IsDlgButtonChecked(hwnd, IDE_OPTION4))
			{
				::EnableWindow(::GetDlgItem(hwnd, IDE_MIN), TRUE);
				::EnableWindow(::GetDlgItem(hwnd, IDE_MIN_SPIN), TRUE);
				::EnableWindow(::GetDlgItem(hwnd, IDE_MAX), TRUE);
				::EnableWindow(::GetDlgItem(hwnd, IDE_MAX_SPIN), TRUE);
			}
			else
			{
				::EnableWindow(::GetDlgItem(hwnd, IDE_MIN), FALSE);
				::EnableWindow(::GetDlgItem(hwnd, IDE_MIN_SPIN), FALSE);
				::EnableWindow(::GetDlgItem(hwnd, IDE_MAX), FALSE);
				::EnableWindow(::GetDlgItem(hwnd, IDE_MAX_SPIN), FALSE);
			}
			break;
		}

		case IDOK:
		{
			if (FAILED(doEncipher(application, hwnd))) {
				::SetDlgItemTextW(hwnd, IDE_DATA, LPCWSTR(L"FAIL: cannot encipher string"));
			}

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
