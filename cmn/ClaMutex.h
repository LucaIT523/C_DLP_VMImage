#pragma once

class ClaMutex
{
public:
	ClaMutex() {};
	~ClaMutex() {};

public:
	static bool IsCreate(const wchar_t* p_wszName);
	static HANDLE Create(const wchar_t* p_wszName);
};

