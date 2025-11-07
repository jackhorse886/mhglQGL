// Update.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在對 PCH 包含此檔案前先包含 'stdafx.h'
#endif

#include "resource.h"		// 主要符號
#include "UpdateDlg.h"
#include "QGLPatcher\GameStringTable.h"


// CUpdateApp:
// 請參閱實作此類別的 Update.cpp
//

class CUpdateApp : public CWinApp
{
public:
	CUpdateApp();

// 覆寫
	public:
	virtual BOOL InitInstance();

// 程式碼實作

	DECLARE_MESSAGE_MAP()

public:
	CGameStringTable* GetStringTable()		{ return &m_stringTable;};
	VOID CopyProcess();
	VOID RestartGameAndExit();

private:
	CUpdateDlg* GetMainDialog()				{ return m_dlg; };
	BOOL LoadConfiguration();
	VOID SetMainDialog(CUpdateDlg* dlg)		{ m_dlg = dlg; };
	VOID RecursiveMove(CONST String SrcFolder, CONST String TargetFolder);
	BOOL MoveUnzipFile(CONST String SrcPath, CONST String TargetFolder);
	BOOL CreateFolder(LPCTSTR szFolder);

	VOID SetPatcherFilename(CONST String filename)		{ m_patcherFilename = filename; };
	String GetPatcherFilename()							{ return m_patcherFilename; };
	VOID SetStartGameFilename(CONST String filename)	{ m_gameFilename = filename; };
	String GetStartGameFilename()						{ return m_gameFilename; };

private:
	CUpdateDlg*	m_dlg;
	String		m_patcherFilename;
	String		m_gameFilename;
	CGameStringTable		m_stringTable;
};

extern CUpdateApp theApp;
