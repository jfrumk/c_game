#pragma warning(push, 3)

#include <windows.h>

#pragma warning(pop)

#include "../Header_Files/Main.h"

HWND g_GameWindow;
BOOL g_GameIsRunning;
GAMEBITMAP g_DrawingSurface = {0};

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

    g_DrawingSurface.BitmapInfo.bmiHeader.biSize = sizeof(g_DrawingSurface.BitmapInfo.bmiHeader);
    g_DrawingSurface.BitmapInfo.bmiHeader.biWidth = GAME_RES_WIDTH;
    g_DrawingSurface.BitmapInfo.bmiHeader.biHeight = GAME_RES_HEIGHT;
    g_DrawingSurface.BitmapInfo.bmiHeader.biBitCount = GAME_BPP;
    g_DrawingSurface.BitmapInfo.bmiHeader.biCompression = BI_RGB;
    g_DrawingSurface.BitmapInfo.bmiHeader.biPlanes = 1;
    if ((g_DrawingSurface.Memory = VirtualAlloc(NULL, GAME_DRAWING_MEM_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) == NULL)
    {
        MessageBoxA(NULL, "Failed to allocate memory for drawing surface!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    MSG msg = {0};

    g_GameIsRunning = TRUE;

    while (g_GameIsRunning) // MAIN GAME LOOP
    {
        // this while loop represents one frame in the game
        while (PeekMessageA(&msg, g_GameWindow, 0, 0, PM_REMOVE))
        {
            DispatchMessageA(&msg);
        }

        ProcPlayerInput();

        // RenderFrame(); function

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
    WNDCLASSEXA wc = {0}; // initialize data struct to all 0s

    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = 0;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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
}