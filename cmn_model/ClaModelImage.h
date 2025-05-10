#pragma once

#include "ClaModelBase.h"
#include "KVmMgr.h"

typedef enum _ImageStatus
{
	image_status_none = 0,
	image_status_downloadable = 1,
	image_status_runable = 2,
	image_status_download_runable = 3,
}ImageStatus;

class ClaModelImage : public IModel
{
public:
	ClaModelImage() { m_bRunning = false; };
	~ClaModelImage() {};

public:
	void test();

public:
	virtual CString toString();
	virtual int fromString();

public:
	int m_nID;
	CString m_strName;
	CString m_strOS;
	CString getPath() { return m_strPath; };
	CString getPassword() { return m_strPassword; };

public:
	CString m_strPath;
	ImageStatus m_nStatus;
	CString m_strPassword;
	BOOL m_bRunning;
	CString m_strSize;

public:
	bool isRunable(void) { return (!m_strPath.IsEmpty() && GetFileAttributes(m_strPath) != INVALID_FILE_ATTRIBUTES); };
	bool isRunning(void) { 
		if (isRunable() == FALSE) return FALSE; 
		return m_bRunning;
	};
	void refreshState() {
		m_bRunning = isRunable() && KVmMgr::IsRunning(m_strPath, m_strPassword);
	}
	CString getStatusString(void) {
		if (isRunning()) {
			return L"Running";
		}
		if (isRunable()) {
			return L"Runable";
		}
		return L"Downloadable";
	}
};

