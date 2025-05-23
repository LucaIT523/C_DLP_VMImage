// ClaPageMain.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "afxdialogex.h"
#include "ClaPageMain.h"
#include "ClaPathMgr.h"
#include "mfcCmn.h"
#include "KGlobal.h"
#include "json.hpp"
#include "base64enc.h"
#include "ClaRegMgr.h"
#include "ClaDlgTimeout.h"
#include "ClaOsInfo.h"
#include "CaptureVM.h"
#include "kncrypto.h"
#include "ClaProcess.h"
#include <codecvt>

#define WM_USER_LIST_ITEM_SEL		(WM_USER+0x0001)
#define WM_USER_DOWNLOAD_UPDATE		(WM_USER+0x0002)
#define WM_USER_DOWNLOAD_FAILED		(WM_USER+0x0003)
#define WM_USER_DOWNLOAD_COMPLETE	(WM_USER+0x0004)
#define WM_USER_DOWNLOAD_UNZIPPING	(WM_USER+0x0005)
#define WM_USER_VM_USER_STOP		(WM_USER+0x0006)
#define WM_USER_VM_AGENT_NOT_FOUND	(WM_USER+0x0007)
#define WM_USER_VM_IDLE_TIMEOUT		(WM_USER+0x0008)
#define WM_USER_KCLIENT_TERMINATED	(WM_USER+0x0009)
#define WM_USER_VIOLATION			(WM_USER+0x000A)
#define WM_USER_UPDATE_READY		(WM_USER+0x000B)
#define WM_USER_MENU_CLICKED		(WM_USER+0x000C)
#define WM_USER_QR_INVALID			(WM_USER+0x000D)
#define WM_USER_DOWNLOAD_CANCELED	(WM_USER+0x000E)
#define CURRENT_VERSION				"1.0.0.4"

//#define DEBUG_NO_LAUNCH_VM

using json = nlohmann::json;

ClaPageMain* lv_pMainDialog = NULL;
HANDLE lv_thdDownload = NULL;
HANDLE lv_thdEnvironment = NULL;
HANDLE lv_thdPolicyUpdate = NULL;
HANDLE lv_thdUpdateVersion = NULL;
CString lv_strAgentIP;
std::wstring lv_updatePath = L"";
ClaKcVersions* lv_updateVersionDown = NULL;
std::string lv_currentAgentVersion = "";
UINT TF_CHECK_UPDATE(void* p);
BOOL lv_isUpdating = FALSE;
CString lv_key_vm;
std::string lv_vm_os = "";
std::string lv_vm_mac = "";
BOOL lv_bQrFinished = FALSE;
typedef int(* FN_REC_QR_INFO)(char* p_pEngfolderPath, char* p_pPNGFilePath, char* p_pValue);
std::string _getQrText();

typedef enum _ENUM_STOP_VM_REASON {
	stop_vm_reason_none = 0,
	stop_vm_reason_user_control,
	stop_vm_reason_fail_launch,
	stop_vm_reason_fail_qr,
	stop_vm_reason_logout,
	stop_vm_reason_client_update,
	stop_vm_reason_client_force_stop,
	stop_vm_reason_agent_not_found,
	stop_vm_reason_expire_idle_vm,
	stop_vm_reason_kclient_terminated,
	stop_vm_reason_user_close_vm,
	stop_vm_reason_violation_logout,
	stop_vm_reason_violation_delete,
}ENUM_STOP_VM_REASON;
ENUM_STOP_VM_REASON lv_stop_reason = stop_vm_reason_none;

// ClaPageMain dialog

ClaPageMain::ClaPageMain(CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_PAGE_MAIN, 0, L"PNG", 0, pParent, FALSE)
	, m_strInfo1(_T(""))
	, m_strInfo2(_T(""))
	, m_strPath(_T(""))
	, m_strInfo3(_T(""))
	, m_bStarting(FALSE)
	, m_strPercent(_T(""))
	, m_strSpeed(_T(""))
	, m_strDownName(_T(""))
	, m_strRate(_T(""))
{

}

ClaPageMain::~ClaPageMain()
{
}

void ClaPageMain::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_CONNECT, m_stcConnected);
	DDX_Control(pDX, IDC_LST_GROUP, m_lstGroup);
	DDX_Control(pDX, IDC_STC_PANEL, m_grpPanel);
	DDX_Control(pDX, IDC_STC_TITLE3, m_stcTitle3);
	DDX_Control(pDX, IDC_STC_INFO1, m_stcInfo1);
	DDX_Control(pDX, IDC_STC_INFO2, m_stcInfo2);
	DDX_Control(pDX, IDC_EDT_PATH, m_edtPath);
	DDX_Control(pDX, IDC_STC_INFO3, m_stcInfo3);
	DDX_Control(pDX, IDC_BTN_REFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_PGS_PERCENT, m_pgsPercent);
	DDX_Control(pDX, IDC_STC_PERCENT, m_stcPercent);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPause);
	DDX_Control(pDX, IDC_BTN_DOWNLOAD, m_btnDownload);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_BTN_POLICY, m_btnPolicy);
	DDX_Text(pDX, IDC_STC_INFO1, m_strInfo1);
	DDX_Text(pDX, IDC_STC_INFO2, m_strInfo2);
	DDX_Text(pDX, IDC_EDT_PATH, m_strPath);
	DDX_Text(pDX, IDC_STC_INFO3, m_strInfo3);
	DDX_Text(pDX, IDC_STC_PERCENT, m_strPercent);
	DDX_Control(pDX, IDC_STC_SPEED, m_stcSpeed);
	DDX_Text(pDX, IDC_STC_SPEED, m_strSpeed);
	DDX_Control(pDX, IDC_STATIC_NET_BG, m_netSpeedBg);
	DDX_Control(pDX, IDC_BTN_COMBO, m_cmbMenu);
	DDX_Control(pDX, IDC_STC_DOWN_NAME, m_stcDownName);
	DDX_Text(pDX, IDC_STC_DOWN_NAME, m_strDownName);
	DDX_Text(pDX, IDC_STC_RATE, m_strRate);
	DDX_Control(pDX, IDC_STC_RATE, m_stcRate);
	DDX_Control(pDX, IDC_STC_POPUP, m_stcPopup);
}


BEGIN_MESSAGE_MAP(ClaPageMain, KDialog)
	ON_BN_CLICKED(IDC_BTN_POLICY, &ClaPageMain::OnBnClickedBtnPolicy)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &ClaPageMain::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_START, &ClaPageMain::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_DOWNLOAD, &ClaPageMain::OnBnClickedBtnDownload)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_PAUSE, &ClaPageMain::OnBnClickedBtnPause)
END_MESSAGE_MAP()


// ClaPageMain message handlers


BOOL ClaPageMain::OnInitDialog()
{
	KDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	lv_pMainDialog = this;
	g_pVmMgr->initialize(m_hWnd);

	SetTimer(0, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ClaPageMain::OnInitChildrenWnds() {
	m_stcTitle.LoadImageK(IDB_PNG_TITLE);
	RegisterTranslucentWnd(&m_stcTitle);

	m_stcConnected.LoadImageK(IDB_PNG_STC_CONNECTED);
	RegisterTranslucentWnd(&m_stcConnected);

	m_stcTitle3.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255));
	m_stcTitle3.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle3);

	m_stcDownName.setFont(L"Arial", 10, FALSE, RGB(255, 255, 255));
	m_stcDownName.setAlign(0);
	RegisterTranslucentWnd(&m_stcDownName);

	m_grpPanel.LoadImageK(IDB_PNG_PANEL_MAIN);
	RegisterTranslucentWnd(&m_grpPanel);

	m_stcInfo1.setFont(L"Arial", 10, FALSE, RGB(255, 255, 255), 20);
	m_stcInfo1.setAlign(0, 0);
	RegisterTranslucentWnd(&m_stcInfo1);

	m_stcInfo2.setFont(L"Arial", 10, FALSE, RGB(255, 255, 255), 20);
	m_stcInfo2.setAlign(0, 0);
	RegisterTranslucentWnd(&m_stcInfo2);

	UINT nImgIdEdit[2] = { IDB_PNG_EDT_UNSEL, IDB_PNG_EDT_SEL };
	m_edtPath.LoadImageK(nImgIdEdit);
	m_edtPath.setFont(L"Arial", 10, FALSE, RGB(0xA4, 0xA4, 0xA4), RGB(0xFF, 0xFF, 0xFF));
	m_edtPath.setAlign(0);
	((CEdit*)&m_edtPath)->SetReadOnly(TRUE);
	RegisterTranslucentWnd(&m_edtPath);

	m_stcInfo3.setFont(L"Arial", 10, FALSE, RGB(255, 255, 255), 20);
	m_stcInfo3.setAlign(0, 0);
	RegisterTranslucentWnd(&m_stcInfo3);

	UINT nImgIdRefresh[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_REFRESH, IDB_PNG_BTN_ICON_REFRESH_H, 0, 0 };
	m_btnRefresh.LoadImageList(nImgIdRefresh);
	RegisterTranslucentWnd(&m_btnRefresh);

	UINT nImgIdStart[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_START, IDB_PNG_BTN_ICON_START_H, 0, 0 };
	m_btnStart.LoadImageList(nImgIdStart);
	RegisterTranslucentWnd(&m_btnStart);

	UINT nImgIdPause[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_PAUSE, IDB_PNG_BTN_ICON_PAUSE_H, 0, 0 };
	m_btnPause.LoadImageList(nImgIdPause);
	RegisterTranslucentWnd(&m_btnPause);

	UINT nImgIdDown[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_DOWN, IDB_PNG_BTN_ICON_DOWN_H, 0, 0 };
	m_btnDownload.LoadImageList(nImgIdDown);
	RegisterTranslucentWnd(&m_btnDownload);

	UINT nImgIdPolicy[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_POLICY, IDB_PNG_BTN_ICON_POLICY_H, 0, 0 };
	m_btnPolicy.LoadImageList(nImgIdPolicy);
	RegisterTranslucentWnd(&m_btnPolicy);

	m_pgsPercent.SetImageBackground(IDB_PNG_PGS_BACK);
	m_pgsPercent.SetImageFilled(IDB_PNG_PGS_FILL);
	RegisterTranslucentWnd(&m_pgsPercent);
	m_pgsPercent.setRange(0, 100);
	m_pgsPercent.setPosition(0);

	m_stcPercent.setFont(L"Arial", 10, FALSE, RGB(255, 255, 255), 20);
	m_stcPercent.setAlign(0, 0);
	RegisterTranslucentWnd(&m_stcPercent);

	m_lstGroup.LoadImageList(IDB_PNG_LIST_HEAD, IDB_PNG_LIST_ROW1, IDB_PNG_LIST_ROW2, IDB_PNG_LIST_ROW_SEL, IDB_PNG_LIST_SCROLL);
	m_lstGroup.setRowConfig(L"Arial", 12, FALSE, RGB(255, 255, 255), 60);
	m_lstGroup.setHeadConfig(L"Arial", 10, TRUE, RGB(255, 255, 255));
	m_lstGroup.setMessageID(WM_USER_LIST_ITEM_SEL);
	m_lstGroup.setHead(L"ID;Group Name;Image Name");
	m_lstGroup.setHeadWidth(90, (int)160, (int)120);
	RegisterTranslucentWnd(&m_lstGroup);

	m_stcSpeed.setFont(L"Arial", 7, FALSE, RGB(255, 255, 255));
	m_stcSpeed.setAlign(0);
	RegisterTranslucentWnd(&m_stcSpeed);

	m_stcRate.setFont(L"Arial", 7, FALSE, RGB(255, 255, 255));
	m_stcRate.setAlign(0);
	RegisterTranslucentWnd(&m_stcRate);

	m_netSpeedBg.LoadImageK(IDB_PNG_NET_STATUS_BG);
	RegisterTranslucentWnd(&m_netSpeedBg);

	UINT nImgCombo[TWS_COMBO_NUM] = { IDB_PNG_COMBO_NORMAL, IDB_PNG_COMBO_ACTIVE, IDB_PNG_COMBO_FIRST, IDB_PNG_COMBO_FIRST_H, IDB_PNG_COMBO_LAST, IDB_PNG_COMBO_LAST_H, IDB_PNG_COMBO_MIDDLE, IDB_PNG_COMBO_MIDDLE_H };
	m_cmbMenu.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255), 20);
	m_cmbMenu.LoadImageList(nImgCombo);
	m_cmbMenu.addItem(L"Home");
	m_cmbMenu.addItem(L"My Tickets (soon)");
	m_cmbMenu.addItem(L"Log out");
	m_cmbMenu.addItem(L"About Us");
	m_cmbMenu.setCurSel(0);
	m_cmbMenu.setMsgID(WM_USER_MENU_CLICKED);
	RegisterTranslucentWnd(&m_cmbMenu);

	m_stcPopup.LoadImageK(IDB_PNG_MENU, IDB_PNG_MENU_H);
	m_stcPopup.addItem(TRUE, L"aaaaaaa1", NULL, 0);
	m_stcPopup.addItem(TRUE, L"aaaaaaa2", NULL, 0);
	m_stcPopup.addItem(TRUE, L"aaaaaaa3", NULL, 0);
	m_stcPopup.addItem(TRUE, L"aaaaaaa4", NULL, 0);
	RegisterTranslucentWnd(&m_stcPopup);
}

void ClaPageMain::OnBnClickedBtnPolicy()
{
	//if (lv_thdDownload) {
	//	KMessageBox(L"Downloading VMware image now. Please wait");
	//	return;
	//}

	int nSel = m_lstGroup.getCurSel();
	if (nSel == -1) {
		KMessageBox(L"Please select the vmware item on left side list");
		return;
	}

	int i = m_lstGroup.GetItemData(nSel) >> 16;
	int j = m_lstGroup.GetItemData(nSel) & 0xFFFF;

	int gid = m_policy._lstGroup[i]._id;
	int iid = m_policy._lstGroup[i]._lstVmImage[j]._id;
	ClaKcGroup* pGroup;
	pGroup = kc_down_config(gid, iid);
	if (pGroup == NULL) {
		KMessageBox(L"Unable to connect to server.");
		return;
	}
	g_pMain->ShowPortPage(pGroup);
}

void ClaPageMain::OnShowPage()
{
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main start");

	_download_group_list();

	_show_group_list();

	_check_update_version();

	m_cmbMenu.setCurSel(0);
}

void ClaPageMain::_download_group_list()
{
	ClaKcPolicy* pRet = kc_down_list();

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main load = %d", pRet);

	if (pRet == 0) {
		KMessageBox(L"Unable to connect to server.");
		return;
	}

	m_policy = pRet[0];
}
int ClaPageMain::_download_update_version()
{
	CString downloadName;
	downloadName.Format(L"updates.zip");
	CreateDirectory(ClaPathMgr::GetDP() + L"\\updates", NULL);

	//CString wszDown;
	wchar_t wszDown[MAX_PATH]; memset(wszDown, 0, sizeof(wszDown));
	swprintf_s(wszDown, MAX_PATH, L"%s\\%s", (LPCWSTR)ClaPathMgr::GetDP(), (LPCWSTR)downloadName);
	if (1 && kc_down_update_file(lv_updateVersionDown->launcher->_nID, wszDown) != 0) {
		return -1;
	}

	wchar_t wszupdatePath[256]; memset(wszupdatePath, 0, sizeof(wszupdatePath));
	swprintf_s(wszupdatePath, 256, L"%s\\updates", (LPCWSTR)ClaPathMgr::GetDP());
	wchar_t wszCmd[512]; memset(wszCmd, 0, sizeof(wszCmd));
	swprintf_s(wszCmd, 512, L"e \"%s\" -o\"%s\" \"launcher\\*\" -y", wszDown, wszupdatePath);
	ClaPathMgr::UnzipFile(ClaPathMgr::GetDP(), wszCmd);
	//_execute(L"winrar", wszCmd);
	lv_updatePath = wszupdatePath;
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - download new Version Launcher");

	return SendMessage(WM_USER_UPDATE_READY, 0, 0);
}

int ClaPageMain::_download_agent_update()
{
	CString downloadName;
	downloadName.Format(L"updateAgent.zip");
	CreateDirectory(ClaPathMgr::GetDP() + L"\\updates", NULL);

	//CString wszDown;
	wchar_t wszDown[MAX_PATH]; memset(wszDown, 0, sizeof(wszDown));
	swprintf_s(wszDown, MAX_PATH, L"%s\\%s", (LPCWSTR)ClaPathMgr::GetDP(), (LPCWSTR)downloadName);
	if (1 && kc_down_update_file(lv_updateVersionDown->agent->_nID, wszDown) != 0) {
		return -1;
	}
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - download new Version Agent");
	lv_isUpdating = TRUE;
	return 0;
}

void ClaPageMain::_show_group_list()
{
	while (m_lstGroup.GetItemCount() > 0)
		m_lstGroup.DeleteItem(0);

	int count = 1;
	for (int j = 0; j < 1; j++) {
		for (int i = 0; i < m_policy._lstGroup.GetCount(); i++) {
			CString strGroupName; strGroupName.Format(L"%S", m_policy._lstGroup[i]._szName);
			for (int j = 0; j < m_policy._lstGroup[i]._lstVmImage.GetCount(); j++) {
				CString strID; strID.Format(L"%02d", count);
				CString strName; strName.Format(L"%S", m_policy._lstGroup[i]._lstVmImage[j]._szTitle);
				CString strDesc; strDesc.Format(L"%S", m_policy._lstGroup[i]._lstVmImage[j]._szDesc);
				CString strSize; strSize.Format(L"%lld", m_policy._lstGroup[i]._lstVmImage[j]._lSize);
				CString strStatus;
				int nIndex = m_lstGroup.addRecord(strID, strGroupName, strName);

				DWORD dwData = i << 16 | j;
				m_lstGroup.SetItemData(nIndex, (DWORD_PTR)dwData);
				count++;
			}
		}
	}

	m_strInfo1 = L"";
	m_strInfo2 = L"";
	m_strInfo3 = L"";
	m_strPath = L"";

	UpdateData(FALSE);
}

void ClaPageMain::_check_update_version()
{
	DWORD dwTID;
	lv_thdUpdateVersion = CreateThread(
		NULL, 0, (LPTHREAD_START_ROUTINE)TF_CHECK_UPDATE,
		this, 0, &dwTID
	);
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

void ClaPageMain::OnGroupListSelChanged(int p_nSel)
{
	if (p_nSel == -1) {
		m_strInfo1 = L"";
		m_strInfo2 = L"";
		m_strInfo3 = L"";
		m_strPath = L"";
		UpdateData(FALSE);
		return;
	}

	DWORD dwData = m_lstGroup.GetItemData(p_nSel);
	int i = dwData >> 16;
	int j = dwData & 0xFFFF;

	ClaKcVmImage* p = &(m_policy._lstGroup[i]._lstVmImage[j]);
	_nGroupID = m_policy._lstGroup[i]._id;
	_nImageID = m_policy._lstGroup[i]._lstVmImage[j]._id;

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main down policy");

	m_strInfo1.Format(L"ID: %02d\nName: %S\nGroup: %S\nDescription: %S",
		p_nSel + 1,
		m_policy._lstGroup[i]._lstVmImage[j]._szTitle,
		m_policy._lstGroup[i]._szName,
		m_policy._lstGroup[i]._lstVmImage[j]._szDesc
	);

	m_policy._lstGroup[i]._lstVmImage[j].findVMX();

	CString strStatus;
	if (g_pVmMgr->isRunning()) {
		strStatus = L"Running";
	}
	else if (wcslen(m_policy._lstGroup[i]._lstVmImage[j]._wszPath) == 0 ||
		GetFileAttributes(m_policy._lstGroup[i]._lstVmImage[j]._wszPath) == INVALID_FILE_ATTRIBUTES)
	{
		strStatus = L"No VMware image";
	}
	else {
		strStatus = L"Ready";
	}

	unsigned long long lSize = 0;
	ClaKcVmImage image = m_policy._lstGroup[i]._lstVmImage[j];
	kc_down_vmimage_size(image._szDropboxVmPath, image._szDropboxAppKey, image._szDropboxAppSecret, image._szDropboxAppRefresgToken, &lSize);

	CString strSize = getSize(lSize);

	m_strInfo2.Format(L"Operating System: %S\nSize: %s\nStatus: %s\nPath:",
		m_policy._lstGroup[i]._lstVmImage[j]._szOS,
		strSize, strStatus
	);

	m_strPath = m_policy._lstGroup[i]._lstVmImage[j]._wszPath;
	
	CString strMem = getSize(m_policy._lstGroup[i]._lstVmImage[j]._nReqMemory<<20);
	CString strDisk = getSize(m_policy._lstGroup[i]._lstVmImage[j]._nReqFreeSpace << 20);

	m_strInfo3.Format(L""
		L"Minimum Requirements\n"
		L"Operating System: %S\n"
		L"CPU: %d * %d\n"
		L"Memory: %s\n"
		L"Hard drive: %s\n"
		L"Antimalware/Antivirus: %S\n"
		,
		m_policy._lstGroup[i]._lstVmImage[j]._szReqOS,
		m_policy._lstGroup[i]._lstVmImage[j]._nReqCpuCount,
		m_policy._lstGroup[i]._lstVmImage[j]._nReqCoreCount,
		strMem,
		strDisk,
		m_policy._lstGroup[i]._lstVmImage[j]._szReqAntiVirus
	);

	UpdateData(FALSE);
}

void ClaPageMain::OnBnClickedBtnRefresh()
{
	_download_group_list();
	_show_group_list();
}

UINT TF_SET_ENVIRONMENT(void* p) {
	ClaPageMain* pThis = (ClaPageMain*)p;
	pThis->set_environment();
	return 0;
}

UINT TF_POLICY_UPDATE(void* p) {
	ClaPageMain* pThis = (ClaPageMain*)p;
	pThis->policy_update();
	lv_thdPolicyUpdate = NULL;
	return 0;
}

UINT TF_CHECK_UPDATE(void* p) {
	ClaPageMain* pThis = (ClaPageMain*)p;
	pThis->check_update();
	//lv_thdPolicyUpdate = NULL;
	return 0;
}

extern CString g_strMail;
unsigned int ClaPageMain::post_to_agent(const char* p_szIP, BOOL p_bIP, BOOL p_bPORT, BOOL p_bViolation, BOOL p_bAgentScript)
{
	char szID[10]; kc_get_uid(szID);
	int uid = atoi(szID);
	char szMID[100];
	sprintf_s(szMID, 100, "%S", KGlobal::GetMachineID().GetBuffer());
	char szVmOS[20];
	char szVmMAC[20];

	char szSession[64]; memset(szSession, 0, sizeof(szSession));
	kc_get_session(szSession, 64);
	{
		char szHost[512]; memset(szHost, 0, sizeof(szHost));
		sprintf_s(szHost, 512, "{\"ip\":\"%S\", \"gip\":\"%S\", \"user_name\":\"%S\", \"fw\":%d, \"session\":\"%s\", \"key\":\"%S\", \"qr_finished\":%d }",
			g_strLocalIP.GetBuffer(), g_strGlobalIP.GetBuffer(),
			g_strMail.GetBuffer(),
			m_group._config._bCreateOutboundRule ? 0 : 1,
			szSession, lv_key_vm.GetBuffer(), 
			lv_bQrFinished);
		std::string str_base64_server = szHost;
		std::string str_base64_https;
		std::string str_base64_script;
		std::string str_base64_agent_script;

		{
			json list;

			for (int i = 0; i < m_group._lstPort.GetCount(); i++) {
				json item = json{
					{"user_id", uid},
					{"machine_id", szMID},
					{"listen_port", m_group._lstPort[i]._nSourcePort},
					{"target_port", m_group._lstPort[i]._nTargetPort},
					{"target", m_group._lstPort[i]._szTarget},
					{"is_https", m_group._lstPort[i]._bIsHttps},
					{"is_direct", m_group._lstPort[i]._bIsDirect},
					{"is_active", 1},
				};
				list.push_back(item);
			}
			std::string str_dump = list.dump();
			str_base64_https = str_dump;
		}

		{
			json json_violation_list;

			for (int i = 0; i < m_group._lstViolation.GetCount(); i++) {
				ClaKcViolation violation = m_group._lstViolation[i];

				json json_list_scripts;
				for (int j = 0; j < violation._policy_scripts.GetCount(); j++) {
					ClaKcPolicyScript script = violation._policy_scripts[j];
					json json_script = json{
						{"id", script._nID},
						{"os", script._os},
						{"script", script._script},
						{"expect", script._expected_result}
					};
					json_list_scripts.push_back(json_script);
				}

				json json_violation = json{
					{"id", violation._nID},
					{"action1", violation._action1},
					{"action2", violation._action2},
					{"check_at_start", violation._check_at_start},
					{"check_interval", violation._check_interval},
					{"message", violation._message},
					{"show_warning", violation._show_warning},
					{"scripts", json_list_scripts}
				};
				json_violation_list.push_back(json_violation);
			}
			std::string str_dump = json_violation_list.dump();
			str_base64_script = str_dump;
		}

		{
			json json_agent_script_list;

			for (int i = 0; i < m_group._lstAgentScriptPlan.GetCount(); i++) {
				ClaKcAgentScriptPlan agentScript = m_group._lstAgentScriptPlan[i];
				CTime timeSchedule(agentScript._scheduled_time);
				char szTime[64]; memset(szTime, 0, sizeof(szTime));
				sprintf_s(szTime, 64, "%S", (LPCWSTR)timeSchedule.Format(L"%Y-%m-%d %H:%M:%S"));

				json json_list_agentScript = json{
					{"id", agentScript._nID},
					{"agent_script_id", agentScript._agent_script_id},
					{"agent_script_title", agentScript._agent_script_title},
					{"agent_script", agentScript._agent_script},
					{"os", agentScript._os},
					{"execution_type", agentScript._execution_type},
					{"scheduled_time", szTime},
					{"run_at_once", agentScript._run_at_once},
				};
				json_agent_script_list.push_back(json_list_agentScript);
			}
			std::string str_dump = json_agent_script_list.dump();
			str_base64_agent_script = str_dump;
		}


		BOOL bSts = FALSE;
		for (int i = 0; i < 10; i++) {
#ifndef DEBUG_NO_LAUNCH_VM
			if (g_pVmMgr->isRunning() == FALSE) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - up port and ip - vm stoped");
				PostMessage(WM_USER_VM_USER_STOP, 0, 0);
				return -1;
			}
#endif
			g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread upip[%S]", str_base64_server.c_str());

			if (p_bIP) {
				if (kc_up_ip(p_szIP, str_base64_server.c_str(), szVmOS, 20, szVmMAC, 20) != 0) {
					Sleep(2000);
					continue;
				}
				lv_vm_os = szVmOS;
				lv_vm_mac = szVmMAC;
			}

			if (str_base64_https == "none") {
				str_base64_https = "[]";
			}

			if (p_bPORT) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread up port");
				if (kc_up_port(p_szIP, str_base64_https.c_str()) != 0) {
					Sleep(1000);
					continue;
				}
			}

			if (str_base64_script == "none" || str_base64_script == "null") {
				str_base64_script = "[]";
			}

			if (str_base64_agent_script == "none" || str_base64_agent_script == "null") {
				str_base64_agent_script = "[]";
			}

			if (p_bViolation) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread up violation");
				if (kc_up_violation(p_szIP, str_base64_script.c_str()) != 0) {
					Sleep(1000);
					continue;
				}
			}

			if (p_bAgentScript) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread up agentscript[%S]", str_base64_agent_script.c_str());
				if (kc_up_agent_script(p_szIP, str_base64_agent_script.c_str()) != 0) {
					Sleep(1000);
					continue;
				}
			}

			bSts = TRUE;
			break;
		}
		if (bSts == FALSE) {
			g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread fail set ip and port");
			PostMessage(WM_USER_VM_AGENT_NOT_FOUND, 0, 0);
			return -2;
		}
	}
	return 0;
}

std::string ClaPageMain::getScripts()
{
	//for (int i = 0; i < m_group._lstViolation.GetCount(); i++) {
	//	ClaKcViolation violation = m_group._lstViolation.GetAt(i);
	//	violation.
	//}
	return "";
}

void ClaPageMain::report_agent_script(const char* p_szResultScript) {
	try {
		json rsp = json::parse(p_szResultScript);
		for (const auto& item : rsp) {
			int id = item["id"];
			int status = item["status"];
			std::string report = item["report"];
			g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"SfT - agent-script-result = [%S]", report.c_str());
			kc_report_agent_script(_nImageID, id, status, report.c_str());
		}
	}
	catch (std::exception e) {

	}
}

void ClaPageMain::set_environment() {
	char szID[10]; kc_get_uid(szID);
	int uid = atoi(szID);
	char szMID[100];
	sprintf_s(szMID, 100, "%S", KGlobal::GetMachineID().GetBuffer());
	char szIP[20]; memset(szIP, 0, sizeof(szIP));
	std::string scriptResult;
	char agentResultScripts[2048];
	char agentVersion[20];
	int nIdleTime;
	int violationID;
	int scriptID;
	char szScriptResult[512]; memset(szScriptResult, 0, sizeof(szScriptResult));
	BOOL bQR = FALSE;
	wchar_t wszTempDir[MAX_PATH];
	wchar_t wszTempPath[MAX_PATH];
	char szAgentIP[30];
	sprintf_s(szAgentIP, "%S", lv_strAgentIP.GetBuffer());

	g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread ");

	while (TRUE) {
		g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread try to get ip address of vm");

#ifndef DEBUG_NO_LAUNCH_VM
		if (g_pVmMgr->isRunning() == FALSE) {
			g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - waiting IP - vm stoped");
			PostMessage(WM_USER_VM_USER_STOP, 1, 0);
			return;
		}
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

	lv_strAgentIP.Format(L"%S", szIP);
	g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread ip = [%s]", lv_strAgentIP);
	if (post_to_agent(szIP, TRUE, TRUE, TRUE, TRUE) != 0) {
		return;
	}

	if (lv_thdPolicyUpdate == NULL) {
		DWORD dwTID;
		lv_thdPolicyUpdate = CreateThread(
			NULL, 0, (LPTHREAD_START_ROUTINE)TF_POLICY_UPDATE,
			this, 0, &dwTID
		);
	}

	int nRetry = 0;
	{
		while (TRUE) {
			if(lv_isUpdating){
				Sleep(5000);
				lv_isUpdating = FALSE;
			}
#ifndef DEBUG_NO_LAUNCH_VM
			if (g_pVmMgr->isRunning() == FALSE) {
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - idle waiting - vm stoped");
				PostMessage(WM_USER_VM_USER_STOP, 0, 0);
				return;
			}
#endif
			if ((kc_get_idle(szIP, &nIdleTime, szScriptResult, &violationID, &scriptID, agentResultScripts) != 0 || nIdleTime == -1)) {
				if (nRetry > 1)
				{
					g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread fail get idle");
					PostMessage(WM_USER_VM_AGENT_NOT_FOUND, 0, 0);
				}
				else {
					nRetry++;
					g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread fail get idle[%d]", nRetry);
					continue;
				}
				break;
			}

			if (1)
			{
				kc_get_agent_version(szIP, agentVersion);
				nRetry = 0;
				lv_currentAgentVersion = agentVersion;
				if (!lv_currentAgentVersion.empty() && lv_currentAgentVersion != "") {
					if (lv_updateVersionDown != NULL) {
						if (lv_updateVersionDown->agent->_version != "" &&
							(lv_updateVersionDown->agent->_version != lv_currentAgentVersion))
						{
							if (_download_agent_update() == 0) {
								kc_upload_update(szIP, ClaPathMgr::GetDP() + L"\\updateAgent.zip");
							}
						}
					}
				}
			}

			if (strlen(szScriptResult) != 0) {
				m_strViolationResult.Format(L"%S", szScriptResult);
				SendMessage(WM_USER_VIOLATION, violationID, scriptID);
			}

			OutputDebugStringA(agentResultScripts);
			report_agent_script(agentResultScripts);

			if (m_policy.agent_timeout != 0
				&& m_policy.agent_timeout < (nIdleTime / 1000))
			{
				g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main thread %d", nIdleTime);
				if (SendMessage(WM_USER_VM_IDLE_TIMEOUT, 0, 0) == 1) {
					//.	stoped
					break;
				}
				else {
					m_policy.agent_timeout *= 2;
				}
			}

			if (bQR == FALSE) {
				GetTempPath(MAX_PATH, wszTempDir);
				swprintf_s(wszTempPath, MAX_PATH, L"%skqr.bmp", wszTempDir);
				if (CaptureScreen(wszTempPath) == 0) {

					HMODULE mod = LoadLibrary(L"qrreceng.dll");
					if (mod != NULL) {
						FN_REC_QR_INFO rec_qr_info = (FN_REC_QR_INFO)GetProcAddress(mod, "rec_qr_info");
						if (rec_qr_info != NULL) {
							char szDir[MAX_PATH]; memset(szDir, 0, sizeof(szDir));
							sprintf_s(szDir, MAX_PATH, "%S", ClaPathMgr::GetDP().GetBuffer());
							char szPngPath[MAX_PATH]; memset(szPngPath, 0, sizeof(szPngPath));
							sprintf_s(szPngPath, MAX_PATH, "%S", wszTempPath);
							char szReturnValue[1024]; memset(szReturnValue, 0, sizeof(szReturnValue));
							rec_qr_info(szDir, szPngPath, szReturnValue);
							CString strQrDecode; strQrDecode.Format(L"%S", szReturnValue);
							strQrDecode.TrimRight(L"\r\n");
							std::string qr_calc = _getQrText();
							CString strQrCalc; strQrCalc.Format(L"%S", qr_calc.c_str());
							if (strQrDecode == strQrCalc) {
								lv_bQrFinished = TRUE;
								post_to_agent(szIP, TRUE, TRUE, FALSE, FALSE);
								KGlobal::writeConfig(&m_group._config, lv_bQrFinished);
								bQR = TRUE;
							}
						}

						FreeLibrary(mod);
					}
				}
			}


			Sleep(500); continue;
		}
	}
}

std::string _getQrText()
{
	unsigned char szBuffer[64];
	sprintf_s((char*)szBuffer, 64, "%s-%s", lv_vm_os.c_str(), lv_vm_mac.c_str());
	int l = strlen((char*)szBuffer);
	memset(&szBuffer[l], 0, 64 - l);

	CString strKey; strKey.Format(L"KAgent-QR-Code - %s", lv_key_vm);

	kn_encrypt_with_pwd((unsigned char*)szBuffer, 64, strKey);

	char ret[260];memset(ret, 0, sizeof(ret));
	for (int i = 0; i < 64; i++) {
		char szNum[3];
		sprintf_s(szNum, 3, "%02X", szBuffer[i]);
		strcat_s(ret, 260, szNum);
	}

	return ret;
}

void ClaPageMain::policy_update() {
	char szAgentIP[30]; 
	sprintf_s(szAgentIP, "%S", lv_strAgentIP.GetBuffer());

	while (TRUE) {
		Sleep(1000 * g_nPolicyUpdatePeriod);
		if (lv_thdEnvironment == NULL) break;

		ClaKcGroup* pGroupDown = kc_down_config(_nGroupID, _nImageID);
		if (pGroupDown != NULL) {
			if (m_group != pGroupDown[0]) {

				KGlobal::writeConfig(&pGroupDown->_config, lv_bQrFinished);
				int nEqualPort = m_group.comparePort(pGroupDown[0]);
				int nEqualScriptViolation = m_group.compareScriptViolation(pGroupDown[0]);
				int nEqualScriptPlan = m_group.compareScriptPlan(pGroupDown[0]);
				m_group = pGroupDown[0];

				post_to_agent(szAgentIP, TRUE, nEqualPort != 0, nEqualScriptViolation != 0, nEqualScriptPlan != 0);
			}
		}
	}
}
void ClaPageMain::check_update() {
	std::string sClientVersion;
	while (TRUE) {
		lv_updateVersionDown = kc_down_check_update();
		if (lv_updateVersionDown != NULL) {
			if (sClientVersion != lv_updateVersionDown->launcher->_version &&
				lv_updateVersionDown->launcher->_version != "" && 
				(lv_updateVersionDown->launcher->_version != CURRENT_VERSION ||
					lv_updateVersionDown->launcher->_action_type == 0))
			{
				sClientVersion = lv_updateVersionDown->launcher->_version;

				_download_update_version();
			}
		}
		Sleep(1000 * g_checkUpdateInterval);
	}
}

void ClaPageMain::onAfterStopVM(BOOL p_bStop, int reason)
{
	if (g_pVmMgr->isWorking()) {
		return;
	}

	_nReason = reason;
	kc_report_action(_nImageID, "stop");

	if (p_bStop)
		g_pVmMgr->stopVM();
	else
		g_pVmMgr->suspendVM();

}

void ClaPageMain::onAfterVMStoped()
{
	if (lv_thdEnvironment) TerminateThread(lv_thdEnvironment, 0);
	lv_thdEnvironment = NULL;
	if (lv_thdPolicyUpdate) TerminateThread(lv_thdPolicyUpdate, 0);
	lv_thdPolicyUpdate = NULL;
	if (lv_thdDownload) TerminateThread(lv_thdDownload, 0);
	lv_thdDownload = NULL;

	kc_release();
	g_pVmMgr->m_strVMX.Empty();
	SetButtonImage(m_btnStart, IDB_PNG_BTN_ICON_START, IDB_PNG_BTN_ICON_START_H);
	OnGroupListSelChanged(m_lstGroup.getCurSel());

	CString strExeUpdate; strExeUpdate.Format(L"%s\\KUpdateApp.exe", lv_updatePath.c_str());

	switch(_nReason){
	case stop_vm_reason_user_control: break;
	case stop_vm_reason_fail_launch: break;
	case stop_vm_reason_fail_qr:
		KMessageBox(L"It has been failed to detect QR code image.");
		break;
	case stop_vm_reason_logout:
		g_pMain->ShowLoginPage();
		break;
	case stop_vm_reason_client_update:
		ClaProcess::CreateProcessEx(strExeUpdate, L"", lv_updatePath.c_str(), NULL, NULL);
		g_pMain->OnCancel();
		break;
	case stop_vm_reason_client_force_stop:
		g_pMain->OnCancel();
		break;
	case stop_vm_reason_agent_not_found:
		KMessageBox(L"Failed to connect to VM agent.");
		break;
	case stop_vm_reason_expire_idle_vm: break;
	case stop_vm_reason_kclient_terminated:
		KMessageBox(L"KClient is not running. Please restart the launcher and try again.");
		g_pMain->OnCancel();
		break;
	case stop_vm_reason_user_close_vm: break;
	case stop_vm_reason_violation_logout:
		g_pMain->ShowLoginPage();
		break;
	case stop_vm_reason_violation_delete:
		CString strPath;
		strPath.Format(L"%s\\VMs\\%S", ClaPathMgr::GetDP(), m_policy._lstGroup[_nGroupIdx]._lstVmImage[_nImageIdx]._szTitle);
		ClaPathMgr::DeleteDirectory(strPath);
		break;

	}
}

void ClaPageMain::OnBnClickedBtnStart()
{
	char szKey[128]; memset(szKey, 0, sizeof(szKey));
	char szPwd[128]; memset(szPwd, 0, sizeof(szPwd));
	if (g_pVmMgr->isWorking()) {
		KMessageBox(L"VMware is working now. Please wait a little.");
		return;
	}
	//if (lv_thdDownload) {
	//	KMessageBox(L"Downloading VMware image now. Please wait.");
	//	return;
	//}

	int nSelect = m_lstGroup.getCurSel();
	if (nSelect == -1) {
		KMessageBox(L"Please select the vmware on left side list before launch");
		return;
	}

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main launch");

	DWORD dwData = m_lstGroup.GetItemData(nSelect);
	int i = dwData >> 16;
	int j = dwData & 0xFFFF;

	if (_checkReqirement(&m_policy._lstGroup[i]._lstVmImage[j]) == FALSE) {
		return;
	}


	ClaKcVmImage* p = &(m_policy._lstGroup[i]._lstVmImage[j]);
	_nGroupID = m_policy._lstGroup[i]._id;
	_nGroupIdx = i;
	_nImageID = m_policy._lstGroup[i]._lstVmImage[j]._id;
	_nImageIdx = j;
	CString strPath = p->_wszPath;
	CString strPwd; strPwd.Format(L"%S", p->_szPwd);

	g_pVmMgr->m_strVMX = strPath;
	g_pVmMgr->m_strPWD = strPwd;

	if (g_pVmMgr->isRunning()) {
		UINT nSts = KMessageBox(L"VMware is running. Do you want to stop it?", MB_CANCELTRYCONTINUE);
		if (nSts == IDTRYAGAIN) {
			//.	suspend
			onAfterStopVM(FALSE, stop_vm_reason_user_control);
		}
		else if (nSts == IDOK) {
			//. terminate
			onAfterStopVM(TRUE, stop_vm_reason_user_control);
		}
		else {
			//. Cancel
			return;
		}
		return;
	}

	if (strPath.IsEmpty() || GetFileAttributes(strPath) == INVALID_FILE_ATTRIBUTES) {
		KMessageBox(L"You need to download VMware image before launch");
		return;
	}

	kc_report_action(_nImageID, "start");

	int nSts = 0;
	ClaKcGroup* pGroupDown = kc_down_config(_nGroupID, _nImageID);
	if (pGroupDown == NULL) {
		KMessageBox(L"Unable to connect to the server");
		return;
	}
	lv_bQrFinished = FALSE;

	m_group = pGroupDown[0];

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main launch getconfig = %d", nSts);
	if (nSts != 0) {
		KMessageBox(L"Unable to connect to the server");
		return;
	}

	nSts = kc_get_password(_nImageID, szKey, 128, szPwd, 128);
	if (nSts != 0) {
		KMessageBox(L"Unable to connect to the server");
		return;
	}

	char szNew[MAX_PATH];
	GUID guid; CoCreateGuid(&guid);
	sprintf_s(szNew, MAX_PATH, "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	if (g_pVmMgr->encryptVMX(strPath, szPwd, szNew) != 0) {
		KMessageBox(L"Password is not correct for this vm image");
		return;
	}

	nSts = kc_set_password(_nImageID, szNew);
	if (nSts != 0) {
		KMessageBox(L"Unable to connect to the server");
		return;
	}

	memset(m_group._config._wszEncPwd, 0, sizeof(m_group._config._wszEncPwd));
	swprintf_s(m_group._config._wszEncPwd, 100, L"%S", szNew);
	KGlobal::writeConfig(&m_group._config);

	nSts = g_pVmMgr->launchVM(strPath, szKey, szNew);
	if (nSts != 0) {
		KMessageBox(L"Password is invalid or vmx file is corrupted.");
		return;
	}

	m_bStarting = TRUE;
	SetButtonImage(m_btnStart, IDB_PNG_BTN_ICON_STOP, IDB_PNG_BTN_ICON_STOP_H);
}

void ClaPageMain::onAfterVmLaunched(int p_result)
{
	if (p_result != 0) {
		g_pLog->LogFmtW(LOG_LEVEL_ERROR, L"Launcher - main launch failed[%d][%d]", p_result, GetLastError());
		if (GetLastError() == ERROR_PASSWORD_RESTRICTION) {
			KMessageBox(L"It has been failed to launch VMware image due to incorrect password");
		}
		else {
			KMessageBox(L"Failed to launch VM");
		}

		onAfterStopVM(TRUE, stop_vm_reason_fail_launch);
		return;
	}

	GUID guid; CoCreateGuid(&guid);
	lv_key_vm.Format(L"%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	OnGroupListSelChanged(m_lstGroup.getCurSel());

	m_bStarting = FALSE;

	DWORD dwTID = 0;
	lv_thdEnvironment = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_SET_ENVIRONMENT, this, 0, &dwTID);
}

LRESULT ClaPageMain::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	unsigned int nSts = 0;
	UINT nImgIdStart[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_START, IDB_PNG_BTN_ICON_START_H, 0, 0 };

	if (message == WM_USER_LIST_ITEM_SEL) {
		int nSel = (int)wParam;
		OnGroupListSelChanged(nSel);
	}

	if (message == WM_USER_QR_INVALID) {
		onAfterStopVM(TRUE, stop_vm_reason_fail_qr);
		return 0;
	}

	if (message == WM_USER_VM_LAUNCHED) {
		onAfterVmLaunched(wParam);
	}

	if (message == WM_USER_VM_STOPED) {
		onAfterVMStoped();
	}
	if (message == WM_USER_VM_SUSPENDED) {
		onAfterVMStoped();
	}

	if (message == WM_USER_MENU_CLICKED) {
		int p_nCurSel = m_cmbMenu.getCurSel();
		if (p_nCurSel == 3) {
			g_pMain->ShowAboutPage();
		}
		else if (p_nCurSel == 2) {
			if (KMessageBox(L"Are you sure you want to logout? If vmware is running, it will be stop.", MB_OKCANCEL) == IDOK) {
				onAfterStopVM(TRUE, stop_vm_reason_logout);
			}
		}
		m_cmbMenu.setCurSel(0);
	}

	if (message == WM_USER_DOWNLOAD_UPDATE) {
		if (_bPausedDownload == FALSE)
		{
			m_pgsPercent.setPosition(wParam);
			m_strPercent.Format(L"%d.%02d %%", wParam, lParam);
			UpdateData(FALSE);
		}
	}
	if (message == WM_USER_DOWNLOAD_UNZIPPING) {
		m_strPercent = L"Extract ...";
		UpdateData(FALSE);
	}
	if (message == WM_USER_DOWNLOAD_COMPLETE) {

		KMessageBox(L"Download vm image is complete successfully.", MB_ICONINFORMATION);
		m_strPercent = L"Complete";
		UpdateData(FALSE);
		lv_thdDownload = NULL;
		m_btnPause.ShowWindow(SW_HIDE);

		SetButtonImage(m_btnDownload, IDB_PNG_BTN_ICON_DOWN, IDB_PNG_BTN_ICON_DOWN_H);
	}
	if (message == WM_USER_UPDATE_READY) {
		CString strExeUpdate; strExeUpdate.Format(L"%s\\KUpdateApp.exe", lv_updatePath.c_str());
		if (GetFileAttributes(strExeUpdate) == INVALID_FILE_ATTRIBUTES) {
			return 2;
		}
		ClaProcess::CreateAndWait(strExeUpdate, L"--check", lv_updatePath.c_str(), &nSts);
		if (nSts != 0x8000) {
			return 2;
		}

		if (lv_updatePath != L""){
			if (KMessageBox(L"New version of launcher is ready to update. Do you want to update it now?", MB_OKCANCEL) == IDOK) {
				onAfterStopVM(FALSE, stop_vm_reason_client_update);
				return 0;
			}
			else {
				ClaRegMgr reg(HKEY_LOCAL_MACHINE);
				if (reg.writeInt(L"Software\\Kenan", L"update_flag", (DWORD)lv_updateVersionDown->launcher->_action_type) == FALSE) {
					//goto L_EXIT;
				}
				if (lv_updateVersionDown->launcher->_action_type == 0) {
					if (KMessageBox(L"You have to update launcher", MB_OK)) {
						onAfterStopVM(FALSE, stop_vm_reason_client_force_stop);
					}
				}
				return 2;
			}
		}
	}
	if (message == WM_USER_DOWNLOAD_FAILED) {
		KMessageBox(L"Failed to download VMware image or the image downloaded is invalid");
		lv_thdDownload = NULL;
		m_strPercent = L"Failed";
		UpdateData(FALSE);
		SetButtonImage(m_btnDownload, IDB_PNG_BTN_ICON_DOWN, IDB_PNG_BTN_ICON_DOWN_H);
		m_btnPause.ShowWindow(SW_HIDE);

		m_btnDownload.Invalidate();
	}
	if (message == WM_USER_DOWNLOAD_CANCELED) {
		UpdateData(FALSE);
		SetButtonImage(m_btnDownload, IDB_PNG_BTN_ICON_DOWN, IDB_PNG_BTN_ICON_DOWN_H);
		m_btnPause.ShowWindow(SW_HIDE);
		m_pgsPercent.ShowWindow(SW_HIDE);
		m_stcPercent.ShowWindow(SW_HIDE);
		m_btnPause.ShowWindow(SW_HIDE);
		m_stcDownName.ShowWindow(SW_HIDE);
	}
	if (message == WM_USER_VM_AGENT_NOT_FOUND) {
		onAfterStopVM(TRUE, stop_vm_reason_agent_not_found);
	}
	if (message == WM_USER_VM_IDLE_TIMEOUT) {
		//.	idle time over
		ClaDlgTimeout dlg(IDB_PNG_BG_ALERT);

		dlg.m_strTitle = L"Kenan Launcher";
		dlg.m_strData = L"You have no input to VMWare.";
		dlg.m_nMax = 60;

		if (dlg.DoModal() == IDCANCEL) {
			onAfterStopVM(TRUE, stop_vm_reason_expire_idle_vm);
			return 1;
		}
		else {
			return 0;
		}
	}
	if (message == WM_USER_KCLIENT_TERMINATED) {
		onAfterStopVM(FALSE, stop_vm_reason_kclient_terminated);
	}
	else if (message == WM_USER_VM_USER_STOP) {
		onAfterStopVM(TRUE, stop_vm_reason_user_close_vm);
	}
	else if (message == WM_USER_VIOLATION) {
		int violationID = (int)wParam;
		int scriptID = (int)lParam;
		int nAction1 = 3, nAction2 = 4;
		CString strMessage;
		bool isShowWarning = FALSE;

		for (int i = 0; i < m_group._lstViolation.GetCount(); i++) {
			ClaKcViolation violation = m_group._lstViolation[i];
			if (violation._nID == violationID) {
				nAction1 = violation._action1;
				nAction2 = violation._action2;
				isShowWarning = violation._show_warning;
				strMessage.Format(L"%S", violation._message.c_str());
				break;
			}
		}

		if (nAction1 == 0 || nAction1 == 1 || nAction1 == 2 || nAction2 != 4 ) {
			if (nAction2 == 1)
				onAfterStopVM(nAction1 != 1, stop_vm_reason_violation_delete);
			else if (nAction2 != 4)
				onAfterStopVM(nAction1 != 1, stop_vm_reason_violation_logout);
		}

		char* szViolation = (char*)malloc(m_strViolationResult.GetLength() + 1);
		sprintf_s(szViolation, m_strViolationResult.GetLength() + 1, "%S", m_strViolationResult.GetBuffer());
		kc_report_violation(_nImageID, violationID, scriptID, 1, 1, szViolation);
		free(szViolation);
		if (isShowWarning) {
			KMessageBox(strMessage);
		}
	}
	return KDialog::WindowProc(message, wParam, lParam);
}

UINT TF_DOWN_UNZIP(void* p) {
	ClaPageMain* pThis = (ClaPageMain*)p;

	pThis->download_unzip();

	lv_thdDownload = NULL;

	return 0;
}

int lastD = -1, lastP = -1;
BOOL CALLBACK_DOWN(long long lCur, long long lTotal)
{
	if (lv_thdDownload == NULL) return FALSE;
	if (lTotal == 0) return TRUE;
	int d = (int)(lCur * 100 / lTotal);
	int p = (lCur * 10000 / lTotal) % 100;

	if (lastD != d || lastP != p) {
		lv_pMainDialog->SendMessage(WM_USER_DOWNLOAD_UPDATE, d, p);
		lastD = d;
		lastP = p;
	}
	return TRUE;
}

int ClaPageMain::_execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam)
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
BOOL lv_bStopDownload = FALSE;
void ClaPageMain::download_unzip() {

	lastD = -1, lastP = -1;
	CString downloadName;
	downloadName.Format(L"%S", m_policy._lstGroup[m_nDownGID]._lstVmImage[m_nDownIID]._szTitle);
	int downloadID = m_policy._lstGroup[m_nDownGID]._lstVmImage[m_nDownIID]._id;
	ClaKcVmImage image = m_policy._lstGroup[m_nDownGID]._lstVmImage[m_nDownIID];

	CreateDirectory(ClaPathMgr::GetDP() + L"\\VMs", NULL);
	wchar_t wszDown[MAX_PATH]; memset(wszDown, 0, sizeof(wszDown));
	swprintf_s(wszDown, MAX_PATH, L"%s\\VMs\\%s", ClaPathMgr::GetDP().GetBuffer(), downloadName.GetBuffer());

	kc_up_vmimage_status(image._id, 1);

	if (image._szDropboxVmPath[0] == 0
		|| image._szDropboxAppKey[0] == 0
		|| image._szDropboxAppSecret[0] == 0
		|| image._szDropboxAppRefresgToken[0] == 0)
	{
		SendMessage(WM_USER_DOWNLOAD_FAILED, 0, 0);
		return;
	}

	if (1 && kc_down_vmimage(image._szDropboxVmPath, wszDown, m_bDownloadContinue, 
		image._szDropboxAppKey, image._szDropboxAppSecret, image._szDropboxAppRefresgToken,
		(KC_CALLBACK_DOWN_VM)CALLBACK_DOWN) != 0) {
		if (lv_thdDownload == NULL) {
			SendMessage(WM_USER_DOWNLOAD_CANCELED, 0, 0);
			return;
		}
		else {
			BOOL bSuccess = FALSE;
			for (int i = 0; i < 5; i++) {
				if (kc_down_vmimage(image._szDropboxVmPath, wszDown, TRUE,
					image._szDropboxAppKey, image._szDropboxAppSecret, image._szDropboxAppRefresgToken,
					(KC_CALLBACK_DOWN_VM)CALLBACK_DOWN) != 0) {
					continue;
				}
				else {
					bSuccess = TRUE;
					break;
				}
			}
			if (!bSuccess) {
				SendMessage(WM_USER_DOWNLOAD_FAILED, 0, 0);
				return;
			}
		}
	}

	wchar_t szZipped[MAX_PATH]; swprintf_s(szZipped, MAX_PATH, L"%s", wszDown);
	wchar_t szUnzipped[MAX_PATH]; swprintf_s(szUnzipped, MAX_PATH, L"%s.e", wszDown);

	SendMessage(WM_USER_DOWNLOAD_UNZIPPING, 0, 0);

	CreateDirectory(szUnzipped, NULL);

	wchar_t wszCmd[512]; memset(wszCmd, 0, sizeof(wszCmd));
	swprintf_s(wszCmd, 512, L"x \"%s\" -o\"%s.e\" -y -spe -spf", wszDown, wszDown);
	ClaPathMgr::UnzipFile(ClaPathMgr::GetDP(), wszCmd);
	DeleteFile(wszDown);
	
	CString vmx_path = findVMX(szUnzipped);
	if (vmx_path.IsEmpty()) {
		SendMessage(WM_USER_DOWNLOAD_FAILED, 0, 0);
		return;
	}

	char szNew[MAX_PATH];
	GUID guid; CoCreateGuid(&guid);
	sprintf_s(szNew, MAX_PATH, "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	kc_set_password(downloadID, szNew);
		
	if (g_pVmMgr->encryptVMX(vmx_path, "", szNew) != 0) {
	}

	//kn_encrypt_file_with_pwd(vmx_path, vmx_path, );
	SendMessage(WM_USER_DOWNLOAD_COMPLETE, 0, 0);
}

void ClaPageMain::SetButtonImage(KButtonPng& btn, UINT imgID, UINT imgID_Hover)
{
	UINT imageIDs[] = { imgID, imgID_Hover, 0, 0, 0 };
	btn.LoadImageList(imageIDs);
}

void ClaPageMain::OnBnClickedBtnDownload()
{
	if (lv_thdDownload != NULL) {
		if (KMessageBox(L"A VMware image is downloading now. Do you want to stop download?", MB_OKCANCEL) == IDCANCEL) {
			return;
		}
		else {
			lv_thdDownload = NULL;
			Sleep(1000);
			return;
		}
	}
	int nSelect = m_lstGroup.getCurSel();

	if (nSelect == -1) {
		KMessageBox(L"Please select the vmware item on left side listbox before download.");
		return;
	}

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - main down start");

	DWORD dwData = m_lstGroup.GetItemData(nSelect);
	int i = dwData >> 16;
	int j = dwData & 0xFFFF;

	if (_checkReqirement(&m_policy._lstGroup[i]._lstVmImage[j]) == FALSE) {
		return;
	}

	ClaKcVmImage* pImage = &(m_policy._lstGroup[i]._lstVmImage[j]);

	wchar_t wszDown[MAX_PATH]; memset(wszDown, 0, sizeof(wszDown));
	CString downloadName;
	downloadName.Format(L"%S", pImage->_szTitle);
	swprintf_s(wszDown, MAX_PATH, L"%s\\VMs\\%s", ClaPathMgr::GetDP().GetBuffer(), downloadName.GetBuffer());

	if (pImage->_wszPath && GetFileAttributes(pImage->_wszPath) != INVALID_FILE_ATTRIBUTES) {
		KMessageBox(L"You have already download this VMware image.");
		return;
	}
	else {
		if (GetFileAttributes(wszDown) != INVALID_FILE_ATTRIBUTES) {
			UINT button = KMessageBox(L"Click [Continue] to resume download, click [Start] to restart download.\r\n", MB_YESNOCANCEL);
			if (button == IDTRYAGAIN) {
				m_bDownloadContinue = TRUE;
			}
			else if (button == IDOK) {
				m_bDownloadContinue = FALSE;
			}
			else {
				return;
			}
		}else{
			if (KMessageBox(L"Do you want to download the VM image?", MB_OKCANCEL) != IDOK) {
				return;
			}
			m_bDownloadContinue = FALSE;
		}
	}

	m_pgsPercent.setRange(0, 100);
	DWORD dwTID = 0;

	m_nDownGID = i;
	m_nDownIID = j;

	_bPausedDownload = FALSE;
	lv_thdDownload = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_DOWN_UNZIP, this, 0, &dwTID);

	m_pgsPercent.ShowWindow(SW_SHOW);
	m_stcPercent.ShowWindow(SW_SHOW);
	m_btnPause.ShowWindow(SW_SHOW);
	m_stcDownName.ShowWindow(SW_SHOW);

	SetButtonImage(m_btnDownload, IDB_PNG_BTN_ICON_STOP, IDB_PNG_BTN_ICON_STOP_H);
	m_strDownName.Format(L"Downloading ... [%S]", m_policy._lstGroup[i]._lstVmImage[j]._szTitle);
	UpdateData(FALSE);
}


void ClaPageMain::OnDestroy()
{
	KDialog::OnDestroy();

	KillTimer(0);
}

std::string last_connect = "";
void ClaPageMain::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 0) {
		int qr_state = -1;
		std::string strConn = kc_get_status(&qr_state);
		if (last_connect != strConn) {
			if (strConn == "connected") {
				m_stcConnected.LoadImageK(IDB_PNG_STC_CONNECTED);
			}
			else { 
				m_stcConnected.LoadImageK(IDB_PNG_STC_DISCONNECT);
			}
			m_stcConnected.UpdateState();
		}
		char units[256] = "KMGT";
		int unit_index = 0;
		int speed = g_pMain->_nValidateSpeed;
		while (speed > 1000) {
			speed = (unsigned long)speed >> 10;
			unit_index++;
		}
		m_strSpeed.Format(L"%d %cbps", speed, ((unit_index > 0 && unit_index < 4) ? units[unit_index - 1] : ' '));
		m_strRate = g_pMain->_strRate;
		UpdateData(false);

		//.	check node_client is running
		if (WaitForSingleObject(g_hProcNodeClient, 100) != WAIT_TIMEOUT) {
			PostMessage(WM_USER_KCLIENT_TERMINATED, 0, 0);
			KillTimer(0);
		}
		if (qr_state == 0 && g_pVmMgr->isRunning()) {
			PostMessage(WM_USER_QR_INVALID, 0, 0);
			KillTimer(0);
		}
	}

	KDialog::OnTimer(nIDEvent);
}

BOOL ClaPageMain::_checkReqirement(ClaKcVmImage* image)
{
	BOOL bRet = FALSE;
	CString strMessage;
	std::string curOS = ClaOSInfo::getOS();
	unsigned long long curMem;
	ClaOSInfo::getRamInfo(&curMem);
	wchar_t wszDrive[] = L"?:\\";
	wszDrive[0] = ClaPathMgr::GetDP()[0];
	unsigned long long curDiskFree;
	unsigned long long curDiskTotal;
	ClaOSInfo::getDriveSize(wszDrive, &curDiskTotal, &curDiskFree);
	int nCore, nProcessor, nSpeed;
	std::wstring strAVs;
	ClaOSInfo::getAntivirusList(strAVs);
	CString curAvs = strAVs.c_str();
	ClaOSInfo::getCpuInfo(&nCore, &nProcessor, &nSpeed);
	wchar_t wszAvs[MAX_PATH]; memset(wszAvs, 0, sizeof(wszAvs));
	CString strAvs; strAvs.Format(L"%S", image->_szReqAntiVirus);
	strAvs.Trim(L", ");
	swprintf_s(wszAvs, MAX_PATH, L"%s", strAvs.GetBuffer());
	wchar_t* pAV = wszAvs;
	wchar_t* pAVNext = wszAvs;

	if (strstr(image->_szReqOS, curOS.c_str()) == NULL){
		strMessage.Format(L"Require operating system is [%S].", image->_szReqOS);
		goto L_EXIT;
	}

	if (curMem < (unsigned long long(image->_nReqMemory) << 20)) {
		strMessage.Format(L"Require memory is [%s].", getSize(unsigned long long(image->_nReqMemory) << 20).GetBuffer());
		goto L_EXIT;
	}

	if (curDiskFree < (unsigned long long(image->_nReqFreeSpace) << 20)) {
		strMessage.Format(L"Require free space is [%s].", getSize(unsigned long long(image->_nReqFreeSpace) << 20).GetBuffer());
		goto L_EXIT;
	}

	if (nCore < image->_nReqCpuCount || nProcessor < image->_nReqCoreCount) {
		strMessage.Format(L"CPU requirement is [core : %d, processor : %d].", image->_nReqCpuCount, image->_nReqCoreCount);
		goto L_EXIT;
	}

	pAVNext = wcsstr(pAV, L",");
	if (pAVNext != NULL) {
		pAVNext--; pAVNext[0] = 0;
	}

	while (TRUE) {
		int find = strAVs.find(pAV);
		if (find < 0) {
			strMessage.Format(L"Anti-Virus products need to be run[%S].", image->_szReqAntiVirus);
			goto L_EXIT;
		}
		if (pAVNext == NULL)
			break;
		pAV = &pAVNext[1];
		pAVNext = wcsstr(pAV, L",");
		if (pAVNext != NULL) {
			pAVNext--; pAVNext[0] = 0;
		}
	}

	bRet = TRUE;
L_EXIT:
	if (bRet == FALSE)
		KMessageBox(strMessage);
	return bRet;
}



void ClaPageMain::OnBnClickedBtnPause()
{
	if (lv_thdDownload != NULL) {
		if (!_bPausedDownload) {
			_bPausedDownload = TRUE;
			SuspendThread(lv_thdDownload);
			m_strPercent = L"Paused";
			UpdateData(FALSE);
			UINT nImgIdPause[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_START, IDB_PNG_BTN_ICON_START_H, 0, 0 };
			m_btnPause.LoadImageList(nImgIdPause);
		}
		else {
			_bPausedDownload = FALSE;
			ResumeThread(lv_thdDownload);
			UINT nImgIdPause[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_PAUSE, IDB_PNG_BTN_ICON_PAUSE_H, 0, 0 };
			m_btnPause.LoadImageList(nImgIdPause);
		}
	}
}
