#pragma once

#include <string>

class ClaOSInfo
{
public:
	ClaOSInfo() {};
	~ClaOSInfo() {};

public:
	static BOOL is64bitWindows();
	static std::string getOS();
	static int getCpuInfo(int* p_nCoreCnt, int* p_nProcessors, int *p_nSpeed);
	static int getRamInfo(unsigned long long* p_nTotal);
	static int getDriveSize(const wchar_t* p_wszDrv, unsigned long long* p_lTotal, unsigned long long* p_lFree);
	static int getAntivirusList(std::wstring& p_strAVs);
};

