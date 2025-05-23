// ClaPageConfig.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "afxdialogex.h"
#include "ClaPageConfig.h"
#include "KGlobal.h"

// ClaPageConfig dialog

ClaPageConfig::ClaPageConfig(CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_PAGE_CONFIG, 0, L"PNG", 0, pParent, FALSE)
	, m_strIP(_T(""))
	, m_strPort(_T(""))
{

}

ClaPageConfig::~ClaPageConfig()
{
}

void ClaPageConfig::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_LOGO, m_stcLogo);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_TITLE2, m_stcTitle2);
	DDX_Control(pDX, IDC_STC_TITLE3, m_stcTitle3);
	DDX_Control(pDX, IDC_STC_PANEL, m_stcPanel);
	DDX_Control(pDX, IDC_EDT_IP, m_edtIp);
	DDX_Control(pDX, IDC_EDT_PORT, m_edtPort);
	DDX_Control(pDX, IDCANCEL, m_btnBack);
	DDX_Control(pDX, IDC_BTN_SAVE, m_btnSave);
	DDX_Text(pDX, IDC_EDT_IP, m_strIP);
	DDX_Text(pDX, IDC_EDT_PORT, m_strPort);
}


BEGIN_MESSAGE_MAP(ClaPageConfig, KDialog)
	ON_BN_CLICKED(IDCANCEL, &ClaPageConfig::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_SAVE, &ClaPageConfig::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// ClaPageConfig message handlers


BOOL ClaPageConfig::OnInitDialog()
{
	KDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ClaPageConfig::OnInitChildrenWnds() {
	m_stcLogo.LoadImageK(IDB_PNG_LOGO);
	RegisterTranslucentWnd(&m_stcLogo);

	m_stcTitle.setFont(L"Arial", 24, TRUE, RGB(255, 255, 255));
	m_stcTitle.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle);

	m_stcTitle2.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255));
	m_stcTitle2.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle2);

	m_stcTitle3.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255));
	m_stcTitle3.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle3);

	m_stcPanel.LoadImageK(IDB_PNG_PANEL_CONFIG);
	RegisterTranslucentWnd(&m_stcPanel);

	UINT nImgIdEdit[2] = { IDB_PNG_EDT_UNSEL, IDB_PNG_EDT_SEL };

	m_edtIp.LoadImageK(nImgIdEdit);
	m_edtIp.setFont(L"Arial", 10, FALSE, RGB(0xA4, 0xA4, 0xA4), RGB(0xFF, 0xFF, 0xFF));
	m_edtIp.setAlign(0);
	m_edtIp.SetIconImage(IDB_PNG_ICON_IP);
	RegisterTranslucentWnd(&m_edtIp);

	m_edtPort.LoadImageK(nImgIdEdit);
	m_edtPort.setFont(L"Arial", 10, FALSE, RGB(0xA4, 0xA4, 0xA4), RGB(0xFF, 0xFF, 0xFF));
	m_edtPort.setAlign(0);
	m_edtPort.SetIconImage(IDB_PNG_ICON_PORT);
	RegisterTranslucentWnd(&m_edtPort);

	UINT nImgIdBack[TWS_BUTTON_NUM] = { IDB_PNG_BTN_HALF_R, IDB_PNG_BTN_HALF_RH, IDB_PNG_BTN_HALF_RH, 0 };
	m_btnBack.LoadImageList(nImgIdBack);
	m_btnBack.setFont(L"Arial", 12, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnBack.setAlign(1);
	RegisterTranslucentWnd(&m_btnBack);

	UINT nImgIdSave[TWS_BUTTON_NUM] = { IDB_PNG_BTN_HALF_B, IDB_PNG_BTN_HALF_BH, IDB_PNG_BTN_HALF_BH, 0 };
	m_btnSave.LoadImageList(nImgIdSave);
	m_btnSave.setFont(L"Arial", 12, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnSave.setAlign(1);
	RegisterTranslucentWnd(&m_btnSave);

	UpdateView();
}


void ClaPageConfig::OnBnClickedBtnBack()
{
	g_pMain->ShowLoginPage();
}


void ClaPageConfig::OnBnClickedBtnSave()
{
	UpdateData(TRUE);

	if (m_strIP.IsEmpty()) {
		KMessageBox(L"Please input the server ip.");
		m_edtIp.SetFocus();
		return;
	}

	if (m_strPort.IsEmpty()) {
		KMessageBox(L"Please input the port.");
		m_edtPort.SetFocus();
		return;
	}

	if (_wtoi(m_strPort) == 0) {
		KMessageBox(L"Please input the correct port.");
		m_edtPort.SetFocus();
		return;
	}

	KGlobal::writeServerIP(m_strIP, m_strPort);
	theApp.relaunchClient(FALSE);
	g_pMain->ShowLoginPage();
}

void ClaPageConfig::OnShowPage()
{
	wchar_t wszIP[64]; wchar_t wszPort[10];

	KGlobal::readServerIP(wszIP, 64, wszPort, 10, NULL);

	m_strIP = wszIP; m_strPort = wszPort;

	UpdateData(FALSE);
}
