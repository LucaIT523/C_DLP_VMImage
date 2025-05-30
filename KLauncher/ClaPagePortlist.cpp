// ClaPagePortlist.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "afxdialogex.h"
#include "ClaPagePortlist.h"

#define WM_USER_MENU_CLICKED		(WM_USER+0x0001)
// ClaPagePortlist dialog

ClaPagePortlist::ClaPagePortlist(CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_PAGE_PORT, 0, L"PNG", 0, pParent, FALSE)
{

}

ClaPagePortlist::~ClaPagePortlist()
{
}

void ClaPagePortlist::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_LST_PORT, m_lstPort);
	DDX_Control(pDX, IDC_STC_TITLE3, m_stcTitle3);
	DDX_Control(pDX, IDC_BTN_OK, m_btnOK);
	DDX_Control(pDX, IDC_BTN_COMBO, m_cmbMenu);
}


BEGIN_MESSAGE_MAP(ClaPagePortlist, KDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &ClaPagePortlist::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// ClaPagePortlist message handlers


BOOL ClaPagePortlist::OnInitDialog()
{
	KDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ClaPagePortlist::OnInitChildrenWnds() {
	m_stcTitle.LoadImageK(IDB_PNG_PORT_TITLE);
	RegisterTranslucentWnd(&m_stcTitle);

	m_stcTitle3.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255));
	m_stcTitle3.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle3);

	m_lstPort.LoadImageList(IDB_PNG_LIST_HEAD, IDB_PNG_LIST_ROW1, IDB_PNG_LIST_ROW2, IDB_PNG_LIST_ROW_SEL, IDB_PNG_LIST_SCROLL);
	m_lstPort.setRowConfig(L"Arial", 12, FALSE, RGB(255, 255, 255), 60);
	m_lstPort.setHeadConfig(L"Arial", 10, TRUE, RGB(255, 255, 255));
	m_lstPort.setHead(L"ID;Title;VM Port;Remote Address;Remote Port");
	m_lstPort.setHeadWidth(90, (int)180, (int)120, (int)200, (int)160);
	RegisterTranslucentWnd(&m_lstPort);

	UINT nImgIdLogin[TWS_BUTTON_NUM] = { IDB_PNG_BTN_HALF_B, IDB_PNG_BTN_HALF_BH, IDB_PNG_BTN_HALF_BH, 0 };
	m_btnOK.LoadImageList(nImgIdLogin);
	m_btnOK.setFont(L"Arial", 12, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnOK.setAlign(1);
	RegisterTranslucentWnd(&m_btnOK);
	UINT nImgCombo[TWS_COMBO_NUM] = { IDB_PNG_COMBO_NORMAL, IDB_PNG_COMBO_ACTIVE, IDB_PNG_COMBO_FIRST, IDB_PNG_COMBO_FIRST_H, IDB_PNG_COMBO_LAST, IDB_PNG_COMBO_LAST_H, IDB_PNG_COMBO_MIDDLE, IDB_PNG_COMBO_MIDDLE_H };
	m_cmbMenu.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255), 20);
	m_cmbMenu.LoadImageList(nImgCombo);
	m_cmbMenu.addItem(L"Home");
	m_cmbMenu.addItem(L"About Me");
	m_cmbMenu.addItem(L"ABCDEFGHIJKLM");
	m_cmbMenu.setCurSel(0);
	m_cmbMenu.setMsgID(WM_USER_MENU_CLICKED);
	RegisterTranslucentWnd(&m_cmbMenu);

}


void ClaPagePortlist::OnBnClickedBtnOk()
{
	g_pMain->ShowMainPage(FALSE);
}

void ClaPagePortlist::OnShowPage()
{
	while (m_lstPort.GetItemCount()) {
		m_lstPort.DeleteItem(0);
	}

	for (int i = 0; i < _group._lstPort.GetCount(); i++) {
		CString strID; strID.Format(L"%02d", i + 1);
		CString title; title.Format(L"%S", _group._lstPort[i]._szTitle);
		CString port; port.Format(L"%d", _group._lstPort[i]._nSourcePort);
		CString target; target.Format(L"%S", _group._lstPort[i]._szTarget);
		CString target_port; target_port.Format(L"%d", _group._lstPort[i]._nTargetPort);
		m_lstPort.addRecord(strID, title, port, target, target_port);
	}

	m_cmbMenu.setCurSel(0);
}

LRESULT ClaPagePortlist::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER_MENU_CLICKED) {
		int p_nCurSel = m_cmbMenu.getCurSel();
		if (p_nCurSel == 1) {
			g_pMain->ShowAboutPage();
		}
	}
	return KDialog::WindowProc(message, wParam, lParam);
}
