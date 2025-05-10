// agent_svc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <wtypes.h>
#include <stdio.h>
#include <winsvc.h>
#include "ClaProcess.h"
#include "ClaPathMgr.h"
#include "ClaFwMgr.h"
#include "ClaRegMgr.h"
#include "ClaVM.h"

#define GD_SVC_NAME				L"kenan-agent"

SERVICE_STATUS_HANDLE lv_hServiceStatus;
HANDLE lv_thdService = NULL;
HANDLE lv_thdServiceChk = NULL;
HANDLE lv_thdFwMgr = NULL;
HANDLE lv_hProcess = NULL;

// Service control handler function
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
	switch (dwCtrl) {
	case SERVICE_CONTROL_STOP:
		// Stop the service
		// Perform cleanup tasks here
		// Notify service controller that the service has stopped
		if (lv_hProcess != NULL) {
			TerminateProcess(lv_hProcess, 0);
		}
		if (lv_thdService != NULL) {
			TerminateThread(lv_thdService, 0);
		}
		TerminateThread(lv_thdServiceChk, 0);
		SERVICE_STATUS serviceStatus;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(lv_hServiceStatus, &serviceStatus);
		break;
	default:
		break;
	}
}

int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam)
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
		DWORD dwCode = 0;
		GetExitCodeProcess(ShExecInfo.hProcess, &dwCode);
		return 0;
	}
	return 1;
}

unsigned long checkServiceStatus(const wchar_t* p_wszName)
{
	unsigned long lRet = 0;
	SC_HANDLE scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (scmHandle == NULL) {
		return 0;
	}

	// Open a handle to the service.
	SC_HANDLE serviceHandle = OpenService(scmHandle, p_wszName, SERVICE_QUERY_STATUS);
	if (serviceHandle == NULL) {
		CloseServiceHandle(scmHandle);
		return 0;
	}

	// Query the service status.
	SERVICE_STATUS_PROCESS ssp;
	DWORD bytesNeeded;
	if (!QueryServiceStatusEx(serviceHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {

	}
	else {
		if (ssp.dwCurrentState == SERVICE_STOPPED) {
			lRet = -1;
		}
	}

	// Close handles.
	CloseServiceHandle(serviceHandle);
	CloseServiceHandle(scmHandle);
	return lRet;
}

unsigned long startService(const wchar_t* p_wszName)
{
	unsigned long lRet = 0;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;
	SERVICE_STATUS ssSvcStatus = {};

	// Open the local default service control manager database
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager == NULL)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		lRet = GetLastError();
		goto Cleanup;
	}

	// Open the service with delete, stop, and query status permissions
	schService = OpenService(schSCManager, p_wszName, SERVICE_START);
	if (schService == NULL)
	{
		wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
		lRet = GetLastError();
		goto Cleanup;
	}

	if (StartService(schService, 0, NULL) == false) {
		wprintf(L"StartService failed w/err 0x%08lx\n", GetLastError());
		lRet = GetLastError();
		goto Cleanup;
	}

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
		schSCManager = NULL;
	}
	if (schService)
	{
		CloseServiceHandle(schService);
		schService = NULL;
	}
	return lRet;
}

unsigned int TF_THREAD_CHK(void*) {
	do 
	{
		Sleep(1000 * 5);

		//.	check mi-...service is runing or not.
		if (checkServiceStatus(L"kenan-agent") != 0) {
			startService(L"kenan-agent");
		}

	} while (TRUE);
}

unsigned int TF_THREAD(void*) {
	{
		CString strPath; strPath.Format(L"%s\\kagent.exe", ClaPathMgr::GetDP());
		DWORD dwPID = ClaProcess::FindProcessID(L"kagent.exe");
		if (dwPID != 0) {
			TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID), 0);
		}

		ClaFwMgr::DeleteRule(L"KenanClientRule", TRUE);
		ClaFwMgr::AddAppRule(L"KenanClientRule", L"Application rule for kenan agent", strPath.GetBuffer(), TRUE);
	}
	{
		CString strPath; strPath.Format(L"%s\\KenanAgent.exe", ClaPathMgr::GetDP());

		ClaFwMgr::DeleteRule(L"KenanAgentRule", TRUE);
		ClaFwMgr::AddAppRule(L"KenanAgentRule", L"Application rule for kenan agent", strPath.GetBuffer(), TRUE);
	}

	wchar_t wszDir[MAX_PATH]; memset(wszDir, 0, sizeof(wszDir));
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));

	GetModuleFileName(NULL, wszDir, MAX_PATH);
	wchar_t* pSlash = wcsrchr(wszDir, L'\\');
	if (pSlash != NULL) pSlash[0] = 0x0;
	swprintf_s(wszPath, MAX_PATH, L"%s\\kenanagent.exe", wszDir);

	STARTUPINFO info = { sizeof(info) };
	info.cb = sizeof(info);
	info.dwFlags = STARTF_USESHOWWINDOW;
	info.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION processInfo;
	while (TRUE) {
		if (ClaProcess::CreateProcessAsTarget(L"winlogon.exe", wszPath, L"", wszDir, 0, &info, &processInfo) == 0) {
			lv_hProcess = processInfo.hProcess;
			break;
		}
	}

	if (lv_hProcess != INVALID_HANDLE_VALUE && lv_hProcess != NULL) {
		WaitForSingleObject(lv_hProcess, INFINITE);
	}
	return 0;
}

std::string getIpScope(const wchar_t* p_wszIP)
{
	try {
		int a, b, c, d;
		swscanf_s(p_wszIP, L"%d.%d.%d.%d", &a, &b, &c, &d);

		char szScope[100]; memset(szScope, 0, sizeof(szScope));
		sprintf_s(szScope, "0.0.0.0-%d.%d.%d.%d,%d.%d.%d.%d-255.255.255.255",
			a, b, c, d-1, a, b, c, d+1%256
			);
		return szScope;
	}
	catch (std::exception e) {
		return "0.0.0.0-255.255.255.255";
	}
}

unsigned int TF_THREAD_FWMGR(void*) {
	do
	{
		//if (ClaFwMgr::IsFirewallEnabled() == false) {
			ClaFwMgr::EnableFirewall();
		//}
		if (ClaFwMgr::IsExistRule(L"KenanBlockRule", true) == FALSE) {
			ClaFwMgr::AddPortRule(L"KenanBlockRule", L"Block all ports without agent", "0-18079,18081-65535", true);
		}
		else {
			ClaFwMgr::UpdatePortRule(L"KenanBlockRule", L"Block all ports without agent", "0-18079,18081-65535", true);
		}

		ClaRegMgr reg(HKEY_LOCAL_MACHINE);
		wchar_t wszIP[260]; memset(wszIP, 0, sizeof(wszIP));
		if (reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\Windows Search", L"fw", wszIP, 260) != 0
			|| wszIP[0] == 0)
		{
			ClaFwMgr::DeleteRule(L"KenanOutRule", false);
			ClaFwMgr::DeleteRule(L"KenanOutRulePort", false);
		}
		else {
			if (ClaFwMgr::IsExistRule(L"KenanOutRule", false) == FALSE) {
				ClaFwMgr::AddRemoteIpRule(L"KenanOutRule", L"Block all except host ip", getIpScope(wszIP), false);
			}
			else {
				ClaFwMgr::UpdateRemoteIpRule(L"KenanOutRule", L"Block all except host ip", getIpScope(wszIP), false);
			}
			if (ClaFwMgr::IsExistRule(L"KenanOutRulePort", false) == FALSE) {
				ClaFwMgr::AddPortRule(L"KenanOutRulePort", L"Block all except 443", "0-16442,16445-65535", false);
			}
			else {
				ClaFwMgr::UpdatePortRule(L"KenanOutRulePort", L"Block all except 443", "0-16442,16445-65535", false);
			}
		}

		Sleep(1000);
	} while (TRUE);
}


void RunService() {

	if (ClaVM::is_on_vm() == false) {
		return;
	}

	DWORD dwTID;
	lv_thdServiceChk = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_THREAD_CHK, NULL, 0, &dwTID);
	lv_thdService = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_THREAD, NULL, 0, &dwTID);
	lv_thdFwMgr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_THREAD_FWMGR, NULL, 0, &dwTID);
	WaitForSingleObject(lv_thdService, INFINITE);


}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
	//OutputDebugStringA("SfT - Service Main\n");

	// Register service control handler
	lv_hServiceStatus = RegisterServiceCtrlHandler(GD_SVC_NAME, ServiceCtrlHandler);
	if (!lv_hServiceStatus) {
		return;
	}

	// Notify service controller that the service is starting
	SERVICE_STATUS serviceStatus;
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;
	SetServiceStatus(lv_hServiceStatus, &serviceStatus);

	// Perform initialization tasks here

	// Notify service controller that the service is running
	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(lv_hServiceStatus, &serviceStatus);

	// Run the service
	RunService();

//	_execute(L"cmd.exe", L"/C shutdown -f -t 00 -s");

	// Notify service controller that the service has stopped
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(lv_hServiceStatus, &serviceStatus);
}

#include "ClaRegMgr.h"

int main()
{
	//OutputDebugStringA("SfT---Start");
	wchar_t p[] = GD_SVC_NAME;

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.deleteValue(L"SOFTWARE\\Microsoft\\Windows\\Windows Search", L"fw");

	SERVICE_TABLE_ENTRYW ServiceTable[] =
	{
		{ (LPWSTR)GD_SVC_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	// Start the service control dispatcher
	if (!StartServiceCtrlDispatcher(ServiceTable)) {
		//		std::cerr << "Failed to start service control dispatcher\n";
		//OutputDebugStringA("SfT---exit");
		return GetLastError();
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
