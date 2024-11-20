#include "MyImage.h"


MyImage* MyImage::mInstance = nullptr;

std::wstring OpenPngFileDialog() {
    OPENFILENAMEW ofn;       // Common dialog box structure
    wchar_t szFile[260] = { 0 }; // Buffer for the file name
    wchar_t szFilter[] = L"PNG Files\0*.png\0"; // Buffer for the filter string

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Replace NULL with your window handle if needed
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = szFilter; // Assign filter buffer
    ofn.nFilterIndex = 1;       // Default filter index
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    // Display the Open dialog box
    if (GetOpenFileNameW(&ofn) == TRUE) {
        return std::wstring(szFile); // Return the selected .png file path
    }
    return L""; // Return empty string if no valid file is selected
}

MyImage* MyImage::GetInstance()
{
	{
		if (mInstance == nullptr)
		{
			mInstance = new MyImage();
		}
		return mInstance;
	}
}

MyImage::MyImage()
{
	mImage = nullptr;
    mRect = new Rect(100, 100, 0, 0);
    mX = 100;
    mY = 100;
    mHasMessageHidden = false;
}

void MyImage::LoadFromBrowser(HWND hWnd)
{
    std::wstring path = OpenPngFileDialog();
    const wchar_t* wcharPath = path.c_str();

    if (mImage) {
        delete mImage;
        mImage = nullptr;
    }
    mImage = new Bitmap(wcharPath);
    if (!mImage)
    {
        MessageBox(hWnd, L"Failed to load image", L"Load Error", MB_ICONERROR | MB_OK);
    }
    if (mRect) {
        delete mRect;
        mRect = nullptr;
    }
    mRect = new Rect(mX, mY, mImage->GetWidth(), mImage->GetHeight());

    if (!InvalidateRect(hWnd, nullptr, TRUE))
    {
        MessageBox(hWnd, L"Failed to invalidate user window", L"Load Error", MB_ICONERROR | MB_OK);
    }
}

void MyImage::ScaleImage()
{
    int width = (int)mImage->GetWidth();
    int height = (int)mImage->GetHeight();

    if (width > 450 || height > 450)
    {
        while (width > 450 && height > 450)
        {
            width--;
            height--;
        }
    }
    else if (width < 450 || height < 450)
    {
        while (width < 450 && height < 450)
        {
            width++;
            height++;
        }
    }
    mX = 400 - width / 2;
    mY = 300 - height / 2;
    delete mRect;
    mRect = nullptr;
    mRect = new Rect(mX, mY, width, height);
}

void MyImage::GrayScale(HWND hWnd)
{
    for (int i = 0; i < mImage->GetWidth(); i++)
    {
        for (int j = 0; j < mImage->GetHeight(); j++)
        {
            Color color;
            mImage->GetPixel(i, j, &color);

            int colorIndex = ( (int)(color.GetR()) + (int)(color.GetG()) + (int)(color.GetB()) ) / 3;
            color.SetValue(Color::MakeARGB(color.GetA(), colorIndex, colorIndex, colorIndex));
            mImage->SetPixel(i, j, color);
        }
    }
    if (!InvalidateRect(hWnd, nullptr, TRUE))
    {
        MessageBox(hWnd, L"Failed to invalidate user window", L"Load Error", MB_ICONERROR | MB_OK);
    }
}

void MyImage::InvertImage(HWND hWnd)
{
    for (int i = 0; i < mImage->GetWidth(); i++)
    {
        for (int j = 0; j < mImage->GetHeight(); j++)
        {
            Color color;
            mImage->GetPixel(i, j, &color);

            color.SetValue(Color::MakeARGB(color.GetA(), 255 - color.GetR(), 255 - color.GetG(), 255 - color.GetB()));
            mImage->SetPixel(i, j, color);
        }
    }
    if (!InvalidateRect(hWnd, nullptr, TRUE))
    {
        MessageBox(hWnd, L"Failed to invalidate user window", L"Load Error", MB_ICONERROR | MB_OK);
    }
}

void MyImage::FreeLightestBit(int x, int y)
{
    Color color;
    mImage->GetPixel(x, y, &color);
    int colorIndex = (color.GetBlue());
    colorIndex &= 254;
    color.SetValue(Color::MakeARGB(color.GetA(), color.GetR(), color.GetG(), colorIndex));
    mImage->SetPixel(x, y, color);
}

void MyImage::WriteOnLightestBit(int x, int y)
{
    Color color;
    mImage->GetPixel(x, y, &color);
    int colorIndex = (color.GetBlue());

    //wchar_t str[20];
    //swprintf_s(str, L"%d", colorIndex);
    //MessageBox(NULL, str, L"BEFORE OP", MB_OK);

    colorIndex |= 1;

    color.SetValue(Color::MakeARGB(color.GetA(), color.GetR(), color.GetG(), colorIndex));
    mImage->SetPixel(x, y, color);

    //mImage->GetPixel(x, y, &color);
    //colorIndex = (color.GetBlue());

    //swprintf_s(str, L"%d", colorIndex);
    //MessageBox(NULL, str, L"AFTER OP", MB_OK);
}

bool MyImage::ReadLightestBit(int x, int y)
{
    Color color;
    mImage->GetPixel(x, y, &color);
    int colorIndex = (color.GetBlue());
    colorIndex &= 1;
    return colorIndex == 1;
}

void MyImage::CryptMessage(HWND hWnd, Message* message)
{
    if (!mHasMessageHidden)
    {
        mHasMessageHidden = true;
    }
    else
    {
        return;
    }
    int length = message->GetLength();
    for (int i = 7; i > -1; i--)
    {
        FreeLightestBit(7 - i, 0);
        if (length > (1 << i))
        {
            WriteOnLightestBit(7 - i, 0);
            length -= (1 << i);
        }
    }
    int i = 0;
    while (i < length)
    {
        int x = 0;
        int y = 1;
        int characterCode = message->GetCharacterCode(i);
        for (int i = 7; i > -1; i--)
        {
            FreeLightestBit(x, y);
            if (characterCode > (1 << i))
            {
                WriteOnLightestBit(x, y);
                characterCode -= (1 << i);
            }
            x++;
            if (x > mImage->GetWidth() - 1)
            {
                x = 0;
                y++;
            }
        }
        i++;
    }
    SetMessageCryptInfo();
    if (!InvalidateRect(hWnd, nullptr, TRUE))
    {
        MessageBox(hWnd, L"Failed to invalidate user window", L"Load Error", MB_ICONERROR | MB_OK);
    }
}

void MyImage::SetMessageCryptInfo()
{
    int key = 69420;
    int width = mImage->GetWidth() - 1;
    int height = mImage->GetHeight() - 1;
    for (int i = 16; i > -1; i--)
    {
        FreeLightestBit(width - i, height);
        if (key > (1 << i))
        {
            Color color;
            mImage->GetPixel(width - i, height, &color);
            int colorIndex = (color.GetBlue());

            wchar_t str[20];
            swprintf_s(str, L"%d", colorIndex);
            MessageBox(NULL, str, L"AVANT WRITE", MB_OK);

            WriteOnLightestBit(width - i, height);
            key -= (1 << i);


            mImage->GetPixel(width - i, height, &color);
            colorIndex = (color.GetBlue());
            swprintf_s(str, L"%d", colorIndex);
            MessageBox(NULL, str, L"APRES WRITE", MB_OK);
        }
    }
}

bool MyImage::IsCrypted()
{
    int key = 0;
    int width = mImage->GetWidth();
    int height = mImage->GetHeight();
    for (int i = 16; i > -1; i--)
    {
        if (ReadLightestBit(width - i, height))
        {
            key += (1 << i);
        }
        /*wchar_t str[20];
        swprintf_s(str, L"%d", key);
        MessageBox(NULL, str, L"TOZ", MB_OK);*/
    }
    return key == 69420;
}