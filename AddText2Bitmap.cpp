// AddText2Bitmap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

#define var auto

void AddText2Bitmap(char *sourceBmp, char *text, char *destBmp);
void LoadBmp(char *sourceBmp, char *destBmp);
bool ReadSize(const char *fileName, int *height, int *width);
void ProcessFiles();
void LoadBmp();

int _tmain(int argc, _TCHAR* argv[])
{
	//AddText2Bitmap(argv[1], argv[2], argv[3]);
	//LoadBmp(argv[1], argv[3]);
	/*int height = 0;
	int width = 0;
	ReadSize(argv[1], &height, &width);
	cout << "Height " << height << " Width " << width << endl;*/
	//ProcessFiles();
	LoadBmp();
	return 0;
}

void ProcessFiles()
{
	int minHeight = MAXINT;
	int minWidth = MAXINT;
	int h = 0;
	int w = 0;
	
	for (const char *fn : { "Image1.bmp", "Image2.bmp", "Image3.bmp", "Image4.bmp" })
	{
		ReadSize(fn, &h, &w);
		cout << fn << " Height " << h << " Width " << w << endl;
		minHeight = min(minHeight, h);
		minWidth = min(minWidth, w);
	}
	cout << "Min Height " << minHeight << " Min Width " << minWidth << endl;
}

HBITMAP         Create24BPPDIBSection(HDC hDC, int iWidth, int iHeight)
	{
	    BITMAPINFO      bmi;
	    HBITMAP         hbm;
	    void *          pBits;/*LPBYTE*/
	    // Initialize to 0s.       
		ZeroMemory(&bmi, sizeof(bmi));
	    // Initialize the header.
	    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	    bmi.bmiHeader.biWidth = iWidth;
	    bmi.bmiHeader.biHeight = iHeight;
	    bmi.bmiHeader.biPlanes = 1;
	    bmi.bmiHeader.biBitCount = 24;
	    bmi.bmiHeader.biCompression = BI_RGB;   // Create the surface.
	    hbm = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	    return (hbm);
	}
int CalcLen(HDC hdc, char *str)
{
  int l = 0, s = 0;
  while(*str)
  {
		GetCharWidth32(hdc, *str, *str, &l);
		s += l;
		str++;
  }
  return s;
}
void errhandler(const char *s, HWND h = NULL)
{
	MessageBox(h, s, "Error", 0);
}

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;
	
    // Retrieve the bitmap's color format, width, and height.
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
        errhandler("GetObject", hwnd);

    // Convert the color format to a count of bits.
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD
    // data structures.)

     if (cClrBits != 24)
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER) +
                    sizeof(RGBQUAD) * (1<< cClrBits));

     // There is no RGBQUAD array for the 24-bit-per-pixel format.

     else
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color
    // indices and store the result in biSizeImage.
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8
                                  * pbmi->bmiHeader.biHeight * cClrBits;
	/*pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~ 31) /8
                                  * pbmi->bmiHeader.biHeight;*/
// Set biClrImportant to 0, indicating that all of the
    // device colors are important.
     pbmi->bmiHeader.biClrImportant = 0;
     return pbmi;
}

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
                  HBITMAP hBMP, HDC hDC)
{
    HANDLE hf;                  // file handle
    BITMAPFILEHEADER hdr;       // bitmap file-header
    PBITMAPINFOHEADER pbih;     // bitmap info-header
    LPBYTE lpBits;              // memory pointer
    DWORD dwTotal;              // total count of bytes
    DWORD cb;                   // incremental count of bytes
    BYTE *hp;                   // byte pointer
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER) pbi;
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
         errhandler("GlobalAlloc", hwnd);

    // Retrieve the color table (RGBQUAD array) and the bits
    // (array of palette indices) from the DIB.
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi,
        DIB_RGB_COLORS))
    {
        errhandler("GetDIBits", hwnd);
    }

    // Create the .BMP file.
    hf = CreateFile(pszFile,
                   GENERIC_READ | GENERIC_WRITE,
                   (DWORD) 0,
                    NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   (HANDLE) NULL);
    if (hf == INVALID_HANDLE_VALUE)
        errhandler("CreateFile", hwnd);
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +
                 pbih->biSize + pbih->biClrUsed
                 * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
                    pbih->biSize + pbih->biClrUsed
                    * sizeof (RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD) &dwTmp,  NULL))
    {
       errhandler("WriteFile", hwnd);
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
                  + pbih->biClrUsed * sizeof (RGBQUAD),
                  (LPDWORD) &dwTmp,  NULL))
        errhandler("WriteFile", hwnd);

    // Copy the array of color indices into the .BMP file.
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
           errhandler("WriteFile", hwnd);

    // Close the .BMP file.
     if (!CloseHandle(hf))
           errhandler("CloseHandle", hwnd);

    // Free memory.
    GlobalFree((HGLOBAL)lpBits);
}

void SaveBmp2File(HBITMAP hBmp, HDC hDC, char *fileName)
{
	PBITMAPINFO pbmi = CreateBitmapInfoStruct(NULL, hBmp);
    CreateBMPFile(NULL, fileName, pbmi, hBmp, hDC);
	LocalFree(pbmi);
}

void LoadBmp(char *fileName, char *destBmp)
{
	HBITMAP hSourceBmp = (HBITMAP)LoadImage(
		NULL,   // handle to instance
		fileName,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
		IMAGE_BITMAP,        // image types
		100,     // desired width
		100,     // desired height
		LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);

	int width = bmp.bmWidth;
	int height = bmp.bmHeight;
	HDC hdcScreen = GetDC(NULL);
	HDC fileHDC = CreateCompatibleDC(hdcScreen);
	HDC memHDC = CreateCompatibleDC(hdcScreen);
	HBITMAP memBmp = Create24BPPDIBSection(hdcScreen, width, height);
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	BitBlt(memHDC, 0, 0, width, height, fileHDC, 0, 0, SRCCOPY);
	SaveBmp2File(memBmp, memHDC, destBmp);

	cout << "Delete objects" << endl;
	cout << DeleteObject(memBmp) << endl;
	cout << DeleteObject(memHDC) << endl;
	cout << DeleteObject(fileHDC) << endl;
	cout << DeleteObject(hSourceBmp) << endl;

	cout << "Release objects" << endl;
	cout << ReleaseDC(NULL, hdcScreen) << endl;
}

void LoadBmp2(initializer_list<string> vals)
{
	char *file1 = "Image1.bmp";
	char *file2 = "Image2.bmp";
	char *file3 = "Image3.bmp";
	char *destBmp = "Image.bmp";
	int widthOffset = 0;
	cout << "Parameters: " << endl;
	for (auto image = vals.begin(); image != vals.end(); ++image)
	{
		auto& elem = *image;
		cout << (*image).c_str() << endl;
	}
	HBITMAP hSourceBmp = (HBITMAP)LoadImage(
		NULL,   // handle to instance
		file1,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
		IMAGE_BITMAP,        // image types
		100,     // desired width
		100,     // desired height
		LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);

	int width = bmp.bmWidth;
	int height = bmp.bmHeight;
	HDC hdcScreen = GetDC(NULL);
	HDC fileHDC = CreateCompatibleDC(hdcScreen);
	HDC memHDC = CreateCompatibleDC(hdcScreen);
	HBITMAP memBmp = Create24BPPDIBSection(hdcScreen, width*3, height);
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	BitBlt(memHDC, 0 + widthOffset, 0, width, height, fileHDC, 0, 0, SRCCOPY);

	widthOffset += 100;


	/*
	Free resources
	перед загрузкой туда другого
	*/
	cout << DeleteObject(hSourceBmp) << endl;

	/*HBITMAP*/ hSourceBmp = (HBITMAP)LoadImage(
		NULL,   // handle to instance
		file2,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
		IMAGE_BITMAP,        // image types
		100,     // desired width
		100,     // desired height
		LR_LOADFROMFILE);
	//BITMAP bmp;
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);

	/*int*/ width = bmp.bmWidth;
	/*int*/ height = bmp.bmHeight;
	/*HDC hdcScreen = GetDC(NULL);*/
	/*HDC fileHDC = CreateCompatibleDC(hdcScreen);*/
	/*HDC memHDC = CreateCompatibleDC(hdcScreen);*/
	/*HBITMAP memBmp = Create24BPPDIBSection(hdcScreen, width, height);*/
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	BitBlt(memHDC, 0 + widthOffset, 0, width, height, fileHDC, 0, 0, SRCCOPY);

	widthOffset += 100;
	cout << DeleteObject(hSourceBmp) << endl;
	hSourceBmp = (HBITMAP)LoadImage(
		NULL,   // handle to instance
		file3,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
		IMAGE_BITMAP,        // image types
		100,     // desired width
		100,     // desired height
		LR_LOADFROMFILE);
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);

	width = bmp.bmWidth;
	/*int*/ height = bmp.bmHeight;
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	BitBlt(memHDC, 0 + widthOffset, 0, width, height, fileHDC, 0, 0, SRCCOPY);


	SaveBmp2File(memBmp, memHDC, destBmp);

	cout << "Delete objects" << endl;
	cout << DeleteObject(memBmp) << endl;
	cout << DeleteObject(memHDC) << endl;
	cout << DeleteObject(fileHDC) << endl;
	cout << DeleteObject(hSourceBmp) << endl;
	
	cout << "Release objects" << endl;
	cout << ReleaseDC(NULL, hdcScreen) << endl;
	/*
	cout << ReleaseDC(NULL, memHDC) << endl;*/
}

void LoadBmp()
{
	LoadBmp2({ "Image1.bmp", "Image2.bmp", "Image3.bmp", "Image4.bmp" });
}

void AddText2Bitmap(char *sourceBmp, char *text, char *destBmp)
{
	HBITMAP hSourceBmp = (HBITMAP)LoadImage(
    NULL,   // handle to instance
    sourceBmp,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
    IMAGE_BITMAP,        // image types
    0,     // desired width
    0,     // desired height
    LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);
	
	TEXTMETRIC tm;
	
	HDC hdcScreen = GetDC(NULL)/*CreateDC("DISPLAY", NULL, NULL, NULL)*/; 
	GetTextMetrics(hdcScreen, &tm);
	char buf[200];
	/*sprintf(buf, "BITSPIXEL %d", GetDeviceCaps(
  hdcScreen,     
  BITSPIXEL   
));*/
	sprintf_s(buf, "BITSPIXEL %s %d", sourceBmp, bmp.bmBitsPixel);
	MessageBox(NULL, buf, "BP", 0);
	int width = CalcLen(hdcScreen, text);
	int height = tm.tmHeight;
	HDC txtDC = CreateCompatibleDC(hdcScreen);
	HDC maskDC = CreateCompatibleDC(hdcScreen);
	HBITMAP maskBmp = CreateCompatibleBitmap(hdcScreen, width, height);
	HBITMAP txtBmp = CreateCompatibleBitmap(hdcScreen, width, height);
	SelectObject(txtDC, txtBmp);
	SelectObject(maskDC, maskBmp);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = width;
	r.bottom = height;
	SetTextColor(txtDC, RGB(0, 255, 0));
	SetBkColor(txtDC, RGB(0, 0, 0));
	DrawText(txtDC, text, strlen(text), &r, 0);

	SetTextColor(maskDC, RGB(0, 0, 0));
	SetBkColor(maskDC, RGB(255, 255, 255));
	DrawText(maskDC, text, strlen(text), &r, 0);
	
	if (bmp.bmWidth > width)
	{
  		width = bmp.bmWidth;
	}
	if (bmp.bmHeight > tm.tmHeight)
	{
		height = bmp.bmHeight;
	}
	HDC fileHDC = CreateCompatibleDC(hdcScreen);
	HDC memHDC = CreateCompatibleDC(hdcScreen);
	
	HBITMAP memBmp = Create24BPPDIBSection(hdcScreen, width*2, height*2);//CreateCompatibleBitmap(hdcScreen, width, height);
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	HBRUSH hBrush = CreateSolidBrush(GetPixel(fileHDC, 0, 0));
	SelectObject(memHDC, hBrush);
	FloodFill(memHDC, 0, 0, GetPixel(fileHDC, 0, 0));
	BitBlt(memHDC, 0, 0, width, height, fileHDC, 0, 0, SRCCOPY);
	
	BitBlt(memHDC, 0+width, 0, width+width, height, fileHDC, 0, 0, SRCCOPY);

	BitBlt(memHDC, 0, 0, r.right, r.bottom, maskDC, 0, 0, SRCAND);
	BitBlt(memHDC, 0, 0, r.right, r.bottom, txtDC, 0, 0, SRCINVERT);
	SaveBmp2File(memBmp, memHDC, destBmp);
	/*PBITMAPINFO pbmi = CreateBitmapInfoStruct(NULL, memBmp);
    CreateBMPFile(NULL, destBmp, pbmi, memBmp, memHDC);*/
}

bool ReadSize(const char *fileName, int *height, int *width)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	//int Width, Height;
	HANDLE hIn;
	DWORD RW;
	hIn = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hIn == INVALID_HANDLE_VALUE)
		return false;
	// Прочтем данные
	ReadFile(hIn, &bfh, sizeof(bfh), &RW, NULL);
	ReadFile(hIn, &bih, sizeof(bih), &RW, NULL);
	cout << "Height " << bih.biHeight << " Width " << bih.biWidth << endl;
	cout << RW << " bytes read" << endl;
	CloseHandle(hIn);
	*height = bih.biHeight;
	*width = bih.biWidth;
	return true;
}

BOOL Convert256To24(char *fin, char *fout)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	int Width, Height;
	RGBQUAD Palette[256];
	BYTE *inBuf;
	RGBTRIPLE *outBuf;
	HANDLE hIn, hOut;
	DWORD RW;
	DWORD OffBits;
	int i, j;

	hIn = CreateFile(fin, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hIn == INVALID_HANDLE_VALUE)
		return FALSE;

	hOut = CreateFile(fout, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hIn);
		return FALSE;
	}

	// Прочтем данные
	ReadFile(hIn, &bfh, sizeof(bfh), &RW, NULL);
	ReadFile(hIn, &bih, sizeof(bih), &RW, NULL);
	ReadFile(hIn, Palette, 256 * sizeof(RGBQUAD), &RW, NULL);

	// Установим указатель на начало растра
	SetFilePointer(hIn, bfh.bfOffBits, NULL, FILE_BEGIN);
	Width = bih.biWidth;
	Height = bih.biHeight;
	OffBits = bfh.bfOffBits;

	// Выделим память
	inBuf = new BYTE[Width];
	outBuf = new RGBTRIPLE[Width];

	// Заполним заголовки
	bfh.bfOffBits = sizeof(bfh) + sizeof(bih);                               // Не будем писать палитру
	bih.biBitCount = 24;
	bfh.bfSize = bfh.bfOffBits + 4 * Width * Height + Height * (Width % 4);    // Размер файла

	// А остальное не меняется
	// Запишем заголовки
	WriteFile(hOut, &bfh, sizeof(bfh), &RW, NULL);
	WriteFile(hOut, &bih, sizeof(bih), &RW, NULL);

	// Начнем преобразовывать
	for (i = 0; i < Height; i++)
	{
		ReadFile(hIn, inBuf, Width, &RW, NULL);
		for (j = 0; j < Width; j++)
		{
			outBuf[j].rgbtRed = Palette[inBuf[j]].rgbRed;
			outBuf[j].rgbtGreen = Palette[inBuf[j]].rgbGreen;
			outBuf[j].rgbtBlue = Palette[inBuf[j]].rgbBlue;
		}
		WriteFile(hOut, outBuf, sizeof(RGBTRIPLE) * Width, &RW, NULL);

		// Пишем мусор для выравнивания
		WriteFile(hOut, Palette, Width % 4, &RW, NULL);
		SetFilePointer(hIn, (3 * Width) % 4, NULL, FILE_CURRENT);
	}

	delete []inBuf;
	delete []outBuf;
	CloseHandle(hIn);
	CloseHandle(hOut);
	return TRUE;
}