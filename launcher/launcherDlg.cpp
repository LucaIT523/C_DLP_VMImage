
// launcherDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "launcher.h"
#include "launcherDlg.h"
#include "afxdialogex.h"

#include "ClaDlgPortList.h"

#include "ClaModelManager.h"
#include "ClaNetAdapter.h"
#include "SessionManager.h"
#include "ClaPathMgr.h"
#include "ClaProcess.h"
#include "KC_common.h"

#include "json.hpp"
#include "base64enc.h"

#include "ClaDlgTest.h"
#include "ClaDlgTimeout.h"
#include "mfcCmn.h"
#include "ClaRegMgr.h"
#include "kncrypto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using json = nlohmann::json;

// ClauncherDlg dialog

HANDLE lv_hProcNODE = NULL;
HANDLE lv_thdEnv = NULL;
HANDLE lv_thdUpdatePolicy = NULL;
HANDLE lv_thdDownload = NULL;

ClauncherDlg* lv_pwndLauncher = NULL;

UINT TF_SET_ENVIRONMENT(void* p);
UINT TF_UPDATE_POLICY(void* p);

extern HANDLE g_hProcNodeClient;

ClauncherDlg::ClauncherDlg(CWnd* pParent /*=nullptr*/)
	: ClaDialogPng(IDD_LAUNCHER_DIALOG, pParent)
	, m_strStatus(L"Status : -")
	, m_strOS(_T("Operating System : -"))
	, m_strSize(_T("Size : -"))
	, m_strDesc(_T("Description : \n - "))
	, m_strPath(_T("Path : -"))
	, m_strDown(_T("-"))
	, _bStarting(FALSE)
	, m_bSessionOut(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	ClaDialogPng::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_ITEM, m_lstItems);
	DDX_Control(pDX, IDC_BTN_INFO, m_btnInfo);
	DDX_Control(pDX, IDC_BTN_SETTING, m_btnSetting);
	DDX_Control(pDX, IDC_BTN_REFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_STC_ICON, m_stcIcon);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_BTN_MIN, m_btnMin);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_GRP_BG, m_grpBG);
	DDX_Control(pDX, IDC_STC_SUBTITLE, m_stcSubTitle);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_BTN_DOWN, m_btnDownload);
	DDX_Control(pDX, IDC_GRP_PANEL, m_stcPanel);
	DDX_Control(pDX, IDC_GRP_PANEL2, m_stcPanel2);
	DDX_Control(pDX, IDC_STC_OS, m_stcOS);
	DDX_Control(pDX, IDC_STC_SIZE, m_stcSize);
	DDX_Control(pDX, IDC_STC_DESC, m_stcDesc);
	DDX_Text(pDX, IDC_STC_OS, m_strOS);
	DDX_Text(pDX, IDC_STC_SIZE, m_strSize);
	DDX_Text(pDX, IDC_STC_DESC, m_strDesc);
	DDX_Control(pDX, IDC_STC_STATE, m_stcStatus);
	DDX_Text(pDX, IDC_STC_STATE, m_strStatus);
	DDX_Control(pDX, IDC_STC_PATH, m_stcPath);
	DDX_Text(pDX, IDC_STC_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDT_XPATH, m_strXPath);
	DDX_Control(pDX, IDC_EDT_XPATH, m_edtXPath);
	DDX_Control(pDX, IDC_PGS_DOWN, m_pgsDown);
	DDX_Control(pDX, IDC_STC_DOWN, m_stcDown);
	DDX_Text(pDX, IDC_STC_DOWN, m_strDown);
}

BEGIN_MESSAGE_MAP(ClauncherDlg, ClaDialogPng)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_DOWN, &ClauncherDlg::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDC_BTN_START, &ClauncherDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &ClauncherDlg::OnBnClickedBtnStop)
	ON_NOTIFY(NM_CLICK, IDC_LST_ITEM, &ClauncherDlg::OnNMClickLstItem)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_ITEM, &ClauncherDlg::OnNMDblclkLstItem)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &ClauncherDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_EXIT, &ClauncherDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_INFO, &ClauncherDlg::OnBnClickedBtnInfo)
	ON_BN_CLICKED(IDC_BTN_SETTING, &ClauncherDlg::OnBnClickedBtnSetting)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &ClauncherDlg::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDCANCEL, &ClauncherDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_MIN, &ClauncherDlg::OnBnClickedBtnMin)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_NCMOUSEMOVE()
END_MESSAGE_MAP()


// ClauncherDlg message handlers

BOOL ClauncherDlg::OnInitDialog()
{
	ClaDialogPng::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	LoadLibrary(L"KenanHelperVM32.dll");

	lv_pwndLauncher = this;

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main start");

	g_pVmMgr->initialize(m_hWnd);

	void* pMem;
	_map.createMap(L"Global\\kpol", 100, &pMem);

	_init_ui();

	_load();

	_show_list();

	//m_btnInfo.SetIcon(theApp.LoadIconW(IDI_INFO));
	m_btnSetting.SetIcon(theApp.LoadIconW(IDI_POLICY));
	//m_btnRefresh.SetIcon(theApp.LoadIconW(IDI_REFRESH));

	_show_status();
	_update_ui();

	_bTimer = TRUE;
	_lLastTime = GetTickCount();
	SetTimer(0, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void ClauncherDlg::OnPaint()
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
		ClaDialogPng::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ClauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString findVMX(const wchar_t* p_wszDir) {
	CString strFind; strFind.Format(L"%s\\*", p_wszDir);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strFind);

	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) continue;
		if (finder.IsDirectory()) {
			CString strRet = findVMX(finder.GetFilePath());
			if (!strRet.IsEmpty()) return strRet;
		}
		else {
			CString strRet = finder.GetFilePath();
			if (strRet.Right(4).MakeUpper() == L".VMX") {
				return strRet;
			}
		}
	}

	return L"";
}

UINT TF_DOWN_UNZIP(void* p) {
	ClauncherDlg* pThis = (ClauncherDlg*)p;

	pThis->download_unzip();

	lv_thdDownload = NULL;

	return 0;
}

BOOL CALLBACK_DOWN(long long lCur, long long lTotal)
{
	int d = lCur * 100 / lTotal;
	int p = (lCur * 10000 / lTotal) % 100;
	lv_pwndLauncher->SendMessage(WM_USER + 31, d, p);
	return TRUE;
}

int ClauncherDlg::_execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam)
{
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = L"open";
	ShExecInfo.lpFile = p_szEXE;
	ShExecInfo.lpParameters = p_pszCommandParam;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE; // SW_NORMAL

	if (ShellExecuteEx(&ShExecInfo)) {
		// Wait for the process to exit
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	}
	return 1;
}
void ClauncherDlg::download_unzip() {
	CreateDirectory(ClaPathMgr::GetDP() + L"\\VMs", NULL);

	wchar_t wszDown[MAX_PATH]; memset(wszDown, 0, sizeof(wszDown));
	swprintf_s(wszDown, MAX_PATH, L"%s\\VMs\\%s", ClaPathMgr::GetDP(), m_strDownName);

	//if (1 && kc_down_vmimage(m_nDownID, wszDown, FALSE, (KC_CALLBACK_DOWN_VM)CALLBACK_DOWN) != 0) {
	//	SendMessage(WM_USER + 34, 0, 0);
	//	return;
	//}

	char szZipped[MAX_PATH]; sprintf_s(szZipped, MAX_PATH, "%S", wszDown);
	char szUnzipped[MAX_PATH]; sprintf_s(szUnzipped, MAX_PATH, "%S.e", wszDown);

	SendMessage(WM_USER + 32, 0, 0);

	CreateDirectoryA(szUnzipped, NULL);

	wchar_t wszCmd[512]; memset(wszCmd, 0, sizeof(wszCmd));
	swprintf_s(wszCmd, 512, L"x \"%s\" -o\"%s.e\" -y -spe -spf", wszDown, wszDown);
	ClaPathMgr::UnzipFile(ClaPathMgr::GetDP(), wszCmd);
	//_execute(L"winrar", wszCmd);
	//if (MyUnzipData(szZipped, szUnzipped) != 0) {
	//	lv_pThis->SendMessage(WM_USER + 4, 0, 0);
	//	return;
	//}

	SendMessage(WM_USER + 33, 0, 0);
}
void ClauncherDlg::OnBnClickedBtnDown()
{
	if (g_pVmMgr->isRunning()) {
		AfxMessageBox(L"VMware image is running. Please try again after close running vmware.");
		return;
	}

	if (lv_thdDownload != NULL) {
		if (AfxMessageBox(L"A VMware image is downloading now. Do you want to stop download?", MB_OKCANCEL) == IDCANCEL){
			return;
		}
		else {
			TerminateThread(lv_thdDownload, 0);
			lv_thdDownload = NULL;
			m_pgsDown.ShowWindow(SW_HIDE);
			m_stcDown.ShowWindow(SW_HIDE);
			m_btnDownload.setBgImage(IDB_PNG_BTN_DOWN);
			m_btnDownload.Invalidate();
			return;
		}
	}
	int nSelect = m_lstItems.getSelectedIndex();

	if (nSelect == -1) {
		AfxMessageBox(L"Please select the vmware item on left side listbox before download.");
		return;
	}

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main down start");

	DWORD dwData = m_lstItems.GetItemData(nSelect);
	int i = dwData >> 16;
	int j = dwData & 0xFFFF;

	m_nDownGID = i;
	m_nDownIID = j;

	ClaKcVmImage* pImage = &(m_policy._lstGroup[i]._lstVmImage[j]);
	if (pImage->_wszPath && GetFileAttributes(pImage->_wszPath) != INVALID_FILE_ATTRIBUTES) {
		AfxMessageBox(L"You have already download this VMware image.");
		return;
	}
	else {
		if (AfxMessageBox(L"Do you want to download the VM image?", MB_OKCANCEL) != IDOK) {
			return;
		}
	}

	m_pgsDown.SetRange(0, 100);
	DWORD dwTID = 0;

	m_nDownID = m_policy._lstGroup[i]._lstVmImage[j]._id;
	m_strDownName.Format(L"%S", m_policy._lstGroup[i]._lstVmImage[j]._szTitle);
	lv_thdDownload = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_DOWN_UNZIP, this, 0, &dwTID);

	m_pgsDown.ShowWindow(SW_SHOW);
	m_stcDown.ShowWindow(SW_SHOW);

	m_btnDownload.setBgImage(IDB_PNG_STOP);
	m_btnDownload.Invalidate();
}


void ClauncherDlg::OnBnClickedBtnStart()
{
	if (lv_thdDownload) {
		AfxMessageBox(L"Downloading VMware image now. Please wait");
		return;
	}


	int nSelect = m_lstItems.getSelectedIndex();
	if (nSelect == -1) {
		AfxMessageBox(L"Please select the vmware on left side list before launch");
		return;
	}

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main launch");

	DWORD dwData = m_lstItems.GetItemData(nSelect);
	int i = dwData >> 16;
	int j = dwData & 0xFFFF;

	ClaKcVmImage* p = &(m_policy._lstGroup[i]._lstVmImage[j]);
	_nGroupID = m_policy._lstGroup[i]._id;
	_nImageID = m_policy._lstGroup[i]._lstVmImage[j]._id;
	CString strPath = p->_wszPath;
	CString strPwd; strPwd.Format(L"%S", p->_szPwd);
	if (strPath.IsEmpty() || GetFileAttributes(strPath) == INVALID_FILE_ATTRIBUTES) {
		AfxMessageBox(L"You need to download VMware image before launch");
		return;
	}

	g_pVmMgr->m_strVMX = strPath;
	g_pVmMgr->m_strPWD = strPwd;

	if (g_pVmMgr->isRunning()) {
		if (AfxMessageBox(L"VMware is running. Do you want to stop it?", MB_OKCANCEL) == IDOK) {
			g_pVmMgr->stopVM();

			TerminateThread(lv_thdEnv, 0);
			lv_thdEnv = NULL;
			TerminateThread(lv_thdUpdatePolicy, 0);
			lv_thdUpdatePolicy = NULL;

			g_pVmMgr->m_strVMX.Empty();

			m_btnStart.setBgImage(IDB_PNG_BTN_RUN);
			m_strStatus.Format(L"Status : Ready");
			UpdateData(FALSE);
//			m_lstItems.EnableWindow(TRUE);
		}
		return;
	}

	_curGroup = m_policy._lstGroup[i];
	int nSts = 0;
	ClaKcGroup* pGroupDown = kc_down_config(_nGroupID, _nImageID);
	if (pGroupDown == NULL) {
		AfxMessageBox(L"Unable to connect to the server");
		return;
	}
	_curGroup = pGroupDown[0];
	KGlobal::writeConfig(&_curGroup._config);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main launch getconfig = %d", nSts);
	if (nSts != 0) {
		AfxMessageBox(L"Unable to connect to the server");
		return;
	}

	_bStarting = TRUE;
	//nSts = g_pVmMgr->launchVM(strPath, strPwd);
	if (nSts != 0) {
		g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main launch failed[%d][%d]", nSts, GetLastError());
		if (GetLastError() == ERROR_PASSWORD_RESTRICTION) {
			AfxMessageBox(L"It has been failed to launch VMware image due to incorrect password");
		}
		else {
			AfxMessageBox(L"Failed to launch VM"); 
		}
		return;
	}

	m_strStatus.Format(L"Status : Running");

	UpdateData(FALSE);

	m_strVMX = strPath; m_strPwd = strPwd;

	m_btnStart.setBgImage(IDB_PNG_BTN_TURN);
//	m_lstItems.EnableWindow(FALSE);

	_bStarting = FALSE;

	DWORD dwTID = 0;
	lv_thdEnv = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_SET_ENVIRONMENT, this, 0, &dwTID);
//	OnBnClickedBtnRefresh();
}

void ClauncherDlg::OnBnClickedBtnStop()
{
	int nSelect = m_lstItems.getSelectedIndex();
	if (nSelect == -1) {
		AfxMessageBox(L"Please select the vmware item on left side list");
		return;
	}

	DWORD dwData = m_lstItems.GetItemData(nSelect);
	int i = dwData >> 16;
	int j = dwData & 0xFFFF;

	ClaKcVmImage* p = &(m_policy._lstGroup[i]._lstVmImage[j]);

	CString strPath = p->_wszPath;
	CString strPwd; strPwd.Format(L"%S", p->_szPwd);

	if (g_pVmMgr->stopVM() != 0) {
		AfxMessageBox(L"Failed to launch VM"); return;
	}

	Sleep(1000);

	OnBnClickedBtnRefresh();
}

void ClauncherDlg::OnNMClickLstItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int nSelect = m_lstItems.getSelectedIndex();
	if (nSelect == -1) return;

	if (lv_thdEnv || lv_thdDownload || _bStarting) {
		AfxMessageBox(L"You can not select or run another VM, while VM is running or downloading.");
		return;
	}

	DWORD dwData = m_lstItems.GetItemData(nSelect);
	int i = dwData >> 16;
	int j = dwData & 0xFFFF;

	ClaKcVmImage* p = &(m_policy._lstGroup[i]._lstVmImage[j]);
	_nGroupID = m_policy._lstGroup[i]._id;
	_nImageID = m_policy._lstGroup[i]._lstVmImage[j]._id;

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main down policy");

	_curGroup = m_policy._lstGroup[i];

	//ClaKcGroup* pGroup = kc_down_config(_nGroupID, _nImageID);
	//if (pGroup == NULL) {
	//	AfxMessageBox(L"Unable to connect to server. Please try again.");
	//	return;
	//}
	//_curGroup = pGroup[0];

	m_strOS.Format(L"Operating System : Windows 11 x64");

	CString sSize = getSize(m_policy._lstGroup[i]._lstVmImage[j]._lSize);

	m_strSize.Format(L"Size : %s", sSize);

	m_strDesc.Format(L"Description : \n%S", m_policy._lstGroup[i]._lstVmImage[j]._szDesc);

	m_strPath.Format(L"Path :");
	m_strXPath = m_policy._lstGroup[i]._lstVmImage[j]._wszPath;

	if (m_policy._lstGroup[i]._lstVmImage[j]._wszPath[0] == 0
	||	GetFileAttributes(m_policy._lstGroup[i]._lstVmImage[j]._wszPath) == INVALID_FILE_ATTRIBUTES)
	{
		m_strStatus.Format(L"Status : No VMware image");
	}
	else {
		if (g_pVmMgr->isRunning()) {
			m_strStatus.Format(L"Status : Running");
		}
		else {
			m_strStatus.Format(L"Status : Ready");
		}
	}

	UpdateData(FALSE);

	//m_stcPanel.Invalidate();
	//m_stcOS.Invalidate();
	//m_stcSize.Invalidate();
	//m_stcPanel2.Invalidate();
	//m_stcDesc.Invalidate();
	//m_stcStatus.Invalidate();
	//m_stcPath.Invalidate();
	GetDlgItem(IDC_EDT_XPATH)->Invalidate();

//	Invalidate();

	_update_ui();
}

void ClauncherDlg::OnNMDblclkLstItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//int nIndex = m_lstItems.getSelectedIndex();
	//if (nIndex == -1) return;

	//ClaModelImage* p = (ClaModelImage*)m_lstItems.GetItemData(nIndex);
	//if (p->isRunning()) {
	//	return;
	//}
	//if (p->isRunable()) {
	//	OnBnClickedBtnStart();
	//	return;
	//}
	//OnBnClickedBtnDown();
}

void ClauncherDlg::OnDestroy()
{
	for (int i = 0; i < m_lstModelImage.GetCount(); i++) {
		delete m_lstModelImage[i];
	}

	if (_bTimer) {
		KillTimer(0);
	}
	TerminateProcess(lv_hProcNODE, 0);
	ClaDialogPng::OnDestroy();
}

void ClauncherDlg::_init_ui() {
	
	//DWORD dwStyle = TVS_EX_ITEMLINES | TVS_EX_ITEMLINES | TVS_EX_ALTERNATECOLOR | TVS_EX_SUBSELECT;
	//
	//m_lstItems.SetStyle(m_lstItems.GetStyle() | TVS_HASBUTTONS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_FULLROWSELECT);
	//m_lstItems.SetExtendedStyle(dwStyle);
	//m_lstItems.InsertColumn(0, L"");
	//m_lstItems.InsertColumn(1, L"Name");
	//m_lstItems.InsertColumn(2, L"Description");
	//m_lstItems.InsertColumn(3, L"Size");
	//m_lstItems.InsertColumn(4, L"Status");

	CRect rt;
	GetDlgItem(IDCANCEL)->GetClientRect(rt);
	setTitleHeight(rt.Height());

	m_grpBG.SetBackgroundColor(GD_COLOR_GROUP_BG, GD_COLOR_GROUP_BG);
	m_stcIcon.LoadPng(IDB_PNG_LOGO);

	m_stcTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, TRUE, 0);
	m_stcTitle.SetFontColor(GD_COLOR_LABEL);

	m_stcSubTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TITLE, TRUE, 1);
	m_stcSubTitle.SetFontColor(GD_COLOR_LABEL);

	m_btnCancel.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnCancel.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);
	m_btnCancel.setBgImage(IDB_PNG_EXIT);

	m_btnMin.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnMin.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);
	m_btnMin.setBgImage(IDB_PNG_MIN);

	m_stcPanel.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcPanel2.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);

	m_btnStart.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnStart.setBgImage(IDB_PNG_BTN_RUN);

	m_btnDownload.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnDownload.setBgImage(IDB_PNG_BTN_DOWN);

	m_btnRefresh.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnRefresh.setBgImage(IDB_PNG_REFRESH);

	m_btnInfo.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnInfo.setBgImage(IDB_PNG_BTN_INFO);

	m_stcOS.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0, 0);
	m_stcOS.SetFontColor(GD_COLOR_LABEL);

	m_stcSize.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0, 0);
	m_stcSize.SetFontColor(GD_COLOR_LABEL);

	m_stcStatus.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0, 0);
	m_stcStatus.SetFontColor(GD_COLOR_LABEL);

	m_stcDesc.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0, 0);
	m_stcDesc.SetFontColor(GD_COLOR_LABEL);

	m_stcPath.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0, 0);
	m_stcPath.SetFontColor(GD_COLOR_LABEL);

	m_edtXPath.SetBackgroundColor(RGB(0, 255, 255), GD_COLOR_GROUP_BG);
	m_edtXPath.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 1);
	m_edtXPath.SetFontColor(RGB(255, 0, 0));

	m_stcDown.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 1, 1);
	m_stcDown.SetFontColor(GD_COLOR_LABEL);

	m_lstItems.SetFontSize(GD_FONT_SIZE_TEXT, 0);
	m_lstItems.SetHeaderFontHW(GD_FONT_SIZE_TEXT, 0);
	m_lstItems.SetRowHeigt(36);
	m_lstItems.SetHeaderHeight(1.5);
	m_lstItems.SetHeaderBackColorC(GD_COLOR_DLG_BG, 2);
	m_lstItems.SetBgColor(GD_COLOR_LIST1, GD_COLOR_LIST2);

	m_lstItems.SetExtendedStyle(m_lstItems.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lstItems.setHead(L" ;Group Name;Image Name");
	m_lstItems.autoFitWidth();

	m_lstItems.setColor(GD_COLOR_LIST1, GD_COLOR_LIST2);
	m_lstItems.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE);

}

void ClauncherDlg::_load() {

	int nCount = 0;

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main load");

	ClaKcPolicy* pRet = kc_down_list();

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main load = %d", pRet);

	if (pRet == 0) {
		AfxMessageBox(L"Unable to connect to server.");
		return;
	}

	m_policy = pRet[0];

	for (int i = 0; i < m_policy._lstGroup.GetCount(); i++) {
		for (int j = 0; j < m_policy._lstGroup[i]._lstVmImage.GetCount(); j++) {
			CString dir;
			dir.Format(L"%s\\VMs\\%S.e", ClaPathMgr::GetDP(), m_policy._lstGroup[i]._lstVmImage[j]._szTitle);

			swprintf_s(m_policy._lstGroup[i]._lstVmImage[j]._wszPath, 260, L"%s", findVMX(dir));
		}
	}
}

void ClauncherDlg::_show_list() {
	//HTREEITEM hRoot = m_lstItems.InsertItem(L"aaa", 0, 0, TVI_ROOT);
	//HTREEITEM hItem = m_lstItems.InsertItem(L"BBB", 0, 0, hRoot);
	//m_lstItems.SetItem(hItem, 1, TVIF_TEXT, L"BBB1", -1, -1, 0, 0);
	//m_lstItems.SetItem(hItem, 2, TVIF_TEXT, L"BBB2", -1, -1, 0, 0);
	//m_lstItems.SetItem(hItem, 3, TVIF_TEXT, L"BBB3", -1, -1, 0, 0);

	
	while(m_lstItems.GetItemCount() > 0)
		m_lstItems.DeleteItem(0);

	for (int i = 0; i < m_policy._lstGroup.GetCount(); i++) {
		CString strGroupName; strGroupName.Format(L"%S", m_policy._lstGroup[i]._szName);
		for (int j = 0; j < m_policy._lstGroup[i]._lstVmImage.GetCount(); j++) {
			CString strName; strName.Format(L"%S", m_policy._lstGroup[i]._lstVmImage[j]._szTitle);
			CString strDesc; strDesc.Format(L"%S", m_policy._lstGroup[i]._lstVmImage[j]._szDesc);
			CString strSize; strSize.Format(L"%lld", m_policy._lstGroup[i]._lstVmImage[j]._lSize);
			CString strStatus;
			int nIndex = m_lstItems.addRecord(L"", strGroupName, strName);

			DWORD dwData = i << 16 | j;
			m_lstItems.SetItemData(nIndex, (DWORD_PTR)dwData);
		}
	}

	m_lstItems.autoFitWidth();
}

void ClauncherDlg::_show_status() {

}

int ClauncherDlg::_launchRouter() {

	char szID[10]; kc_get_uid(szID);
	int uid = atoi(szID);
	char szMID[100]; 
	sprintf_s(szMID, 100, "%S", KGlobal::GetMachineID());

	int nCount = 0;
	{
		json list;

		for (int i = 0; i < _curGroup._lstPort.GetCount(); i++) {
			char sz[100]; memset(sz, 0, sizeof(sz));
			sprintf_s(sz, 100, "%S", _curGroup._lstPort[i]._szTarget);
			json item = json{
				{"user_id", uid},
				{"machine_id", szMID},
				{"listen_port", _curGroup._lstPort[i]._nSourcePort},
				//{"target_port", _curGroup._lstPort[i]._nTargetPort},
				//{"target", sz},
			};
			list.push_back(item);
		}
		std::string str_dump = list.dump();
		std::string str_base64 = base64_encode(str_dump);
		//FILE* pFile; _wfopen_s(&pFile, ClaPathMgr::GetDP() + L"\\policy.dat", L"wb");
		//if (pFile != NULL) {
		//	fwrite(str_base64.c_str(), 1, str_base64.length(), pFile);
		//	fclose(pFile);
		//}
		//else {
		//	return -1;
		//}

	}
	return 0;
}

BOOL is_top_window(HWND handle)
{
	
	return ::GetParent(handle) == (HWND)0;
}

UINT TF_UPDATE_POLICY(void* p) {
	ClauncherDlg* pThis = (ClauncherDlg*)p;
	pThis->tf_update_policy();
	lv_thdUpdatePolicy = NULL;
	return 0;
}

UINT TF_SET_ENVIRONMENT(void* p) {
	ClauncherDlg* pThis = (ClauncherDlg*)p;
	pThis->tf_set_environment();
	lv_thdEnv = NULL;
	return 0;
}
void ClauncherDlg::OnBnClickedButton1()
{
	if (1)
	{
		ClaDlgTest dlg;
		if (dlg.DoModal() != IDOK) return;

		//if (g_pVmMgr->launchVM(dlg.m_strTest, dlg.m_strPass) != 0) {
		{
			AfxMessageBox(L"Failed to launch VM"); return;
		}

		m_strVMX = dlg.m_strTest; m_strPwd = dlg.m_strPass;
	}

	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_SET_ENVIRONMENT, this, 0, &dwTID);
}

unsigned int ClauncherDlg::post_to_agent(const char* p_szIP, BOOL p_bIP, BOOL p_bPORT)
{
	char szID[10]; kc_get_uid(szID);
	int uid = atoi(szID);
	char szMID[100];
	sprintf_s(szMID, 100, "%S", KGlobal::GetMachineID());

	char szSession[64]; memset(szSession, 0, sizeof(szSession));
	kc_get_session(szSession, 64);
	{
		char szHost[128]; memset(szHost, 0, sizeof(szHost));
		sprintf_s(szHost, 128, "{\"ip\":\"%S\", \"gip\":\"%S\",\"fw\":%d, \"session\":\"%s\" }", 
			g_strIP, g_strGlobalIP, 
			_curGroup._config._bCreateOutboundRule ? 0 : 1,
			szSession);
		std::string str_base64_server = szHost;
		std::string str_base64_https;

		{
			json list;

			for (int i = 0; i < _curGroup._lstPort.GetCount(); i++) {
				json item = json{
//					{"id", _curGroup._lstPort[i]},
//					{"title", https_list[i].m_szTitle},
					{"user_id", uid},
					{"machine_id", szMID},
					{"listen_port", _curGroup._lstPort[i]._nSourcePort},
					{"target_port", _curGroup._lstPort[i]._nTargetPort},
					{"target", _curGroup._lstPort[i]._szTarget},
					{"is_https", _curGroup._lstPort[i]._bIsHttps},
					{"is_active", 1},
				};
				list.push_back(item);
			}
			std::string str_dump = list.dump();
			str_base64_https = str_dump;
		}

		BOOL bSts = FALSE;
		for (int i = 0; i < 10; i++) {
			if (g_pVmMgr->isRunning() == FALSE) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - up port and ip - vm stoped");
				PostMessage(WM_USER + 11, 1, 0);
				return -1;
			}

			g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread upip[%S]", str_base64_server.c_str());

			if (p_bIP) {
				if (kc_up_ip(p_szIP, str_base64_server.c_str(), NULL, 0, NULL, 0) != 0) {
					Sleep(5000);
					continue;
				}
			}

			if (str_base64_https == "none") {
				str_base64_https = "[]";
			}

			if (p_bPORT) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread up port");
				if (kc_up_port(p_szIP, str_base64_https.c_str()) != 0) {
					Sleep(5000);
					continue;
				}
			}

			bSts = TRUE;
			break;
		}
		if (bSts == FALSE) {
			g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread fail set ip and port");
			PostMessage(WM_USER + 10, 1, 0);
			return -2;
		}
	}
	return 0;
}

std::string lv_sIP;
void ClauncherDlg::tf_set_environment()
{
	char szID[10]; kc_get_uid(szID);
	int uid = atoi(szID);
	char szMID[100];
	sprintf_s(szMID, 100, "%S", KGlobal::GetMachineID());

	char szIP[20]; memset(szIP, 0, sizeof(szIP));

	g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread ");

	if (_launchRouter() != 0) {
	}

	g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread launch rooter finished ");

	while (TRUE) {
		g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread try to get ip address of vm");

		if (g_pVmMgr->isRunning() == FALSE) {
			g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - waiting IP - vm stoped");
			PostMessage(WM_USER + 11, 1, 0);
			return;
		}
#if 1
		if (g_pVmMgr->getIP(szIP, 20) != 0) {

		}
#else
		strcpy_s(szIP, 20, "192.168.144.136");
#endif

		if (szIP[0] == 0) {
			Sleep(1000); continue;
		}

		if (strcmp(szIP, "unknown") == 0) {
			Sleep(1000); continue;
		}

		break;
	}

	lv_sIP = szIP;
	g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread ip = [%S]", szIP);
	if (post_to_agent(szIP, TRUE, TRUE) != 0) {
		return;
	}

	int nRetry = 0;
	{
		int nIdleTime;
		std::string scriptResult;
		char agentScriptResult[260];
		int violationID;
		int scriptID;
		while (TRUE) {
			if (g_pVmMgr->isRunning() == FALSE) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - idle waiting - vm stoped");
				PostMessage(WM_USER + 11, 1, 0);
				return;
			}

			if ((kc_get_idle(szIP, &nIdleTime, NULL, &violationID, &scriptID, agentScriptResult) != 0 || nIdleTime == -1)) {
				if (nRetry > 1)
				{
					g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread fail get idle");
					PostMessage(WM_USER + 10, 1, 0);
				}
				else {
					nRetry++;
					g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread fail get idle[%d]", nRetry);
					continue;
				}
				break;
			}

			nRetry = 0;
			
			if (m_policy.agent_timeout != 0
			&& m_policy.agent_timeout < (nIdleTime / 1000))
			{
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread %d", nIdleTime);
				if (SendMessage(WM_USER + 10, 2, 0) == 1) {
					//.	stoped
					break;
				}
			}

			if (lv_thdUpdatePolicy == NULL) {
				DWORD dwTID;
				lv_thdUpdatePolicy = CreateThread(
					NULL, 0, (LPTHREAD_START_ROUTINE)TF_UPDATE_POLICY,
					this, 0, &dwTID
				);
			}

			//.	check node_client is running
			if (WaitForSingleObject(g_hProcNodeClient, 100) != WAIT_TIMEOUT) {
				PostMessage(WM_USER + 10, 9, 0);
				break;
			}
			Sleep(5000); continue;
		}
	}
}

void ClauncherDlg::tf_update_policy()
{
	while (TRUE) {
		Sleep(1000 * g_nPeriod);
		if (lv_thdEnv == NULL) break;

		ClaKcGroup* pGroupDown = kc_down_config(_nGroupID, _nImageID);
		if (pGroupDown != NULL) {
			if (_curGroup != pGroupDown[0]) {

				KGlobal::writeConfig(&pGroupDown->_config);
				int nEqualPort = _curGroup.comparePort(pGroupDown[0]);
				_curGroup = pGroupDown[0];

				if (nEqualPort != 0) {
					post_to_agent(lv_sIP.c_str(), TRUE, TRUE);
				}
				else {
					post_to_agent(lv_sIP.c_str(), TRUE, FALSE);
				}
				//SendMessage(WM_USER + 10, 3, 0);
				//break;
			}
		}
	}
}

void ClauncherDlg::OnBnClickedBtnExit()
{
	g_pVmMgr->stopVM();
	OnCancel();
}

void ClauncherDlg::OnBnClickedBtnInfo()
{
	if (lv_thdDownload) {
		AfxMessageBox(L"Downloading VMware image now. Please wait");
		return;
	}

	int nSel = m_lstItems.getSelectedIndex();
	if (nSel == -1) {
		AfxMessageBox(L"Please select the vmware item on left side list");
		return;
	}

	int i = m_lstItems.GetItemData(nSel) >> 16;
	int j = m_lstItems.GetItemData(nSel) & 0xFFFF;

	int gid = m_policy._lstGroup[i]._id;
	int iid = m_policy._lstGroup[i]._lstVmImage[j]._id;
	ClaKcGroup *pGroup;
	pGroup = kc_down_config(gid, iid);

	ClaDlgPortList dlg;
	dlg.setColor(GD_COLOR_DLG_BG, GD_COLOR_DLG_BD, GD_COLOR_DLG_TITLE);

	dlg.m_group = pGroup[0];
	dlg.DoModal();
}


void ClauncherDlg::OnBnClickedBtnSetting()
{
	_show_status();
}


void ClauncherDlg::OnBnClickedBtnRefresh()
{
	if (lv_thdDownload) {
		AfxMessageBox(L"Downloading VMware image now. Please wait");
		return;
	}
	_load();
	_show_list();
}

void ClauncherDlg::_update_ui() {
	
	int nSel = m_lstItems.getSelectedIndex();
}


LRESULT ClauncherDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER + 31) {
		m_pgsDown.SetPos(wParam);
		m_strDown.Format(L"%d.%d %%", wParam, lParam);
		UpdateData(FALSE);
	}
	if (message == WM_USER + 32) {
		m_strDown = L"Extract ...";
		UpdateData(FALSE);
	}
	if (message == WM_USER + 33) {
		AfxMessageBox(L"Download vm image is complet successfully.", MB_ICONINFORMATION);
		m_strDown = L"Complete";
		UpdateData(FALSE);
		lv_thdDownload = NULL;

		CString dir;
		dir.Format(L"%s\\VMs\\%s.e", ClaPathMgr::GetDP(), m_strDownName);
		swprintf_s(m_policy._lstGroup[m_nDownGID]._lstVmImage[m_nDownIID]._wszPath, 260, L"%s", findVMX(dir));

		m_btnDownload.setBgImage(IDB_PNG_BTN_DOWN);
		m_btnDownload.Invalidate();

	}
	if (message == WM_USER + 34) {
		AfxMessageBox(L"Failed to download VMware image or the image downloaded is invalid");
		lv_thdDownload = NULL;
		m_strDown = L"Failed";
		UpdateData(FALSE);
		m_stcDown.Invalidate(FALSE);
		m_btnDownload.setBgImage(IDB_PNG_BTN_DOWN);

		m_btnDownload.Invalidate();
	}


	if (message == WM_USER + 10) {
		if (wParam == 1) {
			g_pVmMgr->stopVM();
			AfxMessageBox(L"Failed to connect to VM agent.");

			//.	vmware closed
			TerminateThread(lv_thdEnv, 0);
			lv_thdEnv = NULL;
			TerminateThread(lv_thdUpdatePolicy, 0);
			lv_thdUpdatePolicy = NULL;

			g_pVmMgr->m_strVMX.Empty();
			m_btnStart.setBgImage(IDB_PNG_BTN_RUN);
			m_strStatus.Format(L"Status : Ready");
			UpdateData(FALSE);
//			m_lstItems.EnableWindow(TRUE);
		}
		if (wParam == 2) {
			//.	idle time over
			ClaDlgTimeout dlg;

			dlg.m_strTitle = L"Kenan Launcher";
			dlg.m_strData = L"You have no input to VMWare.";
			dlg.m_nMax = 20;

			if (dlg.DoModal() == IDCANCEL) {
				g_pVmMgr->suspendVM();
				g_pVmMgr->m_strVMX.Empty();

				TerminateThread(lv_thdEnv, 0);
				lv_thdEnv = NULL;
				TerminateThread(lv_thdUpdatePolicy, 0);
				lv_thdUpdatePolicy = NULL;

				m_btnStart.setBgImage(IDB_PNG_BTN_RUN);
				m_strStatus.Format(L"Status : Ready");
				UpdateData(FALSE);
				//				m_lstItems.EnableWindow(TRUE);
				return 1;
			}
			else {
				return 0;
			}
		}
		if (wParam == 3) {
			//. Policy changed
			ClaDlgTimeout dlg;

			dlg.m_strTitle = L"Kenan Launcher";
			dlg.m_strData = L"Policy was updated. The VMware will be shutdown soon.";
			dlg.m_nMax = 60;
			dlg.m_bIDOK = FALSE;

			dlg.DoModal();
			g_pVmMgr->stopVM();
			g_pVmMgr->m_strVMX.Empty();

			TerminateThread(lv_thdEnv, 0);
			lv_thdEnv = NULL;
			TerminateThread(lv_thdUpdatePolicy, 0);
			lv_thdUpdatePolicy = NULL;

			m_btnStart.setBgImage(IDB_PNG_BTN_RUN);
			m_strStatus.Format(L"Status : Ready");
			UpdateData(FALSE);
			//			m_lstItems.EnableWindow(TRUE);
		}
		if (wParam == 5) {
			if (g_pVmMgr->isStart()) {
				g_pVmMgr->stopVM();
			}
			AfxMessageBox(L"Router is not running.");
			OnCancel();
		}
		if (wParam == 9) {
			if (g_pVmMgr->isStart()) {
				g_pVmMgr->stopVM();
			}
			AfxMessageBox(L"KClient is not running. Please restart the launcher and try again.");
			OnCancel();
		}
	}
	else if (message == WM_USER + 11) {
		//.	vmware closed
		TerminateThread(lv_thdEnv, 0);
		lv_thdEnv = NULL;
		TerminateThread(lv_thdUpdatePolicy, 0);
		lv_thdUpdatePolicy = NULL;

		g_pVmMgr->m_strVMX.Empty();
		m_btnStart.setBgImage(IDB_PNG_BTN_RUN);
		m_strStatus.Format(L"Status : Ready");
		UpdateData(FALSE);

//		m_lstItems.EnableWindow(TRUE);
	}
	else if (message == WM_USER + 12) {
		//. timeout
		g_pVmMgr->stopVM();
		m_bSessionOut = TRUE;
		OnBnClickedCancel();
	}

	return ClaDialogPng::WindowProc(message, wParam, lParam);
}

void ClauncherDlg::OnBnClickedCancel()
{
	if (lv_thdDownload != NULL) {
		if (AfxMessageBox(L"Downloading VMware image now. Do you want to stop download?", MB_OKCANCEL) == IDOK) {
			TerminateThread(lv_thdDownload, 0);
		}
		else {
			return;
		}
	}

	if (g_pVmMgr != NULL && g_pVmMgr->isRunning()) {
		int n = AfxMessageBox(L"VMware is running and you need to close vmware first.\nClick [Try again] to stop, [Continue] to suspend.", MB_CANCELTRYCONTINUE);
		if (n == IDTRYAGAIN) {
			g_pVmMgr->stopVM();
			TerminateThread(lv_thdEnv, 0);
			lv_thdEnv = NULL;
		}
		else if (n == IDCONTINUE) {
			g_pVmMgr->suspendVM();
			TerminateThread(lv_thdEnv, 0);
			lv_thdEnv = NULL;
		}
		else {
			return;
		}
	}
	CDialog::OnCancel();
}


void ClauncherDlg::OnBnClickedBtnMin()
{
	ShowWindow(SW_MINIMIZE);
}


void ClauncherDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0) {
		KillTimer(0);
		if (m_policy.session_expircy_time != 0 && (!(lv_thdEnv || lv_thdDownload || _bStarting))) {
			DWORD dw = (GetTickCount() - _lLastTime) / 1000;
			CString strOutput; strOutput.Format(L"%d - \n", dw); OutputDebugString(strOutput);
			if (dw > m_policy.session_expircy_time) {
				PostMessage(WM_USER + 12);
				KillTimer(0);
				_bTimer = FALSE;
			}
		}
		else {
			_lLastTime = GetTickCount();
		}
		SetTimer(0, 1000, NULL);

		int qr_state;
		std::string strConn = kc_get_status(&qr_state);
		if (strConn == "connected") {
			GetDlgItem(IDC_STC_DISCON)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STC_CONN)->ShowWindow(SW_SHOW);
		}
		else {
			GetDlgItem(IDC_STC_DISCON)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STC_CONN)->ShowWindow(SW_HIDE);
		}
	}

	ClaDialogPng::OnTimer(nIDEvent);
}


void ClauncherDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	ClaDialogPng::OnMouseMove(nFlags, point);
}


void ClauncherDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	_lLastTime = GetTickCount();

	ClaDialogPng::OnNcMouseMove(nHitTest, point);
}
