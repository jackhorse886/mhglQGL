#include "ConfigureRevertlist.h"

CConfigureRevertlist::CConfigureRevertlist()
{
	m_iniFilename.Empty();
}

CConfigureRevertlist::~CConfigureRevertlist()
{
}

BOOL CConfigureRevertlist::LoadConfigure(PCSTRING iniFilename)
{
	TRACE_ENSURE(iniFilename);
	SetINIFilename(iniFilename);
	return TRUE;
}

BOOL CConfigureRevertlist::SavePatch(PCSTRING filename, UInt32 size, UInt32 crc, UInt32 revertFrom, UInt32 revertTo, UInt32 revertFromVCRC, UInt32 revertToVCRC)
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
	strValue.Format(_T("%d"), revertFrom);
	bSucceed = bSucceed & WritePrivateProfileString(filename, _T("from"), strValue.c_str(), GetINIFilename().c_str());
	strValue.Format(_T("%d"), revertTo);
	bSucceed = bSucceed & WritePrivateProfileString(filename, _T("to"), strValue.c_str(), GetINIFilename().c_str());
	strValue.Format(_T("%x"), revertFromVCRC);
	bSucceed = bSucceed & WritePrivateProfileString(filename, _T("vcrcf"), strValue.c_str(), GetINIFilename().c_str());
	strValue.Format(_T("%x"), revertToVCRC);
	bSucceed = bSucceed & WritePrivateProfileString(filename, _T("vcrct"), strValue.c_str(), GetINIFilename().c_str());
	return bSucceed;
}
