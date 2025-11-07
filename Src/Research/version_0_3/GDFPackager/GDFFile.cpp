#include "common.h"
#include <stdlib.h>
#include "GDFFile.h"

#define STREAM_BUF_MAX	65536

GDFFile::GDFFile()
{
	InitializeCriticalSection(&GDFFileReadThreadLock);
	InitializeCryptTable();

	m_header = NULL;
	m_hashTable.clear();
	m_blockTable.clear();
	m_listFile.clear();
	m_filename.Empty();
	m_backup_filename.Empty();
	m_patchStepMap.clear();

	m_mode	= GDFFILE_STATE_GAME;
}

GDFFile::~GDFFile()
{
	FreePatch();
	DeleteCriticalSection(&GDFFileReadThreadLock);
}

// The encryption and hashing functions use a number table in their procedures. This table must be initialized before the functions are called the first time.
void GDFFile::InitializeCryptTable()
{
    unsigned long seed   = 0x00100001;
    unsigned long index1 = 0;
    unsigned long index2 = 0;
    int   i;

    for (index1 = 0; index1 < 0x100; index1++)
    {
        for (index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
        {
            unsigned long temp1, temp2;

            seed  = (seed * 125 + 3) % 0x2AAAAB;
            temp1 = (seed & 0xFFFF) << 0x10;

            seed  = (seed * 125 + 3) % 0x2AAAAB;
            temp2 = (seed & 0xFFFF);

            m_dwCryptTable[index2] = (temp1 | temp2);
        }
    }
}

// Based on code from StormLib.
unsigned long GDFFile::HashString(const TCHAR *lpszString, unsigned long dwHashType)
{
    DEBUG_ENSURE(lpszString);
    DEBUG_ENSURE(dwHashType <= MPQ_HASH_FILE_KEY);
    
    unsigned long  seed1 = 0x7FED7FEDL;
    unsigned long  seed2 = 0xEEEEEEEEL;
    unsigned char    ch;

	String tempstring = lpszString;
	tempstring.ToUpper();
	tempstring.Replace(_T("/"), _T("\\"));

	if( tempstring.GetLength()>2 && tempstring.Left(2)==_T(".\\") )
		tempstring = tempstring.Right(tempstring.GetLength()-2);

	int len = WideCharToMultiByte(CP_ACP, 0, tempstring.c_str(), -1, NULL, 0, NULL, NULL);
	char* buffer = SafeAllocate(char, len);
	WideCharToMultiByte(CP_ACP, 0, tempstring.c_str(), -1, buffer, len, NULL, NULL);
	char* bufptr = buffer;

	int count = len;
	while (count > 1)
    {
        ch = *bufptr++;
		count--;

        seed1 = m_dwCryptTable[(dwHashType * 0x100) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
    }

	SafeDeallocate(buffer);

    return seed1;
}

bool GDFFile::FindFileInHashTable(const HashTableVector &lpHashTable, const TCHAR *lpszFilePath, unsigned short nLang, unsigned char nPlatform, unsigned long &iFileHashEntry)
{
	DEBUG_ENSURE(lpszFilePath);

	if( lpHashTable.size()==0 ) return false;
	
	// Find the home entry in the hash table for the file
	unsigned long iInitEntry = HashString(lpszFilePath, MPQ_HASH_TABLE_OFFSET) & (lpHashTable.size() - 1);
		
	// Is there anything there at all?
	if (lpHashTable[iInitEntry]->fileBlockIndex == MPQ_HASH_ENTRY_EMPTY)
		return false;
		
	// Compute the hashes to compare the hash table entry against
	unsigned long nNameHashA = HashString(lpszFilePath, MPQ_HASH_NAME_A),
		nNameHashB = HashString(lpszFilePath, MPQ_HASH_NAME_B),
		iCurEntry = iInitEntry;
		
	// Check each entry in the hash table till a termination point is reached
	do
	{
		if (lpHashTable[iCurEntry]->fileBlockIndex != MPQ_HASH_ENTRY_DELETED)
		{
			if (lpHashTable[iCurEntry]->filePathHashA == nNameHashA 
				&& lpHashTable[iCurEntry]->filePathHashB == nNameHashB
				&& lpHashTable[iCurEntry]->language == nLang
				&& lpHashTable[iCurEntry]->platform == nPlatform)
			{
				iFileHashEntry = iCurEntry;
				return true;
			}
		}
			
		iCurEntry = (iCurEntry + 1) & (lpHashTable.size() - 1);
	} while (iCurEntry != iInitEntry && lpHashTable[iCurEntry]->fileBlockIndex != MPQ_HASH_ENTRY_EMPTY);
	return false;
}

bool GDFFile::DeleteFile(const TCHAR *lpszFilePath, unsigned short nLang, unsigned char nPlatform)
{
	String targetFilename = lpszFilePath;
	targetFilename.ToUpper();
	targetFilename.Replace(_T("/"), _T("\\"));

	if( targetFilename.GetLength()>2 && targetFilename.Left(2)==_T(".\\") )
		targetFilename = targetFilename.Right(targetFilename.GetLength()-2);

	// Find the file in the hash table
	unsigned long iFileHashEntry;

	if (!FindFileInHashTable(m_hashTable, targetFilename.c_str(), nLang, nPlatform, iFileHashEntry))
		return false;
	
	// Get the block table index before we nuke the hash table entry
	unsigned long iFileBlockEntry = m_hashTable[iFileHashEntry]->fileBlockIndex;
	
	// Delete the file's entry in the hash table
	memset(m_hashTable[iFileHashEntry], 0xFF, sizeof(GdfHashTable));

	// If the next entry is empty, mark this one as empty; otherwise, mark this as deleted.
	if (m_hashTable[(iFileHashEntry + 1) & (m_hashTable.size() - 1)]->fileBlockIndex == MPQ_HASH_ENTRY_EMPTY)
		m_hashTable[iFileHashEntry]->fileBlockIndex = MPQ_HASH_ENTRY_EMPTY;
	else
		m_hashTable[iFileHashEntry]->fileBlockIndex = MPQ_HASH_ENTRY_DELETED;
	
	// If the block occupies space, mark the block as free space; otherwise, clear the block table entry.
	if (m_blockTable[iFileBlockEntry]->blockSize > 0)
	{
		m_blockTable[iFileBlockEntry]->fileSize = 0;
		m_blockTable[iFileBlockEntry]->flags = 0;
	}
	else
		memset(m_blockTable[iFileBlockEntry], 0, sizeof(GdfBlockTable));
		
	return true;
}

void GDFFile::EncryptData(void *lpbyBuffer, unsigned long dwLength, unsigned long dwKey)
{
    DEBUG_ENSURE(lpbyBuffer);

    unsigned long *lpdwBuffer = (unsigned long *)lpbyBuffer;
    unsigned long seed = 0xEEEEEEEE;
    unsigned long ch;

    dwLength /= sizeof(unsigned long);

    while(dwLength-- > 0)
    {
        seed += m_dwCryptTable[0x400 + (dwKey & 0xFF)];
        ch = *lpdwBuffer ^ (dwKey + seed);

        dwKey = ((~dwKey << 0x15) + 0x11111111) | (dwKey >> 0x0B);
        seed = *lpdwBuffer + seed + (seed << 5) + 3;

		*lpdwBuffer++ = ch;
    }
}

void GDFFile::DecryptData(void *lpbyBuffer, unsigned long dwLength, unsigned long dwKey)
{
    DEBUG_ENSURE(lpbyBuffer);

    unsigned long *lpdwBuffer = (unsigned long *)lpbyBuffer;
    unsigned long seed = 0xEEEEEEEEL;
    unsigned long ch;

    dwLength /= sizeof(unsigned long);

    while(dwLength-- > 0)
    {
        seed += m_dwCryptTable[0x400 + (dwKey & 0xFF)];
        ch = *lpdwBuffer ^ (dwKey + seed);

        dwKey = ((~dwKey << 0x15) + 0x11111111L) | (dwKey >> 0x0B);
        seed = ch + seed + (seed << 5) + 3;

		*lpdwBuffer++ = ch;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GDFFile::SavePatchData(PatchStepMap& patchstep, GDFFile* SrcGdfFile, BOOL mergePatchStep)		// P2M or P2P
{
	if( patchstep.size() > 0 )	// patch step mode
	{
		for( PatchStepMap::iterator itr = patchstep.begin(); itr!=patchstep.end(); ++itr )
		{
			// preprocess Changes
			for( Index i = C_INDEX(0); i < itr->second.size(); ++i )
			{
				if( itr->second[i]->type==MPQ_ARCHIVE_CHANGE_ADD && IsGdfFileExist(itr->second[i]->filename) )
					itr->second[i]->type = MPQ_ARCHIVE_CHANGE_UPDATE;
			}

			// new hash size check
			if( itr->second.size() + m_listFile.size() > 0 && itr->second.size() + m_listFile.size() + 2 > m_hashTable.size() )	// (listfile) + (patchstep)
			{
				Int32 counter = 0;
				for( PatchStepMap::iterator itr = patchstep.begin(); itr!=patchstep.end(); ++itr )
				{
					for( Index i = C_INDEX(0); i < itr->second.size(); ++i )
					{
						switch( itr->second[i]->type )
						{
						case MPQ_ARCHIVE_CHANGE_DELETE:	counter--;	break;
						case MPQ_ARCHIVE_CHANGE_ADD:	counter++;	break;
						}
					}
				}

				UInt32 curPower		= 0;
				Int32 hashsize		= (Int)m_hashTable.size();
				Int32 newhashsize	= (Int)m_listFile.size() + counter + 2;	// listfile & patchstep (hidden)
				if( newhashsize > hashsize )
				{
					// increase current hash size
					curPower = FindMin2PowerInt((UInt32)newhashsize);
					// if hash size must increase before merge, increase it to integer powered by 2
					if( curPower )
					{
						if( !IncreaseHash(curPower) )
						{
							TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (5A) [increase hash failed]"));
							return FALSE;
						}
					}else
					{
						TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (4A) [increase hash, but cur. hash calc. error] newhashsize=%i"), newhashsize);
						return FALSE;
					}
				}
			}

			for( Index i = C_INDEX(0); i < itr->second.size(); ++i )
			{
				if( itr->second[i] && itr->second[i]->type==MPQ_ARCHIVE_CHANGE_DELETE )
				{
					if( DeleteFile(itr->second[i]->filename.c_str(), MPQNeutral, 0) )
					{
						GdfListFile* tmplist;
						for( ListFileVector::iterator itr2 = m_listFile.begin(); itr2 != m_listFile.end(); )
						{
							tmplist = *itr2;
							if( tmplist )
							{
								if( tmplist->filename.CompareNoCase(itr->second[i]->filename.c_str())==0 )
								{
									SafeDeleteObject(tmplist);
									m_listFile.erase(itr2);
									itr2 = m_listFile.begin();
								}else
								{
									++itr2;
								}
							}
						}
					}else
					{
						TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (1A) [Delete File error] filename=%s"), itr->second[i]->filename.c_str());
						return FALSE;
					}
					if( mergePatchStep )
						PatchStepDelete(itr->first, itr->second[i]->filename);
				}
			}
			m_file.SetPosition(m_header->hashTableOffset, Reuben::Platform::File::MOVE_BEGIN);
			for( Index i = C_INDEX(0); i < itr->second.size(); ++i )
			{
				if( itr->second[i] )
				{
					switch( itr->second[i]->type )
					{
					case MPQ_ARCHIVE_CHANGE_ADD:
					case MPQ_ARCHIVE_CHANGE_UPDATE:
						{
							GDFStream* fstream = SrcGdfFile->GetStream(itr->second[i]->filename);
							if( fstream )
							{
								if( !InsertFileFromStream(m_file, itr->second[i]->filename, fstream) )
								{
									TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (2A) [Insert File Stream error] filename=%s"), itr->second[i]->filename.c_str());
									return FALSE;
								}
								if( mergePatchStep )
									PatchStepAdd(itr->first, itr->second[i]->filename);
							}else
							{
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (3A) [source stream not found] filename=%s"), itr->second[i]->filename.c_str());
								return FALSE;
							}
						}
						break;
					}
				}
			}
			// (listfile) handle
			String listfile_filename	= _T("(listfile)");
			UInt32 listfile_tLength		= GetListFileLength();
			TCHAR* tempbuf				= SafeAllocate(TCHAR, listfile_tLength);
			GetListFile(tempbuf);
			unsigned long hashkey = HashString(_T("(listfile)"), MPQ_HASH_FILE_KEY);
			EncryptData((VOID*)tempbuf, listfile_tLength * sizeof(TCHAR), hashkey);

			if(!InsertFileFromMemory(m_file, listfile_filename, (BYTE*)tempbuf, listfile_tLength * sizeof(TCHAR), FALSE))
			{
				TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (6A) [list file save error]"));
				return FALSE;
			}
			SafeDeallocate(tempbuf);
			// (patchstep) handle
			String patchstep_filename	= _T("(patchstep)");
			UInt32 patchstep_tLength	= GetPatchStepLength();
			TCHAR* tempstepbuf			= SafeAllocate(TCHAR, patchstep_tLength);
			GetPatchStep(tempstepbuf);
			hashkey = HashString(_T("(patchstep)"), MPQ_HASH_FILE_KEY);
			EncryptData((VOID*)tempstepbuf, patchstep_tLength * sizeof(TCHAR), hashkey);

			if(!InsertFileFromMemory(m_file, patchstep_filename, (BYTE*)tempstepbuf, patchstep_tLength * sizeof(TCHAR), FALSE))
			{
				TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (7B) [(patchstepfile) save error]"));
				return FALSE;
			}
			SafeDeallocate(tempstepbuf);
			m_header->SetMagic("MPQ.");
			m_header->SetHashTableOffset((UInt32)m_file.GetPosition());
			m_header->SetBlockTableEntries((UInt32)m_blockTable.size());
			m_header->SetHashTableEntries((UInt32)m_hashTable.size());
			m_header->SetHeaderSize(sizeof(GdfHeader));
			m_header->SetSectorSizeShift(1);
			m_header->SetFormatVersion(1);
		}
	}else	// list file mode
	{
		ListFileVector listvec = SrcGdfFile->GetListFile();
		ArchiveChangeVector change;
		for( Index i = C_INDEX(0); i < listvec.size(); ++i )
		{
			GdfArchiveChange* cha = SafeCreateObject(GdfArchiveChange);
			cha->filename		= listvec[i]->filename;
			cha->type			= MPQ_ARCHIVE_CHANGE_ADD;
			change.push_back(cha);
		}
		// preprocess Changes
		for( Index i = C_INDEX(0); i < change.size(); ++i )
		{
			if( change[i]->type==MPQ_ARCHIVE_CHANGE_ADD && IsGdfFileExist(change[i]->filename) )
				change[i]->type = MPQ_ARCHIVE_CHANGE_UPDATE;
		}

		// new hash size check
		if( change.size() + m_listFile.size() > 0 && change.size() + m_listFile.size() + 2 > m_hashTable.size() )	// (listfile) + (patchstep)
		{
			Int32 counter = 0;
			for( Index i = C_INDEX(0); i < change.size(); ++i )
			{
				switch( change[i]->type )
				{
				case MPQ_ARCHIVE_CHANGE_ADD:	counter++;	break;
				}
			}

			UInt32 curPower		= 0;
			Int32 hashsize		= (Int)m_hashTable.size();
			Int32 newhashsize	= (Int)m_listFile.size() + counter + 2;	// listfile & patchstep (hidden)
			if( newhashsize > hashsize )
			{
				// increase current hash size
				curPower = FindMin2PowerInt((UInt32)newhashsize);
				// if hash size must increase before merge, increase it to integer powered by 2
				if( curPower )
				{
					if( !IncreaseHash(curPower) )
					{
						TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (5A) [increase hash failed]"));
						return FALSE;
					}
				}else
				{
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (4A) [increase hash, but cur. hash calc. error] newhashsize=%i"), newhashsize);
					return FALSE;
				}
			}
		}

		m_file.SetPosition(m_header->hashTableOffset, Reuben::Platform::File::MOVE_BEGIN);
		for( Index i = C_INDEX(0); i < change.size(); ++i )
		{
			if( change[i] )
			{
				switch( change[i]->type )
				{
				case MPQ_ARCHIVE_CHANGE_ADD:
				case MPQ_ARCHIVE_CHANGE_UPDATE:
					{
						GDFStream* fstream = SrcGdfFile->GetStream(change[i]->filename);
						if( fstream )
						{
							if( !InsertFileFromStream(m_file, change[i]->filename, fstream) )
							{
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (2A) [Insert File Stream error] filename=%s"), change[i]->filename.c_str());
								return FALSE;
							}
						}else
						{
							TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (3A) [source stream not found] filename=%s"), change[i]->filename.c_str());
							return FALSE;
						}
					}
					break;
				}
			}
		}
		// (listfile) handle
		String listfile_filename	= _T("(listfile)");
		UInt32 listfile_tLength		= GetListFileLength();
		TCHAR* tempbuf				= SafeAllocate(TCHAR, listfile_tLength);
		GetListFile(tempbuf);
		unsigned long hashkey = HashString(_T("(listfile)"), MPQ_HASH_FILE_KEY);
		EncryptData((VOID*)tempbuf, listfile_tLength * sizeof(TCHAR), hashkey);

		if(!InsertFileFromMemory(m_file, listfile_filename, (BYTE*)tempbuf, listfile_tLength * sizeof(TCHAR), FALSE))
		{
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (6A) [list file save error]"));
			return FALSE;
		}
		SafeDeallocate(tempbuf);
		// (patchstep) handle
		String patchstep_filename	= _T("(patchstep)");
		UInt32 patchstep_tLength	= GetPatchStepLength();
		TCHAR* tempstepbuf			= SafeAllocate(TCHAR, patchstep_tLength);
		GetPatchStep(tempstepbuf);
		hashkey = HashString(_T("(patchstep)"), MPQ_HASH_FILE_KEY);
		EncryptData((VOID*)tempstepbuf, patchstep_tLength * sizeof(TCHAR), hashkey);

		if(!InsertFileFromMemory(m_file, patchstep_filename, (BYTE*)tempstepbuf, patchstep_tLength * sizeof(TCHAR), FALSE))
		{
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (7B) [(patchstepfile) save error]"));
			return FALSE;
		}
		SafeDeallocate(tempstepbuf);
		m_header->SetMagic("MPQ.");
		m_header->SetHashTableOffset((UInt32)m_file.GetPosition());
		m_header->SetBlockTableEntries((UInt32)m_blockTable.size());
		m_header->SetHashTableEntries((UInt32)m_hashTable.size());
		m_header->SetHeaderSize(sizeof(GdfHeader));
		m_header->SetSectorSizeShift(1);
		m_header->SetFormatVersion(1);
	}
	return TRUE;
}

BOOL GDFFile::SavePatchData(ArchiveChangeVector& change, UInt32 writeLogVersion)
{
	// preprocess Changes
	for( Index i = C_INDEX(0); i < change.size(); ++i )
	{
		GdfArchiveChange* ac = change[i];
		if( ac && ac->type==MPQ_ARCHIVE_CHANGE_ADD && IsGdfFileExist(ac->filename) )
			ac->type = MPQ_ARCHIVE_CHANGE_UPDATE;
	}
	// new hash size check
	if( m_listFile.size() + change.size() > 0 && m_listFile.size() + change.size() + 2 > m_hashTable.size() )	// (listfile) + (patchstep)
	{
		Int32 counter = 0;
		GdfArchiveChange* tempcheck;
		for( Index i = C_INDEX(0); i < change.size(); ++i )
		{
			tempcheck = change[i];
			if( tempcheck )
			{
				switch( tempcheck->type )
				{
					case MPQ_ARCHIVE_CHANGE_DELETE:		counter--;	break;
					case MPQ_ARCHIVE_CHANGE_ADD:		counter++;	break;
				}
			}
		}
		UInt32 curPower		= 0;
		Int32 hashsize		= (Int)m_hashTable.size();
		Int32 newhashsize	= (Int)m_listFile.size() + counter + 1 + 1;	// listfile & patchstep
		if( newhashsize > hashsize )
		{
			// increase current hash size
			curPower = FindMin2PowerInt((UInt32)newhashsize);
			// if hash size must increase before merge, increase it to integer powered by 2
			if( curPower )
			{
				if( !IncreaseHash(curPower) )
				{
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (5B) [increase hash failed]"));
					return FALSE;
				}
			}else
			{
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (4B) [increase hash, but cur. hash calc. error] newhashsize=%i"), newhashsize);
				return FALSE;
			}
		}
	}

	for( Index i = C_INDEX(0); i < change.size(); ++i )
	{
		GdfArchiveChange* archiveChange = change[i];
		if( archiveChange && archiveChange->type==MPQ_ARCHIVE_CHANGE_DELETE )
		{
			if( DeleteFile(archiveChange->filename.c_str(), MPQNeutral, 0) )
			{
				GdfListFile* tmplist;
				for( ListFileVector::iterator itr2 = m_listFile.begin(); itr2 != m_listFile.end(); )
				{
					tmplist = *itr2;
					if( tmplist )
					{
						if( tmplist->filename.CompareNoCase(archiveChange->filename.c_str())==0 )
						{
							SafeDeleteObject(tmplist);
							m_listFile.erase(itr2);
							itr2 = m_listFile.begin();
						}else
						{
							++itr2;
						}
					}
				}
			}else
			{
				if( writeLogVersion==0 )	// if this step is for real file manipulation, file MUST BE found, so error is displayed
				{
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (1B) [Delete File error] filename=%s"), archiveChange->filename.c_str());
					return FALSE;
				}
			}
			PatchStepDelete(writeLogVersion, archiveChange->filename);
		}
	}
	m_file.SetPosition(m_header->hashTableOffset, Reuben::Platform::File::MOVE_BEGIN);
	for( Index i = C_INDEX(0); i < change.size(); ++i )
	{
		GdfArchiveChange* archiveChange = change[i];
		if( archiveChange )
		{
			switch( archiveChange->type )
			{
			case MPQ_ARCHIVE_CHANGE_ADD:
			case MPQ_ARCHIVE_CHANGE_UPDATE:
				{
					File f0;
					f0.Open(archiveChange->filename.c_str(), Reuben::Platform::File::MODE_READ_EXISTING);
					if( f0.IsOpened() )
					{
						if( !InsertFileFromFile(m_file, archiveChange->filename, archiveChange->filename) )
						{
							TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (2B) [Insert File error] filename=%s"), archiveChange->filename.c_str());
							return FALSE;
						}
						PatchStepAdd(writeLogVersion, archiveChange->filename);
					}else
					{
						TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (3B) [source file not opened] filename=%s"), archiveChange->filename.c_str());
						return FALSE;
					}
				}
				break;
			}
		}
	}
	// (listfile) handle
	String listfile_filename	= _T("(listfile)");
	UInt32 listfile_tLength		= GetListFileLength();
	TCHAR* tempbuf				= SafeAllocate(TCHAR, listfile_tLength);
	GetListFile(tempbuf);
	unsigned long hashkey = HashString(_T("(listfile)"), MPQ_HASH_FILE_KEY);
	EncryptData((VOID*)tempbuf, listfile_tLength * sizeof(TCHAR), hashkey);
	
	if(!InsertFileFromMemory(m_file, listfile_filename, (BYTE*)tempbuf, listfile_tLength * sizeof(TCHAR), FALSE))
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (6B) [(listfile) save error]"));
		return FALSE;
	}
	SafeDeallocate(tempbuf);

	// (patchstep) handle
	String patchstep_filename	= _T("(patchstep)");
	UInt32 patchstep_tLength	= GetPatchStepLength();
	TCHAR* tempstepbuf			= SafeAllocate(TCHAR, patchstep_tLength);
	GetPatchStep(tempstepbuf);
	hashkey = HashString(_T("(patchstep)"), MPQ_HASH_FILE_KEY);
	EncryptData((VOID*)tempstepbuf, patchstep_tLength * sizeof(TCHAR), hashkey);
	
	if(!InsertFileFromMemory(m_file, patchstep_filename, (BYTE*)tempstepbuf, patchstep_tLength * sizeof(TCHAR), FALSE))
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchData Error (7B) [(patchstepfile) save error]"));
		return FALSE;
	}
	SafeDeallocate(tempstepbuf);

	m_header->SetMagic("MPQ.");
	m_header->SetBlockTableEntries((UInt32)m_blockTable.size());
	m_header->SetHashTableEntries((UInt32)m_hashTable.size());
	m_header->SetHeaderSize(sizeof(GdfHeader));
	m_header->SetSectorSizeShift(1);
	m_header->SetFormatVersion(1);

	return TRUE;
}

BOOL GDFFile::PeekPatchInfo(String& PatchFilename)
{
	BOOL succeed = FALSE;
	FreePatch();
	File pFile;
	pFile.Open(PatchFilename.c_str(), File::MODE_READ_EXISTING);
	if (pFile.IsOpened())
	{
		try
		{
			if( LoadHeader(pFile) && 
				LoadHashTable(pFile, m_header->hashTableOffset, m_header->hashTableEntries) && 
				LoadBlockTable(pFile, m_header->blockTableOffset, m_header->blockTableEntries)
				)
			{
				succeed = TRUE;
			}else
			{
				FreePatch();
			}
		}catch(FileException Er)
		{
			FreePatch();
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::PeekPatchInfo Error (1) [general error] PatchFilename=%s"), PatchFilename.c_str());
		}
		pFile.Close();
	}
	return succeed;
}

BOOL GDFFile::PeekPatchInfoWithList(String& PatchFilename)
{
	BOOL succeed = FALSE;
	FreePatch();
	m_file.Open(PatchFilename.c_str(), File::MODE_READ_EXISTING);
	if (m_file.IsOpened())
	{
		try
		{
			if( LoadHeader(m_file) && 
				LoadHashTable(m_file, m_header->hashTableOffset, m_header->hashTableEntries) && 
				LoadBlockTable(m_file, m_header->blockTableOffset, m_header->blockTableEntries) &&
				LoadListFile()
				)
			{
				LoadPatchStep();
				succeed = TRUE;
			}else
			{
				FreePatch();
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::PeekPatchInfoWithList Error (1) [error reading necessary info] PatchFilename=%s"), PatchFilename.c_str());
			}
		}catch(FileException Er)
		{
			FreePatch();
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::PeekPatchInfo Error (2) [general error] PatchFilename=%s"), PatchFilename.c_str());
		}
		m_file.Close();
	}
	return succeed;
}

BOOL GDFFile::LoadHeader(File& pFile)
{
	FreeHeader();

	if( pFile.IsOpened() )
	{
		try
		{
			pFile.SetPosition(0, Reuben::Platform::File::MOVE_BEGIN);
			m_header = SafeCreateObject(GdfHeader);
			pFile.Read((BYTE*)m_header, sizeof(GdfHeader));
		}catch(...)
		{
			FreeHeader();
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::LoadHeader Error (1) [read error]"));
			return FALSE;
		}
	}else
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::LoadHeader Error (2) [file not opened]"));
		return FALSE;
	}

	return TRUE;
}

BOOL GDFFile::LoadHashTable(File& pFile, UInt32 hashOffset, UInt32 hashEntries)
{
	FreeHashTable();
	BOOL succeed = FALSE;

	if( pFile.IsOpened() )
	{
		GdfHashTable* hashTableArray = SafeAllocate(GdfHashTable, hashEntries);
		unsigned long key = HashString(_T("(hash table)"), MPQ_HASH_FILE_KEY);//MPQ_HASH_TABLE_OFFSET
		try
		{
			pFile.SetPosition(hashOffset, Reuben::Platform::File::MoveEnum::MOVE_BEGIN);
			pFile.Read((BYTE*)hashTableArray, hashEntries * sizeof(GdfHashTable));
			DecryptData((VOID*)hashTableArray, hashEntries * sizeof(GdfHashTable), key);
			GdfHashTable* hashRecord;
			for( Index i = C_INDEX(0); i < hashEntries; ++i )
			{
				hashRecord = SafeCreateObject(GdfHashTable);
				memcpy(hashRecord, &hashTableArray[i], sizeof(GdfHashTable));
				m_hashTable.push_back(hashRecord);
			}
			succeed = TRUE;
		}catch(...)
		{
			FreeHashTable();
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::LoadHashTable Error (1) [hash table read error] hashOffset=%i, hashEntries=%i"), hashOffset, hashEntries);
		}
		SafeDeallocate(hashTableArray);
	}else
	{
		TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::LoadHashTable Error (2) [file not opened] hashOffset=%i, hashEntries=%i"), hashOffset, hashEntries);
	}
	return succeed;
}

BOOL GDFFile::LoadBlockTable(File& pFile, UInt32 blockOffset, UInt32 blockEntries)
{
	FreeBlockTable();
	BOOL succeed = FALSE;

	if( pFile.IsOpened() )
	{
		GdfBlockTable* blockTableArray = SafeAllocate(GdfBlockTable, blockEntries);
		unsigned long key = HashString(_T("(block table)"), MPQ_HASH_FILE_KEY);
		try
		{
			pFile.SetPosition(blockOffset, Reuben::Platform::File::MoveEnum::MOVE_BEGIN);
			pFile.Read((BYTE*)blockTableArray, blockEntries * sizeof(GdfBlockTable));
			DecryptData((VOID*)blockTableArray, blockEntries * sizeof(GdfBlockTable), key);

			GdfBlockTable* blockRecord;
			for( Index i = C_INDEX(0); i < blockEntries; ++i )
			{
				blockRecord = SafeCreateObject(GdfBlockTable);
				memcpy(blockRecord, &blockTableArray[i], sizeof(GdfBlockTable));
				m_blockTable.push_back(blockRecord);
			}
			succeed = TRUE;
		}catch(...)
		{
			FreeBlockTable();
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::LoadBlockTable Error (1) [block table read error] hashOffset=%i, hashEntries=%i"), blockOffset, blockEntries);
		}
		SafeDeallocate(blockTableArray);
	}else
	{
		TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::LoadBlockTable Error (2) [file open error] hashOffset=%i, hashEntries=%i"), blockOffset, blockEntries);
	}
	return succeed;
}

BOOL GDFFile::LoadListFile()
{
	FreeListFile();
	BOOL succeed = FALSE;

	if( m_file.IsOpened() )
	{
		String delim;
		delim.Format(_T("%c"), 10);

		UInt outlen = 0;
		String listfile = _T("(listfile)");
		GDFStream* fstream = GetStream(listfile);
		if( fstream )
		{
			UInt32 outlen = 0;
			UInt32 fileSize = fstream->GetFileSize();
			if( fileSize > 0 )
			{
				unsigned long hashkey = HashString(_T("(listfile)"), MPQ_HASH_FILE_KEY);
				BYTE* buf = SafeAllocate(BYTE, fileSize);
//				ZeroMemory(buf, fileSize);
				if( fstream->Read((BYTE*)buf, fileSize, outlen, fileSize) )
				{
					DecryptData(buf, fileSize, hashkey);
					TCHAR* pData = SafeAllocate(TCHAR, fileSize + 1);
					ZeroMemory(pData, fileSize + 2);
					memcpy(pData, buf, fileSize);
//					TCHAR* pData = (TCHAR*)buf;
					String fullFile(pData);

					String::size_type lastPos = fullFile.find_first_not_of(delim.c_str(), 0);
					String::size_type pos     = fullFile.find_first_of(delim.c_str(), lastPos);

					GdfListFile* listfile;

					while (fullFile.npos != pos && fullFile.npos != lastPos)
					{
						listfile = SafeCreateObject(GdfListFile);
						listfile->filename = String(fullFile.substr(lastPos, pos - lastPos - 1).c_str());
						listfile->filePathHashA = HashString(listfile->filename.c_str(), MPQ_HASH_NAME_A);
						listfile->filePathHashB = HashString(listfile->filename.c_str(), MPQ_HASH_NAME_B);
						m_listFile.push_back(listfile);

						lastPos = fullFile.find_first_not_of(delim.c_str(), pos);
						pos = fullFile.find_first_of(delim.c_str(), lastPos);
					}
					succeed = TRUE;
					SafeDeallocate(pData);
				}else
				{
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::LoadListFile Error (1) [file stream read error for list file.]"));
				}
				SafeDeallocate(buf);
			}else
			{
				succeed = TRUE;
				DEBUG_INFODTL(GLOBAL_LOGGER, _T("GDFFile::LoadListFile Warning (2) [file list size=0.]"));
			}
		}else
		{
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::LoadListFile Error (3) [file stream not found for list file.]"));
		}
	}else
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::LoadListFile Error (4) [file open error]"));
	}
	return succeed;
}

VOID GDFFile::FreePatch()
{
	FreeHeader();
	FreeHashTable();
	FreeBlockTable();
	FreeListFile();
	FreePatchStep();
}

VOID GDFFile::FreeHashTable()
{
	for( Index i = C_INDEX(0); i < m_hashTable.size(); ++i )
		SafeDeleteObject(m_hashTable[i]);
	m_hashTable.clear();
}

VOID GDFFile::FreeBlockTable()
{
	for( Index i = C_INDEX(0); i < m_blockTable.size(); ++i )
		SafeDeleteObject(m_blockTable[i]);
	m_blockTable.clear();
}

VOID GDFFile::FreeHeader()
{
	if( m_header )
		SafeDeleteObject(m_header);
	m_header = NULL;
}

VOID GDFFile::FreeListFile()
{
	for( Index i = C_INDEX(0); i < (UInt)m_listFile.size(); ++i )
		SafeDeleteObject(m_listFile[i]);
	m_listFile.clear();
}

VOID GDFFile::FreePatchStep()
{
	for( PatchStepMap::iterator itr = m_patchStepMap.begin(); itr!=m_patchStepMap.end(); ++itr )
	{
		for( Index j = C_INDEX(0); j < itr->second.size(); ++j )
		{
			if( itr->second[j] )
				SafeDeleteObject(itr->second[j]);
		}
		itr->second.clear();
	}
	m_patchStepMap.clear();
}

GdfHashTable* GDFFile::GetHashTable(UInt32 hashPos)
{
	if( hashPos < m_hashTable.size() )
		return m_hashTable[hashPos];
	return NULL;
}

GdfBlockTable* GDFFile::GetBlockTable(UInt32 blockPos)
{
	if( blockPos < m_blockTable.size() )
		return m_blockTable[blockPos];
	return NULL;
}

GDFStream* GDFFile::GetStream(String& TargetFilename)
{
	if( m_mode==GDFFILE_STATE_GAME )	// FILE stream mode
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::GetStream Error (1) [get stream, in game mode], TargetFilename=%s"), TargetFilename.c_str());
		return NULL;
	}

	unsigned long hashPos;
	if( FindFileInHashTable(m_hashTable, TargetFilename.c_str(), 0, 0, hashPos) &&
		m_hashTable[hashPos] &&
		m_blockTable[m_hashTable[hashPos]->fileBlockIndex])
	{
		return SafeCreateObject3(GDFStream, 
			this,
			m_hashTable[hashPos]->fileBlockIndex, 
			m_blockTable[m_hashTable[hashPos]->fileBlockIndex]->fileSize
			);
	}else
	{
		return NULL;
	}
}

BYTE* GDFFile::GetMapViewOfFile(String& TargetFilename, UInt32& beginOffset, UInt32& fileSize)
{
	if( m_mode!=GDFFILE_STATE_GAME )	// FILE stream mode
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::GetMapViewOfFile Error (1) [get mapview error, not in game mode], TargetFilename=%s"), TargetFilename.c_str());
		return NULL;
	}

	unsigned long hashPos;
	if( FindFileInHashTable(m_hashTable, TargetFilename.c_str(), 0, 0, hashPos) &&
		m_hashTable[hashPos] &&
		m_blockTable[m_hashTable[hashPos]->fileBlockIndex])
	{
		GdfBlockTable* block	= m_blockTable[m_hashTable[hashPos]->fileBlockIndex];
		UInt32 pageOffset		= block->blockOffset % MEM_BLOCK_SIZE;
		UInt32 pagePos			= block->blockOffset / MEM_BLOCK_SIZE;
		UInt32 max_read			= block->blockSize + pageOffset;

		// output MapView, page begin offset, file size
		fileSize	= block->fileSize;
		BYTE* buf	= (BYTE*)MapViewOfFile(m_hMapFile, FILE_MAP_READ, 0, pagePos * MEM_BLOCK_SIZE, max_read);
		if( buf )
		{
			UInt32* sectorOffset = (UInt32*)(buf + pageOffset);
			beginOffset	= pageOffset + sectorOffset[0];	// page offset + block offset to file
		}
		return buf;
	}else
	{
		return NULL;
	}
}

BOOL GDFFile::SavePatchHashTable(File& f0, UInt32 fileOffset)
{
	if( m_hashTable.size()==0 )		return FALSE;

	BOOL succeed = FALSE;
	unsigned long hashkey = HashString(_T("(hash table)"), MPQ_HASH_FILE_KEY);

	GdfHashTable* hashTableArray = SafeAllocate(GdfHashTable, (UInt32)m_hashTable.size());
	for( Index i = C_INDEX(0); i < (UInt32)m_hashTable.size(); ++i )
		memcpy(&hashTableArray[i], m_hashTable[i], sizeof(GdfHashTable));
	EncryptData((VOID*)hashTableArray, (UInt32)m_hashTable.size() * sizeof(GdfHashTable), hashkey);
	try
	{
		f0.SetPosition(fileOffset, Reuben::Platform::File::MOVE_BEGIN);
		f0.Write((BYTE*)hashTableArray, (UInt32)m_hashTable.size() * sizeof(GdfHashTable));
		succeed = TRUE;
	}catch(...)
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchHashTable Error (1) [file write error], fileOffset=%i"), fileOffset);
	}
	SafeDeallocate(hashTableArray);
	
	m_header->SetHashTableOffset(fileOffset);		//									!!!
	m_header->SetHashTableOffsetHigh(fileOffset);	//									!!!
	m_header->SetHashTableEntries((UInt32)m_hashTable.size());

	return succeed;
}

BOOL GDFFile::SavePatchBlockTable(File& f0, UInt32 fileOffset)
{
	if( m_blockTable.size()==0 )	return FALSE;

	BOOL succeed = TRUE;
	unsigned long key = HashString(_T("(block table)"), MPQ_HASH_FILE_KEY);

	GdfBlockTable* blockTableArray = SafeAllocate(GdfBlockTable, (UInt32)m_blockTable.size());
	for( Index i = C_INDEX(0); i < (UInt32)m_blockTable.size(); ++i )
	{
		memcpy(&blockTableArray[i], m_blockTable[i], sizeof(GdfBlockTable));
	}
	EncryptData((VOID*)blockTableArray, (UInt32)m_blockTable.size() * sizeof(GdfBlockTable), key);
	try
	{
		f0.SetPosition(fileOffset, Reuben::Platform::File::MOVE_BEGIN);
		f0.Write((BYTE*)blockTableArray, (UInt32)m_blockTable.size() * sizeof(GdfBlockTable));
		succeed = TRUE;
	}catch(...)
	{
		succeed = FALSE;
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::SavePatchBlockTable Error (2) [file write error], fileOffset=%i"), fileOffset);
	}
	SafeDeallocate(blockTableArray);

	m_header->SetArchiveSize((UInt32)f0.GetPosition());//									!!!

	m_header->SetBlockTableEntries((UInt32)m_blockTable.size());
	m_header->SetBlockTableOffset(fileOffset);		//									!!!
	m_header->SetBlockTableOffsetHigh(fileOffset);	//									!!!
	m_header->SetExtendedBlockTableOffset(fileOffset);	//									!!!

	return succeed;
}

BOOL GDFFile::SavePatchHeader(File& f0)
{
	if( !f0.IsOpened() )
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchHeader Error (1) [file not opened]"));
		return FALSE;
	}
	try
	{
		f0.SetPosition(0, Reuben::Platform::File::MOVE_BEGIN);
		f0.Write((BYTE*)m_header, sizeof(GdfHeader));
	}catch(...)
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::SavePatchHeader Error (2) [file write error]"));
		return FALSE;
	}
	return TRUE;
}

ListFileVector& GDFFile::GetListFile()
{
	return m_listFile;
}

BOOL GDFFile::BackupHashBlock()
{
	TRACE_ENSURE( m_hashTable.size() );
	TRACE_ENSURE( m_header );

	BOOL succeed = TRUE;
	GdfBackupHeader	header;
	header.headerSize			= sizeof(GdfBackupHeader);
	header.hashOffset			= sizeof(GdfBackupHeader);
	header.hashSize				= (UInt32)m_hashTable.size();
	header.blockOffset			= header.hashOffset + header.hashSize * sizeof(GdfHashTable);
	header.blockSize			= (UInt32)m_blockTable.size();
	header.restoreHashOffset	= m_header->hashTableOffset;
	header.restoreBlockOffset	= m_header->blockTableOffset;
	header.bFileCompleted = 0;

	String backupFilename = m_backup_filename;

	File bakfile;
	bakfile.Open(backupFilename.c_str(), File::MODE_WRITE);
	if (bakfile.IsOpened())
	{
		bakfile.SetPosition(0, Reuben::Platform::File::MOVE_BEGIN);

		// backup file dummy header
		try
		{
			bakfile.Write((BYTE*)&header, sizeof(GdfBackupHeader));
		}catch(...)
		{
			bakfile.Close();
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::BackupHashBlock Error (1) [header write error]"));
			return FALSE;
		}

		unsigned long hashkey;
		UInt32 fileOffset;
		fileOffset = header.hashOffset;
		// backup file hashtable
		hashkey = HashString(_T("(hash table)"), MPQ_HASH_FILE_KEY);
		GdfHashTable* hashTableArray = SafeAllocate(GdfHashTable, (UInt32)m_hashTable.size());
		for( Index i = C_INDEX(0); i < (UInt32)m_hashTable.size(); ++i )
			memcpy(&hashTableArray[i], m_hashTable[i], sizeof(GdfHashTable));
		EncryptData((VOID*)hashTableArray, (UInt32)m_hashTable.size() * sizeof(GdfHashTable), hashkey);
		try
		{
			bakfile.SetPosition(fileOffset, Reuben::Platform::File::MOVE_BEGIN);
			bakfile.Write((BYTE*)hashTableArray, (UInt32)m_hashTable.size() * sizeof(GdfHashTable));
		}catch(...)
		{
			succeed = FALSE;
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::BackupHashBlock Error (1) [file write hash error] fileOffset=%i"), fileOffset);
		}
		SafeDeallocate(hashTableArray);

		if( !succeed )
		{
			bakfile.Close();
			return FALSE;
		}

		unsigned long key;
		fileOffset = header.blockOffset;
		// backup file blocktable
		key = HashString(_T("(block table)"), MPQ_HASH_FILE_KEY);
		GdfBlockTable* blockTableArray = SafeAllocate(GdfBlockTable, (UInt32)m_blockTable.size());
		for( Index i = C_INDEX(0); i < (UInt32)m_blockTable.size(); ++i )
		{
			memcpy(&blockTableArray[i], m_blockTable[i], sizeof(GdfBlockTable));
		}
		EncryptData((VOID*)blockTableArray, (UInt32)m_blockTable.size() * sizeof(GdfBlockTable), key);
		try
		{
			bakfile.SetPosition(fileOffset, Reuben::Platform::File::MOVE_BEGIN);
			bakfile.Write((BYTE*)blockTableArray, (UInt32)m_blockTable.size() * sizeof(GdfBlockTable));
			succeed = TRUE;
		}catch(...)
		{
			succeed = FALSE;
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::BackupHashBlock Error (2) [file write block error] fileOffset=%i"), fileOffset);
		}
		SafeDeallocate(blockTableArray);

		if( !succeed )
		{
			bakfile.Close();
			return FALSE;
		}

		// backup file completed header replace
		header.bFileCompleted = 1;

		bakfile.SetPosition(0, Reuben::Platform::File::MoveEnum::MOVE_BEGIN);
		try
		{
			bakfile.Write((BYTE*)&header, sizeof(GdfBackupHeader));
		}catch(...)
		{
			bakfile.Close();
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::BackupHashBlock Error (2) [header write error]"));
			return FALSE;
		}
		bakfile.Close();
		return TRUE;
	}
	return succeed;
}

VOID GDFFile::DeleteBackup()
{
	::DeleteFile(m_backup_filename.c_str());
}

BOOL GDFFile::InsertFileFromMemory(File& f0, String& TargetFilename, BYTE* pData, UInt32 bufSize, BOOL addToListfile)	// insert or update
{
	//!!! sector ignored?
	BOOL succeed = FALSE;
	UInt32 currentBlockOffset = (UInt32)f0.GetPosition();
	UInt32 hashA = HashString(TargetFilename.c_str(), MPQ_HASH_NAME_A);
	UInt32 hashB = HashString(TargetFilename.c_str(), MPQ_HASH_NAME_B);
	unsigned long hashPos = 0;
	if( FindFileInHashTable(m_hashTable, TargetFilename.c_str(), 0, 0, hashPos) &&
		m_hashTable[hashPos] &&
		m_blockTable[m_hashTable[hashPos]->fileBlockIndex])
	{
		// UPDATE case
		// compare file
		if( !IsGdfStreamSame(TargetFilename, pData, bufSize) )
		{
			// write file
			try
			{
				UInt32 sectorStart = 4;
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.Write((BYTE*)&sectorStart, sizeof(UInt32));
				if( bufSize > 0 )
					f0.Write(pData, (Count)bufSize);
				succeed = TRUE;
			}catch(...)
			{
				succeed = FALSE;
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.SetLength(currentBlockOffset);
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromMemory Error (1) [write error] TargetFilename=%s"), TargetFilename.c_str());
			}

			if( succeed )
			{
				// change block
				GdfBlockTable* tempblock	= SafeCreateObject(GdfBlockTable);
				tempblock->blockOffset		= currentBlockOffset;
				tempblock->blockSize		= (UInt32)bufSize + sizeof(UInt32);
				tempblock->fileSize			= (UInt32)bufSize;
				tempblock->flags			= MPQ_FILE_EXISTS;

				GdfHashTable* temphash		= m_hashTable[hashPos];
				temphash->fileBlockIndex	= (UInt32)m_blockTable.size();
				temphash->filePathHashA		= hashA;
				temphash->filePathHashB		= hashB;
				temphash->language			= MPQNeutral;
				temphash->platform			= 0;

				m_blockTable.push_back(tempblock);
			}
		}else
		{
			succeed = TRUE;
		}
	}else
	{
		// ADD case
		UInt tryCount = 0;
		hashPos = HashString(TargetFilename.c_str(), MPQ_HASH_TABLE_OFFSET) & (m_hashTable.size() - 1);
		while( m_hashTable[(hashPos+tryCount) % m_hashTable.size()]->fileBlockIndex!=MPQ_HASH_ENTRY_EMPTY && tryCount < m_hashTable.size() )
			tryCount++;
		if( tryCount >= m_hashTable.size() )
		{
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromMemory Error (2) [hash overloaded] TargetFilename=%s"), TargetFilename.c_str());
		}
		else
		{
			// write at the end
			try
			{
				UInt32 sectorStart = 4;
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.Write((BYTE*)&sectorStart, sizeof(UInt32));
				if( bufSize > 0 )
					f0.Write(pData, (Count)bufSize);
				succeed = TRUE;
			}catch(...)
			{
				succeed = FALSE;
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.SetLength(currentBlockOffset);
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromMemory Error (3) [file write error] TargetFilename=%s"), TargetFilename.c_str());
			}

			if( succeed )
			{
				// add blocktable
				GdfBlockTable* tempblock	= SafeCreateObject(GdfBlockTable);
				tempblock->blockOffset		= currentBlockOffset;
				tempblock->blockSize		= (UInt32)bufSize + sizeof(UInt32);
				tempblock->fileSize			= (UInt32)bufSize;
				tempblock->flags			= MPQ_FILE_EXISTS;

				// add hash pos
				GdfHashTable* temphash		= m_hashTable[(hashPos + tryCount)%m_hashTable.size()];
				temphash->fileBlockIndex	= (UInt32)m_blockTable.size();
				temphash->filePathHashA		= hashA;
				temphash->filePathHashB		= hashB;
				temphash->language			= MPQNeutral;
				temphash->platform			= 0;

				m_blockTable.push_back(tempblock);

				if( addToListfile )		// Data prepared in memory, means that it is probably SYSTEM file, which is hidden from normal usage
				{
					GdfListFile* templist		= SafeCreateObject(GdfListFile);
					templist->filename			= TargetFilename;
					templist->filePathHashA		= hashA;
					templist->filePathHashB		= hashB;
					m_listFile.push_back(templist);
				}
			}
		}
	}
	return succeed;
}

BOOL GDFFile::InsertFileFromStream(File& f0, String& TargetFilename, GDFStream* istream)	// insert or update
{
	//!!! sector ignored?
	BOOL succeed = FALSE;
	UInt32 currentBlockOffset = (UInt32)f0.GetPosition();
	UInt32 hashA = HashString(TargetFilename.c_str(), MPQ_HASH_NAME_A);
	UInt32 hashB = HashString(TargetFilename.c_str(), MPQ_HASH_NAME_B);
	unsigned long hashPos = 0;
	if( FindFileInHashTable(m_hashTable, TargetFilename.c_str(), 0, 0, hashPos) &&
		m_hashTable[hashPos] &&
		m_blockTable[m_hashTable[hashPos]->fileBlockIndex])
	{
		// UPDATE case
		// compare file
		if( !IsGdfStreamSame(TargetFilename, istream) )
		{
			UInt32 filesize = istream->GetFileSize();
			// write file
			try
			{
				UInt32 sectorStart = 4;
				UInt32 outlen = 0, testValue = 0, bufOffset = 0, bufLen = 0;
				Byte* buf = SafeAllocate(Byte, STREAM_BUF_MAX);

				succeed = TRUE;
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.Write((BYTE*)&sectorStart, sizeof(UInt32));
				istream->MoveFirst();
				while( bufOffset < istream->GetFileSize() )
				{
					bufLen = STREAM_BUF_MAX;
					if( bufOffset + STREAM_BUF_MAX > istream->GetFileSize() )
						bufLen = istream->GetFileSize() - bufOffset;
					bufOffset += bufLen;

					if( bufLen > 0 )
					{
						if( !istream->Read(buf, bufLen, outlen, STREAM_BUF_MAX) )
						{
							TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromStream Error (1) [file read error] TargetFilename=%s"), TargetFilename.c_str());
							f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
							f0.SetLength(currentBlockOffset);
							succeed = FALSE;
							break;
						}
						testValue = (UInt32)m_file.Write((Byte*)buf, bufLen);
						if( testValue!=bufLen )
						{
							TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromStream Error (2) [file write error] TargetFilenaem=%s"), TargetFilename.c_str());
							f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
							f0.SetLength(currentBlockOffset);
							succeed = FALSE;
							break;
						}
					}
				}
				SafeDeallocate(buf);
			}catch(...)
			{
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.SetLength(currentBlockOffset);
				succeed = FALSE;
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromMemory Error (3) [Write error] TargetFilename=%s"), TargetFilename.c_str());
			}

			if( succeed )
			{
				// change block
				GdfBlockTable* tempblock	= SafeCreateObject(GdfBlockTable);
				tempblock->blockOffset		= currentBlockOffset;
				tempblock->blockSize		= filesize + sizeof(UInt32);
				tempblock->fileSize			= filesize;
				tempblock->flags			= MPQ_FILE_EXISTS;

				GdfHashTable* temphash		= m_hashTable[hashPos];
				temphash->fileBlockIndex	= (UInt32)m_blockTable.size();
				temphash->filePathHashA		= hashA;
				temphash->filePathHashB		= hashB;
				temphash->language			= MPQNeutral;
				temphash->platform			= 0;

				m_blockTable.push_back(tempblock);
			}
		}else
		{
			succeed = TRUE;
		}
	}else
	{
		// ADD case
		UInt tryCount = 0;
		hashPos = HashString(TargetFilename.c_str(), MPQ_HASH_TABLE_OFFSET) & (m_hashTable.size() - 1);
		while( m_hashTable[(hashPos+tryCount) % m_hashTable.size()]->fileBlockIndex!=MPQ_HASH_ENTRY_EMPTY && tryCount < m_hashTable.size() )
			tryCount++;
		if( tryCount >= m_hashTable.size() )
		{
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromStream Error (4) [hash overloaded] TargetFilename=%s"), TargetFilename.c_str());
		}
		else
		{
			// write at the end
			UInt32 filesize = istream->GetFileSize();
			try
			{
				UInt32 sectorStart = 4;
				UInt32 outlen = 0, testValue = 0, bufOffset = 0, bufLen = 0;
				Byte* buf = SafeAllocate(Byte, STREAM_BUF_MAX);

				succeed = TRUE;
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.Write((BYTE*)&sectorStart, sizeof(UInt32));
				istream->MoveFirst();
				while( bufOffset < istream->GetFileSize() )
				{
					bufLen = STREAM_BUF_MAX;
					if( bufOffset + STREAM_BUF_MAX > istream->GetFileSize() )
						bufLen = istream->GetFileSize() - bufOffset;
					bufOffset += bufLen;

					if( bufLen > 0 )
					{
						if( !istream->Read(buf, bufLen, outlen, STREAM_BUF_MAX) )
						{
							TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromStream Error (5) [file read error] TargetFilename=%s"), TargetFilename.c_str());
							f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
							f0.SetLength(currentBlockOffset);
							succeed = FALSE;
							break;
						}
						testValue = (UInt32)m_file.Write((Byte*)buf, bufLen);
						if( testValue!=bufLen )
						{
							TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromStream Error (6) [file write error] TargetFilename=%s"), TargetFilename.c_str());
							f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
							f0.SetLength(currentBlockOffset);
							succeed = FALSE;
							break;
						}
					}
				}
				SafeDeallocate(buf);
			}catch(...)
			{
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromStream Error (7) [write error] TargetFilename=%s"), TargetFilename.c_str());
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.SetLength(currentBlockOffset);
				succeed = FALSE;
			}

			if( succeed )
			{
				// add blocktable
				GdfBlockTable* tempblock	= SafeCreateObject(GdfBlockTable);
				tempblock->blockOffset		= currentBlockOffset;
				tempblock->blockSize		= filesize + sizeof(UInt32);
				tempblock->fileSize			= filesize;
				tempblock->flags			= MPQ_FILE_EXISTS;

				// add hash pos
				GdfHashTable* temphash		= m_hashTable[(hashPos + tryCount)%m_hashTable.size()];
				temphash->fileBlockIndex	= (UInt32)m_blockTable.size();
				temphash->filePathHashA		= hashA;
				temphash->filePathHashB		= hashB;
				temphash->language			= MPQNeutral;
				temphash->platform			= 0;

				m_blockTable.push_back(tempblock);

				GdfListFile* templist		= SafeCreateObject(GdfListFile);
				templist->filename			= TargetFilename;
				templist->filePathHashA		= hashA;
				templist->filePathHashB		= hashB;
				m_listFile.push_back(templist);
			}
		}
	}
	return succeed;
}

BOOL GDFFile::InsertFileFromFile(File& f0, String& TargetFilename, String& SourcePath)	// insert or update
{
	//!!! sector ignored?
	BOOL succeed = FALSE;
	UInt32 currentBlockOffset = (UInt32)f0.GetPosition();
	UInt32 hashA = HashString(TargetFilename.c_str(), MPQ_HASH_NAME_A);
	UInt32 hashB = HashString(TargetFilename.c_str(), MPQ_HASH_NAME_B);
	unsigned long hashPos = 0;
	if( FindFileInHashTable(m_hashTable, TargetFilename.c_str(), 0, 0, hashPos) &&
		m_hashTable[hashPos] &&
		m_blockTable[m_hashTable[hashPos]->fileBlockIndex])
	{
		// UPDATE case
		// compare file
		if( !IsGdfStreamSame(TargetFilename, SourcePath) )
		{
			File fSrc;
			fSrc.Open(TargetFilename.c_str(), Reuben::Platform::File::MODE_READ_EXISTING);
			if( fSrc.IsOpened() )
			{
				UInt32 filesize = (UInt32)fSrc.GetLength();
				try
				{
					UInt32 sectorStart = 4;
					UInt32 outlen = 0, testValue = 0, bufOffset = 0, bufLen = 0;
					Byte* buf = SafeAllocate(Byte, STREAM_BUF_MAX);

					succeed = TRUE;
					f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
					f0.Write((BYTE*)&sectorStart, sizeof(UInt32));
					fSrc.SetPosition(0, Reuben::Platform::File::MOVE_BEGIN);
					while( bufOffset < filesize )
					{
						bufLen = STREAM_BUF_MAX;
						if( bufOffset + STREAM_BUF_MAX > filesize )
							bufLen = filesize - bufOffset;
						bufOffset += bufLen;

						if( bufLen > 0 )
						{
							if( !fSrc.Read(buf, bufLen) )
							{
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (1) [file read error] TargetFilename=%s"), TargetFilename.c_str());
								f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
								f0.SetLength(currentBlockOffset);
								succeed = FALSE;
								break;
							}
							testValue = (UInt32)m_file.Write((Byte*)buf, bufLen);
							if( testValue!=bufLen )
							{
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (2) [file write error] TargetFilename=%s"), TargetFilename.c_str());
								f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
								f0.SetLength(currentBlockOffset);
								succeed = FALSE;
								break;
							}
						}
					}
					SafeDeallocate(buf);
					fSrc.Close();
				}catch(...)
				{
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (3) [write error] TargetFilename=%s"), TargetFilename.c_str());
					f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
					f0.SetLength(currentBlockOffset);
					fSrc.Close();
					succeed = FALSE;
				}

				if( succeed )
				{
					// change block
					GdfBlockTable* tempblock	= SafeCreateObject(GdfBlockTable);
					tempblock->blockOffset		= currentBlockOffset;
					tempblock->blockSize		= filesize + sizeof(UInt32);
					tempblock->fileSize			= filesize;
					tempblock->flags			= MPQ_FILE_EXISTS;

					GdfHashTable* temphash		= m_hashTable[hashPos];
					temphash->fileBlockIndex	= (UInt32)m_blockTable.size();
					temphash->filePathHashA		= hashA;
					temphash->filePathHashB		= hashB;
					temphash->language			= MPQNeutral;
					temphash->platform			= 0;

					m_blockTable.push_back(tempblock);
				}
			}else
			{
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (4) [source file open error] TargetFilename=%s"), TargetFilename.c_str());
				f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
				f0.SetLength(currentBlockOffset);
				succeed = FALSE;
			}
		}else
		{
			succeed = TRUE;
		}
	}else
	{
		// ADD case
		UInt tryCount = 0;
		hashPos = HashString(TargetFilename.c_str(), MPQ_HASH_TABLE_OFFSET) & (m_hashTable.size() - 1);
		while( m_hashTable[(hashPos+tryCount) % m_hashTable.size()]->fileBlockIndex!=MPQ_HASH_ENTRY_EMPTY && tryCount < m_hashTable.size() )
			tryCount++;
		if( tryCount >= m_hashTable.size() )
		{
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (5) [hash overloaded] TargetFilename=%s"), TargetFilename.c_str());
		}
		else
		{
			// write at the end
			File fSrc;
			fSrc.Open(TargetFilename.c_str(), Reuben::Platform::File::MODE_READ_EXISTING);
			if( fSrc.IsOpened() )
			{
				UInt32 filesize = (UInt32)fSrc.GetLength();
				try
				{
					UInt32 sectorStart = 4;
					UInt32 outlen = 0, testValue = 0, bufOffset = 0, bufLen = 0;
					Byte* buf = SafeAllocate(Byte, STREAM_BUF_MAX);

					succeed = TRUE;
					f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
					f0.Write((BYTE*)&sectorStart, sizeof(UInt32));
					fSrc.SetPosition(0, Reuben::Platform::File::MOVE_BEGIN);
					while( bufOffset < filesize )
					{
						bufLen = STREAM_BUF_MAX;
						if( bufOffset + STREAM_BUF_MAX > filesize )
							bufLen = filesize - bufOffset;
						bufOffset += bufLen;

						if( bufLen > 0 )
						{
							if( !fSrc.Read(buf, bufLen) )
							{
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (6) [file read error] TargetFilename=%s"), TargetFilename.c_str());
								f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
								f0.SetLength(currentBlockOffset);
								succeed = FALSE;
								break;
							}
							testValue = (UInt32)m_file.Write((Byte*)buf, bufLen);
							if( testValue!=bufLen )
							{
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (7) [file write error] TargetFilename=%s"), TargetFilename.c_str());
								f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
								f0.SetLength(currentBlockOffset);
								succeed = FALSE;
								break;
							}
						}
					}
					SafeDeallocate(buf);
					fSrc.Close();
				}catch(...)
				{
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (8) [write error] TargetFilename=%s"), TargetFilename.c_str());
					f0.SetPosition(currentBlockOffset, Reuben::Platform::File::MOVE_BEGIN);
					f0.SetLength(currentBlockOffset);
					fSrc.Close();
					succeed = FALSE;
				}

				if( succeed )
				{
					// add blocktable
					GdfBlockTable* tempblock	= SafeCreateObject(GdfBlockTable);
					tempblock->blockOffset		= currentBlockOffset;
					tempblock->blockSize		= filesize + sizeof(UInt32);
					tempblock->fileSize			= filesize;
					tempblock->flags			= MPQ_FILE_EXISTS;

					// add hash pos
					GdfHashTable* temphash		= m_hashTable[(hashPos+tryCount)%m_hashTable.size()];
					temphash->fileBlockIndex	= (UInt32)m_blockTable.size();
					temphash->filePathHashA		= hashA;
					temphash->filePathHashB		= hashB;
					temphash->language			= MPQNeutral;
					temphash->platform			= 0;

					m_blockTable.push_back(tempblock);

					GdfListFile* templist		= SafeCreateObject(GdfListFile);
					templist->filename			= TargetFilename;
					templist->filePathHashA		= hashA;
					templist->filePathHashB		= hashB;
					m_listFile.push_back(templist);
				}
			}else
			{
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromFile Error (9) [source file open error] TargetFilename=%s"), TargetFilename.c_str());
				succeed = FALSE;
			}
		}
	}
	return succeed;
}

BOOL GDFFile::DeleteFile(File& f0, String& TargetFilename)
{
	BOOL succeed;
	succeed = DeleteFile(TargetFilename.c_str(), MPQNeutral, 0);
	if( succeed )
	{
		// (listfile) manipulation
		for( ListFileVector::iterator itr = m_listFile.begin(); itr!=m_listFile.end(); ++itr )
		{
			if( TargetFilename.CompareNoCase((*itr)->filename.c_str())==0 )
			{
				SafeDeleteObject(*itr);
				m_listFile.erase(itr);
				break;
			}
		}
	}
	return succeed;
}

BOOL GDFFile::IsGdfFileExist(String& TargetFilename)
{
	GDFStream* fstream = GetStream(TargetFilename);
	if( fstream )
	{
		SafeDeleteObject(fstream);
		return TRUE;
	}
	return FALSE;
}

BOOL GDFFile::IsGdfStreamSame(String& TargetFilename, String& FilePath)
{
	UInt64 currentOffset = (UInt64)m_file.GetPosition();

	GDFStream* stream1 = GetStream(TargetFilename);
	TRACE_ENSURE(stream1);

	File f0;
	BOOL same = TRUE;
	f0.Open(FilePath.c_str(), Reuben::Platform::File::MODE_READ_EXISTING);
	if( f0.IsOpened() )
	{
		if( f0.GetLength()!=stream1->GetFileSize() )
		{
			f0.Close();
			m_file.SetPosition(currentOffset, Reuben::Platform::File::MOVE_BEGIN);
			return FALSE;
		}

		UInt32 length1 = stream1->GetFileSize();

		Byte* buf1 = SafeAllocate(Byte, STREAM_BUF_MAX);
		Byte* buf2 = SafeAllocate(Byte, STREAM_BUF_MAX);
		f0.SetPosition(0, Reuben::Platform::File::MOVE_BEGIN);
		UInt32 bufLen = 0, offset = 0, outlen;

		while( offset < length1 )
		{
			bufLen = STREAM_BUF_MAX;
			if( offset + STREAM_BUF_MAX > length1 )
				bufLen = length1 - offset;
			offset += bufLen;

			stream1->Read(buf1, bufLen, outlen, STREAM_BUF_MAX);
			f0.Read(buf2, bufLen);

			if( memcmp(buf1, buf2, outlen)!=0 )
			{
				same = FALSE;
				break;
			}
		}

		SafeDeleteObject(stream1);
		SafeDeallocate(buf1);
		SafeDeallocate(buf2);

		f0.Close();

		m_file.SetPosition(currentOffset, Reuben::Platform::File::MOVE_BEGIN);
		return same;
	}else
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::IsGdfStreamSame Error (1) [error load file] FilePath=%s"), FilePath.c_str());
	}
	m_file.SetPosition(currentOffset, Reuben::Platform::File::MOVE_BEGIN);
	return FALSE;
}

BOOL GDFFile::IsGdfStreamSame(String& TargetFilename, GDFStream* stream2)
{
	TRACE_ENSURE(stream2);

	UInt64 currentOffset = (UInt64)m_file.GetPosition();

	GDFStream* stream1 = GetStream(TargetFilename);
	TRACE_ENSURE(stream1);

	if( stream1->GetFileSize()!=stream2->GetFileSize() )
	{
		m_file.SetPosition(currentOffset, Reuben::Platform::File::MOVE_BEGIN);
		return FALSE;
	}

	UInt32 length1 = stream1->GetFileSize();
	UInt32 length2 = stream2->GetFileSize();
	UInt32 offset = 0;
	UInt32 bufLen;		// common shared
	UInt32 outlen;

	BOOL same = TRUE;

	Byte* buf1 = SafeAllocate(Byte, STREAM_BUF_MAX);
	Byte* buf2 = SafeAllocate(Byte, STREAM_BUF_MAX);

	while( offset < length1 )
	{
		bufLen = STREAM_BUF_MAX;
		if( offset + STREAM_BUF_MAX > length1 )
			bufLen = length1 - offset;
		offset += bufLen;

		stream1->Read(buf1, bufLen, outlen, STREAM_BUF_MAX);
		stream2->Read(buf2, bufLen, outlen, STREAM_BUF_MAX);

		if( memcmp(buf1, buf2, outlen)!=0 )
		{
			same = FALSE;
			break;
		}
	}

	SafeDeleteObject(stream1);
	SafeDeallocate(buf1);
	SafeDeallocate(buf2);

	m_file.SetPosition(currentOffset, Reuben::Platform::File::MOVE_BEGIN);
	return same;
}

BOOL GDFFile::IsGdfStreamSame(String& TargetFilename, BYTE* pData, UInt32 bufSize)
{
	GDFStream* stream1 = GetStream(TargetFilename);
	TRACE_ENSURE(stream1);

	UInt64 currentOffset = m_file.GetPosition();

	if( stream1->GetFileSize()!=bufSize )
	{
		m_file.SetPosition(currentOffset, Reuben::Platform::File::MOVE_BEGIN);
		return FALSE;
	}

	UInt32 length1 = stream1->GetFileSize();
	UInt32 offset = 0;
	UInt32 bufLen;		// common shared
	UInt32 outlen;

	BOOL same = TRUE;

	Byte* buf1 = SafeAllocate(Byte, STREAM_BUF_MAX);

	UInt32 round = 0;
	while( offset < length1 )
	{
		bufLen = STREAM_BUF_MAX;
		if( offset + STREAM_BUF_MAX > length1 )
			bufLen = length1 - offset;
		offset += bufLen;

		stream1->Read(buf1, bufLen, outlen, STREAM_BUF_MAX);
		if( memcmp(buf1, &pData[round*STREAM_BUF_MAX], outlen)!=0 )
		{
			same = FALSE;
			break;
		}
		++round;
	}

	SafeDeleteObject(stream1);
	SafeDeallocate(buf1);

	m_file.SetPosition(currentOffset, Reuben::Platform::File::MOVE_BEGIN);
	return same;
}

UInt32 GDFFile::GetListFileLength()		// in TCHAR
{
	UInt outlen	= 0;
	for( Index i = C_INDEX(0); i < (UInt)m_listFile.size(); ++i )
		outlen += m_listFile[i]->filename.GetLength() + 2;
	
	return outlen;
}

UInt32 GDFFile::GetListFile(TCHAR* out)
{
	UInt32 curPos = 0;
	UInt32 outlen;
	for( Index i = C_INDEX(0); i < (UInt)m_listFile.size(); ++i )
	{
		outlen = m_listFile[i]->filename.GetLength();
		memcpy(&out[curPos], m_listFile[i]->filename.GetBuffer(m_listFile[i]->filename.GetLength()), outlen * sizeof(TCHAR));
		curPos			+= outlen;
		out[curPos]		= 0x0d;
		out[curPos+1]	= 0x0a;
		curPos			+= 2;
	}
	return curPos;
}

BOOL GDFFile::LoadFile(String& PatchFilename)		// continuous file stream
{
	m_mode	= GDFFILE_STATE_GAME;
	if( !File::IsExists(PatchFilename.c_str()) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFile Error (3) [file not found] PatchFilename=%s"), PatchFilename.c_str());
		return FALSE;
	}

	BOOL succeed;

	succeed = PeekPatchInfo(PatchFilename);
	if( succeed )
	{
		m_hFile = CreateFile(PatchFilename.c_str(), FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
		m_hMapFile = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, PatchFilename.c_str());
		if( m_hMapFile!=NULL )
		{
//			m_pInFile = (Byte *)MapViewOfFile(m_hMapFile, FILE_MAP_READ, 0, 0, 1000000000);
		}else
		{
			succeed = FALSE;
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFile Error (1) [Create File Mapping failed] PatchFilename=%s"), PatchFilename.c_str());
		}
		if( !succeed )
			FreePatch();
	}
	if( succeed )
	{
		m_filename = PatchFilename;
		m_backup_filename = GetBackupFilename(PatchFilename);
	}
	return succeed;
}

BOOL GDFFile::LoadFileWithList(String& PatchFilename)
{
	m_mode = GDFFILE_STATE_PATCH_READ;
	if( !File::IsExists(PatchFilename.c_str()) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWithList Error (3) [file not found] PatchFilename=%s"), PatchFilename.c_str());
		return FALSE;
	}

	BOOL succeed;

	succeed = PeekPatchInfoWithList(PatchFilename);
	if( succeed )
	{
		try
		{
			m_file.Open(PatchFilename.c_str(), Reuben::Platform::File::MODE_READ_EXISTING);
			if( !m_file.IsOpened() )
			{
				succeed = FALSE;
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWithList Error (1) [error on open] PatchFilename=%s"), PatchFilename.c_str());
			}
		}catch(...)
		{
			succeed = FALSE;
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWithList Error (2) [error on open] PatchFilename=%s"), PatchFilename.c_str());
		}
		if( !succeed )
			FreePatch();
	}
	if( succeed )
	{
		m_filename = PatchFilename;
		m_backup_filename = GetBackupFilename(PatchFilename);
	}
	return succeed;
}

BOOL GDFFile::LoadFileWrite(String& PatchFilename, UInt32 defaultHashEntries_power2)
{
	BOOL succeed = TRUE;
	m_mode = GDFFILE_STATE_MASTER_WRITE;
	String backup_filename = GetBackupFilename(PatchFilename);
	if( Reuben::Platform::File::IsExists(backup_filename.c_str()) )
	{
		m_backup_filename = backup_filename;
		try
		{
			m_header = SafeCreateObject(GdfHeader);
			m_file.Open(PatchFilename.c_str(), Reuben::Platform::File::MODE_READ_WRITE_EXISTING);
			if( !m_file.IsOpened() )
			{
				succeed = FALSE;
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWrite Error (1) [error on open] PatchFilename=%s"), PatchFilename.c_str());
			}else
			{
				if( !RevertFromBackup() )
				{
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWrite Error (2) [revert failed] PatchFilename=%s"), PatchFilename.c_str());
					succeed = FALSE;
				}else
					DeleteBackup();
				m_file.Close();
			}
			SafeDeleteObject(m_header);
			m_header = NULL;
		}catch(...)
		{
			succeed = FALSE;
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWrite Error (3) [error on open] PatchFilename=%s"), PatchFilename.c_str());
		}
	}
	if( succeed && !Reuben::Platform::File::IsExists(PatchFilename.c_str()) )
	{
		if( defaultHashEntries_power2 > 30 )
		{
			succeed = FALSE;
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::LoadFileWrite Error (4) [new file without parameter] PatchFilename=%s, defaultHashEntries_power2=%i"), PatchFilename.c_str(), defaultHashEntries_power2);
		}else
		{
			if( defaultHashEntries_power2 < 3 )
				defaultHashEntries_power2 = 3;
			succeed = NewFile(PatchFilename, defaultHashEntries_power2);
			if( !succeed )
			{
				// !!
				TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::LoadFileWrite Error (5) [new file failed] PatchFilename=%s, defaultHashEntries_power2=%i"), PatchFilename.c_str(), defaultHashEntries_power2);
			}else
			{
			}
		}
	}
	if( succeed )
	{
		succeed = PeekPatchInfoWithList(PatchFilename);
		if( succeed )
		{
			try
			{
				m_file.Open(PatchFilename.c_str(), Reuben::Platform::File::MODE_READ_WRITE_EXISTING);
				if( !m_file.IsOpened() )
				{
					succeed = FALSE;
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWrite Error (6) [error on open] PatchFilename=%s"), PatchFilename.c_str());
				}
			}catch(...)
			{
				succeed = FALSE;
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::LoadFileWrite Error (7) [error on open] PatchFilename=%s"), PatchFilename.c_str());
			}
			if( !succeed )
				FreePatch();
		}
		if( succeed )
		{
			m_filename = PatchFilename;
			m_backup_filename = GetBackupFilename(PatchFilename);
		}
	}
	return succeed;
}

BOOL GDFFile::CloseFile()
{
	try
	{
		if( m_mode!=GDFFILE_STATE_GAME )	// FILE stream mode
		{
			if( m_file.IsOpened() )
				m_file.Close();
		}else
		{
			CloseHandle(m_hMapFile);
			CloseHandle(m_hFile);
		}
		FreePatch();
		m_filename.Empty();
		m_backup_filename.Empty();
	}catch(...)
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::CloseFile Error (1) [error on close or file check]"));
		return FALSE;
	}
	return TRUE;
}

BOOL GDFFile::UpdateFromStream(PatchStepMap& patchstep, GDFFile* SrcGdfFile, BOOL mergePatchStep)
{
	BOOL succeed = FALSE;

	if( BackupHashBlock() &&
		SavePatchHeader(m_file) &&
		SavePatchData(patchstep, SrcGdfFile, mergePatchStep) &&
		SavePatchHashTable(m_file, (UInt32)m_file.GetPosition()) &&
		SavePatchBlockTable(m_file, (UInt32)m_file.GetPosition()) &&
		SavePatchHeader(m_file)
		)
	{
		succeed = TRUE;
	}
	else
	{
		if( !RevertFromBackup() )
		{
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::UpdateFromStream Error (1) [update failed AND revert failed]"));
		}
	}
	DeleteBackup();

	return succeed;
}

BOOL GDFFile::UpdateFromFile(ArchiveChangeVector& change, UInt32 writeLogVersion)
{
	BOOL succeed = FALSE;

	if( BackupHashBlock() &&
		SavePatchHeader(m_file) &&
		SavePatchData(change, writeLogVersion) &&
		SavePatchHashTable(m_file, (UInt32)m_file.GetPosition()) &&
		SavePatchBlockTable(m_file, (UInt32)m_file.GetPosition()) &&
		SavePatchHeader(m_file)
		)
	{
		succeed = TRUE;
	}
	else
	{
		if( !RevertFromBackup() )
		{
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::UpdateFromStream Error (1) [update failed AND revert failed]"));
		}
	}
	DeleteBackup();

	return succeed;
}

BOOL GDFFile::RevertFromBackup()
{
	TRACE_ENSURE(m_header);

	BOOL succeed = TRUE;
	String backupFilename;
	backupFilename = m_backup_filename;

	File bakfile;
	bakfile.Open(backupFilename.c_str(), File::MODE_READ_EXISTING);
	if (bakfile.IsOpened())
	{
		// backup file dummy header
		GdfBackupHeader header;
		bakfile.Read((BYTE*)&header, sizeof(GdfBackupHeader));

		if( header.bFileCompleted==1 ) // 0: incomplete backup data, 1: complete backup data
		{
			FreeHashTable();
			FreeBlockTable();

			if(	LoadHashTable(bakfile, (UInt32)bakfile.GetPosition(), header.hashSize) && 
				LoadBlockTable(bakfile, (UInt32)bakfile.GetPosition(), header.blockSize) )
			{
				if( SavePatchHashTable(m_file, header.restoreHashOffset) &&
					SavePatchBlockTable(m_file, header.restoreBlockOffset) &&
					SavePatchHeader(m_file) 
					)
				{
					succeed = TRUE;
				}else
				{
					succeed = FALSE;
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::RevertFromBackup Error (4) [Backup data write error]"));
				}
			}else
			{
				succeed = FALSE;
				TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::RevertFromBackup Error (3) [Backup data read error]"));
			}
		}else
		{
			succeed = FALSE;
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::RevertFromBackup Error (1) [Revert failed due to corruption of backup data]"));
		}
		bakfile.Close();
	}else
	{
		succeed = FALSE;
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::RevertFromBackup Error (2) [Revert failed due to missing backup data]"));
	}
	return succeed;
}

BOOL GDFFile::ReadStream(GDFStreamData* streamdata, BYTE* OutBuffer, UInt32 readLength, UInt32& OutLength, UInt32 MaxLength)
{
	if( m_mode!=GDFFILE_STATE_GAME )	// FILE stream mode
	{
		try
		{
			if( streamdata->m_managerBlockOffset==0 ) streamdata->m_managerBlockOffset = SECTOR_OFFSET;

			UInt32 reallen = readLength;
			if( reallen > MaxLength ) reallen = MaxLength;
			if( m_blockTable[streamdata->m_blockIndex]->fileSize + SECTOR_OFFSET < reallen + streamdata->m_managerBlockOffset)
				reallen = m_blockTable[streamdata->m_blockIndex]->fileSize + SECTOR_OFFSET - streamdata->m_managerBlockOffset;

			if( reallen > 0 )
			{
				EnterCriticalSection(&GDFFileReadThreadLock);
				m_file.SetPosition(m_blockTable[streamdata->m_blockIndex]->blockOffset + streamdata->m_managerBlockOffset, Reuben::Platform::File::MoveEnum::MOVE_BEGIN);
				m_file.Read((BYTE*)OutBuffer, reallen * sizeof(BYTE));
				LeaveCriticalSection(&GDFFileReadThreadLock);
			}

			OutLength = reallen;
			streamdata->m_managerBlockOffset += reallen;
			return TRUE;
		}
		catch(FileException Er)
		{
			OutLength = 0;
			return FALSE;
		}
	}else
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::ReadStream (1) [View mode cannot use this function.]"));
		return FALSE;
	}
}

BOOL GDFFile::NewFile(String& PatchFilename, UInt32 defaultHashEntries_power2)
{
	FreePatch();

	if( Reuben::Platform::File::IsExists(PatchFilename.c_str()) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::NewFile Error (1) [Filename exists, file create skipped] PatchFilename=%s"), PatchFilename.c_str());
		return FALSE;
	}

	// write sequentially: empty header, content of "(listfile)", hashtable, blocktable, (rewrite) header
	// in case of any error, break condition and erase file.  Return FALSE
	BOOL succeed = TRUE;
	File f0;
	f0.Open(PatchFilename.c_str(), Reuben::Platform::File::MODE_WRITE);
	if( f0.IsOpened() )
	{
		UInt32 blockTableOffset, hashTableOffset, archiveSize;

		m_header = SafeCreateObject(GdfHeader);
		succeed = SavePatchHeader(f0);
		if( succeed )
		{
			UInt32 numHash = (UInt32)pow(2, defaultHashEntries_power2);

			GdfHashTable* hashtable;
			for( Index i = C_INDEX(0); i < numHash; ++i )
			{
				hashtable = SafeCreateObject(GdfHashTable);
				memset(hashtable, 255, sizeof(GdfHashTable));
				m_hashTable.push_back(hashtable);
			}

			unsigned long hashkey = HashString(_T("(listfile)"), MPQ_HASH_FILE_KEY);
			UInt32 listfilelen = GetListFileLength();
			TCHAR* out = SafeAllocate(TCHAR, listfilelen);
			GetListFile(out);
			unsigned long key = HashString(_T("(listfile)"), MPQ_HASH_FILE_KEY);
			EncryptData((VOID*)out, listfilelen * sizeof(TCHAR), hashkey);

			String listfilename = _T("(listfile)");
			succeed = InsertFileFromMemory(f0, listfilename, (BYTE*)out, listfilelen * sizeof(TCHAR), FALSE);
			if( succeed )
			{
				hashTableOffset = (UInt32)f0.GetPosition();
				succeed = SavePatchHashTable(f0, hashTableOffset);
				if( succeed )
				{
					blockTableOffset = (UInt32)f0.GetPosition();
					succeed = SavePatchBlockTable(f0, blockTableOffset);
					if( succeed )
					{
						archiveSize = (UInt32)f0.GetPosition();

						m_header->SetMagic("MPQ.");
						m_header->SetHeaderSize(sizeof(GdfHeader));
						m_header->SetArchiveSize(archiveSize);
						m_header->SetFormatVersion(2);
						m_header->SetSectorSizeShift(1);
						m_header->SetHashTableOffset(hashTableOffset);
						m_header->SetBlockTableOffset(blockTableOffset);
						m_header->SetHashTableEntries(numHash);
						m_header->SetBlockTableEntries((UInt32)m_blockTable.size());
						m_header->SetExtendedBlockTableOffset((UInt32)m_blockTable.size());
						m_header->SetHashTableOffsetHigh(hashTableOffset);
						m_header->SetBlockTableOffsetHigh(blockTableOffset);
						succeed = SavePatchHeader(f0);
						if( !succeed )
							TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::NewFile Error (2) [rewrite header error]"));
					}else
						TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::NewFile Error (3) [hash write error]"));
				}else
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::NewFile Error (4) [block write error]"));
			}else
				TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::NewFile Error (5) [list of file write error]"));
		}else
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::NewFile Error (6) [header write error]"));
		
		f0.Close();
	}

	// delete file in case of any error
	if( !succeed )	
	{
		Reuben::Platform::File::Delete(PatchFilename.c_str());
	}

	return succeed;
}

BOOL GDFFile::IncreaseHash(UInt32 newhashsize)
{
	TRACE_ENSURE(m_listFile.size() > 0);
	
	BOOL succeed = TRUE;

	// new HashTable initialization
	HashTableVector newhashtable;
	newhashtable.clear();
	GdfHashTable* temphashTable;
	UInt32 newrealsize = (UInt32)pow(2, newhashsize);
	for( Index i = C_INDEX(0); i < newrealsize; ++i )
	{
		temphashTable = SafeCreateObject(GdfHashTable);
		memset(temphashTable, 255, sizeof(GdfHashTable));
		newhashtable.push_back(temphashTable);
	}

	// HashTable replacement
	GdfListFile* listfile;
	unsigned long hashPos, newhashPos;
	for( Index i = C_INDEX(0); i < m_listFile.size(); ++i )
	{
		listfile = m_listFile[i];
		if( listfile )
		{
			if( FindFileInHashTable(m_hashTable, listfile->filename.c_str(), 0, 0, hashPos) && m_hashTable[hashPos])
			{
				UInt tryCount = 0;
				newhashPos = HashString(listfile->filename.c_str(), MPQ_HASH_TABLE_OFFSET) & (newhashtable.size() - 1);
				while( newhashtable[(newhashPos+tryCount) % newhashtable.size()]->fileBlockIndex!=MPQ_HASH_ENTRY_EMPTY && tryCount < newhashtable.size() )
					tryCount++;
				if( tryCount >= newhashtable.size() )
				{
					TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::InsertFileFromMemory Error (2) [hash overloaded] TargetFilename=%s, hashSize=%i"), listfile->filename.c_str(), (UInt)newhashtable.size());
					succeed = FALSE;
				}
				else
				{
					memcpy(newhashtable[(newhashPos+tryCount)%newhashtable.size()], m_hashTable[hashPos], sizeof(GdfHashTable));
				}
			}else
			{
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::IncreaseHash Error (2) [memory not found in original hash table, increase failed] filename=%s"), listfile->filename.c_str());
				succeed = FALSE;
				break;
			}
		}else
		{
			TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::IncreaseHash Error (3) [empty listfile found, increase failed] filename=%s"));
			succeed = FALSE;
			break;
		}
	}
	if( succeed )
	{
		// remove old hashtable content
		for( Index i = C_INDEX(0); i < (UInt)m_hashTable.size(); ++i )
			if( m_hashTable[i] )
				SafeDeleteObject(m_hashTable[i]);
		// new hash table assignment
		m_hashTable = newhashtable;
	}

	return succeed;
}

UInt32 GDFFile::FindMin2PowerInt(UInt32 number)
{
	TRACE_CHECK(number > 0);

	for( Index i = C_INDEX(0); i < C_INDEX(32); ++i )
	{
		if( (number - 1)>>i==0 )
			return i;
	}
	return 0;
}

BOOL GDFFile::TestFile(String& patchFilename)
{
	if( !LoadFileWithList(patchFilename) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::TestFile Error (1) [load file with list error] patchFilename=%s"), patchFilename.c_str());
		return FALSE;
	}

	if( m_file.GetLength()!=m_header->archiveSize )
	{
		UInt archivesize = m_header->archiveSize;
		TRACE_ERRORDTL_3(GLOBAL_LOGGER, _T("GDFFile::TestFile Error (2) [file size does not match] patchFilename=%s, filesize=%i, archiveSize=%i"), patchFilename.c_str(), (UInt32)m_file.GetLength(), archivesize);
		CloseFile();
		return FALSE;
	}

	GdfListFile* list;
	GDFStream* fstream;
	for( Index i = C_INDEX(0); i < m_listFile.size(); ++i )
	{
		list = m_listFile[i];
		if( list )
		{
			if( IsGdfFileExist(list->filename) )
			{
				fstream = GetStream(list->filename);
				if( fstream )
				{
					if( fstream->GetFileSize() > m_header->archiveSize )
					{
						TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFFile::TestFile Error (1) [file size is not reasonable] patchFilename=%s, size=%i"), patchFilename.c_str(), fstream->GetFileSize());
						CloseFile();
						SafeDeleteObject(fstream);
						return FALSE;
					}
				}else
				{
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::TestFile Error (1) [file in list does not exist] patchFilename=%s"), patchFilename.c_str());
					CloseFile();
					return FALSE;
				}
			}else
			{
				TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::TestFile Error (1) [file in list does not exist] patchFilename=%s"), patchFilename.c_str());
				CloseFile();
				return FALSE;
			}
		}else
		{
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFFile::TestFile Error (2) [list file content error] patchFilename=%s"), patchFilename.c_str());
			CloseFile();
			return FALSE;
		}
	}
	GdfBlockTable* block;
	UInt32 invalidBlock = 0, usedBlock = 0, unknownBlock = 0;
	for( Index i = C_INDEX(0); i < m_blockTable.size(); ++i )
	{
		block = m_blockTable[i];
		if( block )
		{
			if( block->flags==MPQ_FILE_EXISTS )
				++usedBlock;
			else
				++unknownBlock;
		}else
			++invalidBlock;
	}

	GdfHashTable* hash;
	UInt32 usedHash = 0, deletedHash = 0, invalidHash = 0, emptyHash = 0, overflowedHash = 0;
	for( Index i = C_INDEX(0); i < m_hashTable.size(); ++i )
	{
		hash = m_hashTable[i];
		if( hash )
		{
			switch( hash->fileBlockIndex)
			{
			case MPQ_HASH_ENTRY_EMPTY:
				++emptyHash;
				break;
			case MPQ_HASH_ENTRY_DELETED:
				++deletedHash;
				break;
			default:
				if( hash->fileBlockIndex > m_blockTable.size() )
					++overflowedHash;
				else
					++usedHash;
				break;
			}
		}else
			++invalidHash;
	}

	_tprintf(_T("===================\n"));
	_tprintf(_T("Filename     : %s\n"), m_filename.c_str());
	_tprintf(_T("Statistics:\n"));
	_tprintf(_T("Hash Number:\n"));
	_tprintf(_T("  Total      : %i\n"), m_hashTable.size());
	_tprintf(_T("  Used       : %i\n"), usedHash);
	_tprintf(_T("  Deleted    : %i\n"), deletedHash);
	_tprintf(_T("  Invalid    : %i\n"), invalidHash);
	_tprintf(_T("  Empty      : %i\n"), emptyHash);
	_tprintf(_T("  Overflowed : %i\n"), overflowedHash);
	_tprintf(_T("Block Number:\n"));
	_tprintf(_T("  Total      : %i\n"), m_blockTable.size());
	_tprintf(_T("  Used       : %i\n"), usedBlock);
	_tprintf(_T("  Unknown    : %i\n"), unknownBlock);
	_tprintf(_T("  Invalid    : %i\n"), invalidBlock);
	_tprintf(_T("List File:\n"));
	_tprintf(_T("  Size       : %i\n"), m_listFile.size());
	_tprintf(_T("===================\n"));

	CloseFile();
	return TRUE;
}

UInt32 GDFFile::GetPatchStepLength()
{
	UInt outlen	= 0;
	String versionString;
	for( PatchStepMap::iterator itr = m_patchStepMap.begin(); itr!=m_patchStepMap.end(); ++itr )
	{
		versionString.Format(_T(":%i"), itr->first);
		outlen += versionString.GetLength() + 2;
		for( Index j = C_INDEX(0); j < (UInt)itr->second.size(); ++j )
		{
			if( itr->second[j] )
				outlen += itr->second[j]->filename.GetLength() + 2 + 2;	// 2 for carriage return, 2 for char denoting ADD or DELETE
		}
	}
	return outlen;
}

UInt32 GDFFile::GetPatchStep(TCHAR* out)
{
	UInt curPos = 0;
	String versionString;
	for( PatchStepMap::iterator itr = m_patchStepMap.begin(); itr!=m_patchStepMap.end(); ++itr )
	{
		versionString.Format(_T(":%i"), itr->first);
		_stprintf(&out[curPos], _T("%s"), versionString.c_str());
		curPos			+= versionString.GetLength();
		out[curPos]		= 0x0d;
		out[curPos+1]	= 0x0a;
		curPos			+= 2;
		for( Index j = C_INDEX(0); j < (UInt)itr->second.size(); ++j )
		{
			if( itr->second[j] )
			{
				switch( itr->second[j]->type )
				{
				case MPQ_ARCHIVE_CHANGE_ADD:
					_stprintf(&out[curPos], _T("A "));
					curPos			+= 2;
					memcpy(&out[curPos], itr->second[j]->filename.GetBuffer(itr->second[j]->filename.GetLength()), itr->second[j]->filename.GetLength() * sizeof(TCHAR));
					curPos			+= itr->second[j]->filename.GetLength();
					out[curPos]		= 0x0d;
					out[curPos+1]	= 0x0a;
					curPos			+= 2;
					break;
				case MPQ_ARCHIVE_CHANGE_DELETE:
					_stprintf(&out[curPos], _T("D "));
					curPos			+= 2;
					memcpy(&out[curPos], itr->second[j]->filename.GetBuffer(itr->second[j]->filename.GetLength()), itr->second[j]->filename.GetLength() * sizeof(TCHAR));
					curPos			+= itr->second[j]->filename.GetLength();
					out[curPos]		= 0x0d;
					out[curPos+1]	= 0x0a;
					curPos			+= 2;
					break;
				}
			}
		}
	}
	return curPos;
}

BOOL GDFFile::PatchStepAdd(UInt32 writeLogVersion, String& filename)
{
	if( writeLogVersion==0 )	return FALSE;	// version 0 is an exception case, for not versioning anything here

	String targetFilename = filename;
	targetFilename.ToUpper();
	targetFilename.Replace(_T("/"), _T("\\"));

	if( targetFilename.GetLength()>2 && targetFilename.Left(2)==_T(".\\") )
		targetFilename = targetFilename.Right(targetFilename.GetLength()-2);

	PatchStepMap::iterator itr = m_patchStepMap.find(writeLogVersion);
	if( itr==m_patchStepMap.end() )	// add new log number
	{
		GdfPatchStep* tempstep = SafeCreateObject(GdfPatchStep);
		tempstep->filename	= targetFilename;
		tempstep->type		= MPQ_ARCHIVE_CHANGE_ADD;
		PatchStepVector tempvector;
		tempvector.push_back(tempstep);
		m_patchStepMap.insert(std::make_pair(writeLogVersion, tempvector));
	}else		// reuse current log number
	{
		BOOL found = FALSE;
		for( Index i = C_INDEX(0); i < itr->second.size(); ++i )
		{
			if( targetFilename.CompareNoCase(itr->second[i]->filename.c_str())==0 )
			{
				switch( itr->second[i]->type )
				{
				case MPQ_ARCHIVE_CHANGE_DELETE:
					itr->second[i]->type = MPQ_ARCHIVE_CHANGE_ADD;
					found = TRUE;
					break;
				case MPQ_ARCHIVE_CHANGE_ADD:
					found = TRUE;
					break;
				}
			}
			if( found )
				break;
		}
		if( !found )
		{
			GdfPatchStep* tempstep = SafeCreateObject(GdfPatchStep);
			tempstep->filename	= targetFilename;
			tempstep->type		= MPQ_ARCHIVE_CHANGE_ADD;
			itr->second.push_back(tempstep);
		}
	}
	return TRUE;
}

BOOL GDFFile::PatchStepDelete(UInt32 writeLogVersion, String& filename)
{
	if( writeLogVersion==0 )	return FALSE;	// version 0 is an exception case, for not versioning anything here

	String targetFilename = filename;
	targetFilename.ToUpper();
	targetFilename.Replace(_T("/"), _T("\\"));

	if( targetFilename.GetLength()>2 && targetFilename.Left(2)==_T(".\\") )
		targetFilename = targetFilename.Right(targetFilename.GetLength()-2);

	PatchStepMap::iterator itr = m_patchStepMap.find(writeLogVersion);
	if( itr==m_patchStepMap.end() )	// add new log number
	{
		GdfPatchStep* tempstep = SafeCreateObject(GdfPatchStep);
		tempstep->filename	= targetFilename;
		tempstep->type		= MPQ_ARCHIVE_CHANGE_DELETE;
		PatchStepVector tempvector;
		tempvector.push_back(tempstep);
		m_patchStepMap.insert(std::make_pair(writeLogVersion, tempvector));
	}else		// reuse current log number
	{
		BOOL found = FALSE;
		for( Index i = C_INDEX(0); i < itr->second.size(); ++i )
		{
			if( targetFilename.CompareNoCase(itr->second[i]->filename.c_str())==0 )
			{
				switch( itr->second[i]->type )
				{
				case MPQ_ARCHIVE_CHANGE_DELETE:
					found = TRUE;
					break;
				case MPQ_ARCHIVE_CHANGE_ADD:
					itr->second[i]->type = MPQ_ARCHIVE_CHANGE_DELETE;
					found = TRUE;
					break;
				}
			}
			if( found )
				break;
		}
		if( !found )
		{
			GdfPatchStep* tempstep = SafeCreateObject(GdfPatchStep);
			tempstep->filename	= targetFilename;
			tempstep->type		= MPQ_ARCHIVE_CHANGE_DELETE;
			itr->second.push_back(tempstep);
		}
	}
	return TRUE;
}

PatchStepMap& GDFFile::GetPatchStep()
{
	return m_patchStepMap;
}

BOOL GDFFile::LoadPatchStep()
{
	FreePatchStep();
	BOOL succeed = FALSE;

	if( m_file.IsOpened() )
	{
		String delim;
		delim.Format(_T("%c"), 10);

		UInt outlen = 0;
		String patchstep = _T("(patchstep)");
		GDFStream* fstream = GetStream(patchstep);
		if( fstream )
		{
			UInt32 outlen = 0;
			UInt32 fileSize = fstream->GetFileSize();
			if( fileSize > 0 )
			{
				unsigned long hashkey = HashString(_T("(patchstep)"), MPQ_HASH_FILE_KEY);
				BYTE* buf = SafeAllocate(BYTE, fileSize);
//				ZeroMemory(buf, fileSize);
				if( fstream->Read((BYTE*)buf, fileSize, outlen, fileSize) )
				{
					DecryptData(buf, fileSize, hashkey);
					TCHAR* pData = SafeAllocate(TCHAR, fileSize + 1);
					ZeroMemory(pData, fileSize + 2);
					memcpy(pData, buf, fileSize);
//					TCHAR* pData = (TCHAR*)buf;
					String fullFile(pData);

					String::size_type lastPos = fullFile.find_first_not_of(delim.c_str(), 0);
					String::size_type pos     = fullFile.find_first_of(delim.c_str(), lastPos);

					UInt32 readVer = 0;
					while (fullFile.npos != pos && fullFile.npos != lastPos)
					{
						String wholeline = String(fullFile.substr(lastPos, pos - lastPos - 1).c_str());
						switch( wholeline.GetAt(0) )
						{
						case ':':			// e.g. ":340" indicates this is version 340
							readVer = _ttoi(wholeline.Right(wholeline.GetLength()-1).c_str());
							break;
						case 'A':
							PatchStepAdd(readVer, wholeline.Right(wholeline.GetLength()-2));
							break;
						case 'D':			// e.g. "A Data/addfilename.dat" or "D Data/deletefilename.dat"
							PatchStepDelete(readVer, wholeline.Right(wholeline.GetLength()-2));
							break;
						}
						lastPos = fullFile.find_first_not_of(delim.c_str(), pos);
						pos = fullFile.find_first_of(delim.c_str(), lastPos);
					}
					SafeDeallocate(pData);
					succeed = TRUE;
				}else
				{
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::LoadPatchStep Error (1) [file stream read error for patch step file.]"));
				}
				SafeDeallocate(buf);
			}else
			{
				succeed = TRUE;
				DEBUG_INFODTL(GLOBAL_LOGGER, _T("GDFFile::LoadPatchStep Warning (2) [patch step list size=0.]"));
			}
		}else
		{
			succeed = TRUE;
			TRACE_INFODTL(GLOBAL_LOGGER, _T("GDFFile::LoadPatchStep Warning (3) [file stream not found for patch step list.]"));
		}
	}else
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFFile::LoadPatchStep Error (4) [file open error]"));
	}
	return succeed;
}

String GDFFile::GetBackupFilename(String& PatchFilename)
{
	String dirfile, temp;
	temp = PatchFilename;
	TCHAR delim[2] = _T("\\");
	temp.Replace(_T("/"), _T("\\"));
	String::size_type lastPos = temp.find_last_of(delim);
	if( lastPos==String::npos )
	{
		dirfile.Format(_T("_%s.tmp"), PatchFilename.c_str());
		return dirfile;
	}else
	{
		String dir, filename;
		dir			= temp.substr(0, lastPos);
		filename	= temp.substr(lastPos+1, temp.GetLength()-(lastPos+1));
		dirfile.Format(_T("%s\\_%s.tmp"), dir.c_str(), filename.c_str() );
		return dirfile;
	}
}

BOOL GDFFile::GetBlockStat(UInt32& numAllBlock, UInt32& numUsedBlock, UInt32& sizeAllBlock, UInt32& sizeUsedBlock)
{
	UInt32 numA = 0, numB = 0, numC = 0, numD = 0;

	GdfBlockTable* block;
	GdfHashTable* hash;
	for( Index i = C_INDEX(0); i < m_blockTable.size(); ++i )
	{
		block = m_blockTable[i];
		if( block )
		{
			++numA;
			numC += block->blockSize;
		}
	}
	for( Index i = C_INDEX(0); i < m_hashTable.size(); ++i )
	{
		hash = m_hashTable[i];
		if( hash )
		{
			if( hash->fileBlockIndex!=MPQ_HASH_ENTRY_EMPTY && hash->fileBlockIndex!=MPQ_HASH_ENTRY_DELETED 
				&& hash->fileBlockIndex < m_hashTable.size()
				&& m_blockTable[hash->fileBlockIndex] )
			{
				block = m_blockTable[hash->fileBlockIndex];
				if( block )
				{
					++numB;
					numD += block->blockSize;
				}
			}
		}
	}
	numAllBlock		= numA;
	numUsedBlock	= numB;
	sizeAllBlock	= numC;
	sizeUsedBlock	= numD;
	return TRUE;
}
