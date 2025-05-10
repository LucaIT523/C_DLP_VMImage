#pragma once

class ClaEvent
{
public:
	ClaEvent() {};
	~ClaEvent() {};

public:
	static bool IsCreate(const wchar_t* p_wszName);
	static HANDLE Create(const wchar_t* p_wszName);
	static HANDLE Open(const wchar_t* p_wszName);
};

