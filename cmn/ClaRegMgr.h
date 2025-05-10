#pragma once
class ClaRegMgr
{
public:
	ClaRegMgr(HKEY p_hRootKey = HKEY_LOCAL_MACHINE) { _hRootKey = p_hRootKey; };
	~ClaRegMgr() {};

	void createKey(LPCWSTR p_wszSubKey);
	void deleteKey(LPCWSTR p_wszSubKey);
	bool deleteValue(const wchar_t* subKey, const wchar_t* valueName);
protected:
	HKEY _hRootKey;

public:
	unsigned long writeStringW(const wchar_t* p_wszKey, const wchar_t* p_wszKeyName, const wchar_t* p_wszValue);
	unsigned long readStringW(const wchar_t* p_wszKey, const wchar_t* p_wszKeyName, wchar_t* p_wszValue, int p_nSize = MAX_PATH);
	unsigned long writeInt(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, DWORD p_dwValue);
	unsigned long readInt(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, DWORD& p_dwValue);

	unsigned long writeBinary(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, const unsigned char* p_pBuff, DWORD p_dwSize);
	unsigned long readBinary(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, unsigned char* p_pBuff, DWORD *p_pdwSize);

	static CString getSIDString(void);
};

