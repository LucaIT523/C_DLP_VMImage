#pragma once

typedef struct tagSvcMap
{
	union {
		struct {
			wchar_t m_wszTargetProc[64];
			wchar_t m_wszPath[MAX_PATH];
			wchar_t m_wszCmd[MAX_PATH];
			wchar_t m_wszWorkDir[MAX_PATH];
			int		m_nSessID;
		};
		unsigned char m_pBuff[1024];
	};
}ST_SVC_MAP;

class ClaSecuSvc
{
public:
	ClaSecuSvc() {};
	~ClaSecuSvc() {};

public:
	static unsigned int Launch(const wchar_t* p_wszTargetProc, const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir);
	static unsigned int Register(const wchar_t* p_wszTargetProc, const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir);
	static unsigned int Get(wchar_t* p_wszTargetProc, wchar_t* p_wszPath, wchar_t* p_wszCmd, wchar_t* p_wszWorkDir);
protected:
private:
};
