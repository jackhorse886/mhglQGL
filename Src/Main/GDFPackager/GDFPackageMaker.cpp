#include "common.h"
#include <stdlib.h>
#include "GDFPackageMaker.h"
#include "CFindFile.h"
#include "Reuben/Math/CRC.h"
#include <sys/stat.h>

#define STREAM_BUF_MAX 65536
#define HISTORY_FILENAME _T("PatchHistory/history.dat")

GDFPackageMaker::GDFPackageMaker()
{
//	m_archiveStatus = MPQ_ARCHIVE_STATUS_NOTOPENED;
}

GDFPackageMaker::~GDFPackageMaker()
{
}

void GDFPackageMaker::InitLogger()
{
	gGlobalLogger = SafeCreateObject2(Logger, _T("GDFPackageMaker"), _T("log.txt"));

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void GDFPackageMaker::CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public API
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GDFPackageMaker::Add(PCSTRING SrcFilename, PCSTRING srcFolder)
{
	WIN32_FIND_DATA FileData;
	String fullFilename;
	if( srcFolder )
		fullFilename.Format(_T("%s\\%s"), srcFolder, SrcFilename);
	else
		fullFilename.Format(_T("%s"), SrcFilename);
	HANDLE hSearch = FindFirstFile(fullFilename.c_str(), &FileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		StringVector fileVector;
		BOOL fFinished = FALSE;
		String dirPath;

		// path directory preparation
		String TargetFilename = SrcFilename;
		TargetFilename.Replace(_T("/"), _T("\\"));

		TCHAR delim[2] = _T("\\");
		String::size_type lastPos = TargetFilename.find_last_of(delim);
		if( lastPos==String::npos )
			dirPath = _T("");
		else
			dirPath.Format(_T("%s\\"), TargetFilename.substr(0, lastPos).c_str());

		while (!fFinished) 
		{ 
			// do something
			String tempString;
			tempString.Format(_T("%s%s"), dirPath.c_str(), FileData.cFileName);
			if( _tcscmp(FileData.cFileName, _T(".")) && _tcscmp(FileData.cFileName, _T("..")) )
				fileVector.push_back(tempString);

			// find another
			if (!FindNextFile(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					fFinished = TRUE; 
				else 
					TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::Add Error (1) [Next file find error] filename=%s"), SrcFilename);
			}

		}

		// add after vector of list is prepared
		String filename;
		DWORD attr;
		for( Index i = C_INDEX(0); i < fileVector.size(); ++i )
		{
			filename = fileVector[i];
			String fullFileInList;
			if( srcFolder )
				fullFileInList.Format(_T("%s\\%s"), srcFolder, filename.c_str());
			else
				fullFileInList = filename.c_str();
			attr = GetFileAttributes(fullFileInList.c_str());
			if( attr & FILE_ATTRIBUTE_DIRECTORY) // if directory, add all files inside; if not, add only source file.
			{
				RecursiveAdd(filename.c_str(), srcFolder);
			}else
			{
				AddArchiveChange(filename.c_str(), MPQ_ARCHIVE_CHANGE_SOURCE_FILE);
			}
		}
		FindClose(hSearch);
	}else
	{
		// initial file not found
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::Add Error (2) [File not found] filename=%s"), SrcFilename);
	}
}

VOID GDFPackageMaker::RecursiveAdd(PCSTRING SrcFilename, PCSTRING srcFolder)
{
	String TargetInput;
	TargetInput.Format(_T("%s\\*.*"), SrcFilename);
	String cTargetInput = TargetInput.c_str();
	TCHAR* foldername = cTargetInput.GetBuffer(cTargetInput.GetLength());
	CFindFile finder(foldername);
	String FullPath;

	while (finder.GetPathName(&FullPath) != 0)
	{
		String fullPathInList;
		if( srcFolder )
			fullPathInList.Format(_T("%s\\%s"), srcFolder, FullPath.c_str());
		else
			fullPathInList = FullPath;
		DWORD attr = GetFileAttributes(fullPathInList.c_str());
		if( attr & FILE_ATTRIBUTE_DIRECTORY) // if directory, add all files inside; if not, add only source file.
		{
			RecursiveAdd(FullPath.c_str(), srcFolder);
		}else
			AddArchiveChange(FullPath.c_str(), MPQ_ARCHIVE_CHANGE_SOURCE_FILE);
	}
}

VOID GDFPackageMaker::UpdatePatch(PCSTRING SrcFilename, BOOL detailCheck)
{
	Add(SrcFilename, NULL);		// a list of updates is prepared

	// read history file
	CreateDirectory(_T("PatchHistory"), NULL);
	ReadHistoryFile();

	FILETIME ft1, ft2;
	struct stat st;
	UInt64 filesize2;
	String storeString;
	StringA charString;
	UInt32 checksum1, checksum2;
	GdfHistoryData* history;
	for( Index i = C_INDEX(0); i < m_archiveChange.size(); ++i )
	{
		storeString	= m_archiveChange[i]->filename;
		storeString.ToUpper();
		storeString.Replace(_T("/"), _T("\\"));
		if( storeString.GetLength()>2 && storeString.Left(2)==_T(".\\") )
			storeString = storeString.Right(storeString.GetLength()-2);

		history		= GetHistoryData(storeString.c_str());
		if( history )
		{
			charString = storeString.c_str();
			// Get info of import file
			stat(charString.c_str(), &st);
			memcpy(&ft1, &st.st_mtime, sizeof(UInt64));
			// get history data
			memcpy(&ft2, &history->filedate, sizeof(UInt64));
			filesize2	= history->filesize;

			// 3 checking: 1) filesize, 2) filedate, 3) if filedate changed but filesize not, checksum when compulsory
			if( st.st_size!=filesize2 )
				continue;

			if( ft1.dwHighDateTime!=ft2.dwHighDateTime || ft1.dwLowDateTime!=ft2.dwLowDateTime )
			{
				checksum1 = 0;
				Reuben::Math::CRC32(storeString.c_str(), checksum1);
				checksum2 = history->filechecksum;
				if( checksum1!=checksum2 )
					continue;
				// change of date in list in case that filesize, CHECKSUM the same, but filedate changed
				memcpy(&history->filedate, &st.st_mtime, sizeof(UInt64));
			}else
			{
				if( detailCheck )
				{
					checksum1 = 0;
					Reuben::Math::CRC32(storeString.c_str(), checksum1);
					checksum2 = history->filechecksum;
					if( checksum1!=checksum2 )
						continue;
				}
			}
			SafeDeleteObject(m_archiveChange[i]);
			m_archiveChange[i] = NULL;
		}
	}
	for( ArchiveChangeVector::iterator itr = m_archiveChange.begin(); itr!=m_archiveChange.end(); )
	{
		if( (*itr)==NULL )
		{
			m_archiveChange.erase(itr);
			itr = m_archiveChange.begin();
		}else
		{
			++itr;
		}
	}
	for( Index i = C_INDEX(0); i < m_archiveChange.size(); ++i )
	{
		if( m_archiveChange[i]->type==MPQ_ARCHIVE_CHANGE_UPDATE || m_archiveChange[i]->type==MPQ_ARCHIVE_CHANGE_ADD )
		{
			storeString	= m_archiveChange[i]->filename;
			storeString.ToUpper();
			storeString.Replace(_T("/"), _T("\\"));
			if( storeString.GetLength()>2 && storeString.Left(2)==_T(".\\") )
				storeString = storeString.Right(storeString.GetLength()-2);

			history = GetHistoryData(storeString.c_str());
			if( history )
			{
				charString				= storeString.c_str();
				stat(charString.c_str(), &st);
				memcpy(&history->filedate, &st.st_mtime, sizeof(UInt64));
				history->filesize		= st.st_size;
				history->filechecksum	= 0;
				Reuben::Math::CRC32(storeString.c_str(), history->filechecksum);
			}else
			{
				history					= SafeCreateObject(GdfHistoryData);
				charString				= storeString.c_str();
				stat(charString.c_str(), &st);
				memcpy(&history->filedate, &st.st_mtime, sizeof(UInt64));
				history->filesize		= st.st_size;
				_stprintf(history->filename, _T("%s"), storeString.c_str());
				history->filechecksum	= 0;
				Reuben::Math::CRC32(storeString.c_str(), history->filechecksum);
				m_historyData.push_back(history);
			}
		}
	}
}

BOOL GDFPackageMaker::WriteHistoryFile()
{
	if( m_archiveChange.size()==0 ) return TRUE;

	BOOL succeed = TRUE;
	File file;
	file.Open(HISTORY_FILENAME, File::MODE_WRITE);
	if( file.IsOpened() )
	{
		file.SetLength(0);
		file.SetPosition(0, File::MOVE_BEGIN);

		GdfHistoryHeader header;
		header.archiveSize	= 0;
		header.headerSize	= sizeof(GdfHistoryHeader);
		sprintf(header.magic, "DAT.", 4 * sizeof(CHAR));
		header.numOfEntries = (UInt32)m_historyData.size();
		header.version = HISTORY_FORMAT_VERSION;
		file.Write((BYTE*)&header, sizeof(GdfHistoryHeader));

		GdfHistoryData dummy;
		ZeroMemory(&dummy, sizeof(GdfHistoryData));
		for( Index i = C_INDEX(0); i < m_historyData.size(); ++i )
		{
			if( m_historyData[i] )
				file.Write((BYTE*)m_historyData[i], sizeof(GdfHistoryData));
			else
				file.Write((BYTE*)&dummy, sizeof(GdfHistoryData));
		}

		file.SetLength(file.GetPosition());

		header.archiveSize = (UInt32)file.GetPosition();
		file.SetPosition(0, File::MOVE_BEGIN);
		file.Write((BYTE*)&header, sizeof(GdfHistoryHeader));

		file.Close();
	}else
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GDFPackageMaker::WriteHistoryFile Error (1) [history file oepn for writing error]"));
		succeed = FALSE;
	}
	return succeed;
}

GdfHistoryData* GDFPackageMaker::GetHistoryData(PCSTRING SrcFilename)
{
	for( Index i = C_INDEX(0); i < m_historyData.size(); ++i )
	{
		TCHAR* tptr = m_historyData[i]->filename;
		if( m_historyData[i] && _tcsicmp(tptr, SrcFilename)==0 )
			return m_historyData[i];
	}
	return NULL;
}

BOOL GDFPackageMaker::ReadHistoryFile()
{
	BOOL succeed = TRUE;
	File file;
	file.Open(HISTORY_FILENAME, File::MODE_READ);
	if (file.IsOpened())
	{
		GdfHistoryHeader historyheader;
		file.Read((BYTE*)historyheader.magic, 4 * sizeof(CHAR));
		file.Read((BYTE*)&historyheader.headerSize, sizeof(UInt32));
		file.Read((BYTE*)&historyheader.version, sizeof(UInt32));
		if( historyheader.version==HISTORY_FORMAT_VERSION )
		{
			file.Read((BYTE*)&historyheader.numOfEntries, sizeof(UInt32));
			file.Read((BYTE*)&historyheader.archiveSize, sizeof(UInt32));

			if( historyheader.archiveSize==file.GetLength() )
			{
				for( Index i = C_INDEX(0); i < historyheader.numOfEntries; ++i )
				{
					GdfHistoryData* history	= SafeCreateObject(GdfHistoryData);
					file.Read((BYTE*)history, sizeof(GdfHistoryData));
					m_historyData.push_back(history);
				}
			}else
			{
				TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFPackagerMaker::ReadHistoryFile Error (1) [history file archive size error] declared size=%i, real size=%i"), historyheader.archiveSize, file.GetLength());
				succeed = FALSE;
			}
		}else
		{
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFPackagerMaker::ReadHistoryFile Error (2) [version not match] file ver=%i, app ver=%i"), historyheader.version, HISTORY_FORMAT_VERSION);
			succeed = FALSE;
		}
		file.Close();
	}
	return succeed;
}

VOID GDFPackageMaker::AddArchiveChange(PCSTRING SrcFilename, UInt8 SrcType)
{
	GdfArchiveChange* temp = SafeCreateObject(GdfArchiveChange);
	temp->filename = SrcFilename;
	temp->type = MPQ_ARCHIVE_CHANGE_ADD;
	temp->source = SrcType;

	// ADD validation
	BOOLEAN pendingfound = BOOLEAN_FALSE;
	for( Index i = C_INDEX(0); i < m_archiveChange.size(); ++i )
	{
		if( temp->filename.Compare(m_archiveChange[i]->filename.c_str())==0 )
		{
			SafeDeleteObject(m_archiveChange[i]);
			m_archiveChange[i] = temp;
			pendingfound = BOOLEAN_TRUE;
			break;
		}
	}
	if( !pendingfound )
		m_archiveChange.push_back(temp);
}

BOOL GDFPackageMaker::Extract(PCSTRING patchFilename, PCSTRING targetFilename2)
{
	BOOL succeed = TRUE;

	GDFManager mgr;
	if( mgr.LoadPatchFile(patchFilename, GDFFILE_STATE_GAME) )
	{
		// Extract one file
		UInt32 beginOffset	= 0;
		UInt32 fileSize		= 0;
		BYTE* buf;
		BYTE* mapview = mgr.GetMapViewOfFile(targetFilename2, beginOffset, fileSize);
		if( mapview )
		{
			buf = mapview + beginOffset;
			// Recursive Directory Create
			{
				String targetFilename(targetFilename2);
				TCHAR delim[2] = _T("\\");
				String::size_type lastPos = targetFilename.find_first_not_of(delim, 0);
				String::size_type pos     = targetFilename.find_first_of(delim, lastPos);
				String composedDir;
				String tempdir;

				while (targetFilename.npos != pos || targetFilename.npos != lastPos)
				{
					if( pos==String::npos ) // not create directory is it "\\" is not found
						break;

					tempdir = composedDir;
					if( composedDir.GetLength()!=0 )
						composedDir.Format(_T("%s\\%s"), tempdir.c_str(), targetFilename.substr(lastPos, pos - lastPos).c_str());
					else
						composedDir = targetFilename.substr(lastPos, pos - lastPos).c_str();
					_tmkdir(composedDir.c_str());

					lastPos = targetFilename.find_first_not_of(delim, pos);
					pos = targetFilename.find_first_of(delim, lastPos);
				}
			}
			File file;
			file.Open(targetFilename2, File::MODE_WRITE);
			if (file.IsOpened())
			{
				file.Write(buf, fileSize);
				file.SetLength(fileSize);
				file.Close();
			}else
			{
				TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFPackageMaker::Extract Error (2) [Write file error] patchFilename=%s, targetFilename=%s"), patchFilename, targetFilename2);
				succeed = FALSE;
			}
			UnmapViewOfFile(mapview);
		}else
		{
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFPackageMaker::Extract Error (3) [Target File not found in patch] patchFilename=%s, targetFilename=%s"), patchFilename, targetFilename2);
			succeed = FALSE;
		}
		mgr.ClosePatchFile();
	}else
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::Extract Error (4) [File open error] patchFilename=%s"), patchFilename);
		succeed = FALSE;
	}
	return succeed;
}

BOOL GDFPackageMaker::ExtractAll(PCSTRING patchFilename)
{
	BOOL succeed = TRUE;

	GDFManager mgr;
	mgr.LoadPatchFile(patchFilename, GDFFILE_STATE_PATCH_READ);
	ListFileVector& list = mgr.GetListFile();
	GdfListFile* name;
	GDFStream* fstream;
	BYTE* outbuf = SafeAllocate(BYTE, STREAM_BUF_MAX);
	for( Index i = C_INDEX(0); i < (UInt32)list.size(); ++i )
	{
		name = list[i];
		if( name )
		{
			String SrcFilename = name->filename;
			fstream = mgr.GetStream(SrcFilename.c_str());
			if( fstream )
			{
				// Recursive Directory Create
				{
					TCHAR delim[2] = _T("\\");
					String::size_type lastPos = SrcFilename.find_first_not_of(delim, 0);
					String::size_type pos     = SrcFilename.find_first_of(delim, lastPos);
					String composedDir;
					String tempdir;

					while (SrcFilename.npos != pos || SrcFilename.npos != lastPos)
					{
						if( pos==String::npos ) // not create directory is it "\\" is not found
							break;

						tempdir = composedDir;
						if( composedDir.GetLength()!=0 )
							composedDir.Format(_T("%s\\%s"), tempdir.c_str(), SrcFilename.substr(lastPos, pos - lastPos).c_str());
						else
							composedDir = SrcFilename.substr(lastPos, pos - lastPos).c_str();
						_tmkdir(composedDir.c_str());

						lastPos = SrcFilename.find_first_not_of(delim, pos);
						pos = SrcFilename.find_first_of(delim, lastPos);
					}
				}

				// Extract one file
				UInt32 outlen = 0;
				try{
					File file;
					file.Open(name->filename.c_str(), File::MODE_WRITE);
					if (file.IsOpened())
					{
						while( !fstream->IsEnd() )
						{
							fstream->Read(outbuf, STREAM_BUF_MAX, outlen, STREAM_BUF_MAX);
							if( outlen > 0 )
								file.Write(outbuf, outlen * sizeof(BYTE));
						}
						file.SetLength(fstream->GetFileSize());
						file.Close();
					}else
					{
						DEBUG_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFPackageMaker::ExtractAll Error (1) [File write open error] patchFilename=%s, extractName=%s"), patchFilename, name->filename.c_str());
						succeed = FALSE;
					}
				}catch(FileException Er)
				{
					DEBUG_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFPackageMaker::ExtractAll Error (2) [File write error catch] patchFilename=%s, extractName=%s"), patchFilename, name->filename.c_str());
					succeed = FALSE;
				}
				SafeDeleteObject(fstream);
			}else
			{
				DEBUG_ERRORDTL_2(GLOBAL_LOGGER, _T("GDFPackageMaker::ExtractAll Error (3) [File not found in patch] patchFilename=%s, extractName=%s"), patchFilename, name->filename.c_str());
				succeed = FALSE;
			}
		}else
		{
			DEBUG_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::ExtractAll Error (4) [File list preparation error] patchFilename=%s"), patchFilename);
			succeed = FALSE;
		}
	}
	SafeDeallocate(outbuf);
	mgr.ClosePatchFile();
	return succeed;
}
	
VOID GDFPackageMaker::Delete(PCSTRING TrgFilename, BOOL IsDirectory)
{
	if( IsDirectory ) // if directory, delete all files inside; if not, delete only source file.
	{
		String cTrgFilename = TrgFilename;
		TCHAR* foldername = cTrgFilename.GetBuffer(cTrgFilename.GetLength());
		CFindFile finder(foldername);
		String FullPath;

	    while (finder.GetPathName(&FullPath) != 0)
		{
			DeleteArchiveChange(FullPath.c_str());
		}
	}else
	{
		DeleteArchiveChange(TrgFilename);
	}
}

VOID GDFPackageMaker::DeleteArchiveChange(PCSTRING SrcFilename)
{
	GdfArchiveChange* temp = SafeCreateObject(GdfArchiveChange);
	temp->filename = SrcFilename;
	temp->type = MPQ_ARCHIVE_CHANGE_DELETE;

	// ADD validation
	BOOLEAN pendingfound = BOOLEAN_FALSE;
	for( Index i = C_INDEX(0); i < m_archiveChange.size(); ++i )
	{
		if( temp->filename.Compare(m_archiveChange[i]->filename.c_str())==0 )
		{
			SafeDeleteObject(m_archiveChange[i]);
			m_archiveChange[i] = temp;
			pendingfound = BOOLEAN_TRUE;
			break;
		}
	}
	if( !pendingfound )
		m_archiveChange.push_back(temp);
}

BOOL GDFPackageMaker::View(PCSTRING patchFilename)
{
	if( !File::IsExists(patchFilename) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::View Error (1) [File not found] patchFilename=%s"), patchFilename);
		return FALSE;
	}

	GDFManager mgr;
	if( !mgr.LoadPatchFile(patchFilename, GDFFILE_STATE_PATCH_READ) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::View Error (2) [load patch file failed] patchFilename=%s"), patchFilename);
		return FALSE;
	}
	
	ListFileVector& list = mgr.GetListFile();
	GdfListFile* content;
	for( Index i = C_INDEX(0); i < list.size(); ++i )
	{
		content = list[i];
		if( content )
			_tprintf(_T("%s\n"), content->filename.c_str());
	}
	mgr.ClosePatchFile();
	return TRUE;
}

BOOL GDFPackageMaker::Test(PCSTRING patchFilename)
{
	if( !File::IsExists(patchFilename) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::Test Error (1) [File not found] patchFilename=%s"), patchFilename);
		return FALSE;
	}

	GDFManager mgr;
	if( !mgr.TestPatchFile(patchFilename) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::Test Error (2) [testpatchfile failed] patchFilename=%s"), patchFilename);
		return FALSE;
	}
	return TRUE;
}

BOOL GDFPackageMaker::Repatch(PCSTRING patchFilename)
{
	if( !File::IsExists(patchFilename) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::Repatch Error (1) [File not found] patchFilename=%s"), patchFilename);
		return FALSE;
	}

	GDFManager mgr;
	if( !mgr.Repatch(patchFilename) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::Repatch Error (2) [repatch failed] patchFilename=%s"), patchFilename);
		return FALSE;
	}
	return TRUE;
}

//VOID GDFPackageMaker::GetListFile()
//{
//	for( Index i = C_INDEX(0); i < m_mergedListFile.size(); ++i )
//		SafeDeleteObject(m_mergedListFile[i]);
//	m_mergedListFile.clear();
//
//	GdfListFile* temp;
//	for( Index i = C_INDEX(0); i < m_listFile.size(); ++i )
//	{
//		temp = SafeCreateObject(GdfListFile);
//		temp->filename = m_listFile[i]->filename;
//		temp->filePathHashA = m_listFile[i]->filePathHashA;
//		temp->filePathHashB = m_listFile[i]->filePathHashB;
//		m_mergedListFile.push_back(temp);
//	}
//	for( Index i = C_INDEX(0); i < m_archiveChange.size(); ++i )
//	{
//		temp = SafeCreateObject(GdfListFile);
//		temp->filename = m_archiveChange[i]->filename;
//		temp->filePathHashA = m_archiveChange[i]->filePathHashA;
//		temp->filePathHashB = m_archiveChange[i]->filePathHashB;
//		m_mergedListFile.push_back(temp);
//	}
//}
//

BOOL GDFPackageMaker::ExecuteFileUpdate(PCSTRING patchFilename, PCSTRING srcFolder, UInt32 writeLogVersion, BOOL ForceMaster)
{
	if( m_archiveChange.size()==0 && File::IsExists(patchFilename) )		return TRUE;

	BOOL succeed;
	GDFManager mgr;
	succeed = mgr.UpdatePatch(patchFilename, m_archiveChange, srcFolder, writeLogVersion, ForceMaster);
	return succeed;
}

BOOL GDFPackageMaker::ExecuteMerge(PCSTRING ToFilename, PCSTRING FromFilename, BOOL inheritPatchStep)
{
	if( _tcsicmp(ToFilename, FromFilename)==0 )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::ExecuteMerge (1) [same data source] Filename=%s"), FromFilename);
		return FALSE;
	}

	BOOL succeed;
	GDFManager mgr;
	succeed = mgr.Merge(ToFilename, FromFilename, inheritPatchStep, NULL);
	return succeed;
}


BOOL GDFPackageMaker::ViewPatchStep(PCSTRING patchFilename)
{
	if( !File::IsExists(patchFilename) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::ViewPatchStep (1) [File not found] patchFilename=%s"), patchFilename);
		return FALSE;
	}

	GDFManager mgr;
	if( !mgr.LoadPatchFile(patchFilename, GDFFILE_STATE_PATCH_READ) )
	{
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("GDFPackageMaker::ViewPatchStep (2) [load patch file failed] patchFilename=%s"), patchFilename);
		return FALSE;
	}
	
	PatchStepMap& patchstep = mgr.GetPatchStep();
	for( PatchStepMap::iterator itr = patchstep.begin(); itr!=patchstep.end(); ++itr )
	{
		_tprintf(_T(":%i\n"), itr->first);
		for( Index i = C_INDEX(0); i < itr->second.size(); ++i )
		{
			switch( itr->second[i]->type )
			{
			case MPQ_ARCHIVE_CHANGE_ADD:
				_tprintf(_T("A %s\n"), itr->second[i]->filename.c_str());
				break;
			case MPQ_ARCHIVE_CHANGE_UPDATE:
				_tprintf(_T("U %s\n"), itr->second[i]->filename.c_str());
				break;
			case MPQ_ARCHIVE_CHANGE_DELETE:
				_tprintf(_T("D %s\n"), itr->second[i]->filename.c_str());
				break;
			default:
				_tprintf(_T("I %s\n"), itr->second[i]->filename.c_str());
			}
		}
	}
	mgr.ClosePatchFile();
	return TRUE;
}

