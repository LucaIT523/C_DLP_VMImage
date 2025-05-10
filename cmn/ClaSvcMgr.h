#pragma once

class ClaSvcMgr
{
public:
	ClaSvcMgr(
		const wchar_t* p_wszPath,
		const wchar_t* p_wszNameShort,
		const wchar_t* p_wszNameLong,
		const wchar_t* p_wszDescription
	);

	unsigned long install();
	unsigned long start(int p_nArgCnt, LPCWSTR* p_ppArg);
	unsigned long stop();
	unsigned long uninstall();
	unsigned long sendevent(unsigned int p_nCode);

private:
	wchar_t m_wszName[MAX_PATH];
	wchar_t m_wszNameLong[MAX_PATH];
	wchar_t m_wszPath[MAX_PATH];
	wchar_t m_wszDescription[MAX_PATH];
};

