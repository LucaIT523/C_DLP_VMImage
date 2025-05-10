#pragma once

typedef struct tagSecuMap
{
	union {
		struct {
			BOOL m_bSecuClipboard;
			wchar_t m_wszSecu[64];
			wchar_t m_wszCrypto[100][MAX_PATH];
			int m_nCrypo;
			wchar_t m_wszExcept[100][MAX_PATH];
			int m_nExcept;
			wchar_t m_wszTemp[MAX_PATH];
		};
		unsigned char m_pBuff[40960];
	};
}ST_SECU_MAP;

class ClaSecuProcess
{
public:
	ClaSecuProcess() {};
	~ClaSecuProcess() {};

public:
	static unsigned int RegisterSecuProcess(unsigned int p_nPID);
	static BOOL IsSecuProcess(unsigned int p_nPID);
	static unsigned int RegisterClipboardFlag(BOOL p_bSecu);
	static BOOL IsSecuClipboard(void);
	static void InjectDll(HANDLE p_hProcess, LPCWSTR p_wszDllPath = NULL);

	static unsigned int RegisterSecuPath(const wchar_t* p_wszPath);
	static BOOL IsSecuPathW(const wchar_t* p_wszPath);
	static BOOL IsSecuPathA(const char* p_szPath);
	static unsigned int ClearCryptoPath();
	static unsigned int AddCryptoPath(const wchar_t* p_wszPath);
	static BOOL IsCryptoPathW(const wchar_t* p_wszPath);
	static BOOL IsCryptoPathA(const char* p_szPath);
	static unsigned int ClearExceptPath();
	static unsigned int AddExceptPath(const wchar_t* p_wszPath);
	static BOOL IsExceptPathW(const wchar_t* p_wszPath);
	static BOOL IsExceptPathA(const char* p_szPath);
	static unsigned int RegisterTempPath(const wchar_t* p_wszPath);
	static unsigned int GetTempFilePathW(wchar_t* p_wszPath);
	static unsigned int GetTempFilePathA(char* p_wszPath);
protected:
private:
};
