#pragma once

class IModel
{
public:
	IModel() {};
	~IModel() {};

public:
	virtual CString toString() = 0;
	virtual int fromString() = 0;
};


class ClaModelBase
{
public:
	ClaModelBase() {};
	~ClaModelBase() {};
};

