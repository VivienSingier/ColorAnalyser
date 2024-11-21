#pragma once
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <tchar.h>
#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")

class TrackBar
{
public:
	HWND mWindow;
	HWND mLabel;
	int mPosition;

	TrackBar(int x, int y, HWND hWnd);

};

