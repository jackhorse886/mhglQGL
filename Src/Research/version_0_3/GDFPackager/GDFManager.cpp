#include "common.h"
#include <stdlib.h>
#include "GDFManager.h"
#include "GDFStream.h"
#include "GDFFile.h"

#define STREAM_BUF_MAX	65536

GDFManager::GDFManager()
{
}

GDFManager::~GDFManager()
{
}

bool GDFManager::GetTimeFromFileTime(const FILETIME &fileTime, time_t &time)
{
	// The FILETIME represents a 64-bit integer: the number of 100 ns units since January 1, 1601
	unsigned long long nTime = ((unsigned long long)fileTime.dwHighDateTime << 32) + fileTime.dwLowDateTime;

	if (nTime < EPOCH_OFFSET)
		return false;

	nTime -= EPOCH_OFFSET;	// Convert the time base from 01/01/1601 to 01/01/1970
	nTime /= 10000000ULL;	// Convert 100 ns to sec

	time = (time_t)nTime;

	// Test for overflow (FILETIME is 64 bits, time_t is 32 bits)
	if ((nTime - (unsigned long long)time) > 0)
		return false;

	return true;
}

void GDFManager::GetFileTimeFromTime(const time_t &time, FILETIME &fileTime)
{
	unsigned long long nTime = (unsigned long long)time;

	nTime *= 10000000ULL;
	nTime += EPOCH_OFFSET;

	fileTime.dwLowDateTime = (DWORD)nTime;
	fileTime.dwHighDateTime = (DWORD)(nTime >> 32);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GDFManager::LoadPatchFile(String& PatchFilename, GDFFILE_STATE fileState)
{
	BOOL succeed;
	switch(fileState)
	{
	case GDFFILE_STATE_GAME:
		succeed = m_pMainFile.LoadFile(PatchFilename);
		break;
	case GDFFILE_STATE_MASTER_WRITE:
		succeed = m_pMainFile.LoadFileWrite(PatchFilename);
		break;
	case GDFFILE_STATE_PATCH_READ:
		succeed = m_pMainFile.LoadFileWithList(PatchFilename);
		break;
	}
	return succeed;
}

BOOL GDFManager::ClosePatchFile()
{
	try
	{
		m_pMainFile.CloseFile();
		return TRUE;
	}
	catch(FileException Er)
	{
		_tprintf( _T("Err: File Closing error\n") );
	}
	return FALSE;
}

VOID GDFManager::LoadConfiguration()
{
}

ListFileVector& GDFManager::GetListFile()
{
	return m_pMainFile.GetListFile();;
}

PatchStepMap& GDFManager::GetPatchStep()
{
	return m_pMainFile.GetPatchStep();
}

void GDFManager::InitLogger()
{
	gGlobalLogger = SafeCreateObject2(Logger, _T("GDFPackageMaker"), _T("log.txt"));

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void GDFManager::CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

BOOL GDFManager::Merge(String& ToFilename, String& FromFilename, BOOL inheritPatchStep)
{
	BOOL succeed = TRUE;
	GDFFile gdfMaster;
	GDFFile gdfPatch;
	if( gdfPatch.LoadFileWithList(FromFilename) )
	{
		UInt32 predictedPower2 = 0;
		if( !File::IsExists(ToFilename.c_str()) )
		{
			UInt32 hashSize = gdfPatch.GetHashSize();
			for( Index i = C_INDEX(1); i < C_INDEX(32); ++i )
			{
				if( hashSize>>i == 0 )
				{
					predictedPower2 = i;
					break;
				}
			}
		}
		if( gdfMaster.LoadFileWrite(ToFilename, predictedPower2) )
		{
			// prepare a list of update / add
			PatchStepMap& patchstep	= gdfPatch.GetPatchStep();
			// merge from patch to master by stream
			if( !gdfMaster.UpdateFromStream(patchstep, &gdfPatch, inheritPatchStep) )
			{
				TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFManager::Merge Error (1) [UpdateFromStream failed] masterFile=%s, patchFile=%s"), ToFilename.c_str(), FromFilename.c_str());
				succeed = FALSE;
			}
			// destroy rubbish
			gdfPatch.CloseFile();
			gdfMaster.CloseFile();
		}else
		{
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFManager::Merge Error (3) [master file read error] masterFile=%s"), ToFilename.c_str());
			succeed = FALSE;
		}
	}else
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFManager::Merge Error (2) [patch file read error] patchFile=%s"), FromFilename.c_str());
		succeed = FALSE;
	}
	return succeed;
}

BOOL GDFManager::UpdatePatch(String& patchFilename, ArchiveChangeVector& change, UInt32 writeLogVersion, BOOL ForceMaster)
{
	BOOL succeed;

	UInt predictedPower2 = 0;
	if( ForceMaster )
		predictedPower2 = 19;
	else
	{
		UInt32 estimatedFileNum = (UInt32)change.size() + 1;		// num = size + 2 [2 sys files(listfile & patchstep)]; calculation uses (num - 1)
		for( Index i = C_INDEX(1); i < C_INDEX(32); ++i )
		{
			if( estimatedFileNum>>i == 0 )
			{
				predictedPower2 = i;
				break;
			}
		}
	}

	GDFFile masterFile;
	succeed = masterFile.LoadFileWrite(patchFilename, predictedPower2);
	if( succeed )
	{
		succeed = masterFile.UpdateFromFile(change, writeLogVersion);
	}
	masterFile.CloseFile();
	return succeed;
}

BOOL GDFManager::IsGdfStreamSame(GDFStream* stream1, GDFStream* stream2)
{
	TRACE_ENSURE(stream1);
	TRACE_ENSURE(stream2);

	if( stream1->GetFileSize()!=stream2->GetFileSize() )
		return FALSE;

	UInt32 length1 = stream1->GetFileSize();
	UInt32 length2 = stream2->GetFileSize();
	UInt32 offset = 0;
	UInt32 bufLen;		// common shared
	UInt32 outlen;

	BOOL same = TRUE;;

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

		if( memcpy(buf1, buf2, outlen)!=0 )
		{
			same = FALSE;
			break;
		}
	}
	SafeDeallocate(buf1);
	SafeDeallocate(buf2);

	return same;
}

GDFStream* GDFManager::GetStream(String& TargetFilename)
{
	return m_pMainFile.GetStream(TargetFilename);
}

BOOL GDFManager::TestPatchFile(String& patchFilename)
{
	return m_pMainFile.TestFile(patchFilename);
}

BOOL GDFManager::Repatch(String& patchFilename)
{
	String ToFilename;
	ToFilename.Format(_T("%s.trn.tmp"), patchFilename.c_str());
	if( File::IsExists(ToFilename.c_str()) )
	{
		BOOL found = FALSE;
		for( Index i = C_INDEX(0); i < C_INDEX(10000); ++i )
		{
			ToFilename.Format(_T("%s%03i.trn.tmp"), patchFilename.c_str(), i);
			if( !File::IsExists(ToFilename.c_str()) )
			{
				found = TRUE;
				break;
			}
		}
		if( !found )
		{
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFManager::Merge Repatch (1) [Suitable temp filename not found] SrcFile=%s"), patchFilename.c_str());
			return FALSE;
		}
	}

	if( !Merge(ToFilename, patchFilename, TRUE) )
	{
		TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFManager::Merge Repatch (2) [Repatch failed] SrcFile=%s, TargetFile=%s"), patchFilename.c_str(), ToFilename.c_str());
		return FALSE;
	}else
	{
		if( MoveFileEx(ToFilename.c_str(), patchFilename.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) )
		{
			return TRUE;
		}else
		{
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFManager::Merge Repatch (3) [Move failed] SrcFile=%s, TargetFile=%s"), patchFilename.c_str(), ToFilename.c_str());
			return FALSE;
		}
	}
}

BYTE* GDFManager::GetMapViewOfFile(String& TargetFilename, UInt32& beginOffset, UInt32& fileSize)
{
	return m_pMainFile.GetMapViewOfFile(TargetFilename, beginOffset, fileSize);
}

BOOL GDFManager::GetBlockStat(UInt32& numAllBlock, UInt32& numUsedBlock, UInt32& sizeAllBlock, UInt32& sizeUsedBlock)
{
	return m_pMainFile.GetBlockStat(numAllBlock, numUsedBlock, sizeAllBlock, sizeUsedBlock);
}