#include "pch.h"
#include "ClaSecuAgent.h"
#include "ClaFileMap.h"
#include "ClaSecurityDescriptor.h"
#include "ClaPathMgr.h"
#include <cstdio>
#include <iostream>

#define LD_NAME_MAP_AGENT			L"Global\\KenanMap-AgentPol"
#define LD_NAME_EVENT_AGENT			L"Global\\KenanEvent-Agent"
#define LD_NAME_EVENT_AGENT64		L"Global\\KenanEvent-Agent64"
#define LD_NAME_EVENT_AGENT32		L"Global\\KenanEvent-Agent32"

ClaFileMap lv_filemapAgent;
void* lv_pSharedMemAgent = NULL;

void* OpenFileMapAgent() {
	if (lv_pSharedMemAgent == NULL) {
		if (ClaFileMap::IsCreateMap(LD_NAME_MAP_AGENT) == true) {
			lv_filemapAgent.openMap(LD_NAME_MAP_AGENT, &lv_pSharedMemAgent);
		}
		else {
			lv_filemapAgent.createMap(LD_NAME_MAP_AGENT, sizeof(ST_AGENT_MAP), &lv_pSharedMemAgent);
		}
	}
	return lv_pSharedMemAgent;
}

unsigned int ClaSecuAgent::SetSignalToAgent(unsigned int p_nPID, const wchar_t* p_wszPath, BOOL p_b64)
{
	ST_AGENT_MAP* pMap = (ST_AGENT_MAP*)OpenFileMapAgent();
	if (pMap == NULL) return -1;

	pMap->m_nPID = p_nPID;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(EVENT_ALL_ACCESS);
	HANDLE hRcv = CreateEvent(&sa, FALSE, FALSE, LD_NAME_EVENT_AGENT);

	HANDLE h;
	if (p_b64)
		h = OpenEvent(EVENT_ALL_ACCESS, FALSE, LD_NAME_EVENT_AGENT64);
	else
		h = OpenEvent(EVENT_ALL_ACCESS, FALSE, LD_NAME_EVENT_AGENT32);

	if (h == INVALID_HANDLE_VALUE || h == NULL) {
		return -1;
	}

	BOOL bSts = SetEvent(h);
	CloseHandle(h);
	WaitForSingleObject(hRcv, INFINITE);

	return 0;
}

unsigned int ClaSecuAgent::WaitSignalFromClient(unsigned int& p_nPID, wchar_t* p_wszPath)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(EVENT_ALL_ACCESS);

#ifdef _WIN64
	HANDLE h = CreateEvent(&sa, FALSE, FALSE, LD_NAME_EVENT_AGENT64);
#else
	HANDLE h = CreateEvent(&sa, FALSE, FALSE, LD_NAME_EVENT_AGENT32);
#endif
	delete sa.lpSecurityDescriptor;

	if (h == INVALID_HANDLE_VALUE || h == NULL) {
		return -1;
	}

	WaitForSingleObject(h, INFINITE);

	ST_AGENT_MAP* pMap = (ST_AGENT_MAP*)OpenFileMapAgent();
	if (pMap == NULL) return -1;

	p_nPID = pMap->m_nPID;
	wcscpy_s(p_wszPath, MAX_PATH, pMap->m_wszPath);
	return 0;
}

unsigned int ClaSecuAgent::SetSignalToClient()
{
	HANDLE h;
	h = OpenEvent(EVENT_ALL_ACCESS, FALSE, LD_NAME_EVENT_AGENT);
	SetEvent(h);
	return 0;
}

unsigned int ClaSecuAgent::WaitAgentRunning(int p_nSleepTime)
{
	unsigned int nRet = -1;
	DWORD dwTickEnd = GetTickCount() + p_nSleepTime;


	while (dwTickEnd > GetTickCount()) {
		Sleep(100);
		HANDLE h;
		h = OpenEvent(EVENT_ALL_ACCESS, FALSE, LD_NAME_EVENT_AGENT64);
		if (h == INVALID_HANDLE_VALUE || h == NULL) {
			continue;
		}
		CloseHandle(h);

		h = OpenEvent(EVENT_ALL_ACCESS, FALSE, LD_NAME_EVENT_AGENT32);
		if (h == INVALID_HANDLE_VALUE || h == NULL) {
			continue;
		}
		CloseHandle(h);
		nRet = 0;
		break;
	}
	return nRet;
}
