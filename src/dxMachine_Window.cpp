#include "dxMachine.h"
#include <Windows.h>

extern HWND ex_hwnd;
extern void __InitTimer();
extern void __InitAudio();
extern void __CloseAudio();
extern void __InitGraphics(int max_texture);
extern void __CloseGraphics();
extern void __UpdateInput();
extern void __UpdateTimer();

static bool active{};

namespace dxMachine
{

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
    switch (umessage) {
    case WM_DESTROY: ::PostQuitMessage(0); return 0;
    case WM_CLOSE: ::DestroyWindow(hwnd); active = false; return 0;
    default: return DefWindowProcW(hwnd, umessage, wparam, lparam);
    }
}

void InitWindow(const wchar_t* title, int w, int h, int max_texture)
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

    ex_hwnd = ::CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW, wnd.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, rw, rh,
        NULL, NULL, module, NULL);
    if (ex_hwnd == nullptr) ::MessageBoxW(NULL, L"Window handler is null", L"HATA", MB_ICONERROR | MB_OK);
    active = true;

    __InitGraphics(max_texture);
    __InitTimer();
    __InitAudio();
}

void CloseWindow()
{
    __CloseAudio();
    __CloseGraphics();
    active = false;
    ::DestroyWindow(ex_hwnd);
}

bool WindowActive()
{
    return active;
}

void ShowMessage(const wchar_t *title, const wchar_t *text)
{
    ::MessageBoxW(NULL, text, title, MB_ICONINFORMATION | MB_OK);
}

void ProcessMessage()
{
    MSG msg{};
    while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    __UpdateInput();
    __UpdateTimer();
}

}
    


