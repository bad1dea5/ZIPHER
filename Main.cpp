//
//	2021-02-19T21:15:00Z
//

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include "Application.hpp"

//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		std::unique_ptr<Application> app(new Application(hInstance));
		BOOL result;
		MSG message;

		if (FAILED(app->initialize())) {
			return ERROR_APP_INIT_FAILURE;
		}

		while ((result = ::GetMessageW(&message, NULL, 0, 0)) > 0)
		{
			if (result == -1) {
				return ERROR_FATAL_APP_EXIT;
			}

			::TranslateMessage(&message);
			::DispatchMessageW(&message);
		}
	}
	catch (std::runtime_error)
	{
		::MessageBoxW(NULL, L"Window initialization failed!", L"ZIPHER", MB_ICONEXCLAMATION | MB_OK);
	}

	return 0;
}
