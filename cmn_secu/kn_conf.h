#pragma once

typedef struct tagConfApp {
	wchar_t m_wszName[40];
	wchar_t m_wszPath[MAX_PATH];
	char m_szInfo[1024];
	unsigned int m_nStatus;
	int m_nIconIndex;
}ST_CONF_APP;

typedef struct tagSetting {
	BOOL m_bFile;
	BOOL m_bCapture;
	BOOL m_bClipboard;
	BOOL m_bPrint2PDF;
	BOOL m_bPrint2Dev;
	BOOL m_bGlobal;
	unsigned char m_pKey[32];
	BOOL m_bLog;
	DWORD m_dwLogLevel;
	DWORD m_dwLogArea;
}ST_SETTING;

bool conf_exist();
bool conf_create_default();
bool conf_load(LPCWSTR p_wszPwd = L"123456");
bool conf_save(LPCWSTR p_wszPwd = L"123456");

void conf_get_new_key(unsigned char* p_pKey);
bool conf_get_app_info(const wchar_t* p_wszAppPath, char* p_szInfo, unsigned int p_cchInfo);

int conf_get_app_count(void);
ST_CONF_APP* conf_get_app_info_ptr(int p_nIndex);
ST_SETTING* conf_get_setting_ptr();
