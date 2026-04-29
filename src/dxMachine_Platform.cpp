#include "dxMachine.h"
#include <Windows.h>
#define EXTERNAL

static struct Timer {
    LARGE_INTEGER freq;
    LARGE_INTEGER last;
    float delta;
} timer;

static bool curr_keys[256] = { false };
static bool prev_keys[256] = { false };
static bool input_mouse_left = false;
static bool input_mouse_right = false;
static int input_mouse_x = 0;
static int input_mouse_y = 0;

static LONG wnd_style = 0;
static RECT wnd_rect{};
static bool wnd_fullscreen = false;
static bool wnd_active{};
EXTERNAL HWND wnd_handler{};
EXTERNAL int wnd_w{};
EXTERNAL int wnd_h{};

static LRESULT CALLBACK dxWndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
    switch (umessage) {
    case WM_CLOSE: ::DestroyWindow(hwnd); wnd_active = false; return 0;
    case WM_MOUSEMOVE:
        input_mouse_x = LOWORD(lparam);
        input_mouse_y = HIWORD(lparam);
        return 0;
    case WM_LBUTTONDOWN: input_mouse_left = true; return 0;
    case WM_LBUTTONUP: input_mouse_left = false; return 0;
    case WM_RBUTTONDOWN: input_mouse_right = true; return 0;
    case WM_RBUTTONUP: input_mouse_right = false; return 0;
    default: return ::DefWindowProcW(hwnd, umessage, wparam, lparam);
    }
}

void __InitWindow(const wchar_t* title, int w, int h)
{
    wnd_w = w;
    wnd_h = h;
    HINSTANCE module = ::GetModuleHandleW(NULL);
    WNDCLASSW wnd{};
    wnd.hInstance = module;
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.lpfnWndProc = dxWndProc;
    wnd.lpszClassName = L"dxwndclass1";
    wnd.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    ::RegisterClassW(&wnd);

    ::SetProcessDPIAware();
    if (w < 1 || h < 1) {
        w = ::GetSystemMetrics(SM_CXSCREEN);
        h = ::GetSystemMetrics(SM_CYSCREEN);
    }

    RECT rc = { 0, 0, w, h };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
    ::AdjustWindowRectEx(&rc, style, FALSE, exStyle);
    int rw = rc.right - rc.left;
    int rh = rc.bottom - rc.top;

    wnd_handler = ::CreateWindowExW(
        exStyle, wnd.lpszClassName, title,
        style | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, rw, rh,
        NULL, NULL, module, NULL);
    if (wnd_handler == nullptr) {
        ::MessageBoxW(NULL, L"Pencere Yaratılamadı!", L"HATA", MB_ICONERROR | MB_OK);
        exit(-1);
    }
    wnd_active = true;

    ::QueryPerformanceFrequency(&timer.freq);
    ::QueryPerformanceCounter(&timer.last);
}

void __CloseWindow()
{
    wnd_active = false;
    ::DestroyWindow(wnd_handler);
}

namespace dxMachine
{
    void ToggleFullscreen()
    {
        if (!wnd_fullscreen) 
        {
            ::GetWindowRect(wnd_handler, &wnd_rect);
            wnd_style = ::GetWindowLong(wnd_handler, GWL_STYLE);

            int screen_w = ::GetSystemMetrics(SM_CXSCREEN);
            int screen_h = ::GetSystemMetrics(SM_CYSCREEN);
            ::SetWindowLong(wnd_handler, GWL_STYLE, wnd_style & ~(WS_CAPTION | WS_THICKFRAME));
            ::SetWindowPos(wnd_handler, HWND_TOP, 0, 0, screen_w, screen_h, SWP_NOZORDER | SWP_FRAMECHANGED);

            wnd_w = screen_w;
            wnd_h = screen_h;
            wnd_fullscreen = true;
        } 
        else 
        {
            ::SetWindowLong(wnd_handler, GWL_STYLE, wnd_style);

            int old_w = wnd_rect.right - wnd_rect.left;
            int old_h = wnd_rect.bottom - wnd_rect.top;

            ::SetWindowPos(wnd_handler, NULL, wnd_rect.left, wnd_rect.top, old_w, old_h, SWP_NOZORDER | SWP_FRAMECHANGED);

            RECT client_rect;
            ::GetClientRect(wnd_handler, &client_rect);
            wnd_w = client_rect.right - client_rect.left;
            wnd_h = client_rect.bottom - client_rect.top;
            wnd_fullscreen = false;
        }
    }

    bool ProcessMessage()
    {
        // window messages
        MSG msg{};
        while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

        // input update
        for (int i = 0; i < 256; i++)
        {
            prev_keys[i] = curr_keys[i];
            curr_keys[i] = (::GetAsyncKeyState(i) & 0x8000) != 0;
        }

        // timer update
        LARGE_INTEGER cur;
        ::QueryPerformanceCounter(&cur);
        timer.delta = (float)(cur.QuadPart - timer.last.QuadPart) / (float)timer.freq.QuadPart;
        timer.last = cur;
        if (timer.delta > 0.1f) timer.delta = 0.1f;

        return wnd_active;
    }

    bool KeyDown(int k)
    {
        return curr_keys[k];
    }

    bool KeyPressed(int k)
    {
        return curr_keys[k] && !prev_keys[k];
    }

    bool KeyReleased(int k)
    {
        return !curr_keys[k] && prev_keys[k];
    }

    void MousePos(int* x, int* y)
    {
        *x = input_mouse_x;
        *y = input_mouse_y;
    }

    bool LeftMouseButtonState()
    {
        return input_mouse_left;
    }

    bool RightMouseButtonState()
    {
        return input_mouse_right;
    }

    float GetDeltaTime()
    {
        return timer.delta;
    }

}
    


