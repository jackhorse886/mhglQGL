// QGLPublish.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在對 PCH 包含此檔案前先包含 'stdafx.h'
#endif

#include "resource.h"		// 主要符號
#include "common.h"
#include "Configure.h"
#include "ConfigureSVN.h"
#include "ConfigureFilelist.h"
#include "ConfigureSummary.h"
#include "ConfigureRevertlist.h"

enum PATCH_CHANGE
{
	PATCH_CHANGE_INVALID = 0,
	PATCH_CHANGE_ADD,
	PATCH_CHANGE_UPDATE,
	PATCH_CHANGE_DELETE
};

struct PatchChange
{
	String filename;
	UInt8 type; // 0: invalid, 1: add, 2: update, 3: delete
};

typedef StlVector<PatchChange, ManagedAllocator<PatchChange> > PatchChangeVector;
typedef StlMap<UInt32, String, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, String> > > LineFilenameMap;

struct FData
{
	String name;
	UInt64 size;
	UInt32 crc;
	time_t filetime;
};

enum FILE_PROPERTY_STATUS
{
	FILE_PROPERTY_STATUS_UNVERSIONED = 0,
	FILE_PROPERTY_STATUS_NOCHANGE,
	FILE_PROPERTY_STATUS_CHANGED,
	FILE_PROPERTY_STATUS_DELETED,
};

enum FILE_COPY_PROPERTY
{
	FILE_COPY_PROPERTY_NOCOPY = 0,
	FILE_COPY_PROPERTY_PACK,
	FILE_COPY_PROPERTY_FILE,
};

struct FileProperty
{
	String					displayFilename;
	String					method_packFilename;	// where to pack to
	FILE_PROPERTY_STATUS	status;
	FILE_COPY_PROPERTY		method;
};

typedef StlMap<String, FileProperty, std::less<String>, ManagedAllocator<std::pair<String, FileProperty> > > FileDisplayMap;
typedef StlVector<FData, ManagedAllocator<FData> > FileDataList;
typedef StlVector<String, ManagedAllocator<String> > FilterList;

// CQGLPublishApp:
// 請參閱實作此類別的 QGLPublish.cpp
//
void InitLogger();
void CloseLogger();

class CQGLPublishApp : public CWinApp
{
public:
	CQGLPublishApp();
	~CQGLPublishApp();

// 覆寫
	public:
	virtual BOOL InitInstance();

// 程式碼實作

	DECLARE_MESSAGE_MAP()

private:
	VOID UpdateRecurFolderList(PCSTRING folder, PCSTRING relative, FileDisplayMap& displayMap);
	VOID InitializeQGLPublishSetting();

	VOID SetSrcFolder(PCSTRING src)			{m_strSrcDir = src;}
	VOID InitializeFilterList();
	VOID ImportFileBriefList();
	VOID ExportFileBriefList();
	BOOL ExportFileBriefList(PCSTRING folder, PCSTRING relative, FileDataList& filelist);
	BOOL WritePatchVer();

public:
	CConfigure* GetConfigure()				{return &m_config;}
	CConfigureSVN* GetConfigureSVN()		{return &m_configSVN;}
	CConfigureFilelist* GetConfigureFilelist()		{return &m_configFilelist;}
	CConfigureSummary* GetConfigureSummary()		{return &m_configSummary;}
	CConfigureRevertlist* GetConfigureRevertlist()	{return &m_configRevertlist;}
	VOID SetTargetVersion(UInt32 patch, UInt32 revision)		{m_patchVersion = patch; m_patchRevision = revision;}
	VOID SetPrevVersion(UInt32 patch, UInt32 revision)		{m_prevVersion = patch; m_prevRevision = revision;}
	VOID SetMajorVersion(PCSTRING version)		{m_majorVersion = version;}
	String GetMajorVersion()				{return m_majorVersion;}
	VOID SetTargetFolder(PCSTRING target)	{m_strTargetDir = target;}
	String GetSrcFolder()					{return m_strSrcDir;}
	String GetTargetFolder()				{return m_strTargetDir;}
	VOID SetRevertFolder(PCSTRING revert)	{m_strRevertDir = revert;}
	String GetRevertFolder()				{return m_strRevertDir;}
	VOID SetRevertVersion(UInt32 revertFrom, UInt32 revertTo);
	CONST FileDisplayMap GetSrcFileMap()	{return m_srcFileMap;}
	UInt32 GetTargetVersion()				{return m_patchVersion;}
	UInt32 GetTargetRevision()				{return m_patchRevision;}
	CONST FileDisplayMap GetTargetFileMap()	{return m_fileFileMap;}
	CONST FileDisplayMap GetTargetPackMap()	{return m_packFileMap;}
	VOID GetViewPatchList(PCSTRING filename, FileDisplayMap& fileMap);
	String GetSelectedPackFile()			{return m_selPackFile;}
	VOID SetSelectedPackFile(PCSTRING filename)	{m_selPackFile = filename;}
	VOID UpdateSrcFolderList();
	VOID UpdateFileFolderList();
	VOID UpdatePackFolderList();
	VOID UpdateQGLPublishSetting();
	VOID CompareFileBriefList();
	VOID CheckVersionInfo();
	UInt32 GetLatestVersion()				{return m_latestVersion;}
	UInt32 GetPrevVersion()					{return m_prevVersion;}
	UInt32 GetPrevRevision()				{return m_prevRevision;}
	VOID ClearChangeFileMap();
	CONST PatchChangeVector GetFileChangeList()	{return m_archiveChange;}
	CONST PatchChangeVector GetGdfChangeList()	{return m_gdfChange;}
	BOOL AddChangeFileMap(PCSTRING filename);
	BOOL DeleteChangeFileMap(PCSTRING filename);
	VOID ClearChangeGdfMap();
	BOOL AddChangeGdfMap(PCSTRING filename);
	BOOL DeleteChangeGdfMap(PCSTRING filename);
	BOOL ExecutePatch();
	BOOL ExecuteNormalMerge(UInt32 mergeFrom, UInt32 mergeTo);
	BOOL ExecuteIntegrateMerge(UInt32 mergeFrom, UInt32 mergeTo, UInt32 finalVer);
	VOID NewPublishVersion();
	VOID ReadPatchVer(UInt32 version, UInt32& outRevision, UInt32& outSupportVersion, UInt32& outSupportRevision);
	BOOL GenerateSVNRevision();

private:
	// common function
	BOOL FileCompare(PCSTRING file1, PCSTRING file2);
	BOOL PatcherCopyFile(PCSTRING srcFolder, PCSTRING destFolder, PCSTRING filename);
	BOOL PatcherCreateProcess(PCSTRING execCommand);
	BOOL CopyDirectory_Filter_GDF_PATCH(PCSTRING srcDir, PCSTRING destDir);	// depends on PatchCopyFile
	BOOL CopyDirectory(PCSTRING srcDir, PCSTRING destDir);
	BOOL DeleteDirectory(PCSTRING dir);
	BOOL Merge_GDF_PATCH(PCSTRING srcDir, PCSTRING destDir);				// it finds the .gdf.patch files in srcDir
	BOOL NormalMergePatch(UInt32 mergeFrom, UInt32 mergeTo, UInt32 finalMergeTo);
	BOOL WritePatchVer(UInt32 version, UInt32 revision, UInt32 supVersion, UInt32 supRevision, PCSTRING folder);
	BOOL PatcherNSISGenerate(
		PCSTRING templateFilename, PCSTRING nsisFilename, 
		PCSTRING targetMajor, UInt32 targetVersion, UInt32 targetRevision, UInt32 targetVerCRC,
		PCSTRING prevMajor, UInt32 prevVersion, UInt32 prevRevision, UInt32 prevVerCRC,
		PCSTRING execCommand, BOOL writeRevertFlag
		);

private:
	// last source files
	FileDisplayMap m_srcFileMap;
	// (single) pack folder files to open/new
	FileDisplayMap m_packFileMap;
	FileDisplayMap m_fileFileMap;
	FilterList m_filterlist;

	// change to 1) pack; 2) direct copy
	PatchChangeVector m_archiveChange;
	PatchChangeVector m_gdfChange;	// support only 1 GDF for this version

	CConfigure m_config;
	CConfigureSVN m_configSVN;
	CConfigureFilelist m_configFilelist;
	CConfigureSummary m_configSummary;
	CConfigureRevertlist m_configRevertlist;

	String m_strSrcDir;
	String m_strTargetDir;
	String m_selPackFile;
	String m_strRevertDir;
	// Version and revision
	UInt32 m_patchVersion;
	UInt32 m_patchRevision;
	// From version and revision
	UInt32 m_prevVersion;
	UInt32 m_prevRevision;
	// (single) pack folder directory version
	UInt32 m_latestVersion;
	// major version
	String m_majorVersion;
};

extern CQGLPublishApp theApp;
