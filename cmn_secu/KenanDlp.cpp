#include "pch.h"
#include "KenanDlp.h"
#include "KenanSecuDll.h"
#include "KenanClpDll.h"
#include "KenanVdDll.h"
#include "KenanSvc.h"
#include "ClaPathMgr.h"
#include "mfcCmn.h"
#include "kn_conf.h"
#include "ClaSecurityDescriptor.h"
#include "ClaSecuPolicy.h"
#include "ClaRegMgr.h"
#include "ClaProcess.h"
#include "ClaSecuSvc.h"
#include "ClaMutex.h"
#include "Global.h"
#include "ClaSecuAgent.h"

CString lv_strVDImage;
CString lv_strSecuDll;
BOOL	lv_bMounted = false;
CString lv_strSecuPath;
CArray<HANDLE> lv_lstProcess;
HANDLE hMutexDlp;
HANDLE hMutexExit = NULL;


unsigned int dlp_init()
{
//	SVC_Init();

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"vd init");
	if (VD_Init() != 0) {
		AfxMessageBox(L"Failed to initialize the Virtual Disk module.");
		return -1;
	}

	if (SECU_Init() != 0) {
		AfxMessageBox(L"Failed to initialize the Security module.");
		return -1;
	}

	if (CLP_Init() != 0) {
		AfxMessageBox(L"Failed to initialize the Clipboard module.");
		return -1;
	}

	lv_strVDImage.Format(L"%s\\vimage.si", ClaPathMgr::GetDP());
#ifdef _WIN64
	lv_strSecuDll.Format(L"%s\\KenanSecu64.dll", ClaPathMgr::GetDP());
#else
	lv_strSecuDll.Format(L"%s\\KenanSecu32.dll", ClaPathMgr::GetDP());
#endif
	/*
	if (VD_MountInit() != 0) {
		;
	}
	*/

	hMutexDlp = ClaMutex::Create(L"Global\\Mutex-DLP");

	return 0;
}

int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam)
{
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = L"open";
	ShExecInfo.lpFile = p_szEXE;
	ShExecInfo.lpParameters = p_pszCommandParam; //  L"/C apktool.bat -f d D:\\work\\_FCM\\test_org.apk -o D:\\work\\_FCM\\aaa";
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE; // SW_NORMAL

	if (ShellExecuteEx(&ShExecInfo)) {
		// Wait for the process to exit
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	}
	return 1;
}

unsigned int dlp_mount(ST_SETTING* p_pstSetting)
{
	if (hMutexExit != NULL) CloseHandle(hMutexExit);

	if (lv_bMounted == TRUE) {
		return 0;
	}

	//
//	Do global hook for clp and secu
//  : to do this, it need to be run the agent
//
	//.	wait for agent running.
	if (ClaSecuAgent::WaitAgentRunning(10000) != 0) {
		MsgBox(L"Failed to create security environment.");
		dlp_finish(false);
		return -1;
	}

	unsigned long lSts = 0;
	wchar_t wzDrv = L'Z';
	//
	//.	Mount secu volume
	//

	//.	if no image file, create image file first.
	if (GetFileAttributes(lv_strVDImage) == INVALID_FILE_ATTRIBUTES) {
		//	create image
		lSts = VD_Create(lv_strVDImage, L"Password1234!@#)", 500);
		if (lSts != 0) {
			MsgBox(L"Failed to create virtual disk image file.\n[error code = 0x%08X]", lSts);
			return -1;
		}
	}

	//.	mount image file
	if (!lv_bMounted)
	{
		lSts = VD_Mount(lv_strVDImage, L"Password1234!@#)", wzDrv);
		if (lSts != 0) {
			MsgBox(L"Failed to login.\n[error code = 0x%08X]", lSts);
			return -1;
		}
		lv_strSecuPath.Format(L"%c:\\", wzDrv);
		lv_bMounted = TRUE;
	}

	ClaSecuPolicy::RegisterSecuPath(lv_strSecuPath);

	CString strTemp; strTemp.Format(L"/C rd /s /q \"%s\"", lv_strSecuPath + L".tmp");
	_execute(L"cmd.exe", strTemp);
	Sleep(500);

	CreateDirectory(lv_strSecuPath + L".tmp", NULL);
	SetFileAttributes(lv_strSecuPath + L".tmp", GetFileAttributes(lv_strSecuPath + L".tmp") | FILE_ATTRIBUTE_HIDDEN);
	ClaSecuPolicy::RegisterTempPath(lv_strSecuPath + L".tmp");

	ClaSecuPolicy::SetDlpClipboard(p_pstSetting->m_bClipboard);
	ClaSecuPolicy::SetDlpFile(p_pstSetting->m_bFile);
	ClaSecuPolicy::SetDlpCapture(p_pstSetting->m_bCapture);
	ClaSecuPolicy::SetDlpPrint(p_pstSetting->m_bPrint2PDF, p_pstSetting->m_bPrint2Dev);
	ClaSecuPolicy::SetDlpGlobalFlag(p_pstSetting->m_bGlobal);
	ClaSecuPolicy::SetDlpCryptoKey(p_pstSetting->m_pKey);
	ClaSecuPolicy::SetStartFlag(TRUE);

	return 0;
}
unsigned int dlp_launch_new(ST_CONF_APP* p_pstApp)
{
	unsigned long lSts = 0;
	wchar_t wzDrv;

	if (lv_bMounted == FALSE) {
		MsgBox(L"Before launch security process, you need to mount security volume first.");
		return -1;
	}

	//
	//
	//
	HANDLE hProc = NULL;
	lSts = SECU_CreateProcessWithDll(p_pstApp->m_wszPath, L"", lv_strSecuPath, lv_strSecuDll, TRUE, &hProc);
	if (lSts == -2) {
		return -2;

	} else if (lSts != 0) {
		MsgBox(L"Failed to create process.");
		return -1;
	}

	lv_lstProcess.Add(hProc);
	return 0;
}

unsigned int dlp_finish(bool p_bMsg)
{
	for (int i = 0; i < lv_lstProcess.GetCount(); i++) {
		TerminateProcess(lv_lstProcess[i], 0);
	}

	ClaSecuPolicy::SetStartFlag(FALSE);

	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(MUTEX_ALL_ACCESS);

		if (hMutexExit == NULL)
			hMutexExit = CreateMutex(&sa, FALSE, L"Global\\Kenan_Mutex_Stop");

		delete(sa.lpSecurityDescriptor);

	}


	//.	dismount all volume
	VD_MountInit();

	SECU_Finish();

	lv_bMounted = FALSE;

//	CloseHandle(hMutexExit);

	return 0;
}

bool dlp_is_exist_secu()
{
	for (int i = 0; i < lv_lstProcess.GetCount(); i++) {
		if (WaitForSingleObject(lv_lstProcess[i], 100) == WAIT_TIMEOUT) {
			return true;
		}
	}
	return false;
}