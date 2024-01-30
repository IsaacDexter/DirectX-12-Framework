#include "Window.h"

// disambiguate global min/max macros defined in window headers
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif


void Window::RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName)
{
    // Register a window class for creating our render window with.
    WNDCLASSEXW windowClass = {};

    // Size in bytes of this structure
    windowClass.cbSize = sizeof(WNDCLASSEX);
    // Class styles: HREDRAW/VREDRAW specified the entire window is redrawn after movement/size adjustment of Width/Height
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    // Pointer to windows procedure to handle window messages
    windowClass.lpfnWndProc = &WndProc;
    // Extra bytes to allocate to window class structure
    windowClass.cbClsExtra = 0;
    // Extra bytes to allocate to window instance
    windowClass.cbWndExtra = 0;
    // Handle to the instance that contains the window  procedure for the class
    windowClass.hInstance = hInst;
    // Handle to class icon, can be loaded using LoadIcon function 
    windowClass.hIcon = ::LoadIcon(hInst, NULL);
    // Handle to class cursor, loading default arrow
    windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    // Handle to class background brush, which takes a colour value converted to a HBRUSH type
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    // pointer to character string that specifies class menu's resource name
    windowClass.lpszMenuName = NULL;
    // const string used to uniquely identify window class, used to create window instance
    windowClass.lpszClassName = windowClassName;
    // handle to small icon associated with window class
    windowClass.hIconSm = ::LoadIcon(hInst, NULL);

    static ATOM atom = ::RegisterClassExW(&windowClass);

    assert(atom > 0);
    //now windows class is registered, OS window instance can be created
}

HWND Window::CreateApplicationWindow(const wchar_t* windowClassName, HINSTANCE hInst, const wchar_t* windowTitle, uint32_t width, uint32_t height)
{
    // retrieve specific system information, i.e. width/heigh of primary display
    int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    // calculate required size of window rectangle, describing a window that can be maxed and minned
    RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // dimensions of adjusted window are used to compute width and height of created window
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // center the window within the screen. Clamp to 0, 0 for the top-left corner.
    int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
    int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

    HWND hWnd = ::CreateWindowExW(
        NULL,   // extended style of created window https://learn.microsoft.com/en-gb/windows/win32/winmsg/extended-window-styles
        windowClassName,    // class name registered with register class
        windowTitle,    // name to show in windows title bar
        WS_OVERLAPPEDWINDOW,    // style of window being created
        windowX,    // horizontal pos of window
        windowY,    // vertical pos of window
        windowWidth,    // width of window in device units
        windowHeight,   // height of window in device units
        NULL,   // owner/parent window of created window
        NULL,   // handle to a menu/child window
        hInst,  // handle to the instance of the module associated with the window
        nullptr // pointer to a value to be passed to the window through CREATESTRUCT
    );

    assert(hWnd && "Failed to create window");

    return hWnd;
}

Window::Window(HINSTANCE hInstance)
{
    // set DPI awareness for the current thread, allowing it to achieve 100% pixel scaling for the client area of the window
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // window class name, used for registering / creating the window
    const wchar_t* windowClassName = L"DX12WindowClass";
    

    // register the window class with the application instance
    RegisterWindowClass(hInstance, windowClassName);
    // create the window 
    g_hWnd = CreateApplicationWindow(
        windowClassName,    //registered window class
        hInstance,  // instance
        L"DX12 Application",    //window name
        g_clientWidth,    //width
        g_clientHeight    //height
    );
    // get DPI scaling for window, dividing by windows default 96 PPI https://en.wikipedia.org/wiki/Dots_per_inch#Computer_monitor_DPI_standards:~:text=Microsoft%20Windows%20operating%20system%20has%20used%20a%20default%20of%2096%20PPI.
    m_dpi = ::GetDpiForWindow(g_hWnd) / 96.0f;
    // initialize window rect variable, preparing for toggling full screen state of the windows
    ::GetWindowRect(g_hWnd, &g_windowRect);
}

void Window::Show()
{
    // show application window
    ::ShowWindow(g_hWnd, SW_SHOW);
}

void Window::Shutdown()
{
}

void Window::SetFullscreen(bool fullscreen)
{
    if (g_fullscreen != fullscreen)
    {
        g_fullscreen = fullscreen;
        // if switching to fullscreen
        if (g_fullscreen)
        {
            // store current window dimensions to restore to when leaving fullscreen
            ::GetWindowRect(g_hWnd, &g_windowRect);

            // change window style to BWFS
            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
            ::SetWindowLong(g_hWnd, GWL_STYLE, windowStyle);

            // retrieve handle to nearest monitor to application
            HMONITOR hMonitor = ::MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTONEAREST);
            // query dimensions of nearest monitor to app window
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            ::GetMonitorInfo(hMonitor, &monitorInfo);

            // set window dimensions from monitor info
            ::SetWindowPos(
                g_hWnd, //window's handle
                HWND_TOP,   // place the window in front of other windows
                monitorInfo.rcMonitor.left, // position of the left side of the window
                monitorInfo.rcMonitor.top,  // top
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,   // width
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,   // height
                SWP_FRAMECHANGED | SWP_NOACTIVATE   // size and positioning flags: apply frame style specied in SetWindowLong, do not activate the window
            );
            // display the window in a maximized state
            ::ShowWindow(g_hWnd, SW_MAXIMIZE);
        }
        //if switching from fullscreen
        else
        {
            //restore window decorators
            ::SetWindowLong(g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

            // set window dimensions from stored info
            ::SetWindowPos(
                g_hWnd, //window's handle
                HWND_NOTOPMOST,   // place the window back into the z order
                g_windowRect.left, // position of the left side of the window
                g_windowRect.top,  // top
                g_windowRect.right - g_windowRect.left,   // width
                g_windowRect.bottom - g_windowRect.top,   // height
                SWP_FRAMECHANGED | SWP_NOACTIVATE   // size and positioning flags: apply frame style specied in SetWindowLong, do not activate the window
            );
            // display the window normally
            ::ShowWindow(g_hWnd, SW_NORMAL);
        }
    }
}

bool Window::Resize()
{
    // query area of the window
    RECT clientRect = {};
    ::GetClientRect(g_hWnd, &clientRect);

    // compute width and height to rezise swap chain buffers
    uint32_t width = clientRect.right - clientRect.left;
    uint32_t height = clientRect.bottom - clientRect.top;

    //early out if the window is already this size
    bool sizeChanged = g_clientWidth != width || g_clientHeight != height;
    if (sizeChanged)
    {
        // prevent size 0 swap chain buffers
        g_clientWidth = std::max(1u, width);
        g_clientHeight = std::max(1u, height);
    }
    return sizeChanged;
}

bool Window::Resize(uint32_t width, uint32_t height)
{
    // early out if the windows size is unchanged
    bool sizeChanged = g_clientWidth != width || g_clientHeight != height;
    if (sizeChanged)
    {
        // adjust the existing window rect according to the parameters
        // prevent size 0 swap chain buffers
        g_clientWidth = std::max(1u, width);
        g_clientHeight = std::max(1u, height);

        RECT rect = { width, height };

        g_windowRect = rect;

        // compute the size of the window rectangle
        ::AdjustWindowRectExForDpi(&rect, ::GetWindowLong(g_hWnd, GWL_STYLE), FALSE, m_dpi, WS_EX_OVERLAPPEDWINDOW);
        // size the client window
        // set window dimensions from stored info
        ::SetWindowPos(
            g_hWnd, //window's handle
            HWND_NOTOPMOST,   // place the window back into the z order
            rect.left, // position of the left side of the window
            rect.top,  // top
            rect.right - rect.left,   // width
            rect.bottom - rect.top,   // height
            SWP_FRAMECHANGED | SWP_NOACTIVATE   // size and positioning flags: apply frame style specied in SetWindowLong, do not activate the window
        );
    }
    return sizeChanged;
}

void Window::SetName(const wchar_t* name)
{
    ::SetWindowTextW(g_hWnd, name);
}

void Window::SetClientWidth(uint32_t width)
{
    g_clientWidth = std::max(1u, width);
}

void Window::SetClientHeight(uint32_t height)
{
    g_clientHeight = std::max(1u, height);
}
