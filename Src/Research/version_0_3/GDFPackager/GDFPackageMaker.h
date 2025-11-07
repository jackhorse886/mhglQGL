#pragma once
#ifndef _GDFPACKAGEMAKER_H_
#define _GDFPACKAGEMAKER_H_

#include <vector>
#include "GDFcommon.h"
#include "GDFManager.h"

#define HISTORY_FORMAT_VERSION 1

enum MPQ_ARCHIVE_STATUS
{
	MPQ_ARCHIVE_STATUS_NOTOPENED = 0,
	MPQ_ARCHIVE_STATUS_OPENED,
	MPQ_ARCHIVE_STATUS_SAVING
};

struct GdfHistoryData
{
	TCHAR filename[MAX_PATH];
	UInt64 filedate;
	UInt32 filesize;
	UInt32 filechecksum;

	GdfHistoryData()
	{
		ZeroMemory(filename, sizeof(TCHAR) * MAX_PATH);
	};
};

struct GdfHistoryHeader
{
	CHAR magic[4];
	UInt32 headerSize;
	UInt32 version;
	UInt32 numOfEntries;
	UInt32 archiveSize;

	GdfHistoryHeader()
	{
		sprintf(magic, "DAT.");
		version		= HISTORY_FORMAT_VERSION;
		headerSize	= sizeof(GdfHistoryHeader);
	};
};

typedef StlVector<GdfHistoryData*, ManagedAllocator<GdfHistoryData*> > HistoryDataVector;

class GDFPackageMaker
{
private:
	void InitLogger();
	void CloseLogger();

	VOID AddArchiveChange(CONST String SrcFilename, UInt8 SrcType);
	VOID DeleteArchiveChange(CONST String SrcFilename);
	VOID RecursiveAdd(CONST String SrcFilename);

	VOID LoadConfiguration();

public:
	GDFPackageMaker();
	~GDFPackageMaker();

	BOOL Extract(String& patchFilename, String& targetFilename);
	BOOL View(String& patchFilename);	// display List of file
	BOOL ViewPatchStep(String& patchFilename);	// display patch steps
	BOOL Test(String& patchFilename);	// Complete test to see validity and completeness of file, as well as different statistics
	BOOL Repatch(String& patchFilename);		// repatch
//
//	BOOL GetListFile();

	// nothing to be prepared
	BOOL ExtractAll(String& patchFilename);	// Extract All files from patch
	// m_archiveChange should be prepared before execution
	BOOL ExecuteFileUpdate(String& patchFilename, UInt32 writeLogVersion = 0, BOOL ForceMaster = FALSE);	// Execute update of patch, where file source is current archive list.
	// nothing to be prepared
	BOOL ExecuteMerge(String& ToFilename, String& FromFilename, BOOL inheritPatchStep);

	BOOL ReadHistoryFile();
	BOOL WriteHistoryFile();
	GdfHistoryData* GetHistoryData(String& SrcFilename);

public:
	// file list update manipulation:
	VOID Add(CONST String SrcFilename);							// add/update file to list
	VOID Delete(CONST String TrgFilename, BOOL IsDirectory);	// delete file from list
	VOID UpdatePatch(CONST String SrcFilename, BOOL detailCheck);

private:
	ArchiveChangeVector m_archiveChange;
	HistoryDataVector	m_historyData;
};

#endif
