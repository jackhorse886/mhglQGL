#include "ConfigureFilelist.h"

CConfigureFilelist::CConfigureFilelist()
{
	m_iniFilename.Empty();
}

CConfigureFilelist::~CConfigureFilelist()
{
}

BOOL CConfigureFilelist::LoadConfigure(PCSTRING iniFilename)
{
	TRACE_ENSURE(iniFilename);
	SetINIFilename(iniFilename);
	return TRUE;
}

BOOL CConfigureFilelist::SavePatch(PCSTRING filename, UInt32 size, UInt32 crc, UInt32 vercrc)
{
	TRACE_ENSURE(!m_iniFilename.IsEmpty());

	if( !filename )
		return FALSE;

	BOOL bSucceed = TRUE;
	String strValue;
	strValue.Format(_T("%d"), size);
	bSucceed = bSucceed & WritePrivateProfileString(filename, _T("size"), strValue.c_str(), GetINIFilename().c_str());
	strValue.Format(_T("%x"), crc);
	bSucceed = bSucceed & WritePrivateProfileString(filename, _T("crc"), strValue.c_str(), GetINIFilename().c_str());
	strValue.Format(_T("%x"), vercrc);
	bSucceed = bSucceed & WritePrivateProfileString(filename, _T("vcrc"), strValue.c_str(), GetINIFilename().c_str());
	return bSucceed;
}
