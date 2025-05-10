
// KAgentQRDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KAgentQR.h"
#include "KAgentQRDlg.h"
#include "afxdialogex.h"
#include "ClaOsInfo.h"
#include "ClaNetAdapter.h"
#include "kncrypto.h"
#include "ClaRegMgr.h"
#include "ClaDlgAboutAgent.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKAgentQRDlg dialog



CKAgentQRDlg::CKAgentQRDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_KAGENTQR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKAgentQRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKAgentQRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_TRAY_OPEN, &CKAgentQRDlg::OnTrayOpen)
	ON_COMMAND(ID_TRAY_EXIT, &CKAgentQRDlg::OnTrayExit)
	ON_COMMAND(ID_TRAY_ABOUT, &CKAgentQRDlg::OnTrayAbout)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

int index;
// CKAgentQRDlg message handlers

BOOL CKAgentQRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	index = (rand() + CTime::GetCurrentTime().GetTime()) % 4;

	CRect screenRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

	// Get the dialog dimensions
	CRect dialogRect;
	GetWindowRect(&dialogRect);

	// Calculate the position for the bottom-right corner
	int w = 64 + 12;//max(dialogRect.Width(), dialogRect.Height());

	int xPos = 0;
	int yPos = 0;
	if (index == 0) {
		xPos = 1;
		yPos = 1;
	}
	else if (index == 1) {
		xPos = 0;
		yPos = screenRect.bottom - w;
	}
	else if (index == 2) {
		xPos = screenRect.right - w;
		yPos = 0;
	}
	else {
		xPos = screenRect.right - w;
		yPos = screenRect.bottom - w;
	}


	MoveWindow(xPos, yPos, w, w);

	// Set the dialog position
	std::string textQR = _getQrText();

	GenerateQRCode(textQR);

	_trayMgr.Create(this, WM_TRAY_NOTIFICATION, L"KAgent", m_hIcon, 1);

	LONG_PTR style = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
	style |= WS_EX_TOOLWINDOW; // Add the tool window style
	style &= ~WS_EX_APPWINDOW; // Remove the app window style
	SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, style);

	SetTimer(0, 5000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKAgentQRDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// If the bitmap is valid, draw it stretched to fill the dialog
	if (m_qrBitmap.m_hObject != NULL) {
		CRect rect;
		GetClientRect(&rect);

		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap* pOldBitmap = memDC.SelectObject(&m_qrBitmap);

		// Get the dimensions of the bitmap
		BITMAP bitmapInfo;
		GetObject(m_qrBitmap.m_hObject, sizeof(BITMAP), &bitmapInfo);
		int bitmapWidth = bitmapInfo.bmWidth;
		int bitmapHeight = bitmapInfo.bmHeight;

		CBrush brush; brush.CreateSolidBrush(RGB(255, 255, 255));

		dc.FillRect(rect, &brush);
		// Stretch the bitmap to fill the entire client area
		dc.StretchBlt(6, 6, rect.Width()-12, rect.Height()-12, &memDC, 0, 0, bitmapWidth, bitmapHeight, SRCCOPY);

		// Cleanup
		memDC.SelectObject(pOldBitmap);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKAgentQRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CKAgentQRDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}

std::string CKAgentQRDlg::_getQrText()
{
	std::string os = ClaOSInfo::getOS();
	CString strMac = ClaNetAdapter::GetMAC();
	unsigned char szBuffer[64];
	sprintf_s((char*)szBuffer, 64, "%s-%S", os.c_str(), strMac);
	int l = strlen((char*)szBuffer);
	memset(&szBuffer[l], 0, 64 - l);

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	wchar_t wszKey[MAX_PATH]; memset(wszKey, 0, sizeof(wszKey));
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\Windows Search", L"guid", wszKey);
	CString strKey; strKey.Format(L"KAgent-QR-Code - %s", wszKey);

	kn_encrypt_with_pwd((unsigned char*)szBuffer, 64, strKey);

	char ret[260];memset(ret, 0, sizeof(ret));
	for (int i = 0; i < 64; i++) {
		char szNum[3]; 
		sprintf_s(szNum, 3, "%02X", szBuffer[i]);
		strcat_s(ret, 260, szNum);
	}

	return ret;
}

void CKAgentQRDlg::GenerateQRCode(const std::string& text)
{
	CDC* pDC = GetDC();
	// Generate QR Code
	QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::LOW);

	// Create a bitmap to display the QR code
	const int size = qr.getSize();
	m_qrBitmap.CreateCompatibleBitmap(pDC, size, size);

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&m_qrBitmap);

	// Draw the QR code on the bitmap
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			COLORREF color = qr.getModule(x, y) ? RGB(0, 0, 0) : RGB(255, 255, 255);
			dc.FillSolidRect(x, y, 1, 1, color);
		}
	}

	// Cleanup
	dc.SelectObject(pOldBitmap);

	// Optionally, set the bitmap dimensions manually
	BITMAP bitmapInfo;
	GetObject(m_qrBitmap.m_hObject, sizeof(BITMAP), &bitmapInfo);
}
UINT reg_msg_id = 0;

LRESULT CKAgentQRDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (reg_msg_id == 0) {
		reg_msg_id = RegisterWindowMessage(L"KENAN_QR_FINISHED");
	}

	if (message == reg_msg_id) {
		OnCancel();
	}
	if (message == WM_TRAY_NOTIFICATION) {
		_trayMgr.OnTrayNotification1(wParam, lParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CKAgentQRDlg::OnTrayOpen()
{
	SetForegroundWindow();
	ShowWindow(SW_SHOW);
}

ClaDlgAboutAgent *dlgAbout = NULL;

void CKAgentQRDlg::OnTrayExit()
{
	if (dlgAbout != NULL) {
		dlgAbout->PostMessageW(WM_USER + 2);
//		delete dlgAbout;
//		dlgAbout = NULL;
	}
	OnCancel();
}

void CKAgentQRDlg::OnTrayAbout()
{
	dlgAbout = new ClaDlgAboutAgent(IDB_PNG_ABOUT_US_BG);
	dlgAbout->DoModal();
	delete dlgAbout;
	dlgAbout = NULL;
	// TODO: Add your command handler code here
}


void CKAgentQRDlg::OnTimer(UINT_PTR nIDEvent)
{
	_trayMgr.SetIcon(m_hIcon);

	CRect screenRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

	// Get the dialog dimensions
	CRect dialogRect;
	GetWindowRect(&dialogRect);

	// Calculate the position for the bottom-right corner
	int w = 64 + 12;//max(dialogRect.Width(), dialogRect.Height());

	int xPos = 0;
	int yPos = 0;
	if (index == 0) {
		xPos = 1;
		yPos = 1;
	}
	else if (index == 1) {
		xPos = 0;
		yPos = screenRect.bottom - w;
	}
	else if (index == 2) {
		xPos = screenRect.right - w;
		yPos = 0;
	}
	else {
		xPos = screenRect.right - w;
		yPos = screenRect.bottom - w;
	}

	MoveWindow(xPos, yPos, w, w);
	CDialog::OnTimer(nIDEvent);
}


void CKAgentQRDlg::OnDestroy()
{
	KillTimer(0);
	_trayMgr.DeleteIcon();
	CDialog::OnDestroy();
}
