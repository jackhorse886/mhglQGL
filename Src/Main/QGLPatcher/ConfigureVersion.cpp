#include "ConfigureVersion.h"

#define CONFIGURATION_INI_VERSION_FILE					_T("patch\\fileversion.txt")

CConfigureVersion::CConfigureVersion()
{
}

CConfigureVersion::~CConfigureVersion()
{
	FreeConfigure();
}

BOOL CConfigureVersion::LoadConfigure()
{
	// check validity of the input file
	if( CheckFileValid() )
	{
		String familyVar;
		TCHAR keystr[65536];
		familyVar = _T("VERSION");
		{
			DWORD dwstrlen = GetPrivateProfileSection(familyVar.c_str(), keystr, 1024, CONFIGURATION_INI_VERSION_FILE);
			TCHAR *pkey = keystr;
			m_patchMajorMap.clear();
			while(*pkey!='\0')
			{
				TCHAR view[64];
				TCHAR *ptemp;
				ptemp = pkey;
				_tcscpy(view, ptemp);
				String strTemp(view);
				UInt32 pos = (UInt32)strTemp.find_first_of(_T("="), 0);

				UInt32 version = _ttoi(strTemp.Left(pos).c_str());
				String majorVersion;
				majorVersion = strTemp.Right(strTemp.GetLength()-pos-1).c_str();

				m_patchMajorMap.insert(std::make_pair(version, majorVersion));

				pkey +=_tcslen(ptemp);
				pkey++;
			}
		}
		familyVar = _T("SUFFIX");
		{
			DWORD dwstrlen = GetPrivateProfileSection(familyVar.c_str(), keystr, 1024, CONFIGURATION_INI_VERSION_FILE);
			TCHAR *pkey = keystr;
			m_patchSuffixMap.clear();
			while(*pkey!='\0')
			{
				TCHAR view[64];
				TCHAR *ptemp;
				ptemp = pkey;
				_tcscpy(view, ptemp);
				String strTemp(view);
				UInt32 pos = (UInt32)strTemp.find_first_of(_T("="), 0);

				UInt32 version = _ttoi(strTemp.Left(pos).c_str());
				String suffixVersion;
				suffixVersion = strTemp.Right(strTemp.GetLength()-pos-1).c_str();

				m_patchSuffixMap.insert(std::make_pair(version, suffixVersion));

				pkey +=_tcslen(ptemp);
				pkey++;
			}
		}
	}
	PrintDebug();
	return TRUE;
}

BOOL CConfigureVersion::ReloadConfigure()
{
	FreeConfigure();
	return LoadConfigure();
}

VOID CConfigureVersion::FreeConfigure()
{
	m_patchMajorMap.clear();
	m_patchSuffixMap.clear();
}

VOID CConfigureVersion::PrintDebug()
{
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigure() Verbose (1) major size=%d"), (UInt)m_patchMajorMap.size());
	for( PatchMajorVerMap::iterator itr = m_patchMajorMap.begin(); itr != m_patchMajorMap.end(); ++itr )
	{
		String& major = itr->second;
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigure() Verbose (2) version=%d major=%s"), itr->first, major.c_str());
	}
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigure() Verbose (3) suffix size=%d"), (UInt)m_patchSuffixMap.size());
	for( PatchSuffixMap::iterator itr = m_patchSuffixMap.begin(); itr != m_patchSuffixMap.end(); ++itr )
	{
		String& suffix = itr->second;
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigure() Verbose (2) version=%d suffix=%s"), itr->first, suffix.c_str());
	}
}

VOID CConfigureVersion::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigureVersion::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureVersion::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigureVersion::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigureVersion::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

BOOL CConfigureVersion::CheckFileValid()
{
	UInt32 bFinish = 0;
	LoadConfigureInt(bFinish, _T("FINISH"), _T("FINISH"), 0, CONFIGURATION_INI_VERSION_FILE);
	if( bFinish==0 )
		TRACE_WARNDTL(GLOBAL_LOGGER, _FW("fileversion.txt is not valid, program exit"));
	return (bFinish!=0);
}

VOID CConfigureVersion::GetMajorVersion(UInt32 curVersion, String& outMajor, UInt32& outSinceVer)
{
	UInt32 max = 0;
	outMajor.Empty();
	for( PatchMajorVerMap::iterator itr = m_patchMajorMap.begin(); itr != m_patchMajorMap.end(); ++itr )
	{
		String& major = itr->second;
		CONST UInt32& version = itr->first;
		if( version <= curVersion && version > max )
		{
			outMajor = major.c_str();
			max = version;
		}
	}
	outSinceVer = max;
}

VOID CConfigureVersion::GetSuffixVersion(UInt32 curVersion, String& outSuffix, UInt32& outSinceVer)
{
	UInt32 max = 0;
	outSuffix.Empty();
	for( PatchSuffixMap::iterator itr = m_patchSuffixMap.begin(); itr != m_patchSuffixMap.end(); ++itr )
	{
		String& suffix = itr->second;
		CONST UInt32& version = itr->first;
		if( version <= curVersion && version > max )
		{
			outSuffix = suffix.c_str();
			max = version;
		}
	}
	outSinceVer = max;
}
