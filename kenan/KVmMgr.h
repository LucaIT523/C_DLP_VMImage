#pragma once

#define WM_USER_VM_LAUNCHED		(WM_USER+0x0101)
#define WM_USER_VM_STOPED		(WM_USER+0x0102)
#define WM_USER_VM_SUSPENDED	(WM_USER+0x0103)

class KVmMgr
{
public:
	KVmMgr() {};
	~KVmMgr() {};

public:
	void _vm_process(void);
	void initialize(HWND p_hwndMain);
	int encryptVMX(const wchar_t* p_wszPath, const char* p_szOld, const char* p_szNew);
	int launchVM(const wchar_t* p_wszPath, const char* p_szPwd, const char* p_szEncPwd);
	int stopVM(void);
	int suspendVM();
	int getIP(char* p_szIP, int size);
	bool isStart() { return !m_strVMX.IsEmpty(); };

	static BOOL IsRunning(const wchar_t* p_wszPath, const wchar_t* p_wszPwd);
	BOOL isRunning() {
		if (m_strVMX.IsEmpty()) return FALSE;
		return IsRunning(m_strVMX, m_strPWD);
	};
	BOOL isWorking();

protected:
	int _launchVM(const wchar_t* p_wszPath, const wchar_t* p_wszPwd);
	int _stopVM(void);
	int _suspendVM();

protected:
	HWND _hwndMain;

public:
	CString m_strVMX;
	CString m_strPWD;
};

