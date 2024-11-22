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

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;           // Nombre d'encodeurs
    UINT size = 0;          // Taille du tableau des encodeurs

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;          // Aucun encodeur trouvé

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == nullptr)
        return -1;          // Erreur d'allocation

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;       // CLSID trouvé
        }
    }

    free(pImageCodecInfo);
    return -1;              // Aucun encodeur trouvé
}

bool SaveImageAsPNG(Bitmap* image, const WCHAR* filename) {
    CLSID pngClsid;
    if (GetEncoderClsid(L"image/png", &pngClsid) != -1) {
        if (image->Save(filename, &pngClsid, nullptr) == Ok) {
            return true;
        }
    }
    return false;
}

void MyImage::SaveImage(HWND hWnd)
{
    if (mImage == nullptr)
    {
        MessageBox(hWnd, L"There is no image loaded",
            L"Writing Infringement Error", MB_OK | MB_ICONERROR);
        return;
    }
    OPENFILENAME ofn;
    WCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Images PNG\0.png\0Tous les fichiers\0.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn))
    {
        if (!wcschr(szFile, L'.')) {
            wcscat_s(szFile, L".png");
        }

        if (SaveImageAsPNG(mImage, szFile))
        {
            MessageBox(hWnd, L"Image enregistree avec succes !", L"Succes", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBox(hWnd, L"Echec de l enregistrement de l image.", L"Erreur", MB_OK | MB_ICONERROR);
        }
    }
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
    mDefaultImage = nullptr;
    mMasquedImage = nullptr;

    mRect = new Rect(100, 100, 0, 0);
    mX = 100;
    mY = 100;

    mMaxWidth = 500;
    mMaxHeight = 500;

    redMasc = 122;
    greenMasc = 122;
    blueMasc = 122;
    displayMasc = false;

    mMessageMaxLength = 0;
    mHasMessageHidden = false;
}

int MyImage::GetMaxLength()
{
    return mMessageMaxLength;
}

void MyImage::LoadFromBrowser(HWND hWnd)
{
    std::wstring path = OpenPngFileDialog();
    const wchar_t* wcharPath = path.c_str();

    if (mImage) {
        delete mImage;
        mImage = nullptr;
        delete mDefaultImage;
        mDefaultImage = nullptr;
        delete mMasquedImage; 
        mMasquedImage = nullptr;
    }
    mImage = new Bitmap(wcharPath);
    mDefaultImage = new Bitmap(wcharPath);
    mMasquedImage = new Bitmap(wcharPath);

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
    mHasMessageHidden = false;
}

void MyImage::SetEditLimit(HWND hWndEdit)
{
    if (IsCrypted())
    {
        mHasMessageHidden = true;
    }
    int width = mImage->GetWidth();
    int height = mImage->GetHeight();

    int size = CountValidPixels() - width - 17;
    if (size < 0)
    {
        size = 0;
    }
    size /= 16;

    mMessageMaxLength = size;
    SendMessage(hWndEdit, EM_SETLIMITTEXT, size, 0);
}

int MyImage::CountValidPixels()
{
    int count = 0;
    for (int i = 0; i < mImage->GetWidth(); i++)
    {
        for (int j = 0; j < mImage->GetHeight(); j++)
        {
            Color color;
            Color white = Color::White;
            mMasquedImage->GetPixel(i, j, &color);
            if (color.GetR() == white.GetR() && color.GetG() == white.GetG() && color.GetB() == white.GetB())
            {
                count++;
            }
        }
    }
    return count;
}

void MyImage::ScaleImage()
{
    int width = (int)mImage->GetWidth();
    int height = (int)mImage->GetHeight();

    if (width > mMaxWidth || height > mMaxHeight)
    {
        while (width > mMaxWidth || height > mMaxHeight)
        {
            width--;
            height--;
        }
    }
    else if (width < mMaxWidth || height < mMaxHeight)
    {
        while (width < mMaxWidth || height < mMaxHeight)
        {
            width++;
            height++;
        }
    }
    mX = 300 - width / 2 - 10;
    if (height < mMaxHeight)
    {
        mY = 295 - (height / 2);
    }
    else
    {
        mY = 50;
    }
    delete mRect;
    mRect = nullptr;
    mRect = new Rect(mX, mY, width, height);
}

void MyImage::GrayScale(HWND hWnd)
{
    if (mImage == nullptr)
    {
        MessageBox(hWnd, L"There is no image loaded",
            L"Writing Infringement Error", MB_OK | MB_ICONERROR);
        return;
    }
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
    if (mImage == nullptr)
    {
        MessageBox(hWnd, L"There is no image loaded",
            L"Writing Infringement Error", MB_OK | MB_ICONERROR);
        return;
    }
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

void MyImage::Reset(HWND hWnd)
{
    for (int i = 0; i < mImage->GetWidth(); i++)
    {
        for (int j = 0; j < mImage->GetHeight(); j++)
        {
            Color color;
            mDefaultImage->GetPixel(i, j, &color);

            Color color2;
            color2.SetValue(Color::MakeARGB(color.GetA(), color.GetR(), color.GetG(), color.GetB()));
            mImage->SetPixel(i, j, color2);
        }
    }
    mHasMessageHidden = false;
    if (!InvalidateRect(hWnd, nullptr, TRUE))
    {
        MessageBox(hWnd, L"Failed to invalidate user window", L"Load Error", MB_ICONERROR | MB_OK);
    }
}

void MyImage::SetMasc(HWND hWnd)
{
    for (int i = 0; i < mImage->GetWidth(); i++)
    {
        for (int j = 0; j < mImage->GetHeight(); j++)
        {
            Color color;
            mImage->GetPixel(i, j, &color);
            if (color.GetR() <= redMasc && color.GetG() <= greenMasc && color.GetB() <= blueMasc)
            {
                color.SetValue(Color::Black);
            }
            else 
            {
                color.SetValue(Color::White);
            }
            mMasquedImage->SetPixel(i, j, color);
        }
    }
    if (!InvalidateRect(hWnd, nullptr, TRUE))
    {
        MessageBox(hWnd, L"Failed to invalidate user window", L"Load Error", MB_ICONERROR | MB_OK);
    }
}

void MyImage::SetMascColors(int red, int green, int blue)
{
    redMasc = red;
    greenMasc = green;
    blueMasc = blue;
}

bool MyImage::IsValidPixel(int x, int y)
{
    Color color;
    Color white = Color::White;
    mMasquedImage->GetPixel(x, y, &color);
    return (color.GetR() == white.GetR() && color.GetG() == white.GetG() && color.GetB() == white.GetB());
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

    colorIndex |= 1;

    color.SetValue(Color::MakeARGB(color.GetA(), color.GetR(), color.GetG(), colorIndex));
    mImage->SetPixel(x, y, color);

}

bool MyImage::ReadLightestBit(int x, int y)
{
    Color color;
    mImage->GetPixel(x, y, &color);
    int colorIndex = (color.GetBlue());
    colorIndex &= 1;
    return (colorIndex == 1);
}

void MyImage::CryptMessage(HWND hWnd, Message* message)
{
    if (!mHasMessageHidden)
    {
        mHasMessageHidden = true;
    }
    else
    {
        MessageBox(hWnd, L"The image already contains an hidden message",
            L"Writing Infringement Error", MB_OK | MB_ICONERROR);
        return;
    }
    if (mImage == nullptr)
    {
        MessageBox(hWnd, L"There is no image loaded",
            L"Writing Infringement Error", MB_OK | MB_ICONERROR);
        return;
    }
    int length = message->GetLength();
    wchar_t toz[20];
    swprintf_s(toz, L"%d", length);

    int tempLength = length;
    for (int i = 7; i > -1; i--)
    {
        FreeLightestBit(7 - i, 0);
        if (tempLength >= (1 << i))
        {
            WriteOnLightestBit(7 - i, 0);
            tempLength -= (1 << i);
        }
    }
    int x = 0;
    int y = 1;
    for (int i = 0; i < length; i++)
    {
        int characterCode = message->GetCharacterCode(i);
        for (int j = 15; j > -1; j--)
        {
            while (!IsValidPixel(x, y))
            {
                x++;
                if (x > mImage->GetWidth() - 1)
                {
                    x = 0;
                    y++;
                }
            }
            FreeLightestBit(x, y);
            if (characterCode >= (1 << j))
            {
                WriteOnLightestBit(x, y);
                characterCode -= (1 << j);
            }
            x++;
            if (x > mImage->GetWidth() - 1)
            {
                x = 0;
                y++;
            }
        }
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
    int width = mImage->GetWidth()-1;
    int height = mImage->GetHeight()-1;
    int x = width;
    int y = height;
    for (int i = 0; i < 17; i++)
    {
        while (!IsValidPixel(x, y))
        {
            x--;
            if (x < 0)
            {
                x = width;
                y--;
            }
        }
        FreeLightestBit(x, y);
        if (key >= (1 << (16 - i)))
        {
            WriteOnLightestBit(x, y);
            key -= (1 << (16 - i));
            //wchar_t str[20];
            //swprintf_s(str, L"%d", key);
            //MessageBox(NULL, str,
            //    L"Write", MB_OK | MB_ICONERROR);
        }
        x--;
        if (x < 0)
        {
            x = width;
            y--;
        }
    } 
}

bool MyImage::IsCrypted()
{
    int key = 0;
    int width = mImage->GetWidth()-1;
    int height = mImage->GetHeight()-1;
    int x = width;
    int y = height;
    for (int i = 0; i < 17; i++)
    {
        while (!IsValidPixel(x, y))
        {
            x--;
            if (x < 0)
            {
                x = width;
                y--;
            }
        }
        bool lBit = ReadLightestBit(x, y);
        if (lBit)
        {
            key += (1 << (16 - i));
            wchar_t str[20];
        }
        x--;
        if (x < 0)
        {
            x = width;
            y--;
        }
    }
    return key == 69420;
}

int MyImage::GetMessageLength()
{
    int length = 0;
    for (int i = 0; i < 8; i++)
    {
        bool lBit = ReadLightestBit(i, 0);
        if (lBit)
        {
            length += (1 << (7 - i));
        }
    }
    return length;
}

wchar_t* MyImage::ReadMessage()
{
    if (mImage == nullptr)
    {
        MessageBox(NULL, L"There is no image loaded",
            L"Reading Infringement Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    if (!IsCrypted())
    {
        MessageBox(NULL, L"There is no message written",
            L"Reading Infringement Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    int length = GetMessageLength();
    int x = 0;
    int y = 1;
    wchar_t* message = new wchar_t[length + 1];

    for (int i = 0; i < length; i++)
    {
        int characterCode = 0;
        for (int j = 15; j > -1; j--)
        {
            while (!IsValidPixel(x, y))
            {
                x++;
                if (x > mImage->GetWidth() - 1)
                {
                    x = 0;
                    y++;
                }
            }
            bool lBit = ReadLightestBit(x, y);
            if (lBit)
            {
                characterCode += (1 << j);
            }
            x++;
            if (x > mImage->GetWidth() - 1)
            {
                x = 0;
                y++;
            }
            //wchar_t str[20];
            //swprintf_s(str, L"%d", key);
            
        }
        message[i] = (wchar_t)characterCode;
    }
    message[length] = '\0';
    return message;
}