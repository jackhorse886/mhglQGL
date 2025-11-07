#include "ConfigureApp.h"

#define CONFIGURATION_INI_PATCH_FILE		_T("Config\\patch.ini")
#define CONFIGURATION_INI_LOCAL_FILE		_T("Config\\patchsav.ini")
#define CONFIGURATION_INI_SYS_FILE			_T("Config\\patchcfg.ini")

CConfigureApp::CConfigureApp()
{
}

CConfigureApp::~CConfigureApp()
{
}

BOOL CConfigureApp::LoadConfigure()
{
	String familyVar, accessVar;

	familyVar = _T("APP");
	{
		UpdatePatcherVersion();
		accessVar = _T("lastservername");
		LoadConfigureString(m_lastservername, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_INI_LOCAL_FILE);
		accessVar = _T("lastzonename");
		LoadConfigureString(m_lastzonename, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_INI_LOCAL_FILE);
		accessVar = _T("lastmodifieddate");
		LoadConfigureString(m_lastmodifieddate, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_INI_LOCAL_FILE);
		accessVar = _T("startgame");
		LoadConfigureString(m_startgame, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_INI_SYS_FILE);
		accessVar = _T("startpatcher");
		LoadConfigureString(m_startpatcher, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_INI_SYS_FILE);
		accessVar = _T("startrestart");
		LoadConfigureString(m_startrestart, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_INI_SYS_FILE);

		accessVar = _T("debug_ui");
		LoadConfigureInt(m_debug_ui, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_INI_SYS_FILE);
		accessVar = _T("debug_exportblock");
		LoadConfigureInt(m_debug_exportblock, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_INI_SYS_FILE);
	}
	return TRUE;
}

VOID CConfigureApp::PrintDebug()
{
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigure() Verbose (1) patcherversion=%d"), m_patcherversion);
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigure() Verbose (2) lastservername=%s"), m_lastservername.c_str());
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigure() Verbose (3) lastzonename=%s"), m_lastzonename.c_str());
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigure() Verbose (4) lastmodifieddate=%s"), m_lastmodifieddate.c_str());
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigure() Verbose (5) startgame=%s"), m_startgame.c_str());
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigure() Verbose (6) startpatcher=%s"), m_startpatcher.c_str());
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigure() Verbose (7) startrestart=%s"), m_startrestart.c_str());
}

VOID CConfigureApp::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigureApp::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureApp::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureApp::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureApp::LoadConfigureCRC(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	String strCRC;
	LoadConfigureString(strCRC, family, variable, _T("00000000"), srcFilePath);
	out = 0;
	_stscanf(strCRC.c_str(), _T("%x"), &out);

	TRACE_INFODTL_3(
		GLOBAL_LOGGER, _T("CConfigureApp::LoadConfigureCRC Info (1) [load] out=%X, family=%s variable=%s"),
		out, family, variable
		);
}

BOOL CConfigureApp::UpdatePatcherVersion()
{
	String familyVar, accessVar;
	familyVar = _T("APP");
	accessVar = _T("patcherversion");
	LoadConfigureInt(m_patcherversion, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_INI_PATCH_FILE);
	accessVar = _T("versioncrc");
	LoadConfigureCRC(m_patcherversioncrc, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_INI_PATCH_FILE);
	return (m_patcherversion!=0);
}

BOOL CConfigureApp::SaveLastServer(LPCTSTR lastsrv)
{
	if( !lastsrv ) return FALSE;

	String familyVar, accessVar;
	familyVar = _T("APP");
	accessVar = _T("lastserver");
	WritePrivateProfileString(familyVar.c_str(), accessVar.c_str(), lastsrv, CONFIGURATION_INI_LOCAL_FILE);
	return TRUE;
}

BOOL CConfigureApp::SaveLastPort(UInt32 lastport)
{
	if( !lastport ) return FALSE;

	String familyVar, accessVar;
	familyVar = _T("APP");
	accessVar = _T("lastport");
	String out;
	out.Format(_T("%d"), lastport);
	WritePrivateProfileString(familyVar.c_str(), accessVar.c_str(), out.c_str(), CONFIGURATION_INI_LOCAL_FILE);
	return TRUE;
}

BOOL CConfigureApp::SaveLastModifiedDate(LPCTSTR lastmodifieddate)
{
	if( !lastmodifieddate ) return FALSE;

	String familyVar, accessVar;
	familyVar = _T("APP");
	accessVar = _T("lastmodifieddate");
	WritePrivateProfileString(familyVar.c_str(), accessVar.c_str(), lastmodifieddate, CONFIGURATION_INI_LOCAL_FILE);
	return TRUE;
}

BOOL CConfigureApp::SaveLastSelection(UInt32 lastServerID, PCSTRING lastServerName, PCSTRING lastZoneName)
{
	if( lastServerID==0 || !lastServerName || !lastZoneName )
		return FALSE;

	String familyVar, accessVar;
	String out;
	familyVar = _T("APP");
	{
		accessVar = _T("lastservername");
		out.Format(_T("%s"), lastServerName);
		WritePrivateProfileString(familyVar.c_str(), accessVar.c_str(), out.c_str(), CONFIGURATION_INI_LOCAL_FILE);
		accessVar = _T("lastzonename");
		out.Format(_T("%s"), lastZoneName);
		WritePrivateProfileString(familyVar.c_str(), accessVar.c_str(), out.c_str(), CONFIGURATION_INI_LOCAL_FILE);
		accessVar = _T("lastserverid");
		out.Format(_T("%d"), lastServerID);
		WritePrivateProfileString(familyVar.c_str(), accessVar.c_str(), out.c_str(), CONFIGURATION_INI_LOCAL_FILE);
	}
	return TRUE;
}
