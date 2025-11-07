// QGLPublish.cpp : �w�q���ε{�������O�欰�C
//

#include "stdafx.h"
#include "QGLPublish.h"
#include "QGLPublishDlg.h"
#include "GDFManager.h"
#include "GDFPackageMaker.h"
#include "Reuben/Math/CRC.h"
#include <time.h>
#include <sys/stat.h>

// CQGLPublishApp

BEGIN_MESSAGE_MAP(CQGLPublishApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

#define CONFIGURATION_INI_PUBLISH_FILE _T(".\\Config\\QGLPublish.ini")
#define STRING_CONFIG_FILE _T(".\\Config\\QGLPublish.ini")
#define STRING_OUTPUT_INI_FILE _T(".\\PatchHistory\\FileHistory.ini")
#define STRING_CONFIG_FILE_INI_FILE _T(".\\Config\\QGLPublish.ini")
#define STRING_OUTPUT_LOG _T(".\\Log\\MakePatchFilelist")
#define STRING_OUTPUT_VER_FILE _T("PatchVer.ini")
#define CONFIGURATION_SVN_INI_TEMPLATE_FILE _T("Config\\Publish_SVN_Template.ini")

// CQGLPublishApp �غc

#define BUF_SIZE 65536
#define MAX_BUF_SIZE 65536

//#define TEST_MERGER 1

CQGLPublishApp::CQGLPublishApp()
{
	InitLogger();
	m_latestVersion = 0;
	m_patchVersion = 0;
	m_patchRevision = 0;
	m_config.LoadConfigure();
	m_configSVN.LoadConfigure();
	m_configFilelist.LoadConfigure(m_config.path_ftp_filelist_file.c_str());
	m_configSummary.LoadConfigure(m_config.path_ftp_summary_file.c_str());
	m_configRevertlist.LoadConfigure(m_config.path_ftp_revertlist_file.c_str());
	m_prevVersion = 0;
	m_prevRevision = 0;
	m_config.GetMajorVersion(-1, m_majorVersion);
}

CQGLPublishApp::~CQGLPublishApp()
{
	m_filterlist.clear();
	CloseLogger();
}

// �Ȧ����@�� CQGLPublishApp ����

CQGLPublishApp theApp;


// CQGLPublishApp ��l�]�w

BOOL CQGLPublishApp::InitInstance()
{
	// ���p���ε{����T�M����w�ϥ� ComCtl32.dll 6.0 (�t) �H�᪩��
	// �H�ҥε�ı�Ƽ˦��A�h Windows XP �ݭn InitCommonControls()�C�_�h���ܡA
	// ����������إ߱N���ѡC
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// �зǪ�l�]�w
	// �p�G�z���ϥγo�ǥ\��åB�Q��ֳ̫᧹�����i�����ɤj�p�A�z�i�H�q�U�C
	// �{���X�������ݭn����l�Ʊ`���A�ܧ��x�s�]�w�Ȫ��n�����X
	// TODO: �z���ӾA�׭ק惡�r�� (�Ҧp�A���q�W�٩β�´�W��)
	SetRegistryKey(_T("���� AppWizard �Ҳ��ͪ����ε{��"));

	InitializeQGLPublishSetting();
#ifndef TEST_MERGER
	InitializeFilterList();
	ExportFileBriefList();
	UpdateSrcFolderList();
	NewPublishVersion();
#endif
	CQGLPublishDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �b����m��ϥ� [�T�w] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �b����m��ϥ� [����] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}

	// �]���w�g������ܤ���A�Ǧ^ FALSE�A�ҥH�ڭ̷|�������ε{���A
	// �ӫD���ܶ}�l���ε{�����T���C
	return FALSE;
}

VOID CQGLPublishApp::UpdateSrcFolderList()
{
	m_srcFileMap.clear();
	// obtain list of files
	UpdateRecurFolderList(GetSrcFolder().c_str(), _T(""), m_srcFileMap);
	// value initialization
	for( FileDisplayMap::iterator itr = m_srcFileMap.begin(); itr != m_srcFileMap.end(); ++itr )
	{
		FileProperty& fp = itr->second;
		const String& relativePath = itr->first;
		if( relativePath.GetLength() <= 40 )
			fp.displayFilename = relativePath;
		else
			fp.displayFilename.Format(_T("%s...%s"), relativePath.Left(8).c_str(), relativePath.Right(29).c_str());
		fp.method				= FILE_COPY_PROPERTY_NOCOPY;
		fp.method_packFilename	= _T("");
		fp.status				= FILE_PROPERTY_STATUS_CHANGED;
	}
	CompareFileBriefList();
}

VOID CQGLPublishApp::UpdateFileFolderList()
{
	m_fileFileMap.clear();
	// obtain list of files
	UpdateRecurFolderList(GetTargetFolder().c_str(), _T(""), m_fileFileMap);
	// value initialization
	for( FileDisplayMap::iterator itr = m_fileFileMap.begin(); itr != m_fileFileMap.end(); ++itr )
	{
		FileProperty& fp = itr->second;
		const String& relativePath = itr->first;
		if( relativePath.GetLength() <= 40 )
			fp.displayFilename = relativePath;
		else
			fp.displayFilename.Format(_T("%s...%s"), relativePath.Left(8).c_str(), relativePath.Right(29).c_str());
		fp.method				= FILE_COPY_PROPERTY_NOCOPY;
		fp.method_packFilename	= _T("");
		fp.status				= FILE_PROPERTY_STATUS_UNVERSIONED;
	}
}

VOID CQGLPublishApp::UpdatePackFolderList()
{
	m_packFileMap.clear();
	theApp.GetViewPatchList(m_selPackFile.c_str(), m_packFileMap);
}

VOID CQGLPublishApp::UpdateRecurFolderList(PCSTRING folder, PCSTRING relative, FileDisplayMap& displayMap)
{
	String criteria;
	criteria.Format(_T("%s\\*.*"), folder);

	WIN32_FIND_DATA fileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &fileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL bFinish = FALSE;
		while (!bFinish) 
		{
			// filter useless files
			if( _tcsicmp(fileData.cFileName, _T("."))!=0 &&
				_tcsicmp(fileData.cFileName, _T(".."))!=0 &&
				!(fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				)
			{
				if( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	// recursive if directory
				{
					String nextFolder, relativeFolder;
					nextFolder.Format(_T("%s\\%s"), folder, fileData.cFileName);
					if( _tcslen(relative)==0 )
						relativeFolder = fileData.cFileName;
					else
						relativeFolder.Format(_T("%s\\%s"), relative, fileData.cFileName);
					UpdateRecurFolderList(nextFolder.c_str(), relativeFolder.c_str(), displayMap);
				}else	// add to list if file
				{
					String relativePath;
					if( _tcslen(relative)==0 )
						relativePath = fileData.cFileName;
					else
						relativePath.Format(_T("%s\\%s"), relative, fileData.cFileName);
					FileProperty fp;
					displayMap.insert(std::make_pair(relativePath, fp));
				}
			}
			// find another
			if (!FindNextFile(hSearch, &fileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinish = TRUE; 
				else 
					MessageBox(m_pMainWnd->m_hWnd, _T("list search error"), _T("err"), MB_OK);
			}
		}
		FindClose(hSearch);
	}
}

VOID CQGLPublishApp::InitializeQGLPublishSetting()
{
	UpdateQGLPublishSetting();
}

VOID CQGLPublishApp::UpdateQGLPublishSetting()
{
	SetSrcFolder(GetConfigure()->path_lastSource.c_str());
}

VOID CQGLPublishApp::GetViewPatchList(PCSTRING filename, FileDisplayMap& fileMap)
{
	GDFManager mgr;
	String patchFilename = filename;
	if( !File::IsExists(filename) || !mgr.LoadPatchFile(patchFilename.c_str(), GDFFILE_STATE_PATCH_READ))
		return;
	
	ListFileVector& list = mgr.GetListFile();
	for( Index i = C_INDEX(0); i < list.size(); ++i )
	{
		GdfListFile& content = *list[i];
		FileProperty fp;
		fileMap.insert(std::make_pair(content.filename, fp));
	}
	mgr.ClosePatchFile();

	for( FileDisplayMap::iterator itr = fileMap.begin(); itr != fileMap.end(); ++itr )
	{
		const String& filename = itr->first;
		FileProperty& fp = itr->second;
		if( filename.GetLength() >= 40 )
			fp.displayFilename.Format(_T("%s...%s"), filename.Left(8).c_str(), filename.Right(29).c_str());
		else
			fp.displayFilename = filename;
		fp.method				= FILE_COPY_PROPERTY_NOCOPY;
		fp.method_packFilename	= _T("");
		fp.status				= FILE_PROPERTY_STATUS_UNVERSIONED;
	}
}

void InitLogger()
{
	Reuben::Utility::ConfigFile config(CONFIGURATION_INI_PUBLISH_FILE);
	gGlobalLogger = SafeCreateObject2(Logger, _T("QGLPublish"), CONFIGURATION_INI_PUBLISH_FILE);
	gGlobalLogger->Open(config);

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

VOID CQGLPublishApp::InitializeFilterList()
{
	m_filterlist.clear();
	{
		String tempfile, tempbuf;
		tempfile = STRING_OUTPUT_INI_FILE;
		m_filterlist.push_back(tempfile);
		tempfile = STRING_CONFIG_FILE_INI_FILE;
		m_filterlist.push_back(tempfile);
		tempfile = _T(".");
		m_filterlist.push_back(tempfile);
		tempfile = _T("..");
		m_filterlist.push_back(tempfile);
	}
}

VOID CQGLPublishApp::CompareFileBriefList()
{
	FileDisplayMap& filemap = m_srcFileMap;
	// prepare filter list
	FilterList& filterlist = m_filterlist;
	// loop of source files, e.g. qgl_client/
	for( FileDisplayMap::iterator itr = filemap.begin(); itr != filemap.end(); ++itr )	
	{
		const String& srcFilePath = itr->first;
		FileProperty& fp = itr->second;
		String fullPath;
		fullPath.Format(_T("%s\\%s"), theApp.GetConfigure()->path_lastSource.c_str(), srcFilePath.c_str());
		// local data read
		TCHAR tempInt[30];
		ZeroMemory(tempInt, 30 * sizeof(TCHAR));
		GetPrivateProfileString(srcFilePath.c_str(), _T("size"), _T(""), tempInt, 30, STRING_OUTPUT_INI_FILE);
		UInt64 readsize = _ttoi64(tempInt);
		ZeroMemory(tempInt, 30 * sizeof(TCHAR));
		GetPrivateProfileString(srcFilePath.c_str(), _T("date"), _T(""), tempInt, 30, STRING_OUTPUT_INI_FILE);
		time_t readdate	= (time_t)_ttoi64(tempInt);
		// src data read
		struct stat st;
		StringA srcFilenameA(fullPath.c_str());
		stat(srcFilenameA.c_str(), &st);
		UInt64 srcsize	= st.st_size;
		UInt64 srcdate	= (UInt64)st.st_mtime;

		// check
		if( readdate==0 && readsize==0 )
		{
			fp.status = FILE_PROPERTY_STATUS_UNVERSIONED;
		}else
		{
			BOOL bNoChange = FALSE;
			if( readdate==srcdate && readsize==srcsize )
				bNoChange = TRUE;

			if( !bNoChange )
				fp.status = FILE_PROPERTY_STATUS_CHANGED;
			else
				fp.status = FILE_PROPERTY_STATUS_NOCHANGE;
		}
	}
}

VOID CQGLPublishApp::ExportFileBriefList()
{
	BOOL succeed = TRUE;
	// prepare filter list
	FileDataList filelist;
	filelist.clear();
	// loop of files
	if( !ExportFileBriefList(theApp.GetConfigure()->path_complete.c_str(), NULL, filelist) )
		succeed = FALSE;

	if( succeed )
	{
		// ini output
		String output;
		for( Index i = C_INDEX(0); i < filelist.size(); ++i )
		{
			FData &filedata = filelist[i];
			output.Format(_T("%lu"), filedata.size);
			WritePrivateProfileString(filedata.name.c_str(), _T("size"), output.c_str(), STRING_OUTPUT_INI_FILE);
			output.Format(_T("%x"), filedata.crc);
			WritePrivateProfileString(filedata.name.c_str(), _T("crc"), output.c_str(), STRING_OUTPUT_INI_FILE);
			
			output.Format(_T("%u"), filedata.filetime);
			WritePrivateProfileString(filedata.name.c_str(), _T("date"), output.c_str(), STRING_OUTPUT_INI_FILE);
		}
	}
}

BOOL CQGLPublishApp::ExportFileBriefList(PCSTRING folder, PCSTRING relative, FileDataList& filelist)
{
	BOOL succeed = TRUE;
	FilterList& filterlist = m_filterlist;
	String criteria;
	if( !relative )
		criteria.Format(_T("%s\\*.*"), folder);
	else
		criteria.Format(_T("%s\\%s\\*.*"), folder, relative);
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &FileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL fFinished = FALSE;
		String fullPath;
		String relativePath;
		while (!fFinished) 
		{ 
			if( !relative )
			{
				fullPath.Format(_T("%s\\%s"), folder, FileData.cFileName);
				relativePath = FileData.cFileName;
			}else
			{
				fullPath.Format(_T("%s\\%s\\%s"), folder, relative, FileData.cFileName);
				relativePath.Format(_T("%s\\%s"), relative, FileData.cFileName);
			}
			struct stat st;
			StringA fullPathA(fullPath.c_str());
			stat(fullPathA.c_str(), &st);
			// do filtering
			BOOL skipThis = FALSE;
			if( FileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN || !_tcsicmp(FileData.cFileName, _T(".")) || !_tcsicmp(FileData.cFileName, _T("..")) )
			{
				skipThis = TRUE;
			}else
			{
				if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if( !ExportFileBriefList(folder, relativePath.c_str(), filelist) )
						succeed = FALSE;
					skipThis = TRUE;
				}else
				{
					TCHAR tempInt[30];
					ZeroMemory(tempInt, 30 * sizeof(TCHAR));
					GetPrivateProfileString(relativePath.c_str(), _T("size"), _T(""), tempInt, 30, STRING_OUTPUT_INI_FILE);
					UInt64 readsize = _ttoi64(tempInt);
					ZeroMemory(tempInt, 30 * sizeof(TCHAR));
					GetPrivateProfileString(relativePath.c_str(), _T("date"), _T(""), tempInt, 30, STRING_OUTPUT_INI_FILE);
					UInt64 readdate = _ttoi64(tempInt);
					UInt64 filesize = st.st_size;
					UInt64 filedate = st.st_mtime;
					if( readdate==filedate && readsize!=0 && readsize==filesize )
					{
						skipThis = TRUE;
					}else
					{
						for( Index i = C_INDEX(0); i < filterlist.size(); ++i )
						{
							if( filterlist[i].CompareNoCase(relativePath.c_str())==0 )
							{
								skipThis = TRUE;
								break;
							}
						}
					}
				}
			}

			if( !skipThis )
			{
				// add in file list
				FData filedata;
				filedata.crc = 0;
				Reuben::Math::CRC32(fullPath.c_str(), filedata.crc);
				filedata.size = st.st_size;
				filedata.filetime = st.st_mtime;
				filedata.name = relativePath;
				filelist.push_back(filedata);
			}

			// find another
			if (!FindNextFile(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					fFinished = TRUE; 
				else
				{
					succeed = FALSE;
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GenerateFilelist Error (1) [Next file find error]"));
				}
			}
		}
		FindClose(hSearch);
	}
	return succeed;
}

VOID CQGLPublishApp::CheckVersionInfo()
{
	m_latestVersion = 0;
	m_prevVersion = 0;
	BOOL bFinish = FALSE;
	String criteria;
	criteria.Format(_T("%s\\*.*"), GetConfigure()->path_single.c_str());
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &FileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		while (!bFinish) 
		{ 
			if( !(FileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN || !_tcsicmp(FileData.cFileName, _T(".")) || !_tcsicmp(FileData.cFileName, _T(".."))) )
			{
				if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					UInt32 version = _ttoi(FileData.cFileName);
					// latest version
					if( version > m_latestVersion )
						m_latestVersion = version;
					// latest version before Target version (New or opened)
					if( version > m_prevVersion && version < GetTargetVersion() )
						m_prevVersion = version;
				}
			}
			// find another
			if (!FindNextFile(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinish = TRUE; 
				else
				{
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CheckVersionInfo Error (1) [Next file find error]"));
				}
			}
		}
		FindClose(hSearch);
	}
	// read info from Previous folder
	String prevFilename;
	prevFilename.Format(_T("%s\\%d\\%s"), theApp.GetConfigure()->path_single.c_str(), m_prevVersion, STRING_OUTPUT_VER_FILE);
	m_prevRevision = (UInt32)GetPrivateProfileInt(_T("INFO"), _T("REVISION"), 0, prevFilename.c_str());

	String revert;
	revert.Format(_T("%s\\%d_%d"), theApp.GetConfigure()->path_revertSingle.c_str(), GetTargetVersion(), GetPrevVersion());
	theApp.SetRevertFolder(revert.c_str());
}

VOID CQGLPublishApp::ClearChangeFileMap()
{
	m_archiveChange.clear();
}

BOOL CQGLPublishApp::AddChangeFileMap(PCSTRING filename)
{
	for( PatchChangeVector::iterator itr = m_archiveChange.begin(); itr != m_archiveChange.end(); )
	{
		PatchChange& change = *itr;
		int diff = change.filename.CompareNoCase(filename);
		if( diff < 0 )
			++itr;
		else if( diff > 0 )
		{
			PatchChange change;
			change.filename.Format(_T("%s"), filename);
			change.type = PATCH_CHANGE_ADD;
			for( FileDisplayMap::iterator itrM = m_fileFileMap.begin(); itrM != m_fileFileMap.end(); ++itrM )
			{
				const String& srcFilename = itrM->first;
				if( !srcFilename.CompareNoCase(change.filename.c_str()) )
				{
					change.type = PATCH_CHANGE_UPDATE;
					break;
				}
			}
			m_archiveChange.insert(itr, change);
			return TRUE;
		}else
			return FALSE;
	}

	PatchChange change;
	change.filename.Format(_T("%s"), filename);
	change.type = PATCH_CHANGE_ADD;
	for( FileDisplayMap::iterator itr = m_fileFileMap.begin(); itr != m_fileFileMap.end(); ++itr )
	{
		const String& srcFilename = itr->first;
		if( !srcFilename.CompareNoCase(change.filename.c_str()) )
		{
			change.type = PATCH_CHANGE_UPDATE;
			break;
		}
	}
	m_archiveChange.push_back(change);
	return TRUE;
}

BOOL CQGLPublishApp::DeleteChangeFileMap(PCSTRING filename)
{
	for( PatchChangeVector::iterator itr = m_archiveChange.begin(); itr != m_archiveChange.end(); ++itr )
	{
		PatchChange& change = *itr;
		if( change.filename.CompareNoCase(filename)==0 )
		{
			m_archiveChange.erase(itr);
			return TRUE;
		}
	}
	return FALSE;
}

VOID CQGLPublishApp::ClearChangeGdfMap()
{
	m_gdfChange.clear();
}

BOOL CQGLPublishApp::AddChangeGdfMap(PCSTRING filename)
{
	for( PatchChangeVector::iterator itr = m_gdfChange.begin(); itr != m_gdfChange.end(); )
	{
		PatchChange& change = *itr;
		int diff = change.filename.CompareNoCase(filename);
		if( diff < 0 )
			++itr;
		else if( diff > 0 )
		{
			PatchChange change;
			change.filename.Format(_T("%s"), filename);
			change.type = PATCH_CHANGE_ADD;
			for( FileDisplayMap::iterator itrM = m_packFileMap.begin(); itrM != m_packFileMap.end(); ++itrM )
			{
				const String& srcFilename = itrM->first;
				if( !srcFilename.CompareNoCase(change.filename.c_str()) )
				{
					change.type = PATCH_CHANGE_UPDATE;
					break;
				}
			}
			m_gdfChange.insert(itr, change);
			return TRUE;
		}else
			return FALSE;

	}

	PatchChange change;
	change.filename.Format(_T("%s"), filename);
	change.type = PATCH_CHANGE_ADD;
	for( FileDisplayMap::iterator itr = m_packFileMap.begin(); itr != m_packFileMap.end(); ++itr )
	{
		const String& srcFilename = itr->first;
		if( !srcFilename.CompareNoCase(change.filename.c_str()) )
		{
			change.type = PATCH_CHANGE_UPDATE;
			break;
		}
	}
	m_gdfChange.push_back(change);
	return TRUE;
}

BOOL CQGLPublishApp::DeleteChangeGdfMap(PCSTRING filename)
{
	for( PatchChangeVector::iterator itr = m_gdfChange.begin(); itr != m_gdfChange.end(); ++itr )
	{
		PatchChange& change = *itr;
		if( change.filename.CompareNoCase(filename)==0 )
		{
			m_gdfChange.erase(itr);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CQGLPublishApp::ExecutePatch()
{
	// copy to Single
	CONST PatchChangeVector& fileList = theApp.GetFileChangeList();
	CONST PatchChangeVector& gdfList = theApp.GetGdfChangeList();
	// ASSERT
	for( Index i = C_INDEX(0); i < fileList.size(); ++i )
	{
		CONST PatchChange& change = fileList[i];
		TRACE_ENSURE(change.type==PATCH_CHANGE_ADD);
	}
	for( Index i = C_INDEX(0); i < gdfList.size(); ++i )
	{
		CONST PatchChange& change = gdfList[i];
		TRACE_ENSURE(change.type==PATCH_CHANGE_ADD);
	}

	// Directory creation, File Copy, Gdf generation
	CreateDirectory(theApp.GetTargetFolder().c_str(), NULL);
	CreateDirectory(theApp.GetRevertFolder().c_str(), NULL);
	for( Index i = C_INDEX(0); i < fileList.size(); ++i )
	{
		CONST PatchChange& change = fileList[i];
		// Compare LastSource and CompletePatch files first.  If proven different, copy it.
		String src1, src2;
		src1.Format(_T("%s\\%s"), theApp.GetConfigure()->path_lastSource.c_str(), change.filename.c_str());
		src2.Format(_T("%s\\%s"), theApp.GetConfigure()->path_complete.c_str(), change.filename.c_str());
		if( !FileCompare(src1.c_str(), src2.c_str()) )
		{
			PatcherCopyFile(theApp.GetConfigure()->path_lastSource.c_str(), theApp.GetTargetFolder().c_str(), change.filename.c_str());
			String filename;
			filename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_complete.c_str(), change.filename.c_str());
			if( File::IsExists(filename.c_str()) )
				PatcherCopyFile(theApp.GetConfigure()->path_complete.c_str(), theApp.GetRevertFolder().c_str(), change.filename.c_str());
		}
	}
	GDFPackageMaker packageMaker;
	for( Index i = C_INDEX(0); i < gdfList.size(); ++i )
	{
		CONST PatchChange& change = gdfList[i];
		// Compare LastSource and CompletePatch files first.  If proven different, add it.
		String src1, src2;
		src1.Format(_T("%s\\%s"), theApp.GetConfigure()->path_lastSource.c_str(), change.filename.c_str());
		src2.Format(_T("%s\\%s"), theApp.GetConfigure()->path_complete.c_str(), change.filename.c_str());
		if( !FileCompare(src1.c_str(), src2.c_str()) )
			packageMaker.Add(change.filename.c_str(), theApp.GetConfigure()->path_lastSource.c_str());
	}
	String gdfFilePath;
	gdfFilePath.Format(_T("%s\\%s"), theApp.GetTargetFolder().c_str(), _T("_Data.gdf.patch"));
	BOOL bSucceed = packageMaker.ExecuteFileUpdate(gdfFilePath.c_str(), theApp.GetConfigure()->path_lastSource.c_str(), 0, FALSE);

	// Copy Essential Files for Patch
	theApp.PatcherCopyFile(theApp.GetConfigure()->path_template.c_str(), theApp.GetTargetFolder().c_str(), _T("UpdatePatchU.exe"));
	theApp.PatcherCopyFile(theApp.GetConfigure()->path_template.c_str(), theApp.GetRevertFolder().c_str(), _T("UpdatePatchU.exe"));

	// Major version number read
	String strMajorSupVer;
	GetConfigure()->GetMajorVersion(theApp.GetPrevVersion(), strMajorSupVer);
	// existing version CRC (acts as random number accompanied with version number)
	UInt32 prevVerCRC	= GetConfigure()->GetVersionCRC(theApp.GetPrevVersion());
	UInt32 curVerCRC	= GetConfigure()->GetVersionCRC(theApp.GetTargetVersion());
	if( !curVerCRC )
	{
		Reuben::Math::CRC32(GetConfigure()->path_ftp_filelist_file.c_str(), curVerCRC);
		if( !curVerCRC )
			curVerCRC = 1;
		GetConfigure()->SaveVersionCRC(theApp.GetTargetVersion(), curVerCRC);
	}

	// Wait to Generate SVN version info
	String templateFilename, replacedFilename;
	String execCommand;
	templateFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_template.c_str(), _T("QGLPatchTemplate_CHS.nsi"));
	replacedFilename.Format(_T("%s\\%s"), theApp.GetTargetFolder().c_str(), _T("QGLPatch_CHS.nsi"));
	execCommand.Format(_T("\"%s\" /V1 \"%s\\%s\""),
		theApp.GetConfigure()->path_makensis_file.c_str(),
		theApp.GetTargetFolder().c_str(),
		_T("QGLPatch_CHS.nsi")
		);
	PatcherNSISGenerate(
		templateFilename.c_str(), replacedFilename.c_str(), 
		theApp.GetMajorVersion().c_str(), theApp.GetTargetVersion(), theApp.GetTargetRevision(), curVerCRC,
		strMajorSupVer.c_str(), theApp.GetPrevVersion(), theApp.GetPrevRevision(), prevVerCRC,
		execCommand.c_str(), FALSE
		);

	templateFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_template.c_str(), _T("QGLPatchTemplate_CHS.nsi"));
	replacedFilename.Format(_T("%s\\%s"), theApp.GetRevertFolder().c_str(), _T("QGLPatch_CHS.nsi"));
	execCommand.Format(_T("\"%s\" /V1 \"%s\\%s\""),
		theApp.GetConfigure()->path_makensis_file.c_str(),
		theApp.GetRevertFolder().c_str(),
		_T("QGLPatch_CHS.nsi")
		);
	PatcherNSISGenerate(
		templateFilename.c_str(), replacedFilename.c_str(), 
		strMajorSupVer.c_str(), theApp.GetPrevVersion(), theApp.GetPrevRevision(), prevVerCRC,
		theApp.GetMajorVersion().c_str(), theApp.GetTargetVersion(), theApp.GetTargetRevision(), curVerCRC,
		execCommand.c_str(), TRUE
		);

	// everything all right, copy to CompletePatch
	CreateDirectory(theApp.GetConfigure()->path_complete.c_str(), NULL);
	for( Index i = C_INDEX(0); i < fileList.size(); ++i )
	{
		CONST PatchChange& change = fileList[i];
		PatcherCopyFile(theApp.GetConfigure()->path_lastSource.c_str(), theApp.GetConfigure()->path_complete.c_str(), change.filename.c_str());
	}
	// Compare LastSource and CompletePatch files first.  If proven different, add it.
	for( Index i = C_INDEX(0); i < gdfList.size(); ++i )
	{
		CONST PatchChange& change = gdfList[i];
		PatcherCopyFile(theApp.GetConfigure()->path_lastSource.c_str(), theApp.GetConfigure()->path_complete.c_str(), change.filename.c_str());
	}
	// write the version and revision to PatchVer.ini
	WritePatchVer();

	// FTP folder management
	UInt32 crc, size;
	struct _stat st;
	// Copy product to FTP folder, write size, crc to Filelist for comparison, and write to update table in Summary
	String patchFilename;
	patchFilename.Format(
		_T("qgl_%s.%d.%d_%s.%d.%d_patch.exe"), strMajorSupVer.c_str(),
		GetPrevRevision(), GetPrevVersion(), theApp.GetMajorVersion().c_str(), GetTargetRevision(), GetTargetVersion()
		);
	PatcherCopyFile(theApp.GetConfigure()->path_product.c_str(), theApp.GetConfigure()->path_ftp.c_str(), patchFilename.c_str());
	String fullPatchFilename;
	fullPatchFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_product.c_str(), patchFilename.c_str());
	_tstat(fullPatchFilename.c_str(), &st);
	size = st.st_size;
	crc = 0;
	if( size )
		Reuben::Math::CRC32(fullPatchFilename.c_str(), crc);
	GetConfigureFilelist()->SavePatch(patchFilename.c_str(), size, crc, prevVerCRC);
	GetConfigureSummary()->SavePatch(GetPrevVersion(), patchFilename.c_str());
	GetConfigureSummary()->SaveVersion(GetTargetVersion(), curVerCRC);
	GetConfigure()->SaveMajorVersion(GetTargetVersion(), GetMajorVersion().c_str());
	GetConfigure()->ExportMajorVersion(theApp.GetConfigure()->path_ftp_version_file.c_str());

	// Copy revert product to FTP folder, write size, crc, revert from and revert to info to revert file list for exhausive search use in revert
	String revertFilename;
	revertFilename.Format(
		_T("qgl_%s.%d.%d_%s.%d.%d_patch.exe"), theApp.GetMajorVersion().c_str(),
		GetTargetRevision(), GetTargetVersion(), strMajorSupVer.c_str(), GetPrevRevision(), GetPrevVersion()
		);
	PatcherCopyFile(theApp.GetConfigure()->path_revertProduct.c_str(), theApp.GetConfigure()->path_ftp.c_str(), revertFilename.c_str());
	String fullRevertFilename;
	fullRevertFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_revertProduct.c_str(), revertFilename.c_str());
	_tstat(fullRevertFilename.c_str(), &st);
	size = st.st_size;
	crc = 0;
	if( size )
		Reuben::Math::CRC32(fullRevertFilename.c_str(), crc);
	GetConfigureRevertlist()->SavePatch(revertFilename.c_str(), size, crc, GetTargetVersion(), GetPrevVersion(), curVerCRC, prevVerCRC);

	// finish
	return TRUE;
}

BOOL CQGLPublishApp::FileCompare(PCSTRING file1, PCSTRING file2)
{
	// same file path -> same
	if( !_tcsicmp(file1, file2) )
		return TRUE;

	if( !File::IsExists(file1) || !File::IsExists(file2) )
		return FALSE;

	BOOL bSame = TRUE;
	File fs1;	fs1.Open(file1, Reuben::Platform::File::MODE_READ_EXISTING);
	File fs2;	fs2.Open(file2, Reuben::Platform::File::MODE_READ_EXISTING);
	// either one not exist -> diff
	if( fs1.IsOpened() && fs2.IsOpened() )
	{
		// length diff -> diff
		if( fs1.GetLength()!=fs2.GetLength() )
			bSame = FALSE;

		char buf1[BUF_SIZE], buf2[BUF_SIZE];
		while( bSame )
		{
			size_t thisLength = (size_t)MIN((fs1.GetLength() - fs1.GetPosition()), BUF_SIZE);
			if( !thisLength )
				break;
			fs1.Read((BYTE*)buf1, (Count)thisLength);
			fs2.Read((BYTE*)buf2, (Count)thisLength);
			// any diff -> diff
			if( memcmp(buf1, buf2, thisLength) )
			{
				bSame = FALSE;
				break;
			}
		}
	}

	if( fs1.IsOpened() )	fs1.Close();
	if( fs2.IsOpened() )	fs2.Close();
	return bSame;
}

BOOL CQGLPublishApp::PatcherCopyFile(PCSTRING srcFolder, PCSTRING destFolder, PCSTRING filename)
{
	TRACE_ENSURE(srcFolder);
	TRACE_ENSURE(destFolder);
	TRACE_ENSURE(filename);

// Please create directory if filename contains directory
	String tempFilename(filename);
	String createFolder;
	size_t slashPos = tempFilename.find_first_of(_T("\\"), 0);
	while( slashPos!=String::npos )
	{
		createFolder.Format(_T("%s\\%s"), destFolder, tempFilename.Left((int)slashPos).c_str());
		CreateDirectory(createFolder.c_str(), NULL);
		slashPos = tempFilename.find_first_of(_T("\\"), slashPos+1);
	}

	String src, dest;
	src.Format(_T("%s\\%s"), srcFolder, filename);
	dest.Format(_T("%s\\%s"), destFolder, filename);
	if( !CopyFile(src.c_str(), dest.c_str(), FALSE) )
	{
		TRACE_WARNDTL_3(GLOBAL_LOGGER,
			_T("PatcherCopyFile Warning (1) [copy error] srcFolder=%s, destFolder=%s, filename=%s"),
			srcFolder, destFolder, filename);
		return FALSE;
	}
	return TRUE;
}

BOOL CQGLPublishApp::PatcherCreateProcess(PCSTRING execCommand)
{
	TRACE_ENSURE(execCommand);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	TCHAR execParam[MAX_PATH];
	DWORD retValue;

	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory(execParam, MAX_PATH * sizeof(TCHAR));
	_tcscpy(execParam, execCommand);
	if( !CreateProcess(NULL, execParam, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) ) 
		return FALSE;
	// Wait until child process exits.
	retValue = WaitForSingleObject( pi.hProcess, INFINITE );
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return TRUE;
}

VOID CQGLPublishApp::NewPublishVersion()
{
	ClearChangeFileMap();
	ClearChangeGdfMap();

	CheckVersionInfo();
	SetTargetVersion(GetLatestVersion()+1, 0);
	CheckVersionInfo();
	m_config.GetMajorVersion(GetTargetVersion(), m_majorVersion);
	String target;
	target.Format(_T("%s\\%i"), GetConfigure()->path_single.c_str(), GetTargetVersion());
	SetTargetFolder(target.c_str());

	String gdfFilename;
	gdfFilename.Format(_T("%s\\_Data.gdf.patch"), theApp.GetTargetFolder().c_str());
	SetSelectedPackFile(gdfFilename.c_str());
	UpdateFileFolderList();
	UpdatePackFolderList();
}

VOID CQGLPublishApp::ReadPatchVer(UInt32 version, UInt32& outRevision, UInt32& outSupportVersion, UInt32& outSupportRevision)
{
	String iniFilename;
	iniFilename.Format(_T("%s\\%i\\%s"), GetConfigure()->path_single.c_str(), version, STRING_OUTPUT_VER_FILE);
	outRevision = GetPrivateProfileInt(_T("INFO"), _T("REVISION"), 0, iniFilename.c_str());
	outSupportVersion = GetPrivateProfileInt(_T("INFO"), _T("SUPPORT_VERSION"), 0, iniFilename.c_str());
	outSupportRevision = GetPrivateProfileInt(_T("INFO"), _T("SUPPORT_REVISION"), 0, iniFilename.c_str());
}

BOOL CQGLPublishApp::WritePatchVer(UInt32 version, UInt32 revision, UInt32 supVersion, UInt32 supRevision, PCSTRING folder)
{
	String strVer, strRev, strSupVer, strSupRev;
	String iniFilename;
	strVer.Format(_T("%d"), version);
	strRev.Format(_T("%d"), revision);
	strSupVer.Format(_T("%d"), supVersion);
	strSupRev.Format(_T("%d"), supRevision);
	iniFilename.Format(_T("%s\\%s"), folder, STRING_OUTPUT_VER_FILE);
	return (
		WritePrivateProfileString(_T("INFO"), _T("VERSION"), strVer.c_str(), iniFilename.c_str()) &&
		WritePrivateProfileString(_T("INFO"), _T("REVISION"), strRev.c_str(), iniFilename.c_str()) &&
		WritePrivateProfileString(_T("INFO"), _T("SUPPORT_VERSION"), strSupVer.c_str(), iniFilename.c_str()) &&
		WritePrivateProfileString(_T("INFO"), _T("SUPPORT_REVISION"), strSupRev.c_str(), iniFilename.c_str())
		);
}

BOOL CQGLPublishApp::WritePatchVer()
{
	String iniFilename;
	iniFilename.Format(_T("%s\\%s"), GetTargetFolder().c_str(), STRING_OUTPUT_VER_FILE);
	return WritePatchVer(GetTargetVersion(), GetTargetRevision(), GetPrevVersion(), GetPrevRevision(), GetTargetFolder().c_str());
}

BOOL CQGLPublishApp::ExecuteNormalMerge(UInt32 mergeFrom, UInt32 mergeTo)
{
	if( mergeFrom >= mergeTo )
		return FALSE;
	return NormalMergePatch(mergeFrom, mergeTo, mergeTo);
}

BOOL CQGLPublishApp::NormalMergePatch(UInt32 mergeFrom, UInt32 mergeTo, UInt32 finalMergeTo)
{
	UInt32 mergeFromRev, mergeFromSupportVer, mergeFromSupportRev;
	ReadPatchVer(mergeFrom, mergeFromRev, mergeFromSupportVer, mergeFromSupportRev);
	UInt32 mergeToRev, mergeToSupportVer, mergeToSupportRev;
	ReadPatchVer(mergeTo, mergeToRev, mergeToSupportVer, mergeToSupportRev);
	UInt32 prevVerCRC	= GetConfigure()->GetVersionCRC(mergeFromSupportVer);
	UInt32 curVerCRC	= GetConfigure()->GetVersionCRC(mergeTo);

	String srcTemp, destPath;
	String templateFilename, replacedFilename;
	String strMajorSupVer, strMajorCurVer;
	String execCommand;
	BOOL bSucceed;
	GetConfigure()->GetMajorVersion(mergeFromSupportVer, strMajorSupVer);
	GetConfigure()->GetMajorVersion(mergeTo, strMajorCurVer);

	// STEP 1: Merge Patch
	String randomPath;
	randomPath.Format(_T("%s\\%d_%d"), theApp.GetConfigure()->path_batch.c_str(), mergeFromSupportVer, mergeTo);
	CreateDirectory(randomPath.c_str(), NULL);

	// File Copy and GDF Merge
	bSucceed = TRUE;
	for( Index i = mergeFrom; i <= mergeTo; ++i )
	{
		String srcTemp;
		srcTemp.Format(_T("%s\\%d"), theApp.GetConfigure()->path_single.c_str(), i);
		if( File::IsExists(srcTemp.c_str()) )
		{
			// copy all files from src to dest except *.gdf
			CopyDirectory_Filter_GDF_PATCH(srcTemp.c_str(), randomPath.c_str());
			// merge *.gdf if exists
			Merge_GDF_PATCH(srcTemp.c_str(), randomPath.c_str());
		}else
		{
			TRACE_WARNDTL_1(GLOBAL_LOGGER, 
				_T("CQGLPublishApp::ExecuteIntegrateMerge Warning (1) [folder not found] src=%s"),
				srcTemp.c_str());
			bSucceed = FALSE;
			break;
		}
	}

	// Wait to Generate SVN version info
	templateFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_template.c_str(), _T("QGLPatchTemplate_CHS.nsi"));
	replacedFilename.Format(_T("%s\\%s"), randomPath.c_str(), _T("QGLPatch_CHS.nsi"));
	execCommand.Format(_T("\"%s\" /V1 \"%s\\%s\""),
		theApp.GetConfigure()->path_makensis_file.c_str(),
		randomPath.c_str(),
		_T("QGLPatch_CHS.nsi")
		);
	PatcherNSISGenerate(
		templateFilename.c_str(), replacedFilename.c_str(), 
		strMajorCurVer.c_str(), finalMergeTo, mergeToRev, curVerCRC,
		strMajorSupVer.c_str(), mergeFromSupportVer, mergeFromSupportRev, prevVerCRC,
		execCommand.c_str(), FALSE
		);
	WritePatchVer(mergeTo, mergeToRev, mergeFromSupportVer, mergeFromSupportRev, randomPath.c_str());

	// STEP 2: Merge Revert
	String revertPath;
	revertPath.Format(_T("%s\\%d_%d"), theApp.GetConfigure()->path_revertSingle.c_str(), mergeTo, mergeFromSupportVer);
	CreateDirectory(revertPath.c_str(), NULL);

	// File Copy and GDF Merge
	bSucceed = TRUE;
	for( Index i = mergeTo; i >= mergeFrom; --i )
	{
		UInt32 curRev, prevVer, prevRev;
		ReadPatchVer(i, curRev, prevVer, prevRev);

		String srcTemp;
		srcTemp.Format(_T("%s\\%d_%d"), theApp.GetConfigure()->path_revertSingle.c_str(), i, prevVer);
		if( File::IsExists(srcTemp.c_str()) )
		{
			// copy all files from src to dest except *.gdf
			CopyDirectory_Filter_GDF_PATCH(srcTemp.c_str(), revertPath.c_str());
			// merge *.gdf if exists
			Merge_GDF_PATCH(srcTemp.c_str(), revertPath.c_str());
		}else
		{
			TRACE_WARNDTL_1(GLOBAL_LOGGER, 
				_T("CQGLPublishApp::ExecuteIntegrateMerge Warning (1) [folder not found] src=%s"),
				srcTemp.c_str());
			bSucceed = FALSE;
			break;
		}
	}

	// Wait to Generate SVN version info
	templateFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_template.c_str(), _T("QGLPatchTemplate_CHS.nsi"));
	replacedFilename.Format(_T("%s\\%s"), revertPath.c_str(), _T("QGLPatch_CHS.nsi"));
	execCommand.Format(_T("\"%s\" /V1 \"%s\\%s\""),
		theApp.GetConfigure()->path_makensis_file.c_str(),
		revertPath.c_str(),
		_T("QGLPatch_CHS.nsi")
		);
	PatcherNSISGenerate(
		templateFilename.c_str(), replacedFilename.c_str(), 
		strMajorSupVer.c_str(), mergeFromSupportVer, mergeFromSupportRev, prevVerCRC,
		strMajorCurVer.c_str(), finalMergeTo, mergeToRev, curVerCRC,
		execCommand.c_str(), TRUE
		);

	return TRUE;
}

BOOL CQGLPublishApp::ExecuteIntegrateMerge(UInt32 mergeFrom, UInt32 mergeTo, UInt32 finalVer)
{
	if( mergeFrom >= mergeTo || mergeFrom > finalVer )
		return FALSE;

	UInt32 mergeFromRev, mergeFromSupVer, mergeFromSupRev;
	ReadPatchVer(mergeFrom, mergeFromRev, mergeFromSupVer, mergeFromSupRev);
	UInt32 mergeToRev, mergeToSupVer, mergeToSupRev;
	ReadPatchVer(mergeTo, mergeToRev, mergeToSupVer, mergeToSupRev);

	BOOL bSucceed = NormalMergePatch(mergeFrom, mergeTo, finalVer);

	// move merged versions to random path in Replace folder
	time_t now;
	time(&now);
	tm* ntime = localtime(&now);
	String randomReplacePath;
	String revertReplacePath;
	randomReplacePath.Format(_T("%s\\%04d%02d%02d_%02d%02d%02d"),
		theApp.GetConfigure()->path_replace.c_str(),
		ntime->tm_year + 1900,
		ntime->tm_mon + 1,
		ntime->tm_yday,
		ntime->tm_hour,
		ntime->tm_min,
		ntime->tm_sec
		);
	revertReplacePath.Format(_T("%s\\%04d%02d%02d_%02d%02d%02d_Revert"),
		theApp.GetConfigure()->path_replace.c_str(),
		ntime->tm_year + 1900,
		ntime->tm_mon + 1,
		ntime->tm_yday,
		ntime->tm_hour,
		ntime->tm_min,
		ntime->tm_sec
		);
	// move before-merge "Revert" folder to Replace revert folder
	CreateDirectory(theApp.GetConfigure()->path_replace.c_str(), NULL);
	CreateDirectory(revertReplacePath.c_str(), NULL);
	for( UInt32 curVer = mergeTo; curVer >= mergeFrom; --curVer )
	{
		UInt32 curRev, prevVer, prevRev;
		ReadPatchVer(curVer, curRev, prevVer, prevRev);

		String srcDir;
		srcDir.Format(_T("%s\\%d_%d"), theApp.GetConfigure()->path_revertSingle, curVer, prevVer);
		String curPath;
		curPath.Format(_T("%s\\%d_%d"), revertReplacePath.c_str(), curVer, prevVer);
		if( bSucceed )
			bSucceed = bSucceed && CopyDirectory(srcDir.c_str(), curPath.c_str());
		if( !bSucceed )
			TRACE_ERRORDTL_2(
			GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (1) [copy dir err] srcDir=%s, curDir=%s"),
			srcDir.c_str(), curPath.c_str()
			);
	}
	for( UInt32 curVer = mergeTo; curVer >= mergeFrom; --curVer )
	{
		UInt32 curRev, prevVer, prevRev;
		ReadPatchVer(curVer, curRev, prevVer, prevRev);

		String delDir;
		delDir.Format(_T("%s\\%d_%d"), theApp.GetConfigure()->path_revertSingle, curVer, prevVer);
		if( bSucceed )
			bSucceed = bSucceed && DeleteDirectory(delDir.c_str());
		if( !bSucceed )
			TRACE_ERRORDTL_1(
			GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (2) [del dir err] delDir=%s"),
			delDir.c_str()
			);
	}
	// move before-merge "Single" folder to Replace random folder
	CreateDirectory(theApp.GetConfigure()->path_replace.c_str(), NULL);
	CreateDirectory(randomReplacePath.c_str(), NULL);
	for( UInt32 curVer = mergeFrom; curVer <= mergeTo; ++curVer )
	{
		String srcDir;
		srcDir.Format(_T("%s\\%d"), theApp.GetConfigure()->path_single, curVer);
		String curPath;
		curPath.Format(_T("%s\\%d"), randomReplacePath.c_str(), curVer);
		if( bSucceed )
			bSucceed = bSucceed && CopyDirectory(srcDir.c_str(), curPath.c_str());
		if( !bSucceed )
			TRACE_ERRORDTL_2(
			GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (1) [copy dir err] srcDir=%s, curDir=%s"),
			srcDir.c_str(), curPath.c_str()
			);
	}
	for( UInt32 curVer = mergeFrom; curVer <= mergeTo; ++curVer )
	{
		String delDir;
		delDir.Format(_T("%s\\%d"), theApp.GetConfigure()->path_single, curVer);
		if( bSucceed )
			bSucceed = bSucceed && DeleteDirectory(delDir.c_str());
		if( !bSucceed )
			TRACE_ERRORDTL_1(
			GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (2) [del dir err] delDir=%s"),
			delDir.c_str()
			);
	}
	// move "Batch" to "Single"
	String newPath;
	newPath.Format(_T("%s\\%d"),
		theApp.GetConfigure()->path_single.c_str(),
		finalVer
		);
	String destDir;
	destDir.Format(_T("%s\\%d_%d"),
		theApp.GetConfigure()->path_batch.c_str(),
		mergeFromSupVer,
		mergeTo
		);
	String revertSrcPath;
	revertSrcPath.Format(_T("%s\\%d_%d"),
		theApp.GetConfigure()->path_revertSingle.c_str(),
		mergeTo,
		mergeFromSupVer
		);
	String revertDestPath;
	revertDestPath.Format(_T("%s\\%d_%d"),
		theApp.GetConfigure()->path_revertSingle.c_str(),
		finalVer,
		mergeFromSupVer
		);
	// patch move from Temp to Single
	if( bSucceed )
		bSucceed = bSucceed && CopyDirectory(destDir.c_str(), newPath.c_str());
	if( !bSucceed )
		TRACE_ERRORDTL_2(
		GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (3) [copy dir err] destDir=%s, targetDir=%s"),
		destDir.c_str(), newPath.c_str()
		);
	if( bSucceed )
		bSucceed = bSucceed && DeleteDirectory(destDir.c_str());
	if( !bSucceed )
		TRACE_ERRORDTL_1(
		GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (4) [del dir err] destDir=%s"),
		destDir.c_str()
		);
	// patch move from Revert Temp to Revert
	if( bSucceed )
		bSucceed = bSucceed && CopyDirectory(revertSrcPath.c_str(), revertDestPath.c_str());
	if( !bSucceed )
		TRACE_ERRORDTL_2(
		GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (3) [copy dir err] revertSrcPath=%s, revertDestPath=%s"),
		revertSrcPath.c_str(), revertDestPath.c_str()
		);
	if( bSucceed )
		bSucceed = bSucceed && DeleteDirectory(revertSrcPath.c_str());
	if( !bSucceed )
		TRACE_ERRORDTL_1(
		GLOBAL_LOGGER, _T("CQGLPublishApp::ExecuteIntegrateMerge Error (4) [del dir err] revertSrcPath=%s"),
		revertSrcPath.c_str()
		);

	WritePatchVer(finalVer, mergeToRev, mergeFromSupVer, mergeFromSupRev, newPath.c_str());

	// FTP folder management
	String strMajorSupVer, strMajorCurVer;
	UInt32 prevVerCRC, curVerCRC;
	GetConfigure()->GetMajorVersion(mergeFromSupVer, strMajorSupVer);
	GetConfigure()->GetMajorVersion(mergeTo, strMajorCurVer);
	prevVerCRC = GetConfigure()->GetVersionCRC(mergeFromSupVer);
	curVerCRC = GetConfigure()->GetVersionCRC(mergeTo);

	UInt32 crc, size;
	struct _stat st;
	// Copy product to FTP folder, write size, crc to Filelist for comparison, and write to update table in Summary
	String patchFilename;
	patchFilename.Format(
		_T("qgl_%s.%d.%d_%s.%d.%d_patch.exe"), strMajorSupVer.c_str(),
		mergeFromSupRev, mergeFromSupVer, strMajorCurVer.c_str(), mergeToRev, finalVer
		);
	TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("move patch filename=%s"), patchFilename.c_str());
	PatcherCopyFile(theApp.GetConfigure()->path_product.c_str(), theApp.GetConfigure()->path_ftp.c_str(), patchFilename.c_str());
	String fullPatchFilename;
	fullPatchFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_product.c_str(), patchFilename.c_str());
	_tstat(fullPatchFilename.c_str(), &st);
	size = st.st_size;
	crc = 0;
	if( size )
		Reuben::Math::CRC32(fullPatchFilename.c_str(), crc);
	GetConfigureFilelist()->SavePatch(patchFilename.c_str(), size, crc, prevVerCRC);
	GetConfigureSummary()->SavePatch(mergeFromSupVer, patchFilename.c_str());
	GetConfigureSummary()->SaveVersion(finalVer, curVerCRC);
	GetConfigure()->SaveMajorVersion(finalVer, strMajorCurVer.c_str());
	GetConfigure()->ExportMajorVersion(theApp.GetConfigure()->path_ftp_version_file.c_str());

	// Copy revert product to FTP folder, write size, crc, revert from and revert to info to revert file list for exhausive search use in revert
	String revertFilename;
	revertFilename.Format(
		_T("qgl_%s.%d.%d_%s.%d.%d_patch.exe"), strMajorCurVer.c_str(),
		mergeToRev, finalVer, strMajorSupVer.c_str(), mergeFromSupRev, mergeFromSupVer
		);
	TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("move revert filename=%s"), revertFilename.c_str());
	PatcherCopyFile(theApp.GetConfigure()->path_revertProduct.c_str(), theApp.GetConfigure()->path_ftp.c_str(), revertFilename.c_str());
	String fullRevertFilename;
	fullRevertFilename.Format(_T("%s\\%s"), theApp.GetConfigure()->path_revertProduct.c_str(), revertFilename.c_str());
	_tstat(fullRevertFilename.c_str(), &st);
	size = st.st_size;
	crc = 0;
	if( size )
		Reuben::Math::CRC32(fullRevertFilename.c_str(), crc);
	GetConfigureRevertlist()->SavePatch(revertFilename.c_str(), size, crc, finalVer, mergeFromSupVer, curVerCRC, prevVerCRC);

	// update version CRC configuration
	for( UInt i = mergeFrom; i <= mergeTo; ++i )
		GetConfigure()->ResetVersionCRC(i);
	GetConfigure()->SaveVersionCRC(finalVer, curVerCRC);

	return bSucceed;
}

BOOL CQGLPublishApp::CopyDirectory_Filter_GDF_PATCH(PCSTRING srcDir, PCSTRING destDir)
{
	CreateDirectory(destDir, NULL);
	String criteria;
	criteria.Format(_T("%s\\*.*"), srcDir);
	WIN32_FIND_DATA fileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &fileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL bFinish = FALSE;
		while (!bFinish) 
		{
			// filter useless files
			if( _tcsicmp(fileData.cFileName, _T("."))!=0 &&
				_tcsicmp(fileData.cFileName, _T(".."))!=0 &&
				!(fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				)
			{
				if( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	// recursive if directory
				{
					String srcPath, destPath;
					srcPath.Format(_T("%s\\%s"), srcDir, fileData.cFileName);
					destPath.Format(_T("%s\\%s"), destDir, fileData.cFileName);
					CopyDirectory_Filter_GDF_PATCH(srcPath.c_str(), destPath.c_str());
				}else
				{
					String strFilename(fileData.cFileName);
					if( strFilename.GetLength() < 10 || 
						strFilename.Right(10).CompareNoCase(_T(".gdf.patch"))!=0 )
					{
						PatcherCopyFile(srcDir, destDir, fileData.cFileName);
					}
				}
			}
			// find another
			if (!FindNextFile(hSearch, &fileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinish = TRUE; 
			}
		}
		FindClose(hSearch);
	}
	return TRUE;
}

BOOL CQGLPublishApp::Merge_GDF_PATCH(PCSTRING srcDir, PCSTRING destDir)
{
	String criteria;
	criteria.Format(_T("%s\\*.gdf.patch"), srcDir);
	WIN32_FIND_DATA fileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &fileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL bFinish = FALSE;
		while (!bFinish) 
		{
			// filter useless files
			if( _tcsicmp(fileData.cFileName, _T("."))!=0 &&
				_tcsicmp(fileData.cFileName, _T(".."))!=0 &&
				!(fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				)
			{
				if( !(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					String srcPath, destPath;
					srcPath.Format(_T("%s\\%s"), srcDir, fileData.cFileName);
					destPath.Format(_T("%s\\%s"), destDir, fileData.cFileName);
					GDFPackageMaker packageMaker;
					packageMaker.ExecuteMerge(destPath.c_str(), srcPath.c_str(), FALSE);
				}
			}
			// find another
			if (!FindNextFile(hSearch, &fileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinish = TRUE; 
			}
		}
		FindClose(hSearch);
	}
	return TRUE;
}

BOOL CQGLPublishApp::CopyDirectory(PCSTRING srcDir, PCSTRING destDir)
{
	CreateDirectory(destDir, NULL);
	String criteria;
	criteria.Format(_T("%s\\*.*"), srcDir);
	WIN32_FIND_DATA fileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &fileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL bFinish = FALSE;
		while (!bFinish) 
		{
			// filter useless files
			if( _tcsicmp(fileData.cFileName, _T("."))!=0 &&
				_tcsicmp(fileData.cFileName, _T(".."))!=0 &&
				!(fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				)
			{
				if( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	// recursive if directory
				{
					String srcPath, destPath;
					srcPath.Format(_T("%s\\%s"), srcDir, fileData.cFileName);
					destPath.Format(_T("%s\\%s"), destDir, fileData.cFileName);
					CopyDirectory(srcPath.c_str(), destPath.c_str());
				}else
				{
					PatcherCopyFile(srcDir, destDir, fileData.cFileName);
				}
			}
			// find another
			if (!FindNextFile(hSearch, &fileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinish = TRUE; 
			}
		}
		FindClose(hSearch);
	}
	return TRUE;
}

BOOL CQGLPublishApp::DeleteDirectory(PCSTRING dir)
{
	String criteria;
	criteria.Format(_T("%s\\*.*"), dir);
	WIN32_FIND_DATA fileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &fileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL bFinish = FALSE;
		while (!bFinish) 
		{
			// filter useless files
			if( _tcsicmp(fileData.cFileName, _T("."))!=0 &&
				_tcsicmp(fileData.cFileName, _T(".."))!=0
				)
			{
				if( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	// recursive if directory
				{
					String childDir;
					childDir.Format(_T("%s\\%s"), dir, fileData.cFileName);
					DeleteDirectory(childDir.c_str());
				}else
				{
					String delFile;
					delFile.Format(_T("%s\\%s"), dir, fileData.cFileName);
					DeleteFile(delFile.c_str());
				}
			}
			// find another
			if (!FindNextFile(hSearch, &fileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinish = TRUE; 
			}
		}
		FindClose(hSearch);
	}
	RemoveDirectory(dir);
	return TRUE;
}

VOID CQGLPublishApp::SetRevertVersion(UInt32 revertFrom, UInt32 revertTo)
{
	m_strRevertDir.Format(_T("%s\\%d_%d"), theApp.GetConfigure()->path_revertSingle.c_str(), revertFrom, revertTo);
}

BOOL CQGLPublishApp::PatcherNSISGenerate(
	PCSTRING templateFilename, PCSTRING nsisFilename, 
	PCSTRING targetMajor, UInt32 targetVersion, UInt32 targetRevision, UInt32 curVerCRC,
	PCSTRING prevMajor, UInt32 prevVersion, UInt32 prevRevision, UInt32 prevVerCRC,
	PCSTRING execCommand, BOOL writeRevertFlag
	)
{
	// Wait to Generate SVN version info
	File f0;
	f0.Open(templateFilename, Reuben::Platform::File::MODE_READ_EXISTING);
	if( f0.IsOpened() )
	{
		// template file is theoretically a small file, less then 64KB, with no "\0" character inside.
		TRACE_ENSURE(f0.GetLength() < MAX_BUF_SIZE-1);
		char context[MAX_BUF_SIZE];
		f0.Read((BYTE*)context, (Count)f0.GetLength());
		context[f0.GetLength()] = 0;
		f0.Close();

		StringA strContext(context);
		StringA strMajorSupVerA(prevMajor);
		StringA strMajorCurVerA(targetMajor);

		StringA strCurVer, strSupVer, strCurRev, strSupRev, strSupCRC, strCurCRC;
		strCurVer.Format("%d", targetVersion);
		strCurRev.Format("%d", targetRevision);
		strSupVer.Format("%d", prevVersion);
		strSupRev.Format("%d", prevRevision);
		strSupCRC.Format("%x", prevVerCRC);
		strCurCRC.Format("%x", curVerCRC);
		strContext.Replace("$CURRENT_VERSION$", strCurVer.c_str());
		strContext.Replace("$CURRENT_REVISION$", strCurRev.c_str());
		strContext.Replace("$SUPPORT_VERSION$", strSupVer.c_str());
		strContext.Replace("$SUPPORT_REVISION$", strSupRev.c_str());
		strContext.Replace("$MAJOR_CURRENT_VERSION$", strMajorCurVerA.c_str());
		strContext.Replace("$MAJOR_SUPPORT_VERSION$", strMajorSupVerA.c_str());
		strContext.Replace("$REQUIRED_VERSION_CRC$", strSupCRC.c_str());
		strContext.Replace("$LATEST_VERSION_CRC$", strCurCRC.c_str());
		
		if( writeRevertFlag )
		{
			strContext.Replace("$REVERT_FLAG$", "-r");
			strContext.Replace("$OUTPUT_FOLDER$", "RevertPatch");
		}else
		{
			strContext.Replace("$REVERT_FLAG$", "");
			strContext.Replace("$OUTPUT_FOLDER$", "UpdatePatch");
		}

		f0.Open(nsisFilename, Reuben::Platform::File::MODE_WRITE);
		if( f0.IsOpened() )
		{
			f0.SetPosition(0, Reuben::Platform::File::MoveEnum::MOVE_BEGIN);
			f0.SetLength(0);
			f0.Write((BYTE*)strContext.c_str(), strContext.GetLength());
			f0.Close();
		}else
		{
			TRACE_WARNDTL_1(GLOBAL_LOGGER, _T("QGLPublishApp::ExecutePatch Warning (1) [nsis file unable to be opened] filename=%s"), nsisFilename);
			return FALSE;
		}
	}else
	{
		TRACE_WARNDTL_1(GLOBAL_LOGGER, _T("QGLPublishApp::ExecutePatch Warning (2) [template unable to be opened] filename=%s"), templateFilename);
		return FALSE;
	}

	// Wait to Generate NSIS
	if( !PatcherCreateProcess(execCommand) )
		return FALSE;

	return TRUE;
}

BOOL CQGLPublishApp::GenerateSVNRevision()
{
	String execCommand;
	execCommand.Format(_T("\"%s\" \"%s\" \"%s\" \"%s\""),
		theApp.GetConfigure()->path_subwcrev_file.c_str(), 
		theApp.GetConfigure()->path_lastSourceSvn.c_str(),
		CONFIGURATION_SVN_INI_TEMPLATE_FILE,
		CONFIGURATION_SVN_INI_FILE
		);
	PatcherCreateProcess(execCommand.c_str());
	return TRUE;
}
