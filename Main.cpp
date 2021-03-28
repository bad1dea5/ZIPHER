//
//	2021-03-07T12:03:00Z
//

#define WIN32_LEAN_AND_MEAN

#include "Application.hpp"

//
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	std::unique_ptr<Application> application;
	MSG message;
	BOOL result;

	try { application = std::unique_ptr<Application>(new Application(hInstance)); }
	catch (std::runtime_error)
	{
		::MessageBoxW(NULL, L"Failed to create application!", L"Error", MB_ICONERROR | MB_OK);
		return -1;
	}

	if (FAILED(application->initialize()))
	{
		::MessageBoxW(NULL, L"Failed to initialize application!", L"Error", MB_ICONERROR | MB_OK);
		return -1;
	}

	while ((result = ::GetMessageW(&message, NULL, 0, 0)) != 0)
	{
		if (result == -1) {
			return -1;
		}

		if (!::IsDialogMessageW(application->GetWindow(), &message))
		{
			::TranslateMessage(&message);
			::DispatchMessageW(&message);
		}
	}

	return static_cast<INT>(message.wParam);
}
