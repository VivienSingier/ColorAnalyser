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
	int mMessageMaxLength;
	bool mHasMessageHidden;
	Bitmap* mDefaultImage;

public:
	Bitmap* mImage;
	Rect* mRect;
	  
	static MyImage* GetInstance();
	MyImage();

	int GetMaxLength();

	//Importer Image 
	void LoadFromBrowser(HWND hWnd);
	void SaveImage(HWND hWnd);
	void SetEditLimit(HWND hWndEdit);

	//Filtres
	void ScaleImage();
	void GrayScale(HWND hWnd);
	void InvertImage(HWND hWnd);

	//Opérations sur le bit leger d'un pixel de l'image
	void FreeLightestBit(int x, int y);
	void WriteOnLightestBit(int x, int y);
	bool ReadLightestBit(int x, int y);

	//Ecriture et lecture de message dans l'image
	void CryptMessage(HWND hWnd, Message* message);
	void SetMessageCryptInfo();
	bool IsCrypted();
	int GetMessageLength();
	wchar_t* ReadMessage();

};

