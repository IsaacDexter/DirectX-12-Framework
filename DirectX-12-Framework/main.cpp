#include "stdafx.h"
#include "Engine.h"
#include "Renderer.h"
#include "TestScene.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "misc/cpp/imgui_stdlib.h"
// For mouse input helpers
#include <windowsx.h>
#include <array>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

static std::shared_ptr<Engine> g_engine;
static std::shared_ptr<Renderer> g_renderer;
static std::shared_ptr<Window> g_window;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	g_window = std::make_shared<Window>(hInstance);
	g_renderer = std::make_shared<Renderer>();
	g_renderer->Initialize(g_window->GetHWND(), g_window->GetClientWidth(), g_window->GetClientHeight());
	g_window->Show();
	
	g_engine = std::make_shared<TestScene>(g_renderer, g_window);
	g_engine->Initialize();

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
			g_engine->Update();
			g_engine->Render();
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_engine != nullptr)
	{
		extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
		switch (msg)
		{
		case WM_PAINT:
		{
			g_engine->Update();
			g_engine->Render();
			break;
		}
		case WM_SIZE:
		{
			g_engine->OnResize();
			break;
		}
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			g_engine->OnKeyDown(wParam);
			break;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			switch (wParam)
			{
			case VK_NUMPAD1:
			{
				g_engine = std::make_unique<TestScene>(g_renderer, g_window);
				g_engine->Initialize();
			}
			break;
			case VK_NUMPAD2:
			{
				//g_engine = g_scenes[2];
			}
			break;
			default:
				break;
			}
			g_engine->OnKeyUp(wParam);
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_MOUSEMOVE:
		{
			g_engine->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
			
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
