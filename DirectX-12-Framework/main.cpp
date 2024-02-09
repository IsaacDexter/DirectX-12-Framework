#include "stdafx.h"
#include "Engine.h"
#include "Window.h"
// For mouse input helpers
#include <windowsx.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

static std::unique_ptr<Engine> g_application;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
	g_application = std::make_unique<Engine>(hInstance);
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
		else
		{
			g_application->Update();
			g_application->Render();
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_application != nullptr)
	{
		extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
		switch (msg)
		{
		case WM_PAINT:
		{
			g_application->Update();
			g_application->Render();
			break;
		}
		case WM_SIZE:
		{
			g_application->OnResize();
			break;
		}
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			g_application->OnKeyDown(wParam);
			break;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			g_application->OnKeyUp(wParam);
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_MOUSEMOVE:
		{
			g_application->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
			
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return (0);
			break;
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
