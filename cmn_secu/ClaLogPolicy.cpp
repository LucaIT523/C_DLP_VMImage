#include "pch.h"
#include "ClaLogPolicy.h"
#include "ClaFileMap.h"
#include "ClaSecurityDescriptor.h"
#include "ClaPathMgr.h"
#include <cstdio>
#include <iostream>

#define LD_NAME_MAP_LOG			L"Global\\KenanMap-LogPol"

ClaFileMap lv_filemapLog;
void* lv_pSharedMemLog = NULL;

void* OpenFileMapLog() {
	if (lv_pSharedMemLog == NULL) {
		if (ClaFileMap::IsCreateMap(LD_NAME_MAP_LOG) == true) {
			lv_filemapLog.openMap(LD_NAME_MAP_LOG, &lv_pSharedMemLog);
		}
		else {
			lv_filemapLog.createMap(LD_NAME_MAP_LOG, sizeof(ST_LOG_MAP), &lv_pSharedMemLog);
		}
	}
	return lv_pSharedMemLog;
}

unsigned int ClaLogPolicy::RegisterLogPolicy(BOOL p_bLog, DWORD p_dwLogLevel, DWORD p_dwLogArea) 
{
	ST_LOG_MAP* pMap = (ST_LOG_MAP*)OpenFileMapLog();
	if (pMap == NULL) return -1;

	pMap->m_bLog = p_bLog;
	pMap->m_dwLogLevel = p_dwLogLevel;
	pMap->m_dwLogArea = p_dwLogArea;

	return 0;
}

unsigned int ClaLogPolicy::GetLogPolicy(BOOL* p_bLog, DWORD* p_dwLogLevel, DWORD* p_dwLogArea){
	ST_LOG_MAP* pMap = (ST_LOG_MAP*)OpenFileMapLog();
	if (pMap == NULL) return -1;

	*p_bLog			= pMap->m_bLog;
	*p_dwLogLevel	= pMap->m_dwLogLevel;
	*p_dwLogArea	= pMap->m_dwLogArea;

	return 0;
}
