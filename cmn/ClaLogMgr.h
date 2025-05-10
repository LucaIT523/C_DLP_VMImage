#pragma once

#define LOG_LEVEL_INFO		(1 << 0)
#define LOG_LEVEL_WARNING	(1 << 1)
#define LOG_LEVEL_ERROR		(1 << 2)

class ClaLogMgr
{
public:
	ClaLogMgr() {
		_pTemp = (wchar_t*)malloc(1024);
		_pTemp1 = (wchar_t*)malloc(1024);
		_level = 0; _wszPath[0] = 0; _bOutDebug = FALSE;
		InitializeCriticalSection(&sync_section);
	};
	~ClaLogMgr() { 
		free(_pTemp); 
		free(_pTemp1);
	};

public:
	unsigned long _level;
	wchar_t _wszPath[260];
	BOOL _bOutDebug;
	wchar_t* _pTemp;
	wchar_t* _pTemp1;
	CRITICAL_SECTION sync_section;

public:
	void loadFromIni();
	void setLevel(unsigned long l) { _level = l; };
	void setPath(const wchar_t* p_wszPath) { wcscpy_s(_wszPath, 260, p_wszPath); };
	void setOutFlg(BOOL p_bFlg) { _bOutDebug = p_bFlg; };

	void LogFmtW(unsigned long level, const wchar_t* p_wszFmt, ...);
};

