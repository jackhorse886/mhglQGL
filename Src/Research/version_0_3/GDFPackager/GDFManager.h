#pragma once
#ifndef _GDFMANAGER_H_
#define _GDFMANAGER_H_

#include "GDFcommon.h"
#include "GDFFile.h"
#include <vector>

class GDFStream;

class GDFManager
{
private:
	bool GetTimeFromFileTime(const FILETIME &fileTime, time_t &time);
	void GetFileTimeFromTime(const time_t &time, FILETIME &fileTime);

private:

	void InitLogger();
	void CloseLogger();

	VOID LoadConfiguration();

	BOOL IsGdfStreamSame(GDFStream* stream1, GDFStream* stream2);

public:
	GDFManager();
	~GDFManager();

	// For general purpose
	BOOL LoadPatchFile(String& PatchFilename, GDFFILE_STATE fileState);
	GDFStream* GetStream(String& TargetFilename);
	BYTE* GetMapViewOfFile(String& TargetFilename, UInt32& beginOffset, UInt32& fileSize);
	BOOL ClosePatchFile();
	BOOL IsGdfStreamSame(GDFStream &stream1, GDFStream &stream2);			// completed.  No specific purpose
	UInt32 GetHashSize() {m_pMainFile.GetHashSize();};
	BOOL GetBlockStat(UInt32& numAllBlock, UInt32& numUsedBlock, UInt32& sizeAllBlock, UInt32& sizeUsedBlock);

	// For GDFPackageMaker
	BOOL Merge(String& ToFilename, String& FromFilename, BOOL inheritPatchStep = TRUE);
	BOOL UpdatePatch(String& PatchFilename, ArchiveChangeVector& change, UInt32 writeLogVersion, BOOL ForceMaster);
	BOOL TestPatchFile(String& patchFilename);
	BOOL Repatch(String& patchFilename);
	ListFileVector& GetListFile();
	PatchStepMap& GetPatchStep();

private:
	GDFFile m_pMainFile;

//	ArchiveChangeVector *m_archiveChange; // reading for modification, provided by QGLPackageMaker
};

#endif
