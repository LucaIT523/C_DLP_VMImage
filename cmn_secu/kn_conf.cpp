#include "pch.h"
#include "kn_conf.h"
#include "ClaRegMgr.h"
#include "ClaPathMgr.h"
#include "ClaFileProperty.h"
#include "kncrypto.h"

typedef struct tagKenanConf {
	union {
		struct {
			ST_SETTING m_stSetting;
			ST_CONF_APP m_stConfApp[20];
		};
		unsigned char m_pBuff[40960];
	};
}ST_KENAN_CONF;

ST_KENAN_CONF lv_stConf = { 0, };

wchar_t lv_wszConfPath[MAX_PATH] = L"";
wchar_t* _getPath() {

	if (lv_wszConfPath[0] == 0x0) {
		wchar_t wszPath[MAX_PATH];
		HRESULT hr = SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, wszPath);

		swprintf_s(lv_wszConfPath, L"%s\\kenansecu.conf", wszPath);
	}
	return lv_wszConfPath;
}

bool conf_exist()
{
	if (GetFileAttributes(_getPath()) == INVALID_FILE_ATTRIBUTES)
		return false;
	return true;
}

bool conf_create_default()
{
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	memset(&lv_stConf, 0, sizeof(lv_stConf));

	//
	//.	Setting
	//
	lv_stConf.m_stSetting.m_bFile = TRUE;
	lv_stConf.m_stSetting.m_bCapture = TRUE;
	lv_stConf.m_stSetting.m_bClipboard = TRUE;
	lv_stConf.m_stSetting.m_bPrint2PDF = FALSE;
	lv_stConf.m_stSetting.m_bPrint2Dev = FALSE;
	lv_stConf.m_stSetting.m_bGlobal = FALSE;
	lv_stConf.m_stSetting.m_bLog = FALSE;
	lv_stConf.m_stSetting.m_dwLogLevel = 0xFFFFFFFF;
	lv_stConf.m_stSetting.m_dwLogArea = 0xFFFFFFFF;
	//.	random create
	conf_get_new_key(lv_stConf.m_stSetting.m_pKey);

	//
	//.	Configuration
	//

	//.	notepad
	int nIndex = 0;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Notepad");
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszPath, L"C:\\windows\\system32\\notepad.exe");
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	paint
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Paint");
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszPath, L"C:\\windows\\system32\\mspaint.exe");
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	notepad++
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Notepad++");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\notepad++.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	acrobat reader
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Acrobat Reader");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Acrobat.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	MS Word
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Microsoft Office Word");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\winword.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	MS Excel
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Microsoft Office Excel");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\excel.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	MS PowerPoint
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Microsoft Office PowerPoint");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\powerpnt.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	MS Outlook
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Microsoft Office Outlook");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\OUTLOOK.EXE",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	//.	MS Visio
//	nIndex++;
//	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Acrobat Reader");
//	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Acrobat.exe",
//		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);

	//.	Google Chrome
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Google Chrome");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"SAP");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\saplogon.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;
/*
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Foxit Reader");
	reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\FoxitPDFReader.exe",
		L"", lv_stConf.m_stConfApp[nIndex].m_wszPath);
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;
*/
	nIndex++;
	wcscpy_s(lv_stConf.m_stConfApp[nIndex].m_wszName, L"Eclipse");
	lv_stConf.m_stConfApp[nIndex].m_nStatus = 0xFF;

	for (int i = 0; i <= nIndex; i++) {
		conf_get_app_info(
			lv_stConf.m_stConfApp[i].m_wszPath,
			lv_stConf.m_stConfApp[i].m_szInfo,
			1024
		);
	}

	return true;
}

bool conf_load(LPCWSTR p_wszPwd /*= L"123456"*/)
{
	FILE* pFile;

	_wfopen_s(&pFile, _getPath(), L"rb");
	if (pFile == NULL) return false;

	size_t r = fread(&lv_stConf, 1, sizeof(lv_stConf), pFile);
	fclose(pFile);

	if (r != sizeof(lv_stConf))
		return false;

	kn_decrypt_with_pwd((unsigned char*)&lv_stConf, sizeof(lv_stConf), p_wszPwd);

	return true;
}

bool conf_save(LPCWSTR p_wszPwd /*= L"123456"*/)
{
	FILE* pFile;

	_wfopen_s(&pFile, _getPath(), L"wb");
	if (pFile == NULL) return false;

	unsigned char* pBuff = (unsigned char*)malloc(sizeof(lv_stConf));
	memcpy(pBuff, &lv_stConf, sizeof(lv_stConf));
	kn_encrypt_with_pwd((unsigned char*)pBuff, sizeof(lv_stConf), p_wszPwd);

	fwrite(pBuff, 1, sizeof(lv_stConf), pFile);
	fclose(pFile);

	free(pBuff);

	return true;
}

void conf_get_new_key(unsigned char* p_pKey)
{
	for (int i = 0; i < 32; i++) {
		p_pKey[i] = ((rand() * GetTickCount()) & 0xFF);
	}
}

bool conf_get_app_info(const wchar_t* p_wszAppPath, char* p_szInfo, unsigned int p_cchInfo)
{
	CString strDescription	= ClaFileProperty::GetProperty(p_wszAppPath, L"FileDescription");
	CString strProduct		= ClaFileProperty::GetProperty(p_wszAppPath, L"ProductName");
	CString strVersion		= ClaFileProperty::GetProperty(p_wszAppPath, L"ProductVersion");
	CString strOrgName		= ClaFileProperty::GetProperty(p_wszAppPath, L"OriginalFilename");

	sprintf_s(p_szInfo, p_cchInfo, 
		"Descrition		: %S\n"
		"Product Name		: %S\n"
		"Product Version	: %S\n"
		"Original Name		: %S\n"
		, strDescription, strProduct, strVersion, strOrgName);
	return true;
}

int conf_get_app_count(void)
{
	int nCount = 0;
	for (int i = 0; i < 20; i++) {
		if (lv_stConf.m_stConfApp[i].m_nStatus != 0) {
			nCount++;
		}
	}
	return nCount;
}

ST_CONF_APP* conf_get_app_info_ptr(int p_nIndex)
{
	if (p_nIndex > 40) return NULL;
	return &lv_stConf.m_stConfApp[p_nIndex];
}

ST_SETTING* conf_get_setting_ptr()
{
	return &lv_stConf.m_stSetting;
}