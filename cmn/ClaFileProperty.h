#pragma once

class ClaFileProperty
{
public:
	ClaFileProperty();
	~ClaFileProperty();

public:
	static CString GetProperty(const wchar_t* p_wszPath, const wchar_t* p_wszPropertyName);

};

