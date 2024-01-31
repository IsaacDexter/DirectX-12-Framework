#include "stdafx.h"
#include "Application.h"
#include "Window.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

static std::unique_ptr<Application> g_application;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	g_application = std::make_unique<Application>(hInstance);
	g_application->Initialize();


	// enter application message loop
		// standard message loop for win32 application
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_application != nullptr)
	{
		switch (msg)
		{
		case WM_PAINT:
		{
			g_application->Update();
			g_application->Render();
		}
		default:
		{
			return ::DefWindowProcW(hWnd, msg, wParam, lParam);
			break;
		}
		}
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);;
}
