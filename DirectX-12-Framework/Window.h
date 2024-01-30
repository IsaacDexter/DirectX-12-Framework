#pragma once
#include "stdafx.h"
#include <string>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


class Window
{
private:
	// Size of client area on window creation
	uint32_t g_clientWidth = 1280;
	uint32_t g_clientHeight = 720;
	/**
	* set dpi, which governs scaling factor used in resizing the screen
	*/
	float m_dpi;

	/// <summary>Handle to OS window used to display rendered image</summary>
	HWND g_hWnd;

	/// <summary>Stores previous size of window for switching back to windowed mode</summary>
	RECT g_windowRect;

	bool g_fullscreen = false;

private:
	/// <summary>Before creating OS window instance, register window class. Automatically unregistered on application termination</summary>
	/// <param name="hInst">handle to the instance</param>
	/// <param name="windowClassName">const string used to uniquely identify window class, used to create window instance</param>
	void RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName);
	/// <summary>Create instance of OS window</summary>
	/// <param name="windowClassName">const string used to uniquely identify window class, used to create window instance</param>
	/// <param name="hInst">handle to the instance</param>
	/// <param name="windowTitle">const string as the window title</param>
	/// <param name="width">width of window in pixels</param>
	/// <param name="height">height of window in pixels</param>
	/// <returns>handle to the created window instance</returns>
	HWND CreateApplicationWindow(const wchar_t* windowClassName, HINSTANCE hInst, const wchar_t* windowTitle, uint32_t width, uint32_t height);

public:
	Window(HINSTANCE hInstance);

	// shows the already created window. separate to avoid showing window without running class
	void Show();

	void Shutdown();

	/// <summary>Set the application to fullscreen (windowed borderless as its easier and doesnt lock mouse cursor)</summary>
	/// <param name="fullscreen">true = BWFS, false = windowed</param>
	void SetFullscreen(bool fullscreen);
	void SetFullscreen()
	{
		SetFullscreen(!g_fullscreen);
	}

	/// <summary>Updates g_clientWidth/Height, and returns if theres a change</summary>
	/// <returns>whether or not there was a change in size</returns>
	bool Resize();
	/** 
	* Resize the window to be a certain size, then resize the buffers in the application
	* @param width New width of the window
	* @param height New Height of the window
	* @return whether or not the window size changed
	*/
	bool Resize(uint32_t width, uint32_t height);
	/** 
	* Set the windows display name
	* @param name const wchar_t* to use as the window name. Try using wstring.data()
	*/
	void SetName(const wchar_t* name);


#pragma region Getters/Setters

	const HWND& GetHWND() 
	{
		return g_hWnd;
	}

	uint32_t GetClientWidth()
	{
		return g_clientWidth;
	}
	
	uint32_t GetClientHeight()
	{
		return g_clientHeight;
	}
	
	void SetClientWidth(uint32_t width);

	
	void SetClientHeight(uint32_t height);

#pragma endregion

};

