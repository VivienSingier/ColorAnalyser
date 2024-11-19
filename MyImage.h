#pragma once
#include <objidl.h>
#include <gdiplus.h>
#include <windows.h>
#include <commdlg.h>
#include <string>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class MyImage
{
	static MyImage* mInstance;

public:
	Bitmap* mImage;
	Rect* mRect;
	int x;
	int y;
	  
	static MyImage* GetInstance();
	MyImage();
	 
	void LoadFromBrowser(HWND hWnd);
	void ScaleImage();
	void GrayScale(HWND hWnd);
	void InvertImage(HWND hWnd);

};

