#include "stdafx.h"
#include "InternetSessionEx.h"

// Ensure WMU_SESSIONUPDATE is defined
#ifndef WMU_SESSIONUPDATE
#define WMU_SESSIONUPDATE (WM_USER + 1)
#endif

// CInternetSessionEx

CInternetSessionEx::CInternetSessionEx(LPCTSTR pstrAgent /*= NULL*/,
		DWORD dwContext /*= 1*/,
		DWORD dwAccessType /*= PRE_CONFIG_INTERNET_ACCESS*/,
		LPCTSTR pstrProxyName /*= NULL*/,
		LPCTSTR pstrProxyBypass /*= NULL*/,
		DWORD dwFlags /*= 0*/)
		: CInternetSession (pstrAgent, dwContext, dwAccessType, pstrProxyName, 
							pstrProxyBypass, dwFlags)
{
	hWnd = 0;
}

CInternetSessionEx::~CInternetSessionEx()
{
}

// CInternetSessionEx member functions
void CInternetSessionEx::OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	DWORD st = dwInternetStatus;
//	CString *pMessage = new CString;
	DWORD pMessage=0x00;

	switch (dwInternetStatus) {
		case INTERNET_STATUS_RESOLVING_NAME:
//			*pMessage = "Looking up the IP address of the name contained in lpvStatusInformation.\n";
			break;
		
		case INTERNET_STATUS_NAME_RESOLVED:
//			*pMessage = "Successfully found the IP address of the name contained in lpvStatusInformation.\n";
			break;

		case INTERNET_STATUS_CONNECTING_TO_SERVER:
//			*pMessage = "Connecting to the socket address (SOCKADDR) pointed to by lpvStatusInformation.\n";
			break;

		case INTERNET_STATUS_CONNECTED_TO_SERVER:
//			*pMessage = "Successfully connected to the socket address (SOCKADDR) pointed to by lpvStatusInformation.\n";
			break;

		case INTERNET_STATUS_SENDING_REQUEST:
//			*pMessage = "Sending the information request to the server.\nThe lpvStatusInformation parameter is NULL.\n";
			break;

		case INTERNET_STATUS_REQUEST_SENT:
//			*pMessage = "Successfully sent the information request to the server.\nThe lpvStatusInformation parameter is NULL.\n";
			break;

		case INTERNET_STATUS_RECEIVING_RESPONSE:
//			*pMessage = "Waiting for the server to respond to a request.\n";
			break;

		case INTERNET_STATUS_RESPONSE_RECEIVED:
//			*pMessage = "Successfully received a response from the server.\n";
			break;

		case INTERNET_STATUS_CLOSING_CONNECTION:
			pMessage = 0x00000001;
			//*pMessage = "Closing the connection to the server.";
			break;

		case INTERNET_STATUS_CONNECTION_CLOSED:
			pMessage = 0x00000002;
			//*pMessage = "Successfully closed the connection to the server.";
			break;

		case INTERNET_STATUS_HANDLE_CREATED:
			pMessage = 0x00000003;
			//*pMessage = "Connecting...\n";
			break;

		case INTERNET_STATUS_HANDLE_CLOSING:
			pMessage = 0x00000004;
			//*pMessage = "Successfully terminated this handle value.\n";
			break;

		default:
			#ifdef _DEBUG
			pMessage = 0x00000005;
			//pMessage->Format("Unknown Internet status (%d)\n", dwInternetStatus);
			#endif
			break;
	}

	if (pMessage == 0)
	{
//		delete pMessage;
//		pMessage=NULL;
		return;
	}

//	PostMessage(hWnd, WMU_SESSIONUPDATE, reinterpret_cast<WPARAM>(pMessage), 0);
	PostMessage(hWnd, WMU_SESSIONUPDATE, pMessage, 0);
}