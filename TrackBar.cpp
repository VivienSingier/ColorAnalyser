#include "TrackBar.h"

TrackBar::TrackBar(int x, int y, HWND hWnd)
{
	mWindow = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL,
        WS_CHILD | WS_VISIBLE | TBS_HORZ,
        x, y, 300, 30,
        hWnd, NULL, GetModuleHandle(NULL), NULL);

    SendMessage(mWindow, TBM_SETRANGE, TRUE, MAKELPARAM(0, 255));
    SendMessage(mWindow, TBM_SETPOS, TRUE, 122);
    mPosition = 0;

    mLabel = CreateWindowEx(
        0, TEXT("STATIC"), TEXT("122"),
        WS_CHILD | WS_VISIBLE,
        x - 30, y + 5, 30, 20,
        hWnd, NULL, GetModuleHandle(NULL), NULL);
}