#include "pch.h"
#include <string>

void SaveBitmapToFile(HDC hMemDC, HBITMAP hBitmap, int width, int height, const std::wstring& filename) {
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmp.bmWidth;
	bi.biHeight = bmp.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24; // 24 bits for RGB
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0; // Can be calculated if needed
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;

	// Create the file
	HANDLE hFile = CreateFile(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		AfxMessageBox(L"Could not create file!");
		return;
	}

	// Fill in the BITMAPFILEHEADER
	bmfHeader.bfType = 0x4d42; // BM
	bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
	bmfHeader.bfReserved1 = 0;
	bmfHeader.bfReserved2 = 0;
	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Write the BITMAPFILEHEADER
	DWORD dwWritten;
	WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// Write the BITMAPINFOHEADER
	WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);

	// Write the bitmap data
	BYTE* pPixels = new BYTE[dwBmpSize];
	GetDIBits(hMemDC, hBitmap, 0, height, pPixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	WriteFile(hFile, pPixels, dwBmpSize, &dwWritten, NULL);

	// Cleanup
	delete[] pPixels;
	CloseHandle(hFile);
}

int CaptureScreen(const std::wstring filename) {

	// Find the window by class name
	HWND hwnd = FindWindow(L"VMPlayerFrame", NULL);
	if (hwnd == NULL) {
		//AfxMessageBox(L"Window not found!");
		return -1;
	}

	RECT rt;
	GetWindowRect(hwnd, &rt);

	int dpiX = GetDeviceCaps(GetDC(hwnd), LOGPIXELSX);
	float scaleX = dpiX / 96.0f;

	// Get the dimensions of the window
	int width = 600;
	int height = 600;

	width = width;
	height = height;
	// Create a compatible DC and a bitmap
	HDC hScreenDC = GetDC(NULL); // Get the DC for the entire screen
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

	// Select the bitmap into the memory DC
	SelectObject(hMemoryDC, hBitmap);

	// BitBlt the window into the memory DC
	BitBlt(hMemoryDC, 0, 0, 300, 300, hScreenDC, rt.left, rt.top, SRCCOPY);
	BitBlt(hMemoryDC, 300, 0, 300, 300, hScreenDC, rt.right - 300, rt.top, SRCCOPY);
	BitBlt(hMemoryDC, 0, 300, 300, 300, hScreenDC, rt.left, rt.bottom-300, SRCCOPY);
	BitBlt(hMemoryDC, 300, 300, 300, 300, hScreenDC, rt.right-300, rt.bottom - 300, SRCCOPY);

	// Save the bitmap as a BMP file
	SaveBitmapToFile(hMemoryDC, hBitmap, width, height, filename);

	// Cleanup
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);

	return 0;
}