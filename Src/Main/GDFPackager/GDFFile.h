#pragma once
#ifndef _GDFFILE_H_
#define _GDFFILE_H_

#include "GDFcommon.h"
#include "GDFStream.h"
#include <vector>

class GDFStream;

class GDFFile
{
public:
	GDFFile();
	~GDFFile();

public:
	BOOL NewFile(PCSTRING PatchFilename, UInt32 defaultHashEntries_power2);
	BOOL LoadFile(PCSTRING PatchFilename);
	BOOL LoadFileWithList(PCSTRING PatchFilename);
	BOOL LoadFileWrite(PCSTRING PatchFilename, UInt32 defaultHashEntries = 0, PCSTRING revertFilename = NULL);
	GDFStream* GetStream(PCSTRING TargetFilename);
	BYTE* GetMapViewOfFile(PCSTRING TargetFilename, UInt32& beginOffset, UInt32& fileSize);
	BOOL IsGdfFileExist(PCSTRING TargetFilename);
	BOOL UpdateFromStream(PatchStepMap& patchstep, GDFFile* SrcGdfFile, BOOL mergePatchStep = FALSE);
	BOOL UpdateFromFile(ArchiveChangeVector& change, PCSTRING srcFolder, UInt32 writeLogVersion);
	BOOL CloseFile();
	ListFileVector& GetListFile();
	PatchStepMap& GetPatchStep();
	BOOL TestFile(PCSTRING patchFilename);
	UInt32 GetHashSize() {return m_header->hashTableEntries;};
	// For GDFStream
	BOOL ReadStream(GDFStreamData* streamdata, BYTE* OutBuffer, UInt32 readLength, UInt32& OutLength, UInt32 MaxLength);
	// statistic use
	BOOL GetBlockStat(UInt32& numAllBlock, UInt32& numUsedBlock, UInt32& sizeAllBlock, UInt32& sizeUsedBlock);
	// backup for update "write" process
	BOOL BackupHashBlock(PCSTRING bkFilename);
	BOOL RevertFromBackup(PCSTRING bkFilename);

private:
	void InitializeCryptTable();
	void EncryptData(void *lpbyBuffer, unsigned long dwLength, unsigned long dwKey);
	void DecryptData(void *lpbyBuffer, unsigned long dwLength, unsigned long dwKey);

	BOOL PeekPatchInfo(PCSTRING PatchFilename);
	BOOL PeekPatchInfoWithList(PCSTRING PatchFilename);
	BOOL LoadHeader(File& pFile);
	BOOL LoadHashTable(File& pFile, UInt32 hashOffset, UInt32 hashEntries);
	BOOL LoadBlockTable(File& pFile, UInt32 blockOffset, UInt32 blockEntries);
	BOOL LoadListFile();
	BOOL LoadPatchStep();
	BOOL SavePatchHeader(File& f0);
	BOOL SavePatchHashTable(File& f0, UInt32 fileOffset);
	BOOL SavePatchBlockTable(File& f0, UInt32 fileOffset);
	BOOL SavePatchData(PatchStepMap& patchstep, GDFFile* SrcGdfFile, BOOL mergePatchStep);
	BOOL SavePatchData(ArchiveChangeVector& change, PCSTRING srcFolder, UInt32 writeLogVersion);
	VOID FreePatch();
	VOID FreeHeader();
	VOID FreeHashTable();
	VOID FreeBlockTable();
	VOID FreeListFile();
	VOID FreePatchStep();
	unsigned long HashString(const TCHAR *lpszString, unsigned long dwHashType);
	bool FindFileInHashTable(const HashTableVector &lpHashTable, const TCHAR *lpszFilePath, unsigned short nLang, unsigned char nPlatform, unsigned long &iFileHashEntry);
	BOOL InsertFileFromMemory(File& f0, PCSTRING TargetFilename, BYTE* pData, UInt32 bufSize, BOOL addToListfile = TRUE);
	BOOL InsertFileFromStream(File& f0, PCSTRING TargetFilename, GDFStream* istream);
	BOOL InsertFileFromFile(File& f0, PCSTRING TargetFilename, PCSTRING SourcePath);
	BOOL DeleteFile(File& f0, PCSTRING TargetFilename);
	bool DeleteFile(const TCHAR *lpszFilePath, unsigned short nLang, unsigned char nPlatform);
	BOOL IsGdfStreamSame(PCSTRING TargetFilename, PCSTRING FilePath);
	BOOL IsGdfStreamSame(PCSTRING TargetFilename, GDFStream* stream2);
	BOOL IsGdfStreamSame(PCSTRING TargetFilename, BYTE* pData, UInt32 bufSize);
	UInt32 GetListFileLength();		// in TCHAR
	UInt32 GetListFile(TCHAR* out);
	GdfHashTable* GetHashTable(UInt32 hashPos);
	GdfBlockTable* GetBlockTable(UInt32 blockPos);
	UInt32 GetPatchStepLength();	// in TCHAR
	UInt32 GetPatchStep(TCHAR* out);
	BOOL PatchStepAdd(UInt32 writeLogVersion, PCSTRING filename);
	BOOL PatchStepDelete(UInt32 writeLogVersion, PCSTRING filename);
	VOID DeleteBackup();
	// hash change, special case
	BOOL IncreaseHash(UInt32 newhashsize);
	// calc log, special use
	UInt32 FindMin2PowerInt(UInt32 number);
	CONST String& GetBackupFilename(PCSTRING PatchFilename);


private:
	unsigned long m_dwCryptTable[0x500];

	CRITICAL_SECTION GDFFileReadThreadLock;

	UInt32				m_state;
	File				m_file;			// for persistent connection only
	GdfHeader*			m_header;
	HashTableVector		m_hashTable;
	BlockTableVector	m_blockTable;
	ListFileVector		m_listFile;
	PatchStepMap		m_patchStepMap;

	String				m_filename;
	String				m_backup_filename;

	UInt8				m_mode;

	HANDLE				m_hFile;
	HANDLE				m_hMapFile;
//	BYTE*				m_pInFile;
};

#endif	// _GDFFILE_H_
