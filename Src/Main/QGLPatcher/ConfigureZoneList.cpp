#include "ConfigureZoneList.h"

#define CONFIGURATION_INI_ZONELIST_FILE					_T("patch\\zonelist.txt")

CConfigureZoneList::CConfigureZoneList()
{
}

CConfigureZoneList::~CConfigureZoneList()
{
	FreeConfigure();
}

BOOL CConfigureZoneList::LoadConfigure()
{
	// check validity of the input file
	if( CheckFileValid() )
	{
		String familyVar, accessVar;

		UInt count;
		familyVar = _T("ZONELIST");
		{
			accessVar = _T("count");
			LoadConfigureInt(count, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_INI_ZONELIST_FILE);
			for( Index i = C_INDEX(0); i < count; ++i )
			{
				String tempvar;
				accessVar.Format(_T("name%d"), i+1);
				LoadConfigureString(tempvar, familyVar.c_str(), accessVar.c_str(), _T(""), CONFIGURATION_INI_ZONELIST_FILE);
				if( !tempvar.IsEmpty() )
				{
					Zone zone(tempvar);
					m_zoneMap.insert(std::make_pair(i+1, zone));
				}
			}
		}
	}
	PrintDebug();
	return TRUE;
}

BOOL CConfigureZoneList::ReloadConfigure()
{
	FreeConfigure();
	return LoadConfigure();
}

VOID CConfigureZoneList::FreeConfigure()
{
	m_zoneMap.clear();
}

VOID CConfigureZoneList::PrintDebug()
{
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureZoneList::LoadConfigure() Verbose (1) size=%d"), (UInt)m_zoneMap.size());
	for( ZoneMap::iterator itr = m_zoneMap.begin(); itr != m_zoneMap.end(); ++itr )
	{
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureZoneList::LoadConfigure() Verbose (2) zoneId=%d, name=%s"), itr->first, itr->second.GetName().c_str());
	}
}

VOID CConfigureZoneList::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureZoneList::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigureZoneList::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureZoneList::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureZoneList::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureZoneList::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureZoneList::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureZoneList::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

Zone* CConfigureZoneList::GetZone(UInt32 zoneId)
{
	ZoneMap::iterator itr = m_zoneMap.find(zoneId);
	if( itr != m_zoneMap.end() )
		return &itr->second;
	else
		return NULL;
}

BOOL Zone::RegisterServer(ServerInfo* info)
{
	for( Index i = C_INDEX(0); i < m_serverList.size(); ++i )
	{
		if( m_serverList[i]==info )
			return FALSE;
	}
	m_serverList.push_back(info);
	return TRUE;
}

VOID Zone::UnregisterServer(CONST ServerInfo* info)
{
	for( ServerInfoPtrList::iterator itr = m_serverList.begin(); itr != m_serverList.end(); ++itr )
	{
		if( *itr==info )
		{
			m_serverList.erase(itr);
			return;
		}
	}
}

VOID Zone::UnregisterAllServer()
{
	m_serverList.clear();
}

BOOL CConfigureZoneList::CheckFileValid()
{
	UInt32 bFinish = 0;
	LoadConfigureInt(bFinish, _T("FINISH"), _T("FINISH"), 0, CONFIGURATION_INI_ZONELIST_FILE);
	if( bFinish==0 )
		TRACE_WARNDTL(GLOBAL_LOGGER, _FW("zonelist.txt is not valid, program exit"));
	return (bFinish!=0);
}

VOID Zone::GetServerList(ServerInfoList& serverInfoList)
{
	serverInfoList.clear();
	for( Index i = C_INDEX(0); i < m_serverList.size(); ++i )
	{
		serverInfoList.push_back(*m_serverList[i]);
	}
}