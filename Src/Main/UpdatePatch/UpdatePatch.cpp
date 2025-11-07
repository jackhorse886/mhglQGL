// UpdatePatch.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "UpdatePatch.h"
#include "UpdatePatchDlg.h"
#include "GDFPackager/GDFManager.h"
#include "Reuben/Math/CRC.h"
#include "GDXFile.h"

#define LOCALIZATION_FILENAME	_T("patchmsg.csv")
#define CONFIGURATION_INI_PATCH_FILE		_T("Config\\patch.ini")
#define CONFIGURATION_INI_SYS_FILE			_T("Config\\patchcfg.ini")

#define SUMMARY_INI_FILE		_T("Config/summary.ini")
#define PATCHER_FILENAME		_T("QGLPatcherU.exe")
#define MASTER_FILENAME			_T("Data/Data.gdf")
#define VERSION_TABLE_FILENAME	_T("Config\\VersionTable.GDX")
#define REVERT_FOLDER			_T("Revert")

#define REQUIRED_FILE_1			_T("msvcp71.dll")
#define REQUIRED_FILE_2			_T("QGLPatcherU.exe")
#define FILENAME_GDFPATCH_CRITERIA _T(".\\_*.gdf.patch")
#define MAX_SIZE_FILE_FAT32		2147483648
#define SIZE_FILE_2GB			2097152000
#define SIZE_FILE_200MB			209715200

#define FILENAME_GDF_CRITERIA _T(".\\Data\\*.gdf")

// CUpdatePatchApp

BEGIN_MESSAGE_MAP(CUpdatePatchApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUpdatePatchApp 建構

CUpdatePatchApp::CUpdatePatchApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
	m_stringTable.Load(LOCALIZATION_FILENAME);
	argc = 0;
	argv = CommandLineToArgvW(GetCommandLine(), &argc);		// It seems to be working under NT series
	m_bUpgrade	= TRUE;
	m_uRequiredCRC	= 0;
	m_uCurrentCRC	= 0;
	InitLogger();
#ifdef TEST_GDX_CONFIG
	TestGDX();
	exit(0);
#endif
}

CUpdatePatchApp::~CUpdatePatchApp()
{
	CloseLogger();
}

void CUpdatePatchApp::InitLogger()
{
	Reuben::Utility::ConfigFile config(CONFIGURATION_INI_SYS_FILE);
	gGlobalLogger = SafeCreateObject1(Logger, _T("UpdatePatch"));
	gGlobalLogger->Open(config);

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void CUpdatePatchApp::CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

// 僅有的一個 CUpdatePatchApp 物件

CUpdatePatchApp theApp;


// CUpdatePatchApp 初始設定

BOOL CUpdatePatchApp::InitInstance()
{
	// 假如應用程式資訊清單指定使用 ComCtl32.dll 6.0 (含) 以後版本
	// 以啟用視覺化樣式，則 Windows XP 需要 InitCommonControls()。否則的話，
	// 任何視窗的建立將失敗。
	InitCommonControls();

	CWinApp::InitInstance();

	// 標準初始設定
	// 如果您不使用這些功能並且想減少最後完成的可執行檔大小，您可以從下列
	// 程式碼移除不需要的初始化常式，變更儲存設定值的登錄機碼
	// TODO: 您應該適度修改此字串 (例如，公司名稱或組織名稱)
	//SetRegistryKey(_T("本機 AppWizard 所產生的應用程式"));

	CUpdatePatchDlg dlg;
	m_pMainWnd = &dlg;

	// this is for manual update use or UPDATE.EXE update use

	BOOL succeed = TRUE;

	// required file checking, to see if it is AUTO-patching
	if( succeed && !(File::IsExists(REQUIRED_FILE_1) && File::IsExists(REQUIRED_FILE_2)) )
	{
		succeed = FALSE;
	}

	// Read configuration argument
	// On error, report message
	if( succeed && !ReadConfiguration() )
	{
		MessageBox(m_pMainWnd->m_hWnd, _T("read config error"), _T("err"), MB_OK);
		succeed = FALSE;
	}

	if( succeed && m_uLocalCRC!=0 )
	{
		// 2008/11/13
		// version CRC check Replaced (m_currentVersion >= m_targetVersion || m_currentVersion < m_supportFromVersion)
		if( m_bUpgrade && m_uRequiredCRC!=m_uLocalCRC )
		{
			// message selection:
			String msg;
			if( m_currentVersion==m_supportFromVersion )
				msg.Format(_T("Version revert must be done.  Please download a downgrade patch, and upgrade with newer patches."));
			else
				msg.Format(_T("version out of range, sup=%i cur=%i tar=%i"), m_supportFromVersion, m_currentVersion, m_targetVersion);
			MessageBox(m_pMainWnd->m_hWnd, msg.c_str(), _T("Error"), MB_OK);
			succeed = FALSE;
		}else if( !m_bUpgrade && m_uRequiredCRC!=m_uLocalCRC )
		{
			String msg;
			if( m_currentVersion==m_supportFromVersion )
				msg.Format(_T("Downgrade patch is not valid.  Please check if you have downloaded an appropriate downgrade patch."));
			else
				msg.Format(_T("Revert version out of range, sup=%d, cur=%d, tar=%i"), m_supportFromVersion, m_currentVersion, m_targetVersion);
			MessageBox(m_pMainWnd->m_hWnd, msg.c_str(), _T("Error"), MB_OK);
			succeed = FALSE;
		}
	}

	// Find the respective patch file
	// On error, report message (m_CurrentPatchname is full path composed here)
	// On error, version number does not change, the finished GDF files remains good status.
	if( succeed )
	{
		m_installerSize	= 0;
		m_installerCRC	= 0;
		// installer (.exe) file data read
		File pFile;
		pFile.Open(_T("_instexe.txt"), File::MODE_READ);
		if( pFile.IsOpened() )
		{
			if( pFile.GetLength() )
			{
				char* buf = SafeAllocate(char, (Reuben::System::Size)pFile.GetLength()+1);
				pFile.Read((BYTE*)buf, (Count)pFile.GetLength());
				buf[pFile.GetLength()] = 0;
				String installPath(buf);
				// crc
				Reuben::Math::CRC32(installPath.c_str(), m_installerCRC);
				// size
				struct _stat st;
				_tstat(installPath.c_str(), &st);
				m_installerSize = st.st_size;
			}
			pFile.Close();
		}
		// install by upgrade/download(revert)
		if( m_bUpgrade )
		{
			WIN32_FIND_DATA FileData; 
			HANDLE hSearch = FindFirstFile(FILENAME_GDFPATCH_CRITERIA, &FileData);
			if (hSearch != INVALID_HANDLE_VALUE)
			{
				BOOL fFinished = FALSE;

				while (!fFinished) 
				{ 
					// do something
					if( succeed && Reuben::Platform::File::IsExists(FileData.cFileName) )
					{
						String tempmaster, tempmaster2;
						tempmaster		= FileData.cFileName;
						tempmaster2		= tempmaster.Left((UInt32)_tcslen(FileData.cFileName) - (UInt32)_tcslen(_T(".patch")));
						tempmaster		= tempmaster2.Right(tempmaster2.GetLength() - 1);
						tempmaster2.Format(_T("Data\\%s"), tempmaster.c_str());
						String masterFilename	= tempmaster2;
						String patchFilename	= FileData.cFileName;
						if( IsRepatchRequired(masterFilename.c_str(), patchFilename.c_str()) )
						{
							int reply = IDRETRY;
							while( reply==IDRETRY )
							{
								if( IsRepatchFeasible(masterFilename.c_str()) )
								{
									GDFManager manager;
									if( !manager.Repatch(masterFilename.c_str()) )
									{
										MessageBox(m_pMainWnd->m_hWnd, _T("repatch fail"), _T("err"), MB_OK);
										succeed = FALSE;
									}
									break;
								}else
								{
									reply = MessageBox(m_pMainWnd->m_hWnd, _T("No enough space.  2GB disk space for temporary file is required for this update."), _T("warning"), MB_RETRYCANCEL);
									if( reply==IDCANCEL )
									{
										MessageBox(m_pMainWnd->m_hWnd, _T("Repatch aborted due to not enough spare space"), _T("err"), MB_OK);
										succeed = FALSE;
									}
								}
							}
						}
						// Generation of GDF revert file (containing header, hash and block table)
						String revertFilename;
						String processFilename(FileData.cFileName);
						if( !processFilename.Left(1).Compare(_T("_")) && !processFilename.Right(10).CompareNoCase(_T(".gdf.patch")) )
						{
							GetRevertFilename(processFilename.Mid(1, processFilename.GetLength()-11).c_str(), REVERT_FOLDER, m_targetVersion, m_currentVersion, revertFilename);
						}
						CreateDirectory(_T(".\\Revert"), NULL);
						GDFManager manager;
						if( !manager.Merge(masterFilename.c_str(), patchFilename.c_str(), TRUE, revertFilename.c_str()) )
						{
							MessageBox(m_pMainWnd->m_hWnd, _T("merge fail"), _T("err"), MB_OK);
							succeed = FALSE;
						}
					}else
					{
						MessageBox(m_pMainWnd->m_hWnd, _T("no target file"), _T("err"), MB_OK);
						succeed = FALSE;
					}

					// Delete patch file.
					// No error handling needed
					DeleteFile(FileData.cFileName);
					// find another
					if (!FindNextFile(hSearch, &FileData)) 
					{
						if (GetLastError() == ERROR_NO_MORE_FILES) 
							fFinished = TRUE; 
						else 
							MessageBox(m_pMainWnd->m_hWnd, _T("list search error"), _T("err"), MB_OK);
					}
				}
				FindClose(hSearch);
			}
			// Change version
			// On error, report message
			if( succeed && !ChangeToTargetVersion() )
			{
				MessageBox(m_pMainWnd->m_hWnd, _T("version cannot be changed"), _T("err"), MB_OK);
				succeed = FALSE;
			}
		}else		// Version revert
		{
			CGDXFile gdxFile;
			gdxFile.LoadFile(VERSION_TABLE_FILENAME);
			UInt32 revertTo = gdxFile.GetFrom(m_currentVersion);
			gdxFile.CloseFile();

			// GDF Data search
			WIN32_FIND_DATA FileData; 
			HANDLE hSearch = FindFirstFile(FILENAME_GDF_CRITERIA, &FileData);
			if (hSearch != INVALID_HANDLE_VALUE)
			{
				BOOL fFinished = FALSE;
				while (!fFinished) 
				{ 
					// do something
					String masterPath;
					masterPath.Format(_T("Data\\%s"), FileData.cFileName);
					if( succeed && Reuben::Platform::File::IsExists(masterPath.c_str()) )
					{
						String processPath(FileData.cFileName);
						String revertPath;
						GetRevertFilename(processPath.Left(processPath.GetLength()-4).c_str(), REVERT_FOLDER, m_currentVersion, revertTo, revertPath);

						GDFManager manager;
						if( !manager.Revert(masterPath.c_str(), revertPath.c_str()) )
						{
							MessageBox(m_pMainWnd->m_hWnd, _T("revert fail"), _T("err"), MB_OK);
							succeed = FALSE;
						}
					}else
					{
						MessageBox(m_pMainWnd->m_hWnd, _T("no target file"), _T("err"), MB_OK);
						succeed = FALSE;
					}
					// find another
					if (!FindNextFile(hSearch, &FileData)) 
					{
						if (GetLastError() == ERROR_NO_MORE_FILES) 
							fFinished = TRUE; 
						else 
							MessageBox(m_pMainWnd->m_hWnd, _T("list search error"), _T("err"), MB_OK);
					}
				}
				FindClose(hSearch);
			}
			// Change version
			// On error, report message
			if( succeed && !ChangeToRevertToVersion(revertTo, m_uCurrentCRC) )
			{
				MessageBox(m_pMainWnd->m_hWnd, _T("version cannot be changed"), _T("err"), MB_OK);
				succeed = FALSE;
			}
		}
	}

	// destroy app
	m_pMainWnd = NULL;
	return FALSE;
}

BOOL CUpdatePatchApp::ReadConfiguration()
{
	String param, tempvalue;
	for( Index i = C_INDEX(0); i < (UInt)argc; ++i )
	{
		param = argv[i];
		switch( param.GetAt(0) )
		{
		case '-':			// switch -v, -s, -c, -r...
			if( param.GetLength() > 1 )
			{
				switch( param.GetAt(1) )
				{
				case 's':
				case 'S':
					tempvalue = param.Right(param.GetLength()-2).Trim();
					m_supportFromVersion = _ttoi(tempvalue.c_str());
					break;
				case 'v':
				case 'V':
					tempvalue = param.Right(param.GetLength()-2).Trim();
					m_targetVersion = _ttoi(tempvalue.c_str());
					break;
				case 'r':
				case 'R':
					m_bUpgrade = FALSE;
					break;
				case 'c':
				case 'C':
					tempvalue = param.Right(param.GetLength()-2).Trim();
					_stscanf(tempvalue.c_str(), _T("%x"), &m_uRequiredCRC);
					break;
				case 'u':
				case 'U':
					tempvalue = param.Right(param.GetLength()-2).Trim();
					_stscanf(tempvalue.c_str(), _T("%x"), &m_uCurrentCRC);
				}
			}
			break;
		default:
			break;
		}
	}

	BOOL succeed = TRUE;
	if( m_targetVersion==0 )
	{
		MessageBox(m_pMainWnd->m_hWnd, _T("CUpdatePatchApp::ReadConfiguration"), _T("err target"), MB_OK);
		succeed = FALSE;
	}
	if( !ReadCurrentVersion() )
	{
		MessageBox(m_pMainWnd->m_hWnd, _T("CUpdatePatchApp::ReadConfiguration"), _T("err cur"), MB_OK);
		succeed = FALSE;
	}
	return succeed;
}

BOOL CUpdatePatchApp::ChangeToTargetVersion()
{
	CGDXFile gdxFile;
	gdxFile.LoadFile(VERSION_TABLE_FILENAME);
	gdxFile.AppendSave(m_currentVersion, m_targetVersion, m_installerCRC, m_installerSize);
	gdxFile.CloseFile();
	return WriteVersion(m_targetVersion, m_uCurrentCRC);
}

BOOL CUpdatePatchApp::ChangeToRevertToVersion(UInt32 version, UInt32 crc)
{
	return WriteVersion(version, crc);
}

BOOL CUpdatePatchApp::WriteVersion(UInt32 version, UInt32 crc)
{
	BOOL bSucceed = TRUE;
	String value;
	value.Format(_T("%d"), version);
	String strCRC;
	strCRC.Format(_T("%x"), crc);
	if( !WritePrivateProfileString(_T("APP"), _T("patcherversion"), value.c_str(), CONFIGURATION_INI_PATCH_FILE) )
	{
		MessageBox(m_pMainWnd->m_hWnd, _T("CUpdatePatchApp::ChangeToTargetVersion"), _T("err"), MB_OK);
		bSucceed = FALSE;
	}
	if( !WritePrivateProfileString(_T("APP"), _T("versioncrc"), strCRC.c_str(), CONFIGURATION_INI_PATCH_FILE) )
	{
		MessageBox(m_pMainWnd->m_hWnd, _T("CUpdatePatchApp::ChangeToTargetVersion"), _T("err"), MB_OK);
		bSucceed = FALSE;
	}
	return bSucceed;
}

BOOL CUpdatePatchApp::ReadCurrentVersion()
{
	// version number
	m_currentVersion = GetPrivateProfileInt(_T("APP"), _T("patcherversion"), 0, CONFIGURATION_INI_PATCH_FILE);
	// version crc
	TCHAR strcrc[10];
	GetPrivateProfileString(_T("APP") ,_T("versioncrc"), _T(""), strcrc, 10, CONFIGURATION_INI_PATCH_FILE);
	_stscanf(strcrc, _T("%x"), &m_uLocalCRC);

	if( m_currentVersion==0 )
		return FALSE;
	return TRUE;
}

BOOL CUpdatePatchApp::IsRepatchRequired(LPCTSTR masterFile, LPCTSTR mergeFile)
{
	// Idea by Alan Au, the author
	// Criteria 1: GDF A + GDF B > 1.9GB
	// Criteria 2: (GDF A) Block size utilization < 90%
	if( File::IsExists(masterFile) )
	{
		WIN32_FIND_DATA FileData; 
		HANDLE hSearch;
		UInt64 filesize1, filesize2;
		hSearch = FindFirstFile(masterFile, &FileData);
		filesize1 = ((UInt64)MAXDWORD+1) * FileData.nFileSizeHigh + FileData.nFileSizeLow;
		FindClose(hSearch);
		filesize2 = 0;
		if( File::IsExists(mergeFile) )
		{
			hSearch = FindFirstFile(mergeFile, &FileData);
			filesize2 = ((UInt64)MAXDWORD+1) * FileData.nFileSizeHigh + FileData.nFileSizeLow;
			FindClose(hSearch);
		}
		if( filesize1 + filesize2 > SIZE_FILE_2GB )	// 2.0G limitation, which suspects to have problem with FAT32
		{
			GDFManager manager;
			String tmpMasterFile = masterFile;
			manager.LoadPatchFile(tmpMasterFile.c_str(), GDFFILE_STATE_PATCH_READ);

			UInt32 numAllBlock, numUsedBlock, sizeAllBlock, sizeUsedBlock;
			manager.GetBlockStat(numAllBlock, numUsedBlock, sizeAllBlock, sizeUsedBlock);

			double a = sizeAllBlock;
			double b = sizeUsedBlock;
			double utilization = b / a;
			if( utilization < 0.9 )
				return TRUE;
			else
				return FALSE;
		}else
		{
			return FALSE;
		}
	}else
	{
		return FALSE;
	}
}

ULARGE_INTEGER CUpdatePatchApp::CheckCurrentDriveSpace()
{
	TCHAR appname[MAX_PATH];
	::GetModuleFileName(NULL, appname, MAX_PATH);
	TCHAR driveLetter[4];
	ZeroMemory(driveLetter, 4 * sizeof(TCHAR));
	memcpy(driveLetter, appname, 3 * sizeof(TCHAR));
	ULARGE_INTEGER retNum;

	ULARGE_INTEGER AvailableToCaller, Disk, Free;

	switch( GetDriveType(driveLetter) )
	{
	case DRIVE_FIXED:
	case DRIVE_REMOVABLE:
		if (GetDiskFreeSpaceEx(driveLetter,&AvailableToCaller, &Disk, &Free))
		{
			return Free;
		}else
		{
			retNum.QuadPart = 0;
			return retNum;
		}
		break;
	default:
		{
			retNum.QuadPart = 0;
			return retNum;
		}
	}
}

BOOL CUpdatePatchApp::IsRepatchFeasible(LPCTSTR patchFile)
{
	BOOL bSuccess = TRUE;
	
	if( File::IsExists(patchFile) )
	{
		WIN32_FIND_DATA FileData; 
		HANDLE hSearch;
		UInt64 filesize1;
		hSearch = FindFirstFile(patchFile, &FileData);
		filesize1 = ((UInt64)MAXDWORD+1) * FileData.nFileSizeHigh + FileData.nFileSizeLow;
		FindClose(hSearch);
		ULARGE_INTEGER diskspace = CheckCurrentDriveSpace();
		UInt64 sparespace = SIZE_FILE_200MB;
		if( filesize1 + sparespace < diskspace.QuadPart )
		{
			bSuccess = TRUE;
		}else
			bSuccess = FALSE;
	}

	return bSuccess;
}

VOID CUpdatePatchApp::GetRevertFilename(PCSTRING gdfFilename, PCSTRING targetFolder, UInt32 revertFromVer, UInt32 revertToVer, String& outPath)
{
	outPath.Format(_T("%s\\%s_revert_from%d_to%d.gdf.bak"), targetFolder, gdfFilename, revertFromVer, revertToVer);
}

#ifdef TEST_GDX_CONFIG
VOID CUpdatePatchApp::TestGDX()
{
	TestGDXOpen();
	TestGDXAddVer();
	TestGDXDropVer();
	TestGDXExport();
	TestGDXValid();
	TestGDXManyValid();
}

VOID CUpdatePatchApp::TestGDXOpen()
{
	CGDXFile gdxFile;
	gdxFile.LoadFile(VERSION_TABLE_FILENAME);
	gdxFile.CloseFile();

	TRACE_ENSURE(Reuben::Platform::File::IsExists(VERSION_TABLE_FILENAME));
}

VOID CUpdatePatchApp::TestGDXAddVer()
{
	time_t now;
	time(&now);

	tm* localtm = localtime(&now);
	String filename;
	filename.Format(
		_T("Version_%04d%02d%02d_%02d%02d%02d.GDX"),
		localtm->tm_year, localtm->tm_mon, localtm->tm_mday, localtm->tm_hour, localtm->tm_min, localtm->tm_sec
		);

	CGDXFile gdxFile;
	gdxFile.LoadFile(filename.c_str());
	gdxFile.AppendSave(1, 2, 3, 4);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.AppendSave(2, 3, 4, 5);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.AppendSave(3, 4, 5, 6);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==3);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==5);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==6);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==0);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.CloseFile();

	TRACE_ENSURE(Reuben::Platform::File::IsExists(filename.c_str()));
}

VOID CUpdatePatchApp::TestGDXDropVer()
{
	time_t now;
	time(&now);

	tm* localtm = localtime(&now);
	String filename;
	filename.Format(
		_T("Version_%04d%02d%02d_%02d%02d%02d.GDX"),
		localtm->tm_year, localtm->tm_mon, localtm->tm_mday, localtm->tm_hour, localtm->tm_min, localtm->tm_sec
		);

	CGDXFile gdxFile;
	gdxFile.LoadFile(filename.c_str());
	gdxFile.AppendSave(1, 2, 3, 4);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.AppendSave(2, 3, 4, 5);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.AppendSave(3, 4, 5, 6);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==3);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==5);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==6);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==0);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.CloseFile();
	TRACE_ENSURE(Reuben::Platform::File::IsExists(filename.c_str()));

	gdxFile.LoadFile(filename.c_str());
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==0);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==0);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	gdxFile.DecreaseByOne();
	TRACE_ENSURE(gdxFile.GetFrom(2)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==0);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	TRACE_ENSURE(Reuben::Platform::File::IsExists(filename.c_str()));
}

VOID CUpdatePatchApp::TestGDXExport()
{
	time_t now;
	time(&now);

	tm* localtm = localtime(&now);
	String filename;
	filename.Format(
		_T("Version_%04d%02d%02d_%02d%02d%02d.GDX"),
		localtm->tm_year, localtm->tm_mon, localtm->tm_mday, localtm->tm_hour, localtm->tm_min, localtm->tm_sec
		);
	String copyFilename;
	copyFilename.Format(
		_T("Version_%04d%02d%02d_%02d%02d%02d_copy.GDX"),
		localtm->tm_year, localtm->tm_mon, localtm->tm_mday, localtm->tm_hour, localtm->tm_min, localtm->tm_sec
		);

	CGDXFile gdxFile;
	gdxFile.LoadFile(filename.c_str());
	gdxFile.SaveAs(copyFilename.c_str());
	gdxFile.CloseFile();

	gdxFile.LoadFile(copyFilename.c_str());
	gdxFile.IsValid();
	gdxFile.AppendSave(5, 6, 7, 8);
	TRACE_ENSURE(gdxFile.GetFrom(6)==5);	TRACE_ENSURE(gdxFile.GetCRCFrom(6)==7);	TRACE_ENSURE(gdxFile.GetSizeFrom(6)==8);
	gdxFile.IsValid();
	gdxFile.CloseFile();
}

VOID CUpdatePatchApp::TestGDXValid()
{
	time_t now;
	time(&now);

	tm* localtm = localtime(&now);
	String filename;
	filename.Format(
		_T("Version_%04d%02d%02d_%02d%02d%02d.GDX"),
		localtm->tm_year, localtm->tm_mon, localtm->tm_mday, localtm->tm_hour, localtm->tm_min, localtm->tm_sec
		);

	CGDXFile gdxFile;
	gdxFile.LoadFile(filename.c_str());
	gdxFile.AppendSave(1, 2, 3, 4);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==0);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.AppendSave(2, 3, 4, 5);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==0);
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.AppendSave(3, 4, 5, 6);
	TRACE_ENSURE(gdxFile.GetFrom(2)==1);	TRACE_ENSURE(gdxFile.GetCRCFrom(2)==3);	TRACE_ENSURE(gdxFile.GetSizeFrom(2)==4);
	TRACE_ENSURE(gdxFile.GetFrom(3)==2);	TRACE_ENSURE(gdxFile.GetCRCFrom(3)==4);	TRACE_ENSURE(gdxFile.GetSizeFrom(3)==5);
	TRACE_ENSURE(gdxFile.GetFrom(4)==3);	TRACE_ENSURE(gdxFile.GetCRCFrom(4)==5);	TRACE_ENSURE(gdxFile.GetSizeFrom(4)==6);
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.CloseFile();

	gdxFile.LoadFile(filename.c_str());
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.CloseFile();
}

#define TESTSIZE 2000

VOID CUpdatePatchApp::TestGDXManyValid()
{
	time_t now;
	time(&now);

	tm* localtm = localtime(&now);
	String filename;
	filename.Format(
		_T("Version_%04d%02d%02d_%02d%02d%02d_multi.GDX"),
		localtm->tm_year, localtm->tm_mon, localtm->tm_mday, localtm->tm_hour, localtm->tm_min, localtm->tm_sec
		);

	CGDXFile gdxFile;
	gdxFile.LoadFile(filename.c_str());
	TRACE_ENSURE(gdxFile.IsValid());
	for( Index i = 0; i < TESTSIZE; ++i )
		gdxFile.AppendSave(i, i+1, i+2, i+3);
	TRACE_ENSURE(gdxFile.IsValid());
	for( Index i = 0; i < TESTSIZE; ++i )
	{
		TRACE_ENSURE(gdxFile.GetFrom(i+1)==i);	TRACE_ENSURE(gdxFile.GetCRCFrom(i+1)==i+2);	TRACE_ENSURE(gdxFile.GetSizeFrom(i+1)==i+3);
	}
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.CloseFile();
	
	gdxFile.LoadFile(filename.c_str());
	TRACE_ENSURE(gdxFile.IsValid());
	for( Index i = 0; i < TESTSIZE; ++i )
	{
		TRACE_ENSURE(gdxFile.GetFrom(i+1)==i);	TRACE_ENSURE(gdxFile.GetCRCFrom(i+1)==i+2);	TRACE_ENSURE(gdxFile.GetSizeFrom(i+1)==i+3);
	}
	TRACE_ENSURE(gdxFile.IsValid());
	for( Index i = TESTSIZE; i > 0; --i )
	{
		gdxFile.DecreaseByOne();
		TRACE_ENSURE(gdxFile.IsValid());
		TRACE_ENSURE(gdxFile.GetFrom(i)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(i)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(i)==0);
	}
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.CloseFile();

	gdxFile.LoadFile(filename.c_str());
	TRACE_ENSURE(gdxFile.IsValid());
	for( Index i = 0; i < TESTSIZE; ++i )
	{
		TRACE_ENSURE(gdxFile.GetFrom(i)==0);	TRACE_ENSURE(gdxFile.GetCRCFrom(i)==0);	TRACE_ENSURE(gdxFile.GetSizeFrom(i)==0);
	}
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.CloseFile();

	gdxFile.LoadFile(filename.c_str());
	TRACE_ENSURE(gdxFile.IsValid());
	gdxFile.CloseFile();

	for( Index i = 0; i < TESTSIZE; ++i )
	{
		gdxFile.LoadFile(filename.c_str());
		gdxFile.AppendSave(i, i+1, i+2, i+3);
		for( Index j = 0; j < i+1; ++j )
		{
			TRACE_ENSURE(gdxFile.GetFrom(j+1)==j);	TRACE_ENSURE(gdxFile.GetCRCFrom(j+1)==j+2);	TRACE_ENSURE(gdxFile.GetSizeFrom(j+1)==j+3);
		}
		gdxFile.IsValid();
		gdxFile.CloseFile();
	}
}
#endif	// TEST_GDX_CONFIG
