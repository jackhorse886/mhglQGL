// Update.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "Update.h"
#include "UpdateDlg.h"
#include "GDFPackager\GDFCommon.h"
#include "GDFPackager\CFindFile.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#define CONFIGURATION_INI_SYS_FILE			_T("Config\\patchcfg.ini")

#define GAMESTRINGTABLE_LOCALIZATION_FILENAME	_T("res\\localization.csv")

#define UPDATE_PATCHER_FILENAME				_T("QGLPatcherU.EXE")
#define UPDATE_PATCHER_FILENAME_SRC			_T("patch\\trash\\QGLPatcherU.EXE")
#define UPDATE_PATCHER_RESOURCE_DIR			_T("res")
#define UPDATE_PATCHER_RESOURCE_DIR_SRC		_T("patch\\trash\\res")

#define UPDATE_PATCHER_MSG_PREPROCESS		_T("Preprocessing...")
#define UPDATE_PATCHER_MSG_COPYING			_T("Copying Files...")
#define UPDATE_PATCHER_MSG_MERGING			_T("Merging Files...")

#define UPDATE_PATCHER_PREPROCESS_WAITTIME	3000

// CUpdateApp

BEGIN_MESSAGE_MAP(CUpdateApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUpdateApp 建構

CUpdateApp::CUpdateApp()
{
	SetMainDialog(NULL);
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 僅有的一個 CUpdateApp 物件

CUpdateApp theApp;


// CUpdateApp 初始設定

BOOL CUpdateApp::InitInstance()
{
	CWinApp::InitInstance();

	// 標準初始設定
	// 如果您不使用這些功能並且想減少最後完成的可執行檔大小，您可以從下列
	// 程式碼移除不需要的初始化常式，變更儲存設定值的登錄機碼
	// TODO: 您應該適度修改此字串 (例如，公司名稱或組織名稱)

	m_stringTable.Load(GAMESTRINGTABLE_LOCALIZATION_FILENAME);

	BOOL succeed = LoadConfiguration();

	CUpdateDlg dlg;
	m_pMainWnd = &dlg;
	SetMainDialog(&dlg);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置於使用 [確定] 來停止使用對話方塊時
		// 處理的程式碼
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置於使用 [取消] 來停止使用對話方塊時
		// 處理的程式碼
	}

	// 因為已經關閉對話方塊，傳回 FALSE，所以我們會結束應用程式，
	// 而非提示開始應用程式的訊息。
	return FALSE;
}

VOID CUpdateApp::CopyProcess()
{
	CUpdateDlg* dlg = GetMainDialog();

	String msg;
	msg = GetStringTable()->Get(_T("MSG_UPDATE_PREPROCESSING"));
	if( msg.GetLength()==0 ) msg = UPDATE_PATCHER_MSG_PREPROCESS;
	dlg->GetDlgItem(IDC_STATUS)->SetWindowText(msg.c_str());
	dlg->RedrawWindow();

	// wait until termination of Patcher is guaranteed
	while( true )
	{
		FILE* f0;
		if( f0 = _tfopen(UPDATE_PATCHER_FILENAME, _T("a")) )
		{
			fclose(f0);
			break;
		}
		Sleep(500);
	}

	// "patcher.exe"
	msg = GetStringTable()->Get(_T("MSG_UPDATE_COPYING"));
	if( msg.GetLength()==0 ) msg = UPDATE_PATCHER_MSG_COPYING;
	dlg->GetDlgItem(IDC_STATUS)->SetWindowText(msg.c_str());
	dlg->RedrawWindow();

	BOOL bSuccess = FALSE;
	for( Index i = C_INDEX(0); i < C_INDEX(10); ++i )
	{
		::Sleep(UPDATE_PATCHER_PREPROCESS_WAITTIME);
		String srcfilename;
		srcfilename.Format( _T("patch\\trash\\%s"), GetPatcherFilename().c_str() );
		if( File::IsExists(srcfilename.c_str()) )
		{
			bSuccess = ::CopyFile(	srcfilename.c_str(), GetPatcherFilename().c_str(), FALSE );
			if( bSuccess )
			{
				::DeleteFile( srcfilename.c_str() );
				break;
			}
		}else
		{
			break;
		}
	}

	// "res"
	RecursiveMove(UPDATE_PATCHER_RESOURCE_DIR_SRC, UPDATE_PATCHER_RESOURCE_DIR);
}

VOID CUpdateApp::RestartGameAndExit()
{
	ShellExecute(0, _T("open"), GetPatcherFilename().c_str(), NULL, NULL, SW_SHOWNORMAL);
	exit(0);
}

VOID CUpdateApp::RecursiveMove(CONST String SrcFolder, CONST String TargetFolder)
{
	String TargetInput;
	TargetInput.Format(_T("%s\\*.*"), SrcFolder.c_str());
	String cTargetInput = TargetInput.c_str();
	TCHAR* foldername = cTargetInput.GetBuffer(cTargetInput.GetLength());
	CFindFile finder(foldername);
	String FullPath;

	while (finder.GetPathName(&FullPath) != 0)
	{
		// compose new target folder
		UInt lastpos = (UInt)FullPath.find_last_of(_T("\\"));
		UInt srcpathlen = SrcFolder.GetLength();
		String target;
		DWORD attr = GetFileAttributes(FullPath.c_str());
		if( attr & FILE_ATTRIBUTE_DIRECTORY) // if directory, add all files inside; if not, add only source file.
		{
			if( TargetFolder.GetLength() )
				target.Format(_T("%s\\%s"), TargetFolder.c_str(), FullPath.Right(FullPath.GetLength()-lastpos-1).c_str());
			else
				target = FullPath.Right( FullPath.GetLength() - lastpos - 1 );
			RecursiveMove(FullPath, target);
		}else
		{
			target = TargetFolder;
			MoveUnzipFile(FullPath, target);
		}
	}
	RemoveDirectory(SrcFolder.c_str());
}

BOOL CUpdateApp::MoveUnzipFile(CONST String SrcPath, CONST String TargetFolder)
{
	BOOL succeed	= TRUE;
	DWORD dwAttrib	= GetFileAttributes(TargetFolder.c_str());
	if( !TargetFolder.IsEmpty() && dwAttrib == 0xffffffff )
		CreateFolder(TargetFolder.c_str());

	// compose the filename from src path
	String SrcFilename;
	UInt lastpos = (UInt)SrcPath.find_last_of(_T("\\"));
	SrcFilename = SrcPath.Right(SrcPath.GetLength()-lastpos-1);
	// compose target filename
	String target;
	if( TargetFolder.GetLength() )
        target.Format(_T("%s\\%s"), TargetFolder.c_str(), SrcFilename.c_str());
	else
		target = SrcFilename;

	DeleteFile(target.c_str());
	if( !CopyFile(SrcPath.c_str(), target.c_str(), FALSE) )
		succeed = FALSE;
	else if( !DeleteFile(SrcPath.c_str()) )
		succeed = FALSE;
	return succeed;
}

BOOL CUpdateApp::CreateFolder(LPCTSTR szFolder) // Copy from Unzipper.cpp
{
	if (!szFolder || !lstrlen(szFolder))
		return FALSE;

	DWORD dwAttrib = GetFileAttributes(szFolder);

	// already exists ?
	if (dwAttrib != 0xffffffff)
		return ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

	// recursively create from the top down
	TCHAR* szPath = _tcsdup(szFolder);
	String abc = szPath;
	abc.Replace(_T("/"), _T("\\"));
	TCHAR* p = _tcsrchr(abc.c_str(), '\\');

	if (p) 
	{
		// The parent is a dir, not a drive
		*p = '\0';
			
		// if can't create parent
		if (!CreateFolder(abc.c_str()))
		{
			free(szPath);
			return FALSE;
		}
		free(szPath);

		if (!::CreateDirectory(szFolder, NULL)) 
			return FALSE;
	}
	
	return TRUE;
}

BOOL CUpdateApp::LoadConfiguration()
{
	TCHAR tmpstr[260];
	GetPrivateProfileString(_T("APP"), _T("startrestart"), UPDATE_PATCHER_FILENAME, tmpstr, 260, CONFIGURATION_INI_SYS_FILE);
	String str(tmpstr);
	SetPatcherFilename(str);
	return TRUE;
}
