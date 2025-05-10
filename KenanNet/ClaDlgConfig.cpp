// ClaDlgConfig.cpp : implementation file
//

#include "pch.h"
#include "KenanNet.h"
#include "afxdialogex.h"
#include "ClaDlgConfig.h"


// ClaDlgConfig dialog

IMPLEMENT_DYNAMIC(ClaDlgConfig, CDialog)

ClaDlgConfig::ClaDlgConfig(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_CONFIG, pParent)
	, m_strServer(_T(""))
	, m_strPort(_T(""))
{

}

ClaDlgConfig::~ClaDlgConfig()
{
}

void ClaDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_SERVER, m_strServer);
	DDX_Text(pDX, IDC_EDT_PORT, m_strPort);
}


BEGIN_MESSAGE_MAP(ClaDlgConfig, CDialog)
	ON_BN_CLICKED(IDOK, &ClaDlgConfig::OnBnClickedOk)
END_MESSAGE_MAP()


// ClaDlgConfig message handlers


BOOL ClaDlgConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ClaDlgConfig::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_strServer.IsEmpty()) {
		AfxMessageBox(L"Please input server address.");
		GetDlgItem(IDC_EDT_SERVER)->SetFocus();
		return;
	}
	if (m_strPort.IsEmpty()) {
		AfxMessageBox(L"Please input port.");
		GetDlgItem(IDC_EDT_PORT)->SetFocus();
		return;
	}

	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
