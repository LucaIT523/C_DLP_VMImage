// ClaDlgConfig.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlgConfig.h"


// ClaDlgConfig dialog

IMPLEMENT_DYNAMIC(ClaDlgConfig, ClaDialogPng)

ClaDlgConfig::ClaDlgConfig(CWnd* pParent /*=nullptr*/)
	: ClaDialogPng(IDD_DLG_CONFIG, pParent)
	, m_strServer(_T(""))
	, m_strPort(_T(""))
{

}

ClaDlgConfig::~ClaDlgConfig()
{
}

void ClaDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	ClaDialogPng::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_SERVER, m_strServer);
	DDX_Text(pDX, IDC_EDT_PORT, m_strPort);
	DDX_Control(pDX, IDC_STC_ICON, m_grpLogo);
	DDX_Control(pDX, IDC_GRP_IP, m_grpIP);
	DDX_Control(pDX, IDC_STC_IP, m_stcIP);
	DDX_Control(pDX, IDC_GRP_PORT, m_grpPort);
	DDX_Control(pDX, IDC_STC_PORT, m_stcPort);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STC_GRP_BG, m_grpBG);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_SUBTITLE2, m_stcSubTitle);
	DDX_Control(pDX, IDC_EDT_SERVER, m_edtServer);
	DDX_Control(pDX, IDC_EDT_PORT, m_edtPort);
	DDX_Control(pDX, IDC_BTN_MIN, m_btnMin);
}


BEGIN_MESSAGE_MAP(ClaDlgConfig, ClaDialogPng)
	ON_BN_CLICKED(IDOK, &ClaDlgConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MIN, &ClaDlgConfig::OnBnClickedBtnMin)
END_MESSAGE_MAP()


// ClaDlgConfig message handlers


BOOL ClaDlgConfig::OnInitDialog()
{
	ClaDialogPng::OnInitDialog();

	HICON hIcon = theApp.LoadIconW(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	CRect rt;
	GetDlgItem(IDCANCEL)->GetClientRect(rt);
	setTitleHeight(rt.Height());

	m_grpBG.SetBackgroundColor(GD_COLOR_GROUP_BG, GD_COLOR_GROUP_BG);

	m_grpLogo.LoadPng(IDB_PNG_LOGO);

	m_stcTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, TRUE, 0);
	m_stcTitle.SetFontColor(GD_COLOR_LABEL);

	m_stcSubTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TITLE, TRUE, 1);
	m_stcSubTitle.SetFontColor(GD_COLOR_LABEL);

	m_grpIP.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcIP.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_stcIP.SetFontColor(GD_COLOR_LABEL);
	m_edtServer.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT, FALSE, 1);


	m_grpPort.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcPort.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_stcPort.SetFontColor(GD_COLOR_LABEL);
	m_edtPort.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT, FALSE, 1);

	m_btnOK.setColor(GD_COLOR_BTN_BG, GD_COLOR_BTN_BG, GD_COLOR_BTN_TXT);
	m_btnOK.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);

	m_btnCancel.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnCancel.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);
	m_btnCancel.setBgImage(IDB_PNG_EXIT);

	m_btnMin.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnMin.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);
	m_btnMin.setBgImage(IDB_PNG_MIN);

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
	ClaDialogPng::OnOK();
}


void ClaDlgConfig::OnBnClickedBtnMin()
{
	ShowWindow(SW_MINIMIZE);
}
