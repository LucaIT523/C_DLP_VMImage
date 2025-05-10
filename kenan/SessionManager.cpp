#include "pch.h"
#include "SessionManager.h"
#include "ClaRegMgr.h"

void SessionManager::clearToken()
{
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);

	memset(_szToken, 0, sizeof(_szToken));
	reg.deleteValue(L"Software\\kenan", L"credential");
}

void SessionManager::saveToken(void)
{
	wchar_t wszToken[200]; memset(wszToken, 0, sizeof(wszToken));
	swprintf_s(wszToken, 200, L"%S", _szToken);

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.createKey(L"Software\\Kenan");
	reg.createKey(L"Software\\Kenan\\list");
	reg.writeStringW(L"Software\\Kenan", L"credential", wszToken);
}

void SessionManager::loadToken(void)
{
	memset(_szToken, 0, sizeof(_szToken));

	wchar_t wszToken[MAX_PATH]; memset(wszToken, 0, sizeof(wszToken));

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	if (reg.readStringW(L"Software\\Kenan", L"credential", wszToken) == 0) {
		sprintf_s(_szToken, 200, "%S", wszToken);
	}
}

void SessionManager::clearList(void)
{
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);

	reg.deleteKey(L"Software\\kenan\\list");
	reg.createKey(L"Software\\kenan\\list");
}

void SessionManager::addList(int p_nIndex, const char* p_szName, const char* p_szDesc, const char* p_szSize)
{
	CString strIndex; strIndex.Format(L"%d", p_nIndex);
	CString strData; strData.Format(L"%S;%S;%S", p_szName, p_szDesc, p_szSize);

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);

	reg.writeStringW(L"Software\\kenan\\list", strIndex, strData);

	CString strCount; strCount.Format(L"%d", p_nIndex + 1);
	reg.writeStringW(L"Software\\kenan\\list", L"count", strCount);
}

int SessionManager::getListCount(void)
{
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	wchar_t wszCount[100]; memset(wszCount, 0, sizeof(wszCount));
	reg.readStringW(L"Software\\kenan\\list", L"count", wszCount, 100);

	return _wtoi(wszCount);
}

void SessionManager::getList(int p_nIndex, CString& p_szName, CString& p_szDesc, CString& p_szSize)
{
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	CString strIndex; strIndex.Format(L"%d", p_nIndex);
	wchar_t wszData[2024]; memset(wszData, 0, sizeof(wszData));
	reg.readStringW(L"Software\\kenan\\list", strIndex, wszData, 2024);

	if (wszData[0] == 0) return;

	wchar_t* pPos = wcschr(wszData, L';');
	if (pPos) pPos[0] = 0;
	p_szName = wszData;

	pPos = &pPos[1];
	wchar_t* p1 = wcschr(pPos, L';'); if (p1) p1[0] = 0;
	p_szDesc = pPos;

	p_szSize = &p1[1];

}
