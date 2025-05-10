#pragma once

class ClaSecurityDescriptor
{
public:
	ClaSecurityDescriptor();
	~ClaSecurityDescriptor();

public:
	static PSECURITY_DESCRIPTOR GetSecurityDescriptor(DWORD p_dwPermission);

private:

};

