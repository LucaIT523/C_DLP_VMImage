
// TestDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"
#include "ClaPowerShellMgr.h"

#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDlg dialog



CTestDlg::CTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edtBox);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTestDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CTestDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CTestDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//LoadLibrary(L"KenanHelperVM32.dll");

	SetWindowDisplayAffinity(m_hWnd, WDA_EXCLUDEFROMCAPTURE);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "ClaFwMgr.h"

bool IsInsideVMWare()
{
	bool rc = true;

	//__try
	//{
	//	__asm
	//	{
	//		push   edx
	//		push   ecx
	//		push   ebx

	//		mov    eax, 'VMXh'
	//		mov    ebx, 0 // any value but not the MAGIC VALUE

	//		mov    ecx, 10 // get VMWare version

	//		mov    edx, 'VX' // port number


	//		in     eax, dx // read port

	//		// on return EAX returns the VERSION

	//		cmp    ebx, 'VMXh' // is it a reply from VMWare?

	//		setz[rc] // set return value


	//		pop    ebx
	//		pop    ecx
	//		pop    edx
	//	}
	//}
	//__except (EXCEPTION_EXECUTE_HANDLER)
	//{
	//	rc = false;
	//}

	return rc;
}

int CheckClipboardContentType(BOOL& bText, BOOL& bFile) {
	int ret = 0;

	OpenClipboard(NULL);
	wchar_t wszFormat[MAX_PATH];

	if (OpenClipboard(NULL)) {
		UINT format = 0;
		bText = false;
		bFile = false;

		// Enumerate through all clipboard formats
		while ((format = EnumClipboardFormats(format))) {
			if (format == CF_HDROP) {
				bFile = true; // Clipboard contains files
			}
			else if (format == CF_TEXT || format == CF_UNICODETEXT) {
				bText = true; // Clipboard contains text
			}

			GetClipboardFormatName(format, wszFormat, MAX_PATH);
			OutputDebugString(CString(wszFormat) + L"\r\n");
		}

		CloseClipboard();
	}
	else {
		//		std::cerr << "Failed to open clipboard." << std::endl;
	}
	return ret;
}

void CTestDlg::OnBnClickedButton1()
{
	BOOL t, f;
	CheckClipboardContentType(t, f);
	if (t) AfxMessageBox(L"T");
	if (f) AfxMessageBox(L"F");
	AfxMessageBox(L"end");
}

std::string XORCipher(const std::string& input, const std::string& key) {
	std::string output = input; // Start with the input string

	// Perform XOR operation
	for (size_t i = 0; i < input.size(); ++i) {
		output[i] = input[i] ^ key[i % key.size()]; // XOR with the key, cycling through the key
	}

	return output;
}

#include "..\cmn\ClaOsInfo.h"
#include "..\cmn_crypto\kncrypto.h"

long getFileSize(const std::wstring& filename) {
	struct _stat64i32 fileStat;

	// Get file status
	if (_wstat(filename.c_str(), &fileStat) != 0) {
		return -1; // Return -1 to indicate an error
	}

	return fileStat.st_size; // Return the size of the file
}

void CTestDlg::OnBnClickedButton2()
{
	CFileDialog dlg(TRUE, L"vmx", NULL, 6, L"*.vmx|*.vmx|All Files|*.*||");
	if (dlg.DoModal() != IDOK) {
		return;
	}

	std::wstring path = dlg.GetPathName();
	int nSize = getFileSize(path);

	FILE* pFile; _wfopen_s(&pFile, dlg.GetPathName(), L"rb");
	if (pFile == NULL) {
		return;
	}

	fseek(pFile, SEEK_END, 0);
	fseek(pFile, SEEK_SET, 0);
	void* pBuff = malloc(nSize);
	fread(pBuff, 1, nSize, pFile);
	fclose(pFile);

	_wfopen_s(&pFile, dlg.GetPathName() + L".enc", L"wb");
	kn_encrypt_with_pwd((unsigned char*)pBuff, nSize, L"pwd-1");

	unsigned char pHeader[] = { 0x65, 0x87, 0x65 };
//	fwrite(pHeader, 1, 3, pFile);
	fwrite(pBuff, 1, nSize, pFile);
	fclose(pFile);
}


void CTestDlg::OnBnClickedOk()
{
	wchar_t wszPath[MAX_PATH];

	GetModuleFileName(NULL, wszPath, MAX_PATH);
	wchar_t* p = wcsrchr(wszPath, L'\\');
	p[0] = 0;
	CString strDir = wszPath;
	CString strExe; strExe.Format(L"%s\\launcher64.exe", strDir);
	CString strCmd = L"--pdnd";

	STARTUPINFO si; memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	CreateProcess(strExe, strCmd.GetBuffer(), NULL, NULL, FALSE, 0, NULL, strDir, &si, &pi);

	//// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}


void CTestDlg::OnBnClickedButton3()
{
	char szBuff[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	kn_encrypt_with_pwd((unsigned char*)szBuff, strlen(szBuff), L"pwd-1");

	char szBuff1[64]; memcpy(szBuff1, szBuff, 54);

	kn_decrypt_with_pwd((unsigned char*)szBuff1, 54, L"pwd-1");

}

std::string trim_all(std::string org, const std::string& trimletters) {
	// Use lambda to define a function for trimming characters
	auto trim = [&trimletters](std::string& str) {
		str.erase(0, str.find_first_not_of(trimletters)); // Trim from the start
		str.erase(str.find_last_not_of(trimletters) + 1); // Trim from the end
		};

	trim(org); // Trim the original string
	return org; // Return the trimmed string
}

std::string replace_all(std::string org, const std::string& from, const std::string& to) {
	if (from.empty()) {
		return org; // Return original string if 'from' is empty
	}

	size_t start_pos = 0;
	while ((start_pos = org.find(from, start_pos)) != std::string::npos) {
		org.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Move past the last replacement
	}
	return trim_all(org, " ");
}

void CTestDlg::OnBnClickedButton4()
{
	std::string strRet;
	//std::string script = "# Get the current OS information\r\n"
	//	"$os = Get-CimInstance -ClassName Win32_OperatingSystem\r\n"
	//	"# Check the version and display the appropriate label\r\n"
	//	"if ($os.Version -like '10.0.22000*') {\r\n"
	//	"    echo \"win11\"\r\n"
	//	"} elseif ($os.Version -like '10.0.*') {\r\n"
	//	"    echo \"win10\"\r\n"
	//	"} elseif ($os.Version -like '6.1.*') {\r\n"
	//	"    echo \"win7\"\r\n"
	//	"} else {\r\n"
	//	"    echo \"Unknown OS\"\r\n"
	//	"}";

	//strRet = RunPowerShellScript(script);
	//std::string script = "Get-ChildItem -Path C:\Test";

	//std::string script =
	//	"# Get the firewall profile status\r\n"
	//	"$firewallProfiles = Get-NetFirewallProfile\r\n"
	//	"\r\n"
	//	"# Check if the firewall is enabled for any profile\r\n"
	//	"$firewallEnabled = $firewallProfiles | Where-Object { $_.Enabled -eq 'True' }\r\n"
	//	"\r\n"
	//	"if ($firewallEnabled) {\r\n"
	//	"    \"aaaaaaa aaaaaa aaaaaaa aaaa\"\r\n"
	//	"} else {\r\n"
	//	"    \"bbb bbbbbb bbbbb bbbb bbbbb\"\r\n"
	//	"}";
	std::string script =
		"#Check the status of the Windows Firewall for the Public profile \n"
		"$publicFirewallStatus = Get-NetFirewallProfile -Profile Public| Select-Object Name, Enabled \n"
		"$domainFirewallStatus = Get-NetFirewallProfile -Profile Domain| Select-Object Name, Enabled \n"
		"$privateFirewallStatus = Get-NetFirewallProfile -Profile Private| Select-Object Name, Enabled \n"
		" \n"
		"#Display the status \n"
		"if ($publicFirewallStatus.Enabled -and $domainFirewallStatus.Enabled -and $privateFirewallStatus.Enabled)  {\n"
		"    Write-Output \"Firewall-ON\" \n"
		"}else{ \n"
		"    Write-Output \"Firewall-OFF\" \n"
		"}";
	strRet = RunPowerShellScript(script.c_str());


	//strRet = replace_all(strRet, "\r\n", " ");

	//script = "# Get the list of installed antivirus products\r\n"
	//	"$antivirus = Get-WmiObject -Namespace \"root\\SecurityCenter2\" -Class AntiVirusProduct\r\n"
	//	"\r\n"
	//	"# Check if any antivirus products are found\r\n"
	//	"if ($antivirus) {\r\n"
	//	"    foreach ($product in $antivirus) {\r\n"
	//	"        echo \"Antivirus Name: $($product.displayName)\"\r\n"
	//	"        echo \"Product State: $($product.productState)\"\r\n"
	//	"    }\r\n"
	//	"} else {\r\n"
	//	"    echo \"No antivirus products found.\"\r\n"
	//	"}";
	//strRet = RunPowerShellScript(script);
	//strRet = RunPowerShellScript("ipconfig");
	//MessageBoxA(NULL, strRet.c_str(), "aa", MB_OK);

	wchar_t wzPwd = m_edtBox.GetPasswordChar();

}

void CaptureScreen(CBitmap& bitmap) {
	HDC hScreenDC = GetDC(NULL);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	bitmap.CreateCompatibleBitmap(CDC::FromHandle(hScreenDC), width, height);
	SelectObject(hMemoryDC, bitmap.m_hObject);

	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);
}

std::string DecodeQRCode(CBitmap& bitmap) {
	// Convert CBitmap to a format ZXing can work with (e.g., a grayscale image)
	// This part requires additional implementation to convert the bitmap to a suitable format

	// Example of decoding (assuming you've converted the bitmap to a suitable format)
	//Ref<LuminanceSource> source = ...; // Create LuminanceSource from the bitmap
	//Ref<BinaryBitmap> binaryBitmap(new BinaryBitmap(Ref<GlobalHistogramBinarizer>(new GlobalHistogramBinarizer(source))));
	//QrCodeReader reader;

	//try {
	//	Ref<Result> result = reader.decode(binaryBitmap);
	//	return result->getText()->getText();
	//}
	//catch (const Exception& e) {
	//	// Handle decoding errors
	//	return "";
	//}
	return "";
}

void CTestDlg::OnBnClickedButton5()
{
	CBitmap screenBitmap;
	CaptureScreen(screenBitmap);

	std::string qrData = DecodeQRCode(screenBitmap);
	if (!qrData.empty()) {
		AfxMessageBox(CString(qrData.c_str()));
	}
	else {
		AfxMessageBox(_T("No QR code found or decoding failed."));
	}
}

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

void CaptureScreen(const std::wstring filename) {
	// Find the window by class name
	HWND hwnd = FindWindow(L"VMPlayerFrame", NULL);
	if (hwnd == NULL) {
		AfxMessageBox(L"Window not found!");
		return;
	}

	RECT rt;
	GetWindowRect(hwnd, &rt);

	// Get the dimensions of the window
	int width = rt.right - rt.left;
	int height = rt.bottom - rt.top;

	// Create a compatible DC and a bitmap
	HDC hScreenDC = GetDC(NULL); // Get the DC for the entire screen
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

	// Select the bitmap into the memory DC
	SelectObject(hMemoryDC, hBitmap);

	// BitBlt the window into the memory DC
	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, rt.left, rt.top, SRCCOPY);

	// Save the bitmap as a BMP file
	SaveBitmapToFile(hMemoryDC, hBitmap, width, height, filename);

	// Cleanup
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);
}



void CTestDlg::OnBnClickedButton6()
{
	CaptureScreen(L"E:\\1.png");
}


void CTestDlg::OnBnClickedButton7()
{
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}


void CTestDlg::OnBnClickedButton8()
{
	ClaFwMgr::EnableFirewall();
}
