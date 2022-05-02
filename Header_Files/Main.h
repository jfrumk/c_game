#pragma once

#define GAME_NAME "Untitled_Game"
#define GAME_RES_WIDTH 384
#define GAME_RES_HEIGHT 216
#define GAME_BPP 32
#define GAME_DRAWING_MEM_SIZE (GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))

typedef struct GAMEBITMAP
{
    BITMAPINFO BitmapInfo;
    void *Memory;
} GAMEBITMAP;

LRESULT CALLBACK MainWndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

DWORD CreateMainGameWindow(void);

BOOL GameIsAlreadyRunning(void);

void ProcPlayerInput(void);

void RenderFrame(void);