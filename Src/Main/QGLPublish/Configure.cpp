#include "Configure.h"

#define CONFIGURATION_PUBLISH_INI_FILE		_T("Config\\QGLPublishPath.ini")
#define CONFIGURATION_PUBLISH_FILEHISTORY	_T("\\PatchHistory\\FileHistory.ini")

CConfigure::CConfigure()
{
	m_patchMajorMap.clear();
}

CConfigure::~CConfigure()
{
}

BOOL CConfigure::LoadConfigure()
{
	String familyVar, accessVar;

	familyVar = _T("VERSION");
	{
		m_patchMajorMap.clear();
        TCHAR keystr[65536];
		TCHAR *pkey;
		DWORD dwstrlen = GetPrivateProfileSection(_T("VERSION"), keystr, 1024, CONFIGURATION_PUBLISH_INI_FILE);
		pkey = keystr;
		while(*pkey!='\0')
		{
			TCHAR view[64];
			TCHAR *ptemp;
			ptemp = pkey;
			_tcscpy(view, ptemp);
			String strTemp(view);
			UInt32 pos = (UInt32)strTemp.find_first_of(_T("="), 0);

			UInt32 version = _ttoi(strTemp.Left(pos).c_str());
			MajorVersion majorVersion;
			majorVersion.major = strTemp.Right(strTemp.GetLength()-pos-1).c_str();
			majorVersion.changed = FALSE;

			m_patchMajorMap.insert(std::make_pair(version, majorVersion));

			pkey +=_tcslen(ptemp);
			pkey++;
		}
	}
	familyVar = _T("PATH");
	{
		accessVar = _T("PATH_SINGLE");
		LoadConfigureString(path_single, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_BATCH");
		LoadConfigureString(path_batch, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_REPLACE");
		LoadConfigureString(path_replace, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_COMPLETE");
		LoadConfigureString(path_complete, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		path_filehistory.Format(_T("%s%s"), path_complete.c_str(), CONFIGURATION_PUBLISH_FILEHISTORY);
		accessVar = _T("PATH_PRODUCT");
		LoadConfigureString(path_product, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_LASTSOURCE");
		LoadConfigureString(path_lastSource, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_LASTSOURCESVN");
		LoadConfigureString(path_lastSourceSvn, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_TEMPLATE");
		LoadConfigureString(path_template, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_MAKENSISFILE");
		LoadConfigureString(path_makensis_file, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_SUBWCREVFILE");
		LoadConfigureString(path_subwcrev_file, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_REVERT");
		LoadConfigureString(path_revertSingle, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_REVERTPRODUCT");
		LoadConfigureString(path_revertProduct, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_FTPSUMMARYFILE");
		LoadConfigureString(path_ftp_summary_file, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_FTPFILELISTFILE");
		LoadConfigureString(path_ftp_filelist_file, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_FTPREVERTLISTFILE");
		LoadConfigureString(path_ftp_revertlist_file, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_FTP");
		LoadConfigureString(path_ftp, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
		accessVar = _T("PATH_FTP_VERSION_FILE");
		LoadConfigureString(path_ftp_version_file, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PUBLISH_INI_FILE);
	}
	return TRUE;
}

UInt32 CConfigure::GetVersionCRC(UInt32 version)
{
	String attribVar;
	attribVar.Format(_T("%d"), version);
	String strCRC;
	LoadConfigureString(strCRC, _T("VERSION_CRC"), attribVar.c_str(), _T(""), CONFIGURATION_PUBLISH_INI_FILE);
	UInt32 crc = 0;
	_stscanf(strCRC.c_str(), _T("%x"), &crc);
	return crc;
}

VOID CConfigure::SaveVersionCRC(UInt32 version, UInt32 crc)
{
	TRACE_ENSURE(crc!=0);

	if( !version )
		return;
	String attribVar, strValue;
	attribVar.Format(_T("%d"), version);
	strValue.Format(_T("%x"), crc);
	WritePrivateProfileString(_T("VERSION_CRC"), attribVar.c_str(), strValue.c_str(), CONFIGURATION_PUBLISH_INI_FILE);
}

VOID CConfigure::ResetVersionCRC(UInt32 version)
{
	String attribVar, strValue;
	attribVar.Format(_T("%d"), version);
	WritePrivateProfileString(_T("VERSION_CRC"), attribVar.c_str(), _T("0"), CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveSinglePath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_single = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_SINGLE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveBatchPath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_batch = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_BATCH"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveReplacePath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_replace = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_REPLACE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveCompletePath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_complete = value;
	path_filehistory.Format(_T("%s%s"), path_complete.c_str(), CONFIGURATION_PUBLISH_FILEHISTORY);
	return WritePrivateProfileString(_T("PATH"), _T("PATH_COMPLETE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveProductPath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_product = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_PRODUCT"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveLastSourcePath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_lastSource = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_LASTSOURCE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveLastSourceSvnPath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_lastSourceSvn = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_LASTSOURCESVN"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveTemplatePath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_template = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_TEMPLATE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveMakeNsisFile(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_makensis_file = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_MAKENSISFILE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveSubWCRevFile(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_subwcrev_file = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_SUBWCREVFILE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveRevertPath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_revertSingle = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_REVERT"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveRevertProductPath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_revertProduct = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_REVERTPRODUCT"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveFTPSummaryFile(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_ftp_summary_file = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_FTPSUMMARYFILE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveFTPFilelistFile(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_ftp_filelist_file = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_FTPFILELISTFILE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveFTPRevertlistFile(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_ftp_revertlist_file = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_FTPREVERTLISTFILE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveMajorVersion(UInt32 version, PCSTRING majorVersion)
{
	BOOL bFound = FALSE;
	UInt32 maxVersionBelow = 0;
	for( PatchMajorVerMap::iterator itrMax = m_patchMajorMap.begin(); itrMax != m_patchMajorMap.end(); ++itrMax )
	{
		if( itrMax->first <= version && itrMax->first > maxVersionBelow )
			maxVersionBelow = itrMax->first;
	}
	// if previous version has changed, add one major
	PatchMajorVerMap::iterator itrFind = m_patchMajorMap.find(maxVersionBelow);
	if( itrFind != m_patchMajorMap.end() )
	{
		MajorVersion& majorFind = itrFind->second;
		if( majorFind.major.CompareNoCase(majorVersion) )
		{
			if( itrFind->first==version )
			{
				itrFind->second.changed = TRUE;
				itrFind->second.major = majorVersion;
			}else
			{
				MajorVersion majorAdd;
				majorAdd.major = majorVersion;
				majorAdd.changed = TRUE;
				m_patchMajorMap.insert(std::make_pair(version, majorAdd));
			}
		}
		// whole array save
		for( PatchMajorVerMap::iterator itr = m_patchMajorMap.begin(); itr != m_patchMajorMap.end(); ++itr )\
		{
			MajorVersion& major = itr->second;
			if( major.changed )
			{
				String strVersion;
				strVersion.Format(_T("%d"), itr->first);
				WritePrivateProfileString(_T("VERSION"), strVersion.c_str(), major.major.c_str(), CONFIGURATION_PUBLISH_INI_FILE);
			}
		}
	}
	return TRUE;
}

VOID CConfigure::GetMajorVersion(UInt32 curVersion, String& outMajor)
{
	UInt32 max = 0;
	outMajor.Empty();
	for( PatchMajorVerMap::iterator itr = m_patchMajorMap.begin(); itr != m_patchMajorMap.end(); ++itr )\
	{
		MajorVersion& major = itr->second;
		CONST UInt32& version = itr->first;
		if( version <= curVersion && version > max )
		{
			outMajor = major.major.c_str();
			max = version;
		}
	}
}

VOID CConfigure::ExportMajorVersion(PCSTRING filename)
{
	for( PatchMajorVerMap::iterator itr = m_patchMajorMap.begin(); itr != m_patchMajorMap.end(); ++itr )
	{
		MajorVersion& major = itr->second;
		String strVersion;
		strVersion.Format(_T("%d"), itr->first);
		WritePrivateProfileString(_T("VERSION"), strVersion.c_str(), major.major.c_str(), filename);
	}
}

BOOL CConfigure::SaveFTPPath(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_ftp = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_FTP"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

BOOL CConfigure::SaveFTPVersionFile(LPCTSTR value)
{
	if( !value )
		return FALSE;
	path_ftp_version_file = value;
	return WritePrivateProfileString(_T("PATH"), _T("PATH_FTP_VERSION_FILE"), value, CONFIGURATION_PUBLISH_INI_FILE);
}

VOID CConfigure::PrintDebug()
{
}

VOID CConfigure::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigure::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigure::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigure::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}
