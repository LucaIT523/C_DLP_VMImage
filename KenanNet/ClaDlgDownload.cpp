// ClaDlgDownload.cpp : implementation file
//

#include "pch.h"
#include "KenanNet.h"
#include "afxdialogex.h"
#include "ClaDlgDownload.h"
#include "SessionManager.h"

#include "ServerConn.h"
#include "ZipUtil.h"
#include "ClaPathMgr.h"


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

BOOL ClaDlgDownload::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pgsDownload.SetRange(0, 1000);

	lv_pThis = this;
	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_DOWN_UNZIP, this, 0, &dwTID);

	m_strTitle = m_strName;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CALLBACK_DOWN(long long lCur, long long lTotal)
{
	lv_pThis->SendMessage(WM_USER + 1, lCur, lTotal);
}

void ClaDlgDownload::download_unzip() {
	SessionManager net;

	net.loadToken();
	char szToken[200]; memset(szToken, 0, sizeof(szToken));
	net.getToken(szToken);

	char szName[200]; memset(szName, 0, sizeof(szName));
	sprintf_s(szName, 200, "%S", m_strName);

	downloadFile(szToken, szName, CALLBACK_DOWN);

	char szZipped[MAX_PATH]; sprintf_s(szZipped, MAX_PATH, "%S\\downloaded_%s", ClaPathMgr::GetDP(), szName);
	char szUnzipped[MAX_PATH]; sprintf_s(szUnzipped, MAX_PATH, "%S\\%s.e\\", ClaPathMgr::GetDP(), szName);

	lv_pThis->SendMessage(WM_USER + 2, 0, 0);

	CreateDirectoryA(szUnzipped, NULL);
	MyUnzipData(szZipped, szUnzipped);

	lv_pThis->SendMessage(WM_USER + 3, 0, 0);
}

LRESULT ClaDlgDownload::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER + 1) {
		m_pgsDownload.SetPos(wParam * 1000 / lParam);
		m_strPercent.Format(L"%d.%03d %%", (int)(wParam * 100 / lParam), ((int)(wParam * 100000 / lParam) % 1000));
		UpdateData(FALSE);
	}
	if (message == WM_USER + 2) {
		m_strPercent = L"Extract ...";
		UpdateData(FALSE);
	}
	if (message == WM_USER + 3) {
		OnOK();
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void ClaDlgDownload::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
