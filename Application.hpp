//
//	2021-02-19T22:14:00Z
//

#pragma once

#include <memory>
#include <random>
#include <vector>
#include <utility>

#include "Window.hpp"

class Application
{
protected:
	std::vector<std::pair<std::wstring, std::wstring>> m_vector;
	std::unique_ptr<Window> m_window;
	CONST HINSTANCE m_instance;
	HWND m_dialog;

	INT m_method;

	static LRESULT DlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT DetailProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	template<typename T>
	std::basic_string<T> Encipher(CONST T* ptr)
	{
		std::random_device device;
		std::mt19937_64 generator(device());
		std::uniform_int_distribution<DWORD> distribution(0x200, 0x1ef3);

		std::basic_string<T> str(ptr);

		if (str.empty()) {
			return std::basic_string<T>();
		}

		std::basic_string<T> buffer;

		for (std::size_t i = 0; i < str.size(); i++)
		{
			INT key = distribution(generator);

			buffer.push_back(T(key));
			buffer.push_back(T(key + str[i]));
		}

		return buffer;
	}

	template<typename T = WCHAR>
	std::basic_string<T> Decipher(CONST T* ptr)
	{
		std::basic_string<T> str(ptr);

		if (str.empty()) {
			return std::basic_string<T>();
		}

		std::basic_string<T> buffer;

		for (std::size_t i = 0; i < str.size(); i++)
		{
			buffer.push_back(T(str[i + 1] - str[i]));
			i++;
		}

		return buffer;
	}

	std::wstring GetWindowString(INT handle);

public:
	Application(HINSTANCE instance);
	~Application() {}

	LONG initialize();
};

