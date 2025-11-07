#include "ConfigureSVN.h"

CConfigureSVN::CConfigureSVN()
{
	svnRevision = 0;
}

CConfigureSVN::~CConfigureSVN()
{
}

BOOL CConfigureSVN::LoadConfigure()
{
	String familyVar, accessVar;

	familyVar = _T("SVN");
	{
		accessVar = _T("WCREV");
		LoadConfigureInt(svnRevision, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_SVN_INI_FILE);
	}
	return TRUE;
}

VOID CConfigureSVN::PrintDebug()
{
}

VOID CConfigureSVN::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSVN::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigureSVN::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSVN::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureSVN::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSVN::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureSVN::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureSVN::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}
