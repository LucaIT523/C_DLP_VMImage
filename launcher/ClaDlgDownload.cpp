// ClaDlgDownload.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlgDownload.h"
#include "SessionManager.h"

#include "ZipUtil.h"
#include "ClaPathMgr.h"
#include "KC_common.h"
#include "ClaRegMgr.h"

// ClaDlgDownload dialog

IMPLEMENT_DYNAMIC(ClaDlgDownload, CDialog)

ClaDlgDownload::ClaDlgDownload(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_DOWNLOAD, pParent)
{

}

ClaDlgDownload::~ClaDlgDownload()
{
}

void ClaDlgDownload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDT_NAME, m_strTitle);
	DDX_Text(pDX, IDC_STC_P, m_strPercent);
	DDX_Control(pDX, IDC_PGS, m_pgsDownload);

}


BEGIN_MESSAGE_MAP(ClaDlgDownload, CDialog)
	ON_BN_CLICKED(IDCANCEL, &ClaDlgDownload::OnBnClickedCancel)
END_MESSAGE_MAP()


// ClaDlgDownload message handlers
UINT TF_DOWN_UNZIP(void* p) {
	ClaDlgDownload* pThis = (ClaDlgDownload*)p;

	pThis->download_unzip();

	return 0;
}

ClaDlgDownload* lv_pThis = NULL;
HANDLE lv_hThreadDownload;

BOOL ClaDlgDownload::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = theApp.LoadIconW(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	{
		ClaRegMgr reg(HKEY_LOCAL_MACHINE);
		wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
		reg.readStringW(
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\WinRAR.exe", 
			L"", wszPath, MAX_PATH);
		if (wszPath[0] == 0 || GetFileAttributes(wszPath) == INVALID_FILE_ATTRIBUTES) {
			AfxMessageBox(L"You need to install the winrar program first.");
			EndDialog(IDCANCEL);
			return TRUE;
		}
	}


	m_pgsDownload.SetRange(0, 1000);

	lv_pThis = this;
	DWORD dwTID = 0;
	lv_hThreadDownload = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_DOWN_UNZIP, this, 0, &dwTID);

	m_strTitle = m_strName;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CALLBACK_DOWN(long long lCur, long long lTotal)
{
	int d = lCur * 1000 / lTotal;
	int p = (lCur * 1000000 / lTotal) % 1000;
	lv_pThis->SendMessage(WM_USER + 1, d, p);
	return TRUE;
}

int ClaDlgDownload::_execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam)
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

void ClaDlgDownload::download_unzip() {
	CreateDirectory(ClaPathMgr::GetDP() + L"\\VMs", NULL);

	wchar_t wszDown[MAX_PATH]; memset(wszDown, 0, sizeof(wszDown));
	swprintf_s(wszDown, MAX_PATH, L"%s\\VMs\\%s", ClaPathMgr::GetDP(), m_strName);
	
	if (1 && kc_down_file(m_nID, wszDown, (KC_CALLBACK_DOWN_VM)CALLBACK_DOWN) != 0) {
		lv_pThis->SendMessage(WM_USER + 4, 0, 0);
		return;
	}

	char szZipped[MAX_PATH]; sprintf_s(szZipped, MAX_PATH, "%S", wszDown);
	char szUnzipped[MAX_PATH]; sprintf_s(szUnzipped, MAX_PATH, "%S.e", wszDown);

	lv_pThis->SendMessage(WM_USER + 2, 0, 0);

	CreateDirectoryA(szUnzipped, NULL);

	wchar_t wszCmd[512]; memset(wszCmd, 0, sizeof(wszCmd));
	swprintf_s(wszCmd, 512, L"x \"%s\" \"%s.e\"", wszDown, wszDown);
	_execute(L"winrar", wszCmd);
	//if (MyUnzipData(szZipped, szUnzipped) != 0) {
	//	lv_pThis->SendMessage(WM_USER + 4, 0, 0);
	//	return;
	//}

	lv_pThis->SendMessage(WM_USER + 3, 0, 0);
}

LRESULT ClaDlgDownload::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER + 1) {
		m_pgsDownload.SetPos(wParam);
		m_strPercent.Format(L"%d.%d %%", wParam / 10, wParam%10);
		UpdateData(FALSE);
	}
	if (message == WM_USER + 2) {
		m_strPercent = L"Extract ...";
		UpdateData(FALSE);
	}
	if (message == WM_USER + 3) {
		OnOK();
	}
	if (message == WM_USER + 4) {
		lv_hThreadDownload = NULL;
		AfxMessageBox(L"Failed to download VMware image or the image downloaded is invalid");
		OnCancel();
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void ClaDlgDownload::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if (lv_hThreadDownload != NULL) TerminateThread(lv_hThreadDownload, 0);
	CDialog::OnCancel();
}
