// UpdatePatch.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在對 PCH 包含此檔案前先包含 'stdafx.h'
#endif

#include "common.h"
#include "GDXFile.h"
#include "GDFPackager/GDFCommon.h"
#include "resource.h"		// 主要符號
#include "GameStringTable.h"

//#define TEST_GDX_CONFIG 1

#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6

// CUpdatePatchApp:
// 請參閱實作此類別的 UpdatePatch.cpp
//

class CUpdatePatchApp : public CWinApp
{
public:
	CUpdatePatchApp();
	~CUpdatePatchApp();

// 覆寫
	public:
	virtual BOOL InitInstance();
// 程式碼實作

	DECLARE_MESSAGE_MAP()

public:
	CGameStringTable*	GetStringTable()	{return &m_stringTable;};

	// LOG
	void InitLogger();
	void CloseLogger();

private:
	BOOL ReadConfiguration();
	BOOL ChangeToTargetVersion();
	BOOL ChangeToRevertToVersion(UInt32 version, UInt32 crc);
	BOOL ReadCurrentVersion();
	BOOL IsRepatchRequired(LPCTSTR masterFile, LPCTSTR mergeFile);
	BOOL IsRepatchFeasible(LPCTSTR patchFile);
	ULARGE_INTEGER CheckCurrentDriveSpace();
	VOID GetRevertFilename(PCSTRING gdfFilename, PCSTRING targetFolder, UInt32 revertFromVer, UInt32 revertToVer, String& outPath);
	BOOL WriteVersion(UInt32 version, UInt32 crc);
#ifdef TEST_GDX_CONFIG
	VOID TestGDX();
	VOID TestGDXOpen();
	VOID TestGDXAddVer();
	VOID TestGDXDropVer();
	VOID TestGDXExport();
	VOID TestGDXValid();
	VOID TestGDXManyValid();
#endif

public:
	int			argc;
	LPWSTR*		argv;

private:
	CGameStringTable	m_stringTable;
	UInt32				m_targetVersion;
	UInt32				m_currentVersion;
	UInt32				m_supportFromVersion;
	BOOL				m_bUpgrade;
	UInt32				m_uLocalCRC;
	UInt32				m_uRequiredCRC;
	UInt32				m_uCurrentCRC;
	UInt32				m_installerCRC;
	UInt32				m_installerSize;
};

extern CUpdatePatchApp theApp;
