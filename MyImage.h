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

	int mMessageMaxLength;
	bool mHasMessageHidden;
	Bitmap* mDefaultImage;

public:
	Bitmap* mImage;
	Bitmap* mMasquedImage;
	Rect* mRect;

	int mMaxWidth;
	int mMaxHeight;

	int mX;
	int mY;

	bool displayMasc;

	int redMasc;
	int greenMasc;
	int blueMasc;
	  
	static MyImage* GetInstance();
	MyImage();

	int GetMaxLength();

	//Importer/Enregistrer Image 
	void LoadFromBrowser(HWND hWnd);
	void SaveImage(HWND hWnd);
	void SetEditLimit(HWND hWndEdit);
	int CountValidPixels();

	//Filtres
	void ScaleImage();
	void GrayScale(HWND hWnd);
	void InvertImage(HWND hWnd);
	void Reset(HWND hWnd);
	void SetMasc(HWND hWnd);
	void SetMascColors(int red, int green, int blue);


	//Opérations sur le bit leger d'un pixel de l'image
	bool IsValidPixel(int x, int y);
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

