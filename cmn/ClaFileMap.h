#pragma once

class ClaFileMap
{
public:
	ClaFileMap();
	~ClaFileMap();

public:
	static bool IsCreateMap(const wchar_t* p_wszName);
	unsigned int createMap(const wchar_t* p_wszName, int p_nSize, void** p_ppMem);
	unsigned int openMap(const wchar_t* p_wszName, void** p_ppMem);
	void closeMap();

protected:
	void* _pMem;
	HANDLE _hFilemap;
};

