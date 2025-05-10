#pragma once
class ClaNetAdapter
{
public:
	ClaNetAdapter() {};
	~ClaNetAdapter() {};

public :
	static CString GetIP();
	static CString GetIPs();
	static CString GetMAC();
};

