#include "pch.h"
#include "ClaSecuSvc.h"
#include "ClaSvcMgr.h"
#include "ClaFileMap.h"
#include "ClaSecurityDescriptor.h"
#include "ClaPathMgr.h"
#include <cstdio>
#include <iostream>

#define LD_NAME_MAP_SVC			L"Global\\KenanMap-SvcPol"

ClaFileMap lv_filemapSvc;
void* lv_pSharedMemSvc = NULL;

void* OpenFileMapSvc() {
	if (lv_pSharedMemSvc == NULL) {
		if (ClaFileMap::IsCreateMap(LD_NAME_MAP_SVC) == true) {
			lv_filemapSvc.openMap(LD_NAME_MAP_SVC, &lv_pSharedMemSvc);
		}
		else {
			lv_filemapSvc.createMap(LD_NAME_MAP_SVC, sizeof(ST_SVC_MAP), &lv_pSharedMemSvc);
		}
	}
	return lv_pSharedMemSvc;
}

unsigned int ClaSecuSvc::Launch(
	const wchar_t* p_wszTargetProc, 
	const wchar_t* p_wszPath, 
	const wchar_t* p_wszCmd, 
	const wchar_t* p_wszWorkDir)
{
	ClaSvcMgr mgr(NULL,
		L"KenanSecuSvc",
		L"Kenan Service for Security Environment",
		L"This is service program for Kenan DLP system. It will launch the Launcher program with the same privilege of WinLogon.exe");
	wchar_t wszWorkDir[MAX_PATH]; memset(wszWorkDir, 0, sizeof(wszWorkDir));
	wcscpy_s(wszWorkDir, MAX_PATH, ClaPathMgr::GetDP(p_wszPath));
	if (p_wszWorkDir)
		Register(p_wszTargetProc, p_wszPath, p_wszCmd, p_wszWorkDir);
	else
		Register(p_wszTargetProc, p_wszPath, p_wszCmd, wszWorkDir);
	mgr.sendevent(128);
	return 0;
}

unsigned int ClaSecuSvc::Register(
	const wchar_t* p_wszTargetProc, 
	const wchar_t* p_wszPath, 
	const wchar_t* p_wszCmd, 
	const wchar_t* p_wszWorkDir)
{
	ST_SVC_MAP* pMap = (ST_SVC_MAP*)OpenFileMapSvc();
	if (pMap == NULL) return -1;

	memset(pMap, 0, sizeof(ST_SVC_MAP));
	if (p_wszTargetProc) {
		wcscpy_s(pMap->m_wszTargetProc, 64, p_wszTargetProc);
	}
	if (p_wszPath) {
		wcscpy_s(pMap->m_wszPath, MAX_PATH, p_wszPath);
	}
	if (p_wszCmd) {
		wcscpy_s(pMap->m_wszCmd, MAX_PATH, p_wszCmd);
	}
	if (p_wszWorkDir) {
		wcscpy_s(pMap->m_wszWorkDir, MAX_PATH, p_wszWorkDir);
	}
	ProcessIdToSessionId(GetCurrentProcessId(), (DWORD*)&pMap->m_nSessID);

	return 0;
}
unsigned int ClaSecuSvc::Get(
	wchar_t* p_wszTargetProc, 
	wchar_t* p_wszPath, 
	wchar_t* p_wszCmd, 
	wchar_t* p_wszWorkDir)
{
	ST_SVC_MAP* pMap = (ST_SVC_MAP*)OpenFileMapSvc();
	if (pMap == NULL) return -1;

	if (p_wszTargetProc) {
		wcscpy_s(p_wszTargetProc, 64, pMap->m_wszTargetProc);
	}
	if (p_wszPath) {
		wcscpy_s(p_wszPath, MAX_PATH, pMap->m_wszPath);
	}
	if (p_wszCmd) {
		wcscpy_s(p_wszCmd, MAX_PATH, pMap->m_wszCmd);
	}
	if (p_wszWorkDir) {
		wcscpy_s(p_wszWorkDir, MAX_PATH, pMap->m_wszWorkDir);
	}

	return 0;
}
