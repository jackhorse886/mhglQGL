#pragma once
#ifndef _CONFIGURE_H
#define _CONFIGURE_H

#include "common.h"

struct MajorVersion
{
	String major;
	BOOL changed;
};

typedef StlMap<UInt32, MajorVersion, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, MajorVersion> > > PatchMajorVerMap;

class CConfigure
{
public:
	CConfigure();
	~CConfigure();

public:
	BOOL LoadConfigure();
	BOOL WriteConfigure();
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);

	BOOL SaveSinglePath(LPCTSTR value);
	BOOL SaveBatchPath(LPCTSTR value);
	BOOL SaveReplacePath(LPCTSTR value);
	BOOL SaveCompletePath(LPCTSTR value);
	BOOL SaveProductPath(LPCTSTR value);
	BOOL SaveLastSourcePath(LPCTSTR value);
	BOOL SaveLastSourceSvnPath(LPCTSTR value);
	BOOL SaveTemplatePath(LPCTSTR value);
	BOOL SaveMakeNsisFile(LPCTSTR value);
	BOOL SaveSubWCRevFile(LPCTSTR value);
	BOOL SaveRevertPath(LPCTSTR value);
	BOOL SaveRevertProductPath(LPCTSTR value);
	BOOL SaveFTPSummaryFile(LPCTSTR value);
	BOOL SaveFTPFilelistFile(LPCTSTR value);
	BOOL SaveFTPRevertlistFile(LPCTSTR value);
	BOOL SaveFTPPath(LPCTSTR value);
	BOOL SaveFTPVersionFile(LPCTSTR value);
	BOOL SaveMajorVersion(UInt32 version, PCSTRING majorVersion);
	VOID ExportMajorVersion(PCSTRING filename);
	VOID GetMajorVersion(UInt32 curVersion, String& outMajor);
	UInt32 GetVersionCRC(UInt32 version);
	VOID SaveVersionCRC(UInt32 version, UInt32 crc);
	VOID ResetVersionCRC(UInt32 version);

private:
	VOID PrintDebug();

// Attributes
public:
	String path_single;
	String path_batch;
	String path_replace;
	String path_complete;
	String path_product;
	String path_lastSource;
	String path_lastSourceSvn;
	String path_filehistory;
	String path_template;
	String path_makensis_file;
	String path_subwcrev_file;
	String path_revertSingle;
	String path_revertProduct;
	String path_ftp_summary_file;
	String path_ftp_filelist_file;
	String path_ftp_revertlist_file;
	String path_ftp;
	String path_ftp_version_file;

private:
	PatchMajorVerMap	m_patchMajorMap;
};

#endif // _CONFIGURE_H
