#include "pch.h"
#include "ClaLogMgr.h"
#include "ini.h"
#include "ClaPathMgr.h"

void ClaLogMgr::loadFromIni()
{
	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	std::string level = ini["log"]["level"];
	std::string path = ini["log"]["path"];
	std::string prt = ini["log"]["print"];

	_level = 0;

	if (level.find("debug") != std::string::npos) {
		_level = 0xFFFF;
	}
	else {
		if (level.find("info") != std::string::npos) _level |= LOG_LEVEL_INFO;
		if (level.find("warning") != std::string::npos) _level |= LOG_LEVEL_WARNING;
		if (level.find("error") != std::string::npos) _level |= LOG_LEVEL_ERROR;
	}
	if (path.length() != 0)
		swprintf_s(_wszPath, 260, L"%S", path.c_str());
	else
		_wszPath[0] = 0;

	_bOutDebug = atoi(prt.c_str());                                                                       
}

void ClaLogMgr::LogFmtW(unsigned long level, const wchar_t* p_wszFmt, ...)
{
	if ((level & _level) == 0) return;
	EnterCriticalSection(&sync_section);

	wchar_t wszFmt[MAX_PATH]; 
	swprintf_s(wszFmt, L"SfT - [%s] - %s\n", CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"), p_wszFmt);

	va_list vl;
	va_start(vl, p_wszFmt);

	if (_wszPath[0] != 0)
	{
		FILE* pFile; 
		errno_t e = _wfopen_s(&pFile, _wszPath, L"a");
		if (pFile != NULL) {
			vfwprintf(pFile, wszFmt, vl);
			fclose(pFile);
		}
	}
	va_end(vl);

	LeaveCriticalSection(&sync_section);
}
