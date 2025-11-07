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

	VOID AddArchiveChange(PCSTRING SrcFilename, UInt8 SrcType);
	VOID DeleteArchiveChange(PCSTRING SrcFilename);
	VOID RecursiveAdd(PCSTRING SrcFilename, PCSTRING srcFolder);

	VOID LoadConfiguration();

public:
	GDFPackageMaker();
	~GDFPackageMaker();

	BOOL Extract(PCSTRING patchFilename, PCSTRING targetFilename);
	BOOL View(PCSTRING patchFilename);	// display List of file
	BOOL ViewPatchStep(PCSTRING patchFilename);	// display patch steps
	BOOL Test(PCSTRING patchFilename);	// Complete test to see validity and completeness of file, as well as different statistics
	BOOL Repatch(PCSTRING patchFilename);		// repatch
//
//	BOOL GetListFile();

	// nothing to be prepared
	BOOL ExtractAll(PCSTRING patchFilename);	// Extract All files from patch
	// m_archiveChange should be prepared before execution
	BOOL ExecuteFileUpdate(PCSTRING patchFilename, PCSTRING srcFolder = NULL, UInt32 writeLogVersion = 0, BOOL ForceMaster = FALSE);	// Execute update of patch, where file source is current archive list.
	// nothing to be prepared
	BOOL ExecuteMerge(PCSTRING ToFilename, PCSTRING FromFilename, BOOL inheritPatchStep);

	BOOL ReadHistoryFile();
	BOOL WriteHistoryFile();
	GdfHistoryData* GetHistoryData(PCSTRING SrcFilename);

public:
	// file list update manipulation:
	VOID Add(PCSTRING SrcFilename, PCSTRING srcFolder);							// add/update file to list
	VOID Delete(PCSTRING TrgFilename, BOOL IsDirectory);	// delete file from list
	VOID UpdatePatch(PCSTRING SrcFilename, BOOL detailCheck);

private:
	ArchiveChangeVector m_archiveChange;
	HistoryDataVector	m_historyData;
};

#endif
