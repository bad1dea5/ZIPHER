//
//	2021-03-07T12:04:00Z
//

#pragma once

#include <iomanip>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, \
	"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" \
)

typedef struct {
	TCITEMW name;
	INT resource;
	DLGPROC proc;
} TABPAGE;

typedef struct {
	std::wstring in;
	std::wstring out;
	DWORD min;
	DWORD max;
	INT type;
} DATALIST;

//
//
//
class Application
{
private:
	static INT_PTR DlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static INT_PTR DetailProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

protected:
	HWND m_window;
	HINSTANCE m_instance;
	std::vector<std::pair<HWND, TABPAGE>> m_dialog;

public:
	std::vector<DATALIST> m_datalist;

	Application(HINSTANCE instance);
	~Application();

	LRESULT initialize();

	inline HWND GetWindow() CONST { return m_window; }
	std::wstring GetWindowText(HWND hwnd, INT control);
};

