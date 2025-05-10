#include "pch.h"
#include "KenanHelperVM.h"
#include "KenanSecuProc.h"
#include "ClaProcess.h"
#include "KGlobal.h"

#include <gdiplus.h>
using namespace Gdiplus;

HWND lv_hwndMain = NULL;

BOOL is_top_window(HWND handle)
{
	return GetParent(handle) == (HWND)0;
}

BOOL CALLBACK EnumWindowsProcVM(HWND hwnd, LPARAM lParam) {

	DWORD dwProcessID;
	GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (g_nProcID != dwProcessID)
		return TRUE;

	wchar_t wszTitle[MAX_PATH]; memset(wszTitle, 0, sizeof(wszTitle));
	GetWindowText(hwnd, wszTitle, MAX_PATH);
	if (wcscmp(wszTitle, L"Virtual Machine Settings") == 0) {
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}

	if (!is_top_window(hwnd))
		return TRUE;

	if (wcsstr(wszTitle, L"Player") == NULL)
		return TRUE;

	if (wcsstr(wszTitle, L"VMware") == NULL)
		return TRUE;

	lv_hwndMain = hwnd;
	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {

	DWORD dwProcessID;
	GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (g_nProcID != dwProcessID)
		return TRUE;

	wchar_t wszTitle[MAX_PATH]; memset(wszTitle, 0, sizeof(wszTitle));
	GetWindowText(hwnd, wszTitle, MAX_PATH);
	if (wcscmp(wszTitle, L"Virtual Machine Settings") == 0) {
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}

	if (!is_top_window(hwnd))
		return TRUE;

	lv_hwndMain = hwnd;
	return TRUE;
}

BOOL CALLBACK EnumChildWindowsProcVM(HWND hwnd, LPARAM lParam) {
	wchar_t wszText[MAX_PATH]; memset(wszText, 0, sizeof(wszText));
	GetClassNameW(hwnd, wszText, MAX_PATH);
	CString strText = wszText;
	if (strText == L"wui.unibar.Toolbar") {
		EnableWindow(hwnd, FALSE);
	}

	return TRUE;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT num = 0;          // number of image encoders
	UINT size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0) return -1; // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j; // Success
		}
	}
	free(pImageCodecInfo);
	return -1; // Failure
}

void CaptureScreen(const std::wstring& filename) {
	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Get the dimensions of the screen
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	// Create a compatible DC and a bitmap
	HDC hScreenDC = GetDC(lv_hwndMain);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);

	// Select the bitmap into the memory DC
	SelectObject(hMemoryDC, hBitmap);

	// BitBlt the screen into the memory DC
	BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);

	// Create a GDI+ Bitmap from the HBITMAP
	Bitmap bitmap(hBitmap, NULL);

	// Save the bitmap as a PNG file
	CLSID clsid;
	GetEncoderClsid(L"image/png", &clsid);
	bitmap.Save(filename.c_str(), &clsid, NULL);

	// Cleanup
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);
	GdiplusShutdown(gdiplusToken);
}


UINT TF_PREVENT_SCREEN_CAPTURE(void*) {

	Sleep(5000);

	EnumWindows(EnumWindowsProcVM, NULL);
	if (lv_hwndMain != NULL) {
		g_hwndMain = lv_hwndMain;
	}


	ClaKcConfig conf;
	unsigned int nStateNew = WDA_NONE;
	unsigned int nStateOld = WDA_NONE;

	while (TRUE) {
		//	find vm player window
		EnumWindows(EnumWindowsProcVM, NULL);
		if (lv_hwndMain != NULL){
			g_hwndMain = lv_hwndMain;
		}

		if (g_hwndMain)
			EnumChildWindows(g_hwndMain, EnumChildWindowsProcVM, 0);

		//if (g_hwndMain)
		//	CaptureScreen(L"D:\\1.png");
		KGlobal::readConfig(&conf);
		if (conf._bAllowCaptureScreen) {
			nStateNew = WDA_NONE;
		}
		else {
			nStateNew = WDA_EXCLUDEFROMCAPTURE;
		}

		if (nStateNew != nStateOld) {
			SetWindowDisplayAffinity(g_hwndMain, nStateNew);
			nStateOld = nStateNew;
		}

		Sleep(300);
	}
}


DWORD lv_dwPreventCaptureThreadID;
HANDLE lv_hPreventCaptureThreadID = NULL;

void create_thread_capturesafe() {
	lv_hPreventCaptureThreadID = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_PREVENT_SCREEN_CAPTURE, NULL, 0, &lv_dwPreventCaptureThreadID);
}

void terminate_thread_capturesafe() {
	if (lv_hPreventCaptureThreadID) {
		TerminateThread(lv_hPreventCaptureThreadID, 0);
	}

}
