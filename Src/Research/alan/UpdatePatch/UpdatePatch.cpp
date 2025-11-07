// UpdatePatch.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "UpdatePatch.h"
#include "UpdatePatchDlg.h"
#include "GDFPackager/GDFManager.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#define LOCALIZATION_FILENAME	_T("patchmsg.csv")
#define CONFIGURATION_INI_FILE	_T("Config/patch.ini")
#define SUMMARY_INI_FILE		_T("Config/summary.ini")
#define PATCHER_FILENAME		_T("QGLPatcherU.exe")
#define MASTER_FILENAME			_T("Data/Data.gdf")

#define REQUIRED_FILE_1			_T("msvcp71.dll")
#define REQUIRED_FILE_2			_T("QGLPatcherU.exe")
#define FILENAME_GDFPATCH_CRITERIA _T(".\\_*.gdf.patch")
#define MAX_SIZE_FILE_FAT32		2147483648
#define SIZE_FILE_2GB			2097152000
#define SIZE_FILE_200MB			209715200

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
	InitLogger();
}

CUpdatePatchApp::~CUpdatePatchApp()
{
	CloseLogger();
}

void CUpdatePatchApp::InitLogger()
{
	Reuben::Utility::ConfigFile config(CONFIGURATION_INI_FILE);
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

	if( succeed && (m_currentVersion >= m_targetVersion || m_currentVersion < m_supportFromVersion) )
	{
		String printout;
		printout.Format(_T("version out of range, sup=%i cur=%i tar=%i"), m_supportFromVersion, m_currentVersion, m_targetVersion);
		MessageBox(m_pMainWnd->m_hWnd, printout.c_str(), _T("err"), MB_OK);
		succeed = FALSE;
	}

	// Find the respective patch file
	// On error, report message (m_CurrentPatchname is full path composed here)
	// On error, version number does not change, the finished GDF files remains good status.
	if( succeed )
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
								if( !manager.Repatch(masterFilename) )
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
					GDFManager manager;
					if( !manager.Merge(masterFilename, patchFilename) )
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
		}
	}

	// Change version
	// On error, report message
	if( succeed && !ChangeToTargetVersion())
	{
		MessageBox(m_pMainWnd->m_hWnd, _T("version cannot be changed"), _T("err"), MB_OK);
		succeed = FALSE;
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
		case '-':			// switch -v, -s, -c, -i...
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
	BOOL succeed = TRUE;
	String value;
	value.Format(_T("%i"), m_targetVersion );
	if( !WritePrivateProfileString(_T("APP"), _T("patcherversion"), value.c_str(), CONFIGURATION_INI_FILE) )
	{
		MessageBox(m_pMainWnd->m_hWnd, _T("CUpdatePatchApp::ChangeToTargetVersion"), _T("err"), MB_OK);
		succeed = FALSE;
	}
	return succeed;
}

BOOL CUpdatePatchApp::ReadCurrentVersion()
{
	BOOL succeed = TRUE;
	m_currentVersion = GetPrivateProfileInt(_T("APP"), _T("patcherversion"), 0, CONFIGURATION_INI_FILE);
	if( m_currentVersion==0 )
		succeed = FALSE;
	return succeed;
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
		filesize2 = 0;
		if( File::IsExists(mergeFile) )
		{
			hSearch = FindFirstFile(mergeFile, &FileData);
			filesize2 = ((UInt64)MAXDWORD+1) * FileData.nFileSizeHigh + FileData.nFileSizeLow;
		}
		if( filesize1 + filesize2 > SIZE_FILE_2GB )	// 2.0G limitation, which suspects to have problem with FAT32
		{
			GDFManager manager;
			String tmpMasterFile = masterFile;
			manager.LoadPatchFile(tmpMasterFile, GDFFILE_STATE_PATCH_READ);

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
	memcpy(appname, driveLetter, 3 * sizeof(TCHAR));
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
