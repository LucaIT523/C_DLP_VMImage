
// KLauncherDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "afxdialogex.h"
#include "KGlobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKLauncherDlg dialog
CKLauncherDlg* g_pMain = NULL;
#define WM_USER_CONN_VALIDATE	(WM_USER+0x1001)


CKLauncherDlg::CKLauncherDlg(UINT p_nBgID, CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_KLAUNCHER_DIALOG, p_nBgID, L"PNG", NULL, pParent, TRUE)
	//: KDialog(IDD_KLAUNCHER_DIALOG, pParent)
	, m_bMinimize(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_MIN, m_btnMin);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
}

BEGIN_MESSAGE_MAP(CKLauncherDlg, KDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_MIN, &CKLauncherDlg::OnBnClickedBtnMin)
	ON_COMMAND(ID_TRAY_OPEN, &CKLauncherDlg::OnTrayOpen)
	ON_COMMAND(ID_TRAY_EXIT, &CKLauncherDlg::OnTrayExit)
END_MESSAGE_MAP()


// CKLauncherDlg message handlers

BOOL CKLauncherDlg::OnInitDialog()
{
	KDialog::OnInitDialog();

	RECT rect;
	m_btnClose.GetWindowRect(&rect);
	ScreenToClient(&rect); // Convert to client coordinates
	m_btnClose.MoveWindow(rect.left, rect.top + 1, rect.right - rect.left, rect.bottom - rect.top);
	m_btnMin.GetWindowRect(&rect);
	ScreenToClient(&rect); // Convert to client coordinates
	m_btnMin.MoveWindow(rect.left, rect.top + 1, rect.right - rect.left, rect.bottom - rect.top);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	g_pMain = this;


//	_trayMgr.Create(this, WM_TRAY_NOTIFICATION, L"KLauncher", m_hIcon, 1);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKLauncherDlg::OnPaint()
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
		KDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

KDialog* CKLauncherDlg::CreateStandardDialog()
{
	_pPageLogin = new ClaPageLogin(this);
	_pPageLogin->CreateDialog_();

	_pPageConfig = new ClaPageConfig(this);
	_pPageConfig->CreateDialog_();

	_pPageOTP = new ClaPageOTP(this);
	_pPageOTP->CreateDialog_();

	_pPageMain = new ClaPageMain(this);
	_pPageMain->CreateDialog_();

	_pPagePort = new ClaPagePortlist(this);
	_pPagePort->CreateDialog_();

	_pPageAboutUs = new ClaPageAboutUS(this);
	_pPageAboutUs->CreateDialog_();

	wchar_t wszServer[64], wszPort[10];
	KGlobal::readServerIP(wszServer, 64, wszPort, 10, NULL);
	if (wcslen(wszServer) == 0 || wcslen(wszPort) == 0 || _wtoi(wszPort) == 0) {
		_pCurPage = _pPageConfig;
	}
	else {
		_pCurPage = _pPageLogin;
	}

	return _pCurPage;
}

void CKLauncherDlg::OnInitChildrenWnds()
{
	UINT nImgIdMins[TWS_BUTTON_NUM] = { IDB_PNG_BTN_MIN, IDB_PNG_BTN_MIN_H, 0, 0 };
	m_btnMin.LoadImageList(nImgIdMins);

	UINT nImgIdCloses[TWS_BUTTON_NUM] = { IDB_PNG_BTN_CLOSE, IDB_PNG_BTN_CLOSE_H, 0, 0 };
	m_btnClose.LoadImageList(nImgIdCloses);

	RegisterTranslucentWnd(&m_btnMin);
	RegisterTranslucentWnd(&m_btnClose);
}

void CKLauncherDlg::ShowConfigPage()
{
	_pPageConfig->ShowDialog();
	if (_pCurPage != _pPageConfig)
		_pCurPage->HideDialog();
	_pCurPage = _pPageConfig;
	m_pStandardDlg = _pCurPage;
}

void CKLauncherDlg::ShowLoginPage()
{
	_pPageLogin->ShowDialog();
	if (_pCurPage != _pPageLogin)
		_pCurPage->HideDialog();
	_pCurPage = _pPageLogin;
	m_pStandardDlg = _pCurPage;
}

void CKLauncherDlg::ShowOtpPage()
{
	_pPageOTP->ShowDialog();
	if (_pCurPage != _pPageOTP)
		_pCurPage->HideDialog();
	_pCurPage = _pPageOTP;
	m_pStandardDlg = _pCurPage;
}

void CKLauncherDlg::ShowMainPage(BOOL p_bCallback /*= TRUE*/)
{
	_pPageMain->ShowDialog(p_bCallback);
	if (_pCurPage != _pPageMain)
		_pCurPage->HideDialog();
	_pCurPage = _pPageMain;
	m_pStandardDlg = _pCurPage;
}

void CKLauncherDlg::ShowPortPage(ClaKcGroup* pGroup)
{
	_pPagePort->setGroup(pGroup);
	if (_pCurPage != _pPagePort)
		_pPagePort->ShowDialog();
	_pCurPage->HideDialog();
	_pCurPage = _pPagePort;
	m_pStandardDlg = _pCurPage;
}

void CKLauncherDlg::ShowAboutPage()
{
	_pPageAboutUs->ShowDialog();
	if (_pCurPage != _pPageAboutUs)
		_pCurPage->HideDialog();
	_pCurPage = _pPageAboutUs;
	m_pStandardDlg = _pCurPage;
}


void CKLauncherDlg::OnDestroy()
{
	delete _pPageLogin;
	delete _pPageConfig;
	delete _pPageOTP;
	delete _pPageMain;
	delete _pPagePort;
	delete _pPageAboutUs;

	KDialog::OnDestroy();
}


void CKLauncherDlg::OnCancel()
{
	if (g_pVmMgr->isRunning()) {
		UINT retCode = KMessageBox(L"VMware is running and you need to close vmware first.", MB_CANCELTRYCONTINUE);
		if (retCode == IDTRYAGAIN) {
			//.	suspend
			_pPageMain->onAfterStopVM(FALSE, 6/*stop_vm_reason_client_force_stop*/);
		}
		else if (retCode == IDOK) {
			//. terminate
			_pPageMain->onAfterStopVM(TRUE, 6/*stop_vm_reason_client_force_stop*/);
		}
		else {
			//. Cancel
			return;
		}
	}
	else {
		KDialog::OnCancel();
	}
}

BOOL CALLBACK_VALIDATE(void* p, int result, const char* p_szMessage, int p_nSpeed, char* p_szRate)
{
	CKLauncherDlg* pThis = (CKLauncherDlg*)p;
	pThis->_strValidateMessage.Format(L"%S", p_szMessage);
	pThis->_nValidateStatus = result;
	pThis->_nValidateSpeed = p_nSpeed;
	pThis->_strRate.Format(L"%S", p_szRate);
	return (BOOL)pThis->SendMessage(WM_USER_CONN_VALIDATE);
}


LRESULT CKLauncherDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_USER_CONN_VALIDATE == message) {
		if (_nValidateStatus == 0) {
			if (_strValidateMessage == L"Session invalid") {
				g_pVmMgr->suspendVM();
				KMessageBox(L"Session is disconnected.");
				ShowLoginPage();
				return false;
			}
			else if (_strValidateMessage == L"Disabled User") {
				g_pVmMgr->suspendVM();
				KMessageBox(L"Your user account has been disabled.");
				ShowLoginPage();
				return false;
			}
			else if (_strValidateMessage == L"Disabled Machine") {
				g_pVmMgr->suspendVM();
				KMessageBox(L"Your machine has been disabled.");
				ShowLoginPage();
				return false;
			}
		}
		return TRUE;
	}
	else if (message == WM_TRAY_NOTIFICATION) {
		_trayMgr.OnTrayNotification1(wParam, lParam);
	}
	else if (message == WM_ACTIVATEAPP) {
		if (m_bMinimize && wParam ) {
			m_bMinimize = FALSE;
			ShowWindow(SW_RESTORE);
			UpdateView();
		}
	}

	return KDialog::WindowProc(message, wParam, lParam);
}

void CKLauncherDlg::OnBnClickedBtnMin()
{
	m_bMinimize = TRUE;
	//_pCurPage->ShowWindow(SW_HIDE);
	ShowWindow(SW_MINIMIZE);
}

void CKLauncherDlg::OnTrayOpen()
{
	SetForegroundWindow();
	ShowWindow(SW_SHOWNORMAL);
	_pCurPage->ShowWindow(SW_SHOW);
}

void CKLauncherDlg::OnTrayExit()
{
	OnCancel();
}
