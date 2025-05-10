#pragma once

#define GD_MAX_COUNT_PID	2000
typedef struct tagSecuMap
{
	union {
		struct {
			BOOL m_bSecuClipboard;
			wchar_t m_wszSecu[64];
			wchar_t m_wszTemp[MAX_PATH];
			wchar_t m_wszWorkDir[MAX_PATH];

			BOOL m_bDlpFile;
			BOOL m_bDlpCapture;
			BOOL m_bDlpClipboard;
			BOOL m_bDlpPrintPdf;
			BOOL m_bDlpPrintDev;
			BOOL m_bDlpGlobal;
			unsigned char m_pCryptoKey[32];

			unsigned int m_lPID[GD_MAX_COUNT_PID];
			unsigned int m_nPIDIndex;

			BOOL m_bStartFlag;
		};
		unsigned char m_pBuff[20000];
	};
}ST_SECU_MAP;

typedef struct tagSecuPathMap
{
	union {
		struct {
			wchar_t m_wszCrypto[30][MAX_PATH];
			int m_nCrypo;
			wchar_t m_wszExcept[30][MAX_PATH];
			int m_nExcept;
		};
		unsigned char m_pBuff[20000];
	};
}ST_SECU_PATH_MAP;

class ClaSecuPolicy
{
public:
	ClaSecuPolicy() {};
	~ClaSecuPolicy() {};

public:
	static unsigned int RegisterSecuProcess(unsigned int p_nPID);
	static BOOL IsSecuProcess(unsigned int p_nPID);
	static unsigned int RegisterClipboardFlag(BOOL p_bSecu);
	static BOOL IsSecuClipboard(void);
	static void InjectDll(HANDLE p_hProcess, LPCWSTR p_wszDllPath = NULL);
	static void InjectDllAsUser(int p_nPID, LPCWSTR p_wszDllPath = NULL);
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
	static BOOL IsTempPathW(const wchar_t* p_wszPath);
	static unsigned int GenerateTempFilePathW(wchar_t* p_wszPath);
	static unsigned int GenerateTempFilePathA(char* p_wszPath);

	static unsigned int SetStartFlag(BOOL p_bFlag);
	static BOOL GetStartFlag();

	static unsigned int SetDlpFile(BOOL p_bEnable);
	static unsigned int SetDlpClipboard(BOOL p_bEnable);
	static unsigned int SetDlpCapture(BOOL p_bEnable);
	static unsigned int SetDlpPrint(BOOL p_bEnablePdf, BOOL p_bEnableDev);
	static unsigned int SetDlpGlobalFlag(BOOL p_bEnable);
	static unsigned int SetDlpCryptoKey(unsigned char* p_pKey);
	static BOOL GetDlpFile();
	static BOOL GetDlpClipboard();
	static BOOL GetDlpCapture();
	static BOOL GetDlpPrintPdf();
	static BOOL GetDlpPrintDev();
	static BOOL GetDlpGlobalFlag();
	static unsigned int GetDlpCryptoKey(unsigned char* p_pKey);

	static unsigned int RegistCurPath();
	static unsigned int GetCurPath(wchar_t* p_wszPath);

	static unsigned int AddSecuProcess(unsigned int p_nPID);
	static bool IsAddSecuProcess(unsigned int p_nPID);
protected:
private:
};
