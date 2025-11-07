#pragma once
#include <afxinet.h>

typedef void(*UPDATEFNC)(const CString &);

// CInternetSessionEx command target

class CInternetSessionEx : public CInternetSession
{
public:
	void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
		
	CInternetSessionEx(LPCTSTR pstrAgent = NULL,
		DWORD dwContext = 1,
		DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS,
		LPCTSTR pstrProxyName = NULL,
		LPCTSTR pstrProxyBypass = NULL,
		DWORD dwFlags = 0);
	virtual ~CInternetSessionEx();
	
	// member pointer to function
	//updateFnc update;
	void(*update)(const CString &);

	void setUpdateCallback(void (*pF)(const CString &)) {update = pF;}

	HWND hWnd;
};


