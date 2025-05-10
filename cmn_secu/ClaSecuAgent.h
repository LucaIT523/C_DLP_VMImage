#pragma once

typedef struct tagAgentMap
{
	union {
		struct {
			int m_nPID;
			wchar_t m_wszPath[MAX_PATH];
		};
		unsigned char m_pBuff[1024];
	};
}ST_AGENT_MAP;

class ClaSecuAgent
{
public:
	ClaSecuAgent() {};
	~ClaSecuAgent() {};

public:
	static unsigned int SetSignalToAgent(unsigned int p_nPID, const wchar_t* p_wszPath, BOOL p_b64);
	static unsigned int WaitSignalFromClient(unsigned int &p_nPID, wchar_t* p_wszPath);
	static unsigned int SetSignalToClient();
	static unsigned int WaitAgentRunning(int p_nSleepTime);
protected:
private:
};

void* OpenFileMapAgent();