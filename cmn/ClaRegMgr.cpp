#include "pch.h"
#include "ClaRegMgr.h"
#include "ClaProcess.h"
#include <sddl.h>

void ClaRegMgr::createKey(LPCWSTR p_wszSubKey)
{
	HKEY newKey;
	LONG result = RegCreateKeyEx(_hRootKey, p_wszSubKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_32KEY, nullptr, &newKey, nullptr);
	if (result != ERROR_SUCCESS) {
		
	}

	return;
}

void ClaRegMgr::deleteKey(LPCWSTR p_wszSubKey)
{
	HKEY newKey;
	LONG result = RegDeleteKeyEx(_hRootKey, p_wszSubKey, KEY_WOW64_32KEY, 0);
	if (result != ERROR_SUCCESS) {

	}

	return;
}

bool ClaRegMgr::deleteValue(const wchar_t* subKey, const wchar_t* valueName) {
	// Open the registry key
	HKEY hKey;
	if (RegOpenKeyExW(_hRootKey, subKey, 0, KEY_SET_VALUE | KEY_WOW64_32KEY, &hKey) == ERROR_SUCCESS) {
		// Delete the specified value
		if (RegDeleteValueW(hKey, valueName) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return true;
		}
		else {
			RegCloseKey(hKey);
			return false;
		}
	}
	else {
		return false;
	}
}
unsigned long ClaRegMgr::writeStringW(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, const wchar_t* p_wszValue)
{
	HKEY openedKey;
	LONG result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_SET_VALUE | KEY_WOW64_32KEY, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_SZ, (const BYTE*)p_wszValue, (wcslen(p_wszValue) + 1) * sizeof(wchar_t));
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2;
	}
	return 0;
}

unsigned long ClaRegMgr::readStringW(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, wchar_t* p_wszValue, int p_nSize /*= MAX_PATH*/)
{
	HKEY hKey;
	LONG result;
	
	if (p_wszSubKey[0] == L's' || p_wszSubKey[0] == L'S')
	{
		result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_READ | KEY_WOW64_32KEY, &hKey);
		if (result != ERROR_SUCCESS) {
			return 1;
		}
	}
	else {
		result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS) {
			return 1;
		}
	}

	DWORD dataSize = 0;
	result = RegGetValue(hKey, nullptr, p_wszValueName, REG_SZ, nullptr, nullptr, &dataSize);
	if (result != ERROR_SUCCESS && result != 1630) {
		RegCloseKey(hKey);
//		std::wcerr << L"Failed to get size for value: " << p_wszValueName << L", Error: " << result << std::endl;
		return 2;
	}

	wchar_t* pTemp = (wchar_t*)malloc(dataSize);
	if (!pTemp) {
		RegCloseKey(hKey);
//		std::wcerr << L"Memory allocation failed." << std::endl;
		return 3;
	}

	result = RegGetValue(hKey, nullptr, p_wszValueName, REG_SZ, nullptr, pTemp, &dataSize);
	RegCloseKey(hKey);
	if (result != ERROR_SUCCESS && result != 1630) {
		free(pTemp);
//		std::wcerr << L"Failed to read string value: " << p_wszValueName << L", Error: " << result << std::endl;
		return 4;
	}

	wcsncpy_s(p_wszValue, p_nSize, pTemp, dataSize / sizeof(wchar_t));
	free(pTemp);
	return 0;
}

unsigned long ClaRegMgr::writeInt(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, DWORD p_dwValue)
{
	HKEY openedKey;
	LONG result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_SET_VALUE | KEY_WOW64_32KEY, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_DWORD, (const BYTE*)&p_dwValue, 4);
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2;
	}
	return 0;
}

unsigned long ClaRegMgr::readInt(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, DWORD& p_dwValue)
{
	HKEY hKey;
	LONG result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_READ | KEY_WOW64_32KEY, &hKey);
	if (result != ERROR_SUCCESS) {
		//		std::wcerr << L"Failed to open key: " << p_wszSubKey << L", Error: " << result << std::endl;
		return 1;
	}

	DWORD dataSize = 0;
	result = RegGetValue(hKey, NULL, p_wszValueName, REG_DWORD, nullptr, nullptr, &dataSize);
	if (result != ERROR_SUCCESS && result != 1630) {
		return 1;
	}
	if (dataSize != 4) return 2;

	DWORD dwValue;
	result = RegGetValue(hKey, NULL, p_wszValueName, REG_DWORD, nullptr, &dwValue, &dataSize);
	if (result != ERROR_SUCCESS && result != 1630) {
		return 3;
	}

	p_dwValue = dwValue;
	return 0;
}

unsigned long ClaRegMgr::writeBinary(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, const unsigned char* p_pBuff, DWORD p_dwSize) {
	HKEY openedKey;
	LONG result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_SET_VALUE | KEY_WOW64_32KEY, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1; // Error opening key
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_BINARY, p_pBuff, p_dwSize);
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2; // Error setting value
	}
	return 0; // Success
}

unsigned long ClaRegMgr::readBinary(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, unsigned char* p_pBuff, DWORD* p_dwSize) {
	HKEY openedKey;
	LONG result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_READ | KEY_WOW64_32KEY, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1; // Error opening key
	}

	DWORD type;
	result = RegQueryValueEx(openedKey, p_wszValueName, NULL, &type, p_pBuff, p_dwSize);
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS || type != REG_BINARY) {
		return 2; // Error reading value or wrong type
	}
	return 0; // Success
}

CString ClaRegMgr::getSIDString(void)
{
	DWORD dwSID;
	ProcessIdToSessionId(GetCurrentProcessId(), &dwSID);

	int nExpPID = ClaProcess::FindProcessID(L"explorer.exe", dwSID);
	HANDLE hExp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nExpPID);
	HANDLE hToken;

	if (!OpenProcessToken(hExp, TOKEN_QUERY, &hToken)) {
		return L"";
	}

	DWORD dwSize = 0;
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
	if (dwSize == 0) {
//		std::cerr << "Error getting token information size: " << GetLastError() << std::endl;
		CloseHandle(hToken);
		return L"";
	}

	PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwSize);
	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize)) {
//		std::cerr << "Error getting token information: " << GetLastError() << std::endl;
		free(pTokenUser);
		CloseHandle(hToken);
		return L"";
	}

	LPWSTR pSidString;
	if (!ConvertSidToStringSid(pTokenUser->User.Sid, &pSidString)) {
//		std::cerr << "Error converting SID to string: " << GetLastError() << std::endl;
		free(pTokenUser);
		CloseHandle(hToken);
		return L"";
	}

//	std::wcout << "SID: " << pSidString << std::endl;

	LocalFree(pSidString);
	free(pTokenUser);
	CloseHandle(hToken);
	return pSidString;
}

