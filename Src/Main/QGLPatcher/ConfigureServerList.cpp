#include "ConfigureServerList.h"

#define CONFIGURATION_INI_SRVLIST_FILE					_T("Patch\\srvlistf2.txt")
#define DEFAULT_PORT									9000
#define MAX_NAMES 8192

CConfigureServerList::CConfigureServerList()
{
}

CConfigureServerList::~CConfigureServerList()
{
	FreeConfigure();
}

BOOL CConfigureServerList::LoadConfigure()
{
	// check validity of the input file
	if( CheckFileValid() )
	{
		UInt32 counter = 1;
		LPTSTR lpszReturnBuffer;
		lpszReturnBuffer = new TCHAR[MAX_NAMES];
		TCHAR* pNextSection = NULL;
		GetPrivateProfileSectionNames(lpszReturnBuffer, MAX_NAMES, CONFIGURATION_INI_SRVLIST_FILE);
		pNextSection = lpszReturnBuffer;
		{
			ServerInfo srvInfo;
			ReadServerInfo(srvInfo, pNextSection);
			srvInfo.id = counter++;
			m_serverInfoList.push_back(srvInfo);
		}
		while (*pNextSection != 0x00)
		{
			pNextSection = pNextSection + _tcslen(pNextSection) + 1;
			if(*pNextSection != 0x00)
			{
				if( _tcsicmp(pNextSection, _T("FINISH")) )
				{
					ServerInfo srvInfo;
					ReadServerInfo(srvInfo, pNextSection);
					srvInfo.id = counter++;
					m_serverInfoList.push_back(srvInfo);
				}
			}
		}
		delete [] lpszReturnBuffer;
		PrintDebug();
		return TRUE;
	}
	PrintDebug();
	return FALSE;
}

VOID CConfigureServerList::PrintDebug()
{
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigure() Verbose (1) size=%d"), (UInt32)m_serverInfoList.size());
	for( Index i = C_INDEX(0); i < m_serverInfoList.size(); ++i )
	{
		ServerInfo& info = m_serverInfoList[i];
		TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigure() Verbose (2) name=%s"), info.name.c_str());
		for( Index j = C_INDEX(0); j < info.zoneList.size(); ++j )
		{
			UInt32 zoneid = info.zoneList[j];
			TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigure() Verbose (3) zoneid=%d"), zoneid);
		}
		TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigure() Verbose (4) status=%d"), info.status);
		for( Index j = C_INDEX(0); j < info.proxyList.size(); ++j )
		{
			ProxyIP& address = info.proxyList[j];
			TRACE_VERBOSEDTL_5(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigure() Verbose (5) proxy=%d.%d.%d.%d:%d"), address.pos1, address.pos2, address.pos3, address.pos4, address.port);
		}
		for( Index j = C_INDEX(0); j < info.downloadLinkList.size(); ++j )
		{
			String& link = info.downloadLinkList[j];
			TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigure() Verbose (6) name=%s"), link.c_str());
		}
		TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigure() Verbose (7) rank=%d"), info.rank);
	}
}

VOID CConfigureServerList::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigureServerList::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureServerList::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureServerList::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureServerList::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

BOOL CConfigureServerList::ReloadConfigure()
{
	FreeConfigure();
	return LoadConfigure();
}

VOID CConfigureServerList::FreeConfigure()
{
	for( Index i = C_INDEX(0); i < m_serverInfoList.size(); ++i )
	{
		ServerInfo& info = m_serverInfoList[i];
		info.downloadLinkList.clear();
		info.proxyList.clear();
		info.zoneList.clear();
	}
	m_serverInfoList.clear();
}

BOOL CConfigureServerList::CheckFileValid()
{
	UInt32 bFinish = 0;
	LoadConfigureInt(bFinish, _T("FINISH"), _T("FINISH"), 0, CONFIGURATION_INI_SRVLIST_FILE);
	if( bFinish==0 )
		TRACE_WARNDTL(GLOBAL_LOGGER, _FW("srvlistf2.txt is not valid, program exit"));
	return (bFinish!=0);
}

VOID CConfigureServerList::ReadServerInfo(ServerInfo& info, LPCTSTR family)
{
	String varName;
	TCHAR* token;

//	info.id;
	varName.Format(_T("name"));
	LoadConfigureString(info.name, family, varName.c_str(), _T(""), CONFIGURATION_INI_SRVLIST_FILE);
	varName.Format(_T("zone"));
	String zonelist;
	LoadConfigureString(zonelist, family, varName.c_str(), _T(""), CONFIGURATION_INI_SRVLIST_FILE);
	TCHAR pBuf[MAX_PATH];
	_tcsncpy(pBuf, zonelist.c_str(), MAX_PATH);
	pBuf[MAX_PATH-1] = 0;
	token = _tcstok(pBuf, _T(","));
	while( token!=NULL )
	{
		UInt32 zoneid = _ttoi(token);
		if( zoneid!=0 )
			info.zoneList.push_back(zoneid);
		token = _tcstok(NULL, _T(","));
	}
	varName.Format(_T("status"));
	LoadConfigureInt(info.status, family, varName.c_str(), 0, CONFIGURATION_INI_SRVLIST_FILE);
	varName.Format(_T("proxy_count"));
	UInt32 proxycount = 0;
	LoadConfigureInt(proxycount, family, varName.c_str(), 0, CONFIGURATION_INI_SRVLIST_FILE);
	for( Index i = C_INDEX(0); i < proxycount; ++i )
	{
		varName.Format(_T("proxy_%d"), i+1);
		String strAddress;
		LoadConfigureString(strAddress, family, varName.c_str(), _T(""), CONFIGURATION_INI_SRVLIST_FILE);
		if( !strAddress.IsEmpty() )
		{
			ProxyIP address;
			_tcsncpy(pBuf, strAddress.c_str(), MAX_PATH);
			pBuf[MAX_PATH-1] = 0;
			// IP address
			token = _tcstok(pBuf, _T("."));
			address.pos1 = _ttoi(token);
			token = _tcstok(NULL, _T("."));
			address.pos2 = _ttoi(token);
			token = _tcstok(NULL, _T("."));
			address.pos3 = _ttoi(token);
			token = _tcstok(NULL, _T(":"));
			address.pos4 = _ttoi(token);
			// port
			token = _tcstok(NULL, _T(":"));
			address.port = token ? _ttoi(token) : DEFAULT_PORT;
			info.proxyList.push_back(address);
		}
	}
	varName.Format(_T("dl_count"));
	UInt32 dlcount = 0;
	LoadConfigureInt(dlcount, family, varName.c_str(), 0, CONFIGURATION_INI_SRVLIST_FILE);
	for( Index i = C_INDEX(0); i < dlcount; ++i )
	{
		varName.Format(_T("dl_%d"), i+1);
		String address;
		LoadConfigureString(address, family, varName.c_str(), _T(""), CONFIGURATION_INI_SRVLIST_FILE);
		if( !address.IsEmpty() )
			info.downloadLinkList.push_back(address);
	}
	varName.Format(_T("rank"));
	LoadConfigureInt(info.rank, family, varName.c_str(), 0, CONFIGURATION_INI_SRVLIST_FILE);
}
