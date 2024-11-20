#pragma once
#include <objidl.h>
#include <gdiplus.h>
#include <windows.h>
#include <commdlg.h>
#include <string>

#include "Message.h"
#include <bitset>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class MyImage
{
	static MyImage* mInstance;
	int mX;
	int mY;
	bool mHasMessageHidden;

public:
	Bitmap* mImage;
	Rect* mRect;
	  
	static MyImage* GetInstance();
	MyImage();
	 
	void LoadFromBrowser(HWND hWnd);
	void ScaleImage();
	void GrayScale(HWND hWnd);
	void InvertImage(HWND hWnd);

	void FreeLightestBit(int x, int y);
	void WriteOnLightestBit(int x, int y);
	bool ReadLightestBit(int x, int y);

	void CryptMessage(HWND hWnd, Message* message);
	void SetMessageCryptInfo();
	bool IsCrypted();
	int GetMessageLength();

};

