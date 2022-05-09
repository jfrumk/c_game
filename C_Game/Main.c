#pragma warning(push, 3)

#include <windows.h>

#pragma warning(pop)

#include <stdint.h>

#include "Main.h"

HWND g_GameWindow;
BOOL g_GameIsRunning;
GAMEBITMAP g_BackBuffer = { 0 };
RECT g_GameWindowSize;
MONITORINFO g_MonitorInfo = { sizeof(MONITORINFO) };

int __stdcall WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    INT nCmdShow)
{

    if (GameIsAlreadyRunning() == TRUE)
    {
        MessageBoxA(NULL, "Another instance of this program is already running!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }
    if (CreateMainGameWindow() != ERROR_SUCCESS)
    {
        goto Exit;
    }

    g_BackBuffer.BitmapInfo.bmiHeader.biSize = sizeof(g_BackBuffer.BitmapInfo.bmiHeader);
    g_BackBuffer.BitmapInfo.bmiHeader.biWidth = GAME_RES_WIDTH;
    g_BackBuffer.BitmapInfo.bmiHeader.biHeight = GAME_RES_HEIGHT;
    g_BackBuffer.BitmapInfo.bmiHeader.biBitCount = GAME_BPP;
    g_BackBuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;
    g_BackBuffer.BitmapInfo.bmiHeader.biPlanes = 1;
    if ((g_BackBuffer.Memory = VirtualAlloc(NULL, GAME_DRAWING_MEM_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) == NULL)
    {
        MessageBoxA(NULL, "Failed to allocate memory for drawing surface!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    MSG msg = { 0 };

    g_GameIsRunning = TRUE;

    while (g_GameIsRunning) // MAIN GAME LOOP
    {
        // this while loop represents one frame in the game
        while (PeekMessageA(&msg, g_GameWindow, 0, 0, PM_REMOVE))
        {
            DispatchMessageA(&msg);
        }

        ProcPlayerInput();

        RenderFrame();

        /* Sleep(0) -- yield to any other thread that wants to run.
        Sleep(0) isn't sufficient enough for our app
        Another problem is that we don't know how long the other thread will run
        The game would be paused until that thread is finished */

        Sleep(1); // temporary
    }

Exit:
    return 0;
}

LRESULT CALLBACK MainWndProc(
    _In_ HWND hwnd,     // handle to window
    _In_ UINT uMsg,     // message identifier
    _In_ WPARAM wParam, // first message parameter
    _In_ LPARAM lParam) // second message parameter
{
    LRESULT res = 0;
    switch (uMsg)
    {
    case WM_CLOSE:
    {
        g_GameIsRunning = FALSE;
        PostQuitMessage(0);
        break;
    }
    default:
        res = DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    return res;
}

DWORD CreateMainGameWindow()
{
    DWORD res = ERROR_SUCCESS;
    WNDCLASSEXA wc = { 0 }; // initialize data struct to all 0s

    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = 0;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 0, 255));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = GAME_NAME "_WINDOWCLASS";

    if (RegisterClassExA(&wc) == 0)
    {
        res = GetLastError();
        MessageBoxA(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    g_GameWindow = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        wc.lpszClassName,
        "Window Title",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL, NULL, wc.hInstance, NULL);

    if (g_GameWindow == NULL)
    {
        res = GetLastError();
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    if (GetMonitorInfoA(MonitorFromWindow(g_GameWindow, MONITOR_DEFAULTTOPRIMARY), &g_MonitorInfo) == 0)
    {
        res = ERROR_INVALID_MONITOR_HANDLE;
        goto Exit;
    }

    int MonitorWidth = g_MonitorInfo.rcMonitor.right - g_MonitorInfo.rcMonitor.left;
    int MonitorHeight = g_MonitorInfo.rcMonitor.bottom - g_MonitorInfo.rcMonitor.top;

Exit:
    return res;
}

BOOL GameIsAlreadyRunning(void)
{
    HANDLE mutex = NULL;
    mutex = CreateMutexA(NULL, FALSE, GAME_NAME "_GameMutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

void ProcPlayerInput()
{
    SHORT EscKeyDown = GetAsyncKeyState(VK_ESCAPE);

    if (EscKeyDown)
    {
        SendMessageA(g_GameWindow, WM_CLOSE, 0, 0);
    }
}

void RenderFrame()
{
    HDC DeviceContext = GetDC(g_GameWindow);

    StretchDIBits(DeviceContext, 0, 0, 100, 100, 0, 0, 100, 100, g_BackBuffer.Memory, &g_BackBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

    ReleaseDC(g_GameWindow, DeviceContext);
}