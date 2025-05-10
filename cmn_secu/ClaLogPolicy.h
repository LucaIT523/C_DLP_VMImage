#pragma once

typedef struct tagLogMap
{
	union {
		struct {
			BOOL m_bLog;
			DWORD m_dwLogLevel;
			DWORD m_dwLogArea;
		};
		unsigned char m_pBuff[512];
	};
}ST_LOG_MAP;

class ClaLogPolicy
{
public:
	ClaLogPolicy() {};
	~ClaLogPolicy() {};

public:
	static unsigned int RegisterLogPolicy(BOOL p_bLog, DWORD p_dwLogLevel, DWORD p_dwLogArea);
	static unsigned int GetLogPolicy(BOOL *p_bLog, DWORD *p_dwLogLevel, DWORD *p_dwLogArea);
protected:
private:
};
