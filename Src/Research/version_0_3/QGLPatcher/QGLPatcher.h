// QGLPatcher.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在對 PCH 包含此檔案前先包含 'stdafx.h'
#endif

#include "resource.h"		// 主要符號
#include "common.h"
#include "GDFPackager\GDFcommon.h"
#include "SelectionList.h"
#include "InternetSessionEx.h"
#include "GameStringTable.h"

// CQGLPatcherApp:
// 請參閱實作此類別的 QGLPatcher.cpp
//

#define CONFIGURATION_GAME_INI_FILE				_T("Config\\Setting.ini")
#define CONFIGURATION_INI_FILE					_T("Config\\patch.ini")
#define LOCAL_TRACKER_LIST_FILE					_T("patch\\trackerip.txt")
#define LOCAL_SUMMARY_LIST_FILE					_T("patch\\summary.txt")
#define LOCAL_FILELIST_FILE						_T("patch\\filelist.txt")
#define	PATCHER_UPDATE_FILENAME					_T(".\\Update.EXE")
#define FULL_INSTALL_PATH_PATCH					_T("patch")
#define FULL_INSTALL_PATH_TEMP					_T("patch\\trash")
#define POST_PATCH_INSTRUCTION_FILE				_T("_instcmd.txt")

#define GAMESTRINGTABLE_LOCALIZATION_FILENAME	_T("res\\localization.csv")
#define PATCHER_MASTER_GAME_DATA_FILE			_T("Data\\Data.gdf")
#define PATCHERRESNAME_FILE						_T("res")

#define MASTER_HTTP_PORT						80
#define MASTER_FTP_PORT							21
#define FTP_USERNAME							_T("qglclient")
#define FTP_PASSWORD							_T("qglpassword")

// DEFAULT: http://192.100.100.228:80/QGLClient/trackerip.txt
#define DEFAULT_TRACKER_FILE					_T("trackerip.txt")
#define DEFAULT_TRACKER_IP						_T("222.73.109.184")
#define DEFAULT_TRACKER_PATH					_T("QGLClient")
#define DEFAULT_TRACKER_PORT					80
#define DEFAULT_TRACKER_TYPE					1
#define VERSION_HEADER							_T("0.3.")

enum PATCHER_PAGENO
{
	PATCHER_PAGEINVALID = 0,
	PATCHER_PAGE1,
	PATCHER_PAGE2,
	PATCHER_PAGE3
};

enum PATCHER_STATUS
{
	PATCHER_STATUSINVALID = 0,
	PATCHER_FREENAVIGATE,
	PATCHER_DOWNLOAD,
	PATCHER_SUMMARY_DOWNLOADING,
	PATCHER_TRACKER_DOWNLOADING
};

enum PATCHER_DLSTATUS
{
	PATCHER_DL_STATUSINVALID = 0,
	PATCHER_DL_STATUSDL,
	PATCHER_DL_STATUSIN,
	PATCHER_DL_STATUSEMPTY
};

struct FileDownloadStatus
{
	// 0:Not start, 1:Downloading, 2:Merging
	UInt status;
	UInt curDLpos;
	UInt totalDLsize;
	UInt curMergeIndex;
	UInt totalMerge;

	FileDownloadStatus()
	{
		status			= 0;
		curDLpos		= 0;
		totalDLsize		= 1;
		curMergeIndex	= 0;
		totalMerge		= 1;
	}
};

struct DownloadMethod
{
	UInt	type;
	String	sourceip; // server ip(FTP/HTTP) or tracker ip(P2P)
	UInt	port;
	String	path;
	UInt	priority;
};

struct FileSummary
{
	UInt32 version;
	String filename;
	UInt32 filehash;
	UInt32 filesize;
};

struct WebAccessData
{
	String	ip;
	UInt	port;
	UInt	type; // 0:invalid, 1:http, 2:ftp
	String	path;
	String	file;
};

struct ErrorMap
{
	String src;
	String dest;
};

struct LineCombination;
struct ServerData;

typedef StlVector<DownloadMethod, ManagedAllocator<DownloadMethod> >	DownloadMethodList;
typedef StlVector<FileSummary, ManagedAllocator<FileSummary> >			FileSummaryList;
typedef StlVector<LineCombination, ManagedAllocator<LineCombination> >	LineList;
typedef StlVector<ServerData, ManagedAllocator<LineCombination> >		ServerList;
typedef StlVector<WebAccessData, ManagedAllocator<WebAccessData> >		WebAccessDataList;
typedef StlVector<ErrorMap, ManagedAllocator<ErrorMap> >				ErrorList;
typedef StlVector<UInt, ManagedAllocator<UInt> >						TryTrackerList;

struct LineCombination
{
	String		lineName;
	String		ip;
	UInt		port;
};

struct ServerData
{
	String		name;
	LineList	server;
	UInt		status;
};

class CSocket;
class CSockPacket;
class CQGLPatcherDlg;

class CQGLPatcherApp : public CWinApp
{
public:
	CQGLPatcherApp();
	~CQGLPatcherApp();

// 覆寫
	public:
	virtual BOOL InitInstance();

// 程式碼實作

	DECLARE_MESSAGE_MAP()

private:
	// LOG
	void InitLogger();
	void CloseLogger();

	// Configuration - Tracker and Server
	BOOL LoadGeneralConfig();
	BOOL LoadTrackerConfig();
	BOOL LoadServerListConfig();
	BOOL LoadSummaryConfig();
	BOOL LoadSummaryDownloadLink(UInt32 ServerNo);

	VOID FreeGeneralConfig();
	VOID FreeTrackerConfig();
	VOID FreeServerListConfig();
	VOID FreeSummaryConfig();
	VOID FreeSummaryDownloadLink();

	VOID GetCurrentFolderInfo();

	VOID SaveSelectConfiguration();
	BOOL CopyTrackerConfig();
	VOID CreateDirStructure();

	// Algorithm to select tracker to download server status
	WebAccessData SelectTrackerData();

	// Algorithm to select Summary and Filelist to download
	UInt32 GetSummarySelection();
	VOID SummarySelectionChange();
	VOID ResetSummarySelection();

	// Download and Merge
	BOOL DownloadAllPatchFile();
	BOOL DownloadPatchFile(DownloadMethod downloadmethod, FileSummary filesummary);
	BOOL DecompressPatchFile(FileSummary filesummary);
	BOOL MergePatchToPatch(String& masterfile, String& transfile);

	// General functions
	VOID ConvertURLtoComponents(String& fulladdress, String& ipaddress, UInt& port, UInt& type, String& path, String& file);

	UInt GetPatchTargetVersion()				{return m_patchTargetVersion;};
	VOID SetPatchTargetVersion(UInt ver)		{m_patchTargetVersion = ver;};
	UInt GetPatchCurrentVersion()				{return m_patchCurrentVersion;};
	VOID SetPatchCurrentVersion(UInt ver)		{m_patchCurrentVersion = ver;};
	BOOL ReadPatchCurrentVersion();
	VOID WritePatchCurrentVersion();

	String GetGameAppFilename()					{return m_gameAppFilename;};
	VOID SetGameAppFilename(String filename)	{m_gameAppFilename = filename;};
	String GetPatcherAppFilename()				{return m_patcherAppFilename;};
	VOID SetPatcherAppFilename(String filename)	{m_patcherAppFilename = filename;};
	String GetThisAppFilename()					{return m_thisAppFilename;};
	VOID SetThisAppFilename(String filename)	{m_thisAppFilename = filename;};

	// Recursive move files
	VOID RecursiveMove(CONST String SrcFilename, CONST String TargetFolder);
	BOOL MoveUnzipFile(CONST String SrcFilename, CONST String TargetFolder);
	BOOL CreateFolder(LPCTSTR szFolder); // Copy from Unzipper.cpp
	void RemoveDirectoryRecursive(CString szPath);
	VOID ClearPreviousTempPatch();
	// Recursive move - patcher specific function
	BOOL RecursiveMoveExceptPatcher(LPCTSTR SrcFolder, LPCTSTR TargetFolder, LPCTSTR RelativePath, BOOL& bExceptionFound);

	BOOL IsPatcherUpdateExist();
	BOOL IsPatcherResUpdateExist();

public:
	// General Basic Function
	CQGLPatcherDlg* GetMainDialog()				{return m_curMainDlg;};
	CGameStringTable* GetStringTable()			{return &m_stringTable;};

	// NAVIGATION
	unsigned int GetCurPage()					{return m_curpage;};
	void SetCurPage(unsigned int nextpage)		{m_curpage = nextpage;};
	unsigned int GetCurStatus()					{return m_curstatus;};
	void SetCurStatus(unsigned int status)		{m_curstatus = status;};
	unsigned int GetDLStatus()					{return m_dlStatus;};
	void SetDLStatus(unsigned int status)		{m_dlStatus = status;};

	// Select Server
	VOID SetDefaultServer(UInt serverNo)		{ m_defaultServer = serverNo; };
	UInt GetDefaultServer()						{ return m_defaultServer; };
	VOID SetDefaultLine(UInt lineNo)			{ m_defaultLine = lineNo; };
	UInt GetDefaultLine()						{ return m_defaultLine; };
	VOID SetSelectedServer(UInt serverNo)
	{
		if( serverNo < m_serverData.size() )
			m_selectedServer = serverNo;
	};
	UInt GetSelectedServer()					{ return m_selectedServer; };
	VOID SetSelectedLine(UInt lineNo)			{ m_selectedLine = lineNo; };
	UInt GetSelectedLine()						{ return m_selectedLine; };
	ServerData* GetServerInfo(UInt serverno)
	{
		if( serverno < m_serverData.size() )
			return &m_serverData[serverno];
		else
			return NULL;
	}
	
	// Called by Dialog to download and start process
	BOOL DownloadTrackerIpIni();
	VOID StartDownloadProcess();
	BOOL DownloadPatchSummary();
	VOID ResetInterrupted()	{ m_interrupted = FALSE; };
	VOID SetInterrupted()	{ m_interrupted = TRUE; };
	BOOL IsInterrupted()	{ return m_interrupted; };
	VOID ResetDLError()		{ m_dlerror		= FALSE; };
	VOID SetDLError()		{ m_dlerror		= TRUE; SetInterrupted(); };
	BOOL IsDLError()		{ return m_dlerror;		};
	// Called by Dialog to check patching progress
	UInt GetDownloadPos()						{return m_downloadPos;};
	VOID SetDownloadPos(UInt curPos)			{m_downloadPos = curPos;};
	UInt GetDownloadTotal()						{return (UInt)m_patchFileSummary.size();};
	UInt GetFileDownloadPos()
	{
		if( m_fileDownloadInfo.totalDLsize / 50 > 0 )
			return (m_fileDownloadInfo.curDLpos / (m_fileDownloadInfo.totalDLsize / 50) + m_fileDownloadInfo.curMergeIndex / m_fileDownloadInfo.totalMerge * 50);
		return 0;
	};
	UInt GetFileDownloadTotal()
	{
		return 100;
	};
	VOID SetFileDownloadInfo(FileDownloadStatus info)	{m_fileDownloadInfo = info;};
	FileDownloadStatus GetFileDownloadInfo()			{return m_fileDownloadInfo;};

	BOOL FTPDownload(LPCTSTR cszIP, UInt uPort, LPCTSTR cszUsername, LPCTSTR cszPassword, LPCTSTR cszPath, LPCTSTR cszFilename, LPCTSTR cszLocalFilename);
	BOOL FTPDownload_Testing();		// testing metohd for FTPDownload()

private:
	UInt 					m_defaultServer; // server No: 0-based
	UInt 					m_defaultLine;
	UInt 					m_selectedServer; // server No: 0-based
	UInt 					m_selectedLine;

	String					m_defaultServerIP;
	UInt					m_defaultServerPort;

	CFtpConnection*			pFtpConnection;
	CHttpConnection*		pHttpConnection;
	CInternetSessionEx*		m_pSession;
	CSocket*				m_pTCPClient;

	WebAccessDataList		m_trackerList;
	WebAccessDataList		m_serverSummaryList;
	WebAccessDataList		m_serverFileList;

	int						m_curpage;
	int						m_curstatus;

	UInt					m_patchTargetVersion;
	UInt					m_patchCurrentVersion;
	
	UInt					m_patchProtocolCount;
	DownloadMethodList		m_patchMethod;
	FileSummaryList			m_patchFileSummary;
	VOID ResetPatchProtocolList();
	BOOL SelectPatchProtocol(DownloadMethod& dm);

	// status
	UInt					m_downloadPos;
	FileDownloadStatus		m_fileDownloadInfo;

	CQGLPatcherDlg*			m_curMainDlg;
	CGameStringTable		m_stringTable;

	String					m_gameAppFilename;
	String					m_patcherAppFilename;
	String					m_thisAppFilename;

	BOOL					m_interrupted;
	BOOL					m_dlerror;

	String					m_currentDir;
	String					m_patchDir;
	String					m_trashDir;

	UInt32					m_dlStatus;

	CSelectionList*			m_patchFileSelectionList;

public:
	String					m_curVersion;
	String					m_serVersion;

	ServerList				m_serverData;
	ErrorList				m_errorList;
	TryTrackerList			m_tryTrackerList;

	UInt32					m_summaryStart;
	UInt32					m_summaryTimes;
};

extern CQGLPatcherApp theApp;
