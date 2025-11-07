#include "ConfigureSummary.h"

CConfigureSummary::CConfigureSummary()
{
	m_iniFilename.Empty();
	version = 0;
}

CConfigureSummary::~CConfigureSummary()
{
}

BOOL CConfigureSummary::LoadConfigure(PCSTRING iniFilename)
{
	TRACE_ENSURE(iniFilename);
	SetINIFilename(iniFilename);
	String familyVar, accessVar;

	familyVar = _T("FILESUMMARY");
	{
		accessVar = _T("version");
		LoadConfigureInt(version , familyVar.c_str(), accessVar.c_str(), 0, GetINIFilename().c_str());
	}
	return TRUE;
}

BOOL CConfigureSummary::SaveVersion(UInt32 value, UInt32 versionCRC)
{
	TRACE_ENSURE(!m_iniFilename.IsEmpty());

	if( !value )
		return FALSE;
	version = value;
	String strValue;
	strValue.Format(_T("%d"), value);
	WritePrivateProfileString(_T("FILESUMMARY"), _T("version"), strValue.c_str(), GetINIFilename().c_str());
	strValue.Format(_T("%x"), versionCRC);
	WritePrivateProfileString(_T("FILESUMMARY"), _T("versionCRC"), strValue.c_str(), GetINIFilename().c_str());
	return TRUE;
}

BOOL CConfigureSummary::SavePatch(UInt32 sourceVersion, PCSTRING filename)
{
	TRACE_ENSURE(!m_iniFilename.IsEmpty());

	if( !sourceVersion || !filename )
		return FALSE;
	String attribVar;
	attribVar.Format(_T("patch%08d"), sourceVersion);
	return WritePrivateProfileString(_T("FILESUMMARY"), attribVar.c_str(), filename, GetINIFilename().c_str());
}

VOID CConfigureSummary::PrintDebug()
{
}

VOID CConfigureSummary::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSummary::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigureSummary::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSummary::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureSummary::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSummary::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureSummary::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSummary::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}
