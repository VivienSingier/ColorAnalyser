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
    x = 100;
    y = 100;
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
    mRect = new Rect(x, y, mImage->GetWidth(), mImage->GetHeight());

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
    x = 400 - width / 2;
    y = 300 - height / 2;
    delete mRect;
    mRect = nullptr;
    mRect = new Rect(x, y, width, height);
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


