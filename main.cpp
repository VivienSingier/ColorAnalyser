// ColorAnalyser.cpp : Définit le point d'entrée de l'application.
//
#include <objidl.h>
#include <gdiplus.h>
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <iostream>
#include <string>
#include "framework.h"
#include "ColorAnalyser.h"

#include "MyImage.h"
#include "TrackBar.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND LoadImageButton;
HWND GrayScaleButton;
HWND InvertButton;
HWND WriteMessageButton;
HWND ReadMessageButton;
HWND SaveAsButton;
HWND ResetImageButton;

TrackBar* TBRed;
TrackBar* TBGreen;
TrackBar* TBBlue;

HWND EditMessage;

int WinWidth = 600;
int WinHeight = 900;
int ControlHeight = 555;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Placez le code ici.

    // Initialise les chaînes globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_COLORANALYSER, szWindowClass, MAX_LOADSTRING);

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    MyRegisterClass(hInstance);

    // Effectue l'initialisation de l'application :
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COLORANALYSER));

    MSG msg;

    // Boucle de messages principale :
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}



//
//  FONCTION : MyRegisterClass()
//
//  OBJECTIF : Inscrit la classe de fenêtre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COLORANALYSER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_COLORANALYSER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   OBJECTIF : enregistre le handle d'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        nous créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Stocke le handle d'instance dans la variable globale

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
      CW_USEDEFAULT, 0, WinWidth, WinHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   LoadImageButton = CreateWindow( L"BUTTON", L"Load Image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       (WinWidth / 2) - 160 , 25, 150, 20, hWnd, (HMENU)1, 
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

   GrayScaleButton = CreateWindow( L"BUTTON", L"GrayScale", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       10, ControlHeight + 180, 150, 20, hWnd, (HMENU)2,
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

   InvertButton = CreateWindow( L"BUTTON", L"Invert", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       10, ControlHeight + 210, 150, 20, hWnd, (HMENU)3,
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

   WriteMessageButton = CreateWindow( L"BUTTON", L"Encrypt", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       (WinWidth / 2) - 160, ControlHeight, 150, 20, hWnd, (HMENU)4,
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

   ReadMessageButton = CreateWindow( L"BUTTON", L"Decrypt", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       (WinWidth / 2) + 10, ControlHeight, 150, 20, hWnd, (HMENU)6,
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

   SaveAsButton = CreateWindow( L"BUTTON", L"Save As", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       (WinWidth / 2) + 10, 25, 150, 20, hWnd, (HMENU)7,
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

   ResetImageButton = CreateWindow(L"BUTTON", L"Reset", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       (WinWidth / 2) - 75, ControlHeight + 140, 150, 20, hWnd, (HMENU)5,
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

   TBRed = new TrackBar(280, ControlHeight + 180, hWnd);
   TBGreen = new TrackBar(280, ControlHeight + 210, hWnd);
   TBBlue = new TrackBar(280, ControlHeight + 240, hWnd);

   EditMessage = CreateWindow(
       L"EDIT",
       0,
       WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
       200, ControlHeight + 30, 200, 100,
       hWnd,
       NULL,
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int UpdateTrackbarValue(TrackBar* tb)
{
    // Retrieve the current value of the trackbar
    int value = SendMessage(tb->mWindow, TBM_GETPOS, 0, 0);

    // Update the label text
    wchar_t text[32];
    swprintf_s(text, L"%d", value);
    SetWindowText(tb->mLabel, text);

    return value;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyImage* image = MyImage::GetInstance();

    static HBRUSH hBrushRED;
    static HBRUSH hBrushGREEN;
    static HBRUSH hBrushBLUE;

    int red;
    int green;
    int blue;

    switch (message)
    {
    case WM_CREATE:
    {
        hBrushRED = CreateSolidBrush(RGB(255, 0, 0));
        hBrushBLUE = CreateSolidBrush(RGB(0, 0, 255));
        hBrushGREEN = CreateSolidBrush(RGB(0, 255, 0));
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analyse les sélections de menu :

            if (LOWORD(wParam) == 1)
            {
                image->LoadFromBrowser(hWnd);
                image->ScaleImage();
                image->SetMasc(hWnd);
                image->SetEditLimit(EditMessage);
            }
            if (LOWORD(wParam) == 2)
            {
                image->GrayScale(hWnd);
            }
            if (LOWORD(wParam) == 3)
            {
                image->InvertImage(hWnd);
            }
            if (LOWORD(wParam) == 4)
            {
                wchar_t* buffer = new wchar_t[image->GetMaxLength()];
                GetWindowText(EditMessage, buffer, image->GetMaxLength());
                Message* msg = new Message(buffer);
                image->CryptMessage(hWnd, msg);
            }
            if (LOWORD(wParam) == 5)
            {
                image->Reset(hWnd);
            }
            if (LOWORD(wParam) == 6)
            {
                wchar_t* message = image->ReadMessage();
                SetWindowText(EditMessage, message);
            }
            if (LOWORD(wParam) == 7)
            {
                image->SaveImage(hWnd);
            }
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CTLCOLORSTATIC:
        {
            if ((HWND)lParam == TBRed->mWindow)
            {
                
                HDC hdcStatic = (HDC)wParam;
                SetBkColor(hdcStatic, RGB(255, 0, 0)); 
                return (LRESULT)hBrushRED;
            }
            if ((HWND)lParam == TBBlue->mWindow)
            {
                
                HDC hdcStatic = (HDC)wParam;
                SetBkColor(hdcStatic, RGB(0, 0, 255)); 
                return (LRESULT)hBrushBLUE;
            }
            if ((HWND)lParam == TBGreen->mWindow)
            {
                
                HDC hdcStatic = (HDC)wParam;
                SetBkColor(hdcStatic, RGB(0, 255, 0)); 
                return (LRESULT)hBrushGREEN;
            }
        }
        break;
    case WM_HSCROLL:
        {
            red = image->redMasc;
            green = image->greenMasc;
            blue = image->blueMasc;

            if ((HWND)lParam == TBRed->mWindow)
            {
                red = UpdateTrackbarValue(TBRed);
            }
            if ((HWND)lParam == TBGreen->mWindow)
            {
                green = UpdateTrackbarValue(TBGreen);
            }
            if ((HWND)lParam == TBBlue->mWindow)
            {
                blue = UpdateTrackbarValue(TBBlue);
            }
            image->SetMascColors(red, green, blue);
            image->SetMasc(hWnd);
            image->SetEditLimit(EditMessage);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (image)
            {
                Pen pen(Color(255, 0, 0, 0), 5);
                Graphics graphics(hdc);
                graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
                graphics.DrawRectangle(&pen, *image->mRect);
                graphics.DrawImage(image->mImage, *image->mRect);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
