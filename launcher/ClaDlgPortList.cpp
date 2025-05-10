// ClaDlgPortList.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlgPortList.h"


// ClaDlgPortList dialog

IMPLEMENT_DYNAMIC(ClaDlgPortList, ClaDialogPng)

ClaDlgPortList::ClaDlgPortList(CWnd* pParent /*=nullptr*/)
	: ClaDialogPng(IDD_DLG_PORTLIST, pParent)
{

}

ClaDlgPortList::~ClaDlgPortList()
{
}

void ClaDlgPortList::DoDataExchange(CDataExchange* pDX)
{
	ClaDialogPng::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_PORT, m_lstPortList);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BTN_MIN, m_btnMin);
	DDX_Control(pDX, IDC_STC_ICON, m_stcLogo);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_GRP_BG, m_grpBG);
	DDX_Control(pDX, IDC_STC_SUBTITLE, m_stcSubTitle);
}


BEGIN_MESSAGE_MAP(ClaDlgPortList, ClaDialogPng)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &ClaDlgPortList::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDCANCEL, &ClaDlgPortList::OnBnClickedCancel)
END_MESSAGE_MAP()


// ClaDlgPortList message handlers


BOOL ClaDlgPortList::OnInitDialog()
{
	ClaDialogPng::OnInitDialog();

	m_lstPortList.SetFontSize(GD_FONT_SIZE_TEXT, 0);
	m_lstPortList.SetHeaderFontHW(GD_FONT_SIZE_TEXT, 0);
	m_lstPortList.SetRowHeigt(36);
	m_lstPortList.SetHeaderHeight(1.5);
	m_lstPortList.SetHeaderBackColorC(GD_COLOR_DLG_BG, 2);
	m_lstPortList.SetBgColor(GD_COLOR_LIST1, GD_COLOR_LIST2);

	m_lstPortList.SetExtendedStyle(m_lstPortList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lstPortList.setHead(L"No;Title;VM Port;Remote Address;Remote Port");
	m_lstPortList.autoFitWidth();

	m_lstPortList.setColor(GD_COLOR_LIST1, GD_COLOR_LIST2);
	m_lstPortList.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE);

	CRect rt;
	GetDlgItem(IDCANCEL)->GetClientRect(rt);
	setTitleHeight(rt.Height());

	m_grpBG.SetBackgroundColor(GD_COLOR_GROUP_BG, GD_COLOR_GROUP_BG);

	m_stcLogo.LoadPng(IDB_PNG_LOGO);

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

	// TODO:  Add extra initialization here
	OnBnClickedBtnRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ClaDlgPortList::OnBnClickedBtnRefresh()
{
	m_lstPortList.DeleteAllItems();

	for (int i = 0; i < m_group._lstPort.GetCount(); i++) {
		CString strNo; strNo.Format(L"%d", i + 1);
		CString m_strTitle; m_strTitle.Format(L"%S", m_group._lstPort[i]._szTitle);
		CString m_strLocalPort; m_strLocalPort.Format(L"%d", m_group._lstPort[i]._nSourcePort);
		CString m_strRemoteAddr; m_strRemoteAddr.Format(L"%S", m_group._lstPort[i]._szTarget);
		CString m_strRemotePort; m_strRemotePort.Format(L"%d", m_group._lstPort[i]._nTargetPort);

		m_lstPortList.addRecord(strNo, m_strTitle, m_strLocalPort, m_strRemoteAddr, m_strRemotePort);
	}
	m_lstPortList.autoFitWidth();
}


void ClaDlgPortList::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	ClaDialogPng::OnCancel();
}
