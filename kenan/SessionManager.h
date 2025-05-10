#pragma once

class SessionManager
{
public:
	SessionManager() { memset(_szToken, 0, sizeof(_szToken)); };
	~SessionManager() {};

public:
	void getToken(char* p_szToken) {
		strcpy_s(p_szToken, 200, _szToken);
	};
	void setToken(const char* p_szToken) {
		strcpy_s(_szToken, 200, p_szToken);
	}
	void clearToken();

	void saveToken(void);
	void loadToken(void);

	void clearList(void);
	void addList(int p_nIndex, const char* p_szName, const char* p_szDesc, const char* p_szSize);
	int getListCount(void);
	void getList(int p_nIndex, CString& p_szName, CString& p_szDesc, CString& p_szSize);

protected:
	char _szToken[200];
};
