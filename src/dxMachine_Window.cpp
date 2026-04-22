#include "dxMachine.h"
#include <Windows.h>

static struct Window { // BUNU SİLECEĞİM UNUTMAMAM LAZIM
    HWND hwnd;
} glob;
static bool active;

namespace dxMachine
{

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
    switch (umessage) {
    case WM_DESTROY: ::PostQuitMessage(0); return 0;
    case WM_CLOSE: ::DestroyWindow(hwnd); active = false; return 0;
    default: return DefWindowProcW(hwnd, umessage, wparam, lparam);
    }
}

void InitWindow(const wchar_t* title, int w, int h)
{
    HINSTANCE module = ::GetModuleHandleW(NULL);
    WNDCLASSW wnd{};
    wnd.hInstance = module;
    wnd.lpfnWndProc = WndProc;
    wnd.lpszClassName = L"wndclass1";
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

    glob.hwnd = ::CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW, wnd.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, rw, rh,
        NULL, NULL, module, NULL);
    if (glob.hwnd == nullptr) ShowMessage(L"HATA", L"Window handler is null");
    active = true;
}

void CloseWindow()
{
    active = false;
    ::DestroyWindow(glob.hwnd);
}

bool WindowActive()
{
    return active;
}

void *GetWindow()
{
    return (void*)glob.hwnd;
}

void ShowMessage(const wchar_t *title, const wchar_t *text)
{
    ::MessageBoxW(NULL, text, title, MB_ICONERROR | MB_OK);
}

void ProcessMessage()
{
    MSG msg{};
    while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

}
    


