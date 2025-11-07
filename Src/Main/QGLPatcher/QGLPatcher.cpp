// QGLPatcher.cpp : �w�q���ε{�������O�欰�C
//

#include "stdafx.h"
#include "FTPClient.h"
#include "HTTPClient.h"
#include "common.h"
#include "QGLPatcher.h"
#include "Configure.h"
#include "ConfigureApp.h"
#include "QGLPatcherDlg.h"
#include "GDFPackager/GDFPackageMaker.h"
#include "GDFPackager/CFindFile.h"
#include "GDXFile.h"
#include "Reuben/Math/Random.h"
#include "Misc/Stackwalker.h"
#include "UIManagerZoneList.h"
#include "Reuben/Math/CRC.h"
#include "Psapi.h"

#define STREAM_BUF_MAX	65536
#define MAX_NAMES_BIG	65536

typedef StlMap<String, RevertInfo, std::less<String>, ManagedAllocator<std::pair<String, RevertInfo> > > RevertInfoMap;
typedef StlVector<String, ManagedAllocator<String> > BlockVector;

// CQGLPatcherApp

BEGIN_MESSAGE_MAP(CQGLPatcherApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// CQGLPatcherApp �غc

CQGLPatcherApp::CQGLPatcherApp()
{
	// TODO: �b���[�J�غc�{���X�A
	// �N�Ҧ����n����l�]�w�[�J InitInstance ��
	InitLogger();
	m_stringTable.Load(GAMESTRINGTABLE_LOCALIZATION_FILENAME);
	if( !m_configureApp.LoadConfigure() )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CQGLPatcherApp::CQGLPatcherApp() Err [Configure App LoadConfigure failed]"));
	if( !m_configure.LoadConfigure() )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CQGLPatcherApp::CQGLPatcherApp() Err [Configure LoadConfigure failed]"));

	GetUIManagerRecommList()->SetTotalSlot((UInt32)(theApp.GetConfigure()->m_zoneinfo1.x.size() * theApp.GetConfigure()->m_zoneinfo1.y.size()));
	GetUIManagerZoneList()->SetTotalSlot((UInt32)(theApp.GetConfigure()->m_zoneinfo2.x.size() * theApp.GetConfigure()->m_zoneinfo2.y.size()));
	GetUIManagerServerList()->SetTotalSlot((UInt32)(theApp.GetConfigure()->m_zoneinfo3.x.size() * theApp.GetConfigure()->m_zoneinfo3.y.size()));
}

CQGLPatcherApp::~CQGLPatcherApp()
{
	if( m_patchFileSelectionList )
		SafeDeleteObject(m_patchFileSelectionList);
	FreeGeneralConfig();
	FreeTrackerConfig();
	FreeServerListConfig();
	FreeSummaryConfig();
	CloseLogger();
}


// �Ȧ����@�� CQGLPatcherApp ����

CQGLPatcherApp theApp;


// CQGLPatcherApp ��l�]�w

BOOL CQGLPatcherApp::InitInstance()
{
	if (!Reuben::System::Initialize())
		return (-2);

	InitAllocCheck(ACOutput_XML);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// �зǪ�l�]�w
	// �p�G�z���ϥγo�ǥ\��åB�Q��ֳ̫᧹�����i�����ɤj�p�A�z�i�H�q�U�C
	// �{���X�������ݭn����l�Ʊ`���A�ܧ��x�s�]�w�Ȫ��n�����X
	// TODO: �z���ӾA�׭ק惡�r�� (�Ҧp�A���q�W�٩β�´�W��)
//	SetRegistryKey(_T("���� AppWizard �Ҳ��ͪ����ε{��"));

	SetDefaultServer(NO_SERVER_SELECTED);
	SetDefaultZone(-1);
	SetSelectedServer(NO_SERVER_SELECTED);
	m_bDefaultExists	= TRUE;
	m_curVersion		= _T("");
	m_serVersion		= _T("--");
	SetPatchCurrentVersion(0);
	m_interrupted		= FALSE;
	m_dlerror			= FALSE;
	m_patchFileSelectionList	= NULL;
	m_selectedZone		= NULL;
	m_dlStatus			= PATCHER_DL_STATUSSTOP;
	m_initTrackerLoaded	= FALSE;
	m_trackerList.clear();

	SetDownloadPos(0);

	if( !LoadGeneralConfig() )
		return FALSE;

	CreateDirStructure();

	if( !LoadTrackerConfig() )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("QGLPatcher::Init Error (1) [Tracker not found.  Default is used instead.]"));

	m_errorList.clear();

	BOOL bExceptionFound = FALSE;
	if( RecursiveMoveExceptPatcher(m_trashDir.c_str(), m_currentDir.c_str(), _T(""), bExceptionFound) )
	{
		// succeeded
		if( bExceptionFound )
		{
			// patcher files found
			QGLCreateProcess(GetPatcherAppFilename().c_str());
		}else
		{
			// patcher files not found.  Normal and clean start
			RemoveDirectoryRecursive(m_trashDir.c_str());	// remove temp trash folder

			SetCurPage(PATCHER_PAGE1);
			SetCurStatus(PATCHER_FREENAVIGATE);

			CQGLPatcherDlg dlg;
			m_curMainDlg = &dlg;
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

			Reuben::System::Uninitialize();
			REUEBN_SYSTEM_MEMORY_REGISTER_LEAK_REPORT(GLOBAL_LOGGER.GetLogFileName().c_str());

			DeInitAllocCheck();
		}
	}
	else	// exit to show error or restart
	{
		// failed
		if( m_errorList.size() > 0 )
		{
			String text, attach;
			text.Empty();
			for( Index i = C_INDEX(0); i < m_errorList.size(); ++i )
			{
				TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::InitInstance (1) [cannot move file from %s to %s]"), m_errorList[i].src.c_str(), m_errorList[i].dest.c_str());
				attach.Format(GetStringTable()->Get(_T("MSG_FILEMOVE_ERR")).c_str(), m_errorList[i].src.c_str(), m_errorList[i].dest.c_str());
				text = text + attach + _T("\n");
			}
			text = text + GetStringTable()->Get(_T("MSG_CHECK_OPENAPP_PROCESS")) + _T("\n");
			text = text + GetStringTable()->Get(_T("MSG_ASK_RESTART_SYS")) + _T("\n");

			GetMainDialog()->MessageBox(
				text.c_str(),
				GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
				);
		}
	}
	m_errorList.clear();
	return FALSE;
}

BOOL CQGLPatcherApp::ReadPatchCurrentVersion()
{
	if( GetConfigureApp()->UpdatePatcherVersion() )
	{
		SetPatchCurrentVersion(GetConfigureApp()->m_patcherversion);
		return TRUE;
	}
	return FALSE;
}

BOOL CQGLPatcherApp::LoadGeneralConfig()
{
	FreeGeneralConfig();
	GetCurrentFolderInfo();

	if( !ReadPatchCurrentVersion() )
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("QGLPatcher::LoadGeneralConfig Error (1) [patchcfg.ini, APP[patcherversion] reading error]"));
		GetMainDialog()->MessageBox(
			GetStringTable()->Get(_T("MSG_CONFIGURATION_ERR")).c_str(),
			GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
			);
		return FALSE;
	}

	SetGameAppFilename(GetConfigureApp()->m_startgame);
	SetPatcherAppFilename(GetConfigureApp()->m_startpatcher);
	SetThisAppFilename(GetConfigureApp()->m_startrestart);

	return TRUE;
}

BOOL CQGLPatcherApp::LoadTrackerConfig()
{
	BOOL succeed = TRUE;

	TCHAR pBuf[MAX_PATH];
	String accessVariable;
	// tracker IP address configuration
	UInt trackerCount = GetPrivateProfileInt(_T("TRACKERIP"), _T("count"), 0, CONFIGURATION_INI_LOCAL_FILE);
	if( trackerCount==0 )
	{
		FreeTrackerConfig();

		WebAccessData webdata;
		webdata.type	= DEFAULT_TRACKER_TYPE;
		webdata.ip		= DEFAULT_TRACKER_IP;
		webdata.port	= DEFAULT_TRACKER_PORT;
		webdata.path	= DEFAULT_TRACKER_PATH;
		webdata.file	= DEFAULT_TRACKER_FILE;
		m_trackerList.push_back(webdata);

		succeed = FALSE;
	}else
	{
		for( Index i = C_INDEX(0); i < trackerCount; ++i )
		{
			WebAccessData webdata;

			accessVariable.Format(_T("server%i"), i+1);
			GetPrivateProfileString(_T("TRACKERIP"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, CONFIGURATION_INI_LOCAL_FILE);

			String ipaddress, path, file;
			UInt port, type;
			ConvertURLtoComponents(pBuf, ipaddress, port, type, path, file);

			// last. gather all information up and store
			webdata.type	= type;
			webdata.ip		= ipaddress;
			webdata.port	= port;
			webdata.path	= path;
			webdata.file	= file;
			m_trackerList.push_back(webdata);
		}
	}

	for( Index i = C_INDEX(0); i < m_trackerList.size(); ++i )
	{
		WebAccessData& data = m_trackerList[i];
		TRACE_INFODTL_5(GLOBAL_LOGGER,
			_T("CQGLPatcherApp::LoadTrackerConfig List (1) [webdata] type=%d ip=%s port=%d path=%s file=%s"),
			data.type, data.ip.c_str(), data.port, data.path.c_str(), data.file.c_str()
			);
	}
	return succeed;
}

VOID CQGLPatcherApp::ConvertURLtoComponents(PCSTRING fulladdressIn, String& ipaddress, UInt& port, UInt& type, String& path, String& file)
{
	String ipWithPort;
	String protocol;
	String fulladdress(fulladdressIn);

	// 1. Protocol analysis
	UInt lastpos = 0, nextpos = 0;
	nextpos = (UInt)fulladdress.find_first_of(_T("://"), lastpos);
	protocol = fulladdress.substr(0, nextpos - lastpos);
	if( protocol.CompareNoCase(_T("http"))==0 )
		type = 1;
	else if( protocol.CompareNoCase(_T("ftp"))==0 )
		type = 2;
	else
		type = 0;

	// 2. IP address and port resolve (FTP username/password NOT SUPPORTED)
	lastpos = (UInt)fulladdress.find_first_not_of(_T("/"), nextpos + 3);
	nextpos = (UInt)fulladdress.find_first_of(_T("/"), lastpos);
	ipWithPort = fulladdress.substr(lastpos, nextpos-lastpos);
	UInt ilastpos = 0, inextpos = 0;
	inextpos = (UInt)ipWithPort.find_first_of(_T(":"));
	if( inextpos==ipWithPort.npos )
	{
		ipaddress = ipWithPort;
		switch(type)
		{
		case 1:
			port = MASTER_HTTP_PORT;
			break;
		case 2:
			port = MASTER_FTP_PORT;
			break;
		default:
			port = 0;
		}
	}else
	{
		ipaddress = ipWithPort.substr(0, inextpos - ilastpos);
		port = _ttoi(ipWithPort.substr(inextpos + 1, ipWithPort.GetLength() - inextpos - 1).c_str());
	}

	// 3. Resolve Folder path and file name from the remaining part
	path.Empty();
	while( nextpos!=fulladdress.npos )
	{
		lastpos = (UInt)fulladdress.find_first_not_of(_T("/"), nextpos + 1);
		nextpos = (UInt)fulladdress.find_first_of(_T("/"), lastpos);
		if(nextpos!=fulladdress.npos)
			path.Format(_T("%s/%s"), path.c_str(), fulladdress.substr(lastpos, nextpos - lastpos).c_str());
	}
	if( path.GetLength() )
		path = path.substr(1, path.GetLength()-1);
	file = fulladdress.substr(lastpos, path.GetLength()-lastpos);
}

VOID CQGLPatcherApp::FreeGeneralConfig()
{
}

VOID CQGLPatcherApp::FreeTrackerConfig()
{
	m_trackerList.clear();
}

VOID CQGLPatcherApp::FreeServerListConfig()
{
	for( Index i = C_INDEX(0); i < m_serverData.size(); ++i )
		m_serverData[i].server.clear();
	m_serverData.clear();
}

BOOL CQGLPatcherApp::CopyTrackerConfig()
{
	BOOL success = TRUE;

	TCHAR tempip[100];
	String accessVariable;
	String tempvar;

	// update tracker IP address configuration
	String fulladdress;
	UInt trackerCount = GetPrivateProfileInt(_T("TRACKERIP"), _T("count"), 0, LOCAL_TRACKER_LIST_FILE);
	if( trackerCount==0 )
	{
		return FALSE;
	}else
	{
		tempvar.Format(_T("%i"), trackerCount);
		WritePrivateProfileString(_T("TRACKERIP"), _T("count"), tempvar.c_str(), CONFIGURATION_INI_LOCAL_FILE);
		for( Index i = C_INDEX(0); i < trackerCount; ++i )
		{
			accessVariable.Format(_T("server%i"), i+1);
			GetPrivateProfileString(_T("TRACKERIP"), accessVariable.c_str(), _T("\0"), tempip, 100, LOCAL_TRACKER_LIST_FILE);
			WritePrivateProfileString(_T("TRACKERIP"), accessVariable.c_str(), tempip, CONFIGURATION_INI_LOCAL_FILE);
		}
	}
	return success;
}


BOOL CQGLPatcherApp::DownloadTrackerIpIni()
{
	SetCurStatus(PATCHER_TRACKER_DOWNLOADING);
	m_curMainDlg->m_startGame.LockStatus(3);
	m_curMainDlg->m_startGame.RedrawWindow();

	BOOL bSuccess = FALSE;
	if( !GetConfigureApp()->m_debug_ui )
	{
		WebAccessData curWebData;
		while(true)
		{
			do
			{
				curWebData = SelectTrackerData();
				switch( curWebData.type )
				{
				case 1:		// HTTP protocol
					{
						String strIP		= curWebData.ip;
						UInt uPort			= curWebData.port;
						String strUsername	= _T("");
						String strPassword	= _T("");
						String strPath		= curWebData.path;
						String strFilename	= curWebData.file;
						String strLocalFilename	= LOCAL_TRACKER_LIST_FILE;

						DeleteFile(strLocalFilename.c_str());
						DeleteFile(_T("patch\\srvlistf2.txt"));
						DeleteFile(_T("patch\\zonelist.txt"));
						DeleteFile(_T("patch\\fileversion.txt"));
						bSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
						bSuccess = bSuccess && HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), _T("srvlistf2.txt"), _T("patch\\srvlistf2.txt"));
						bSuccess = bSuccess && HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), _T("zonelist.txt"), _T("patch\\zonelist.txt"));
						bSuccess = bSuccess && HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), _T("fileversion.txt"), _T("patch\\fileversion.txt"));
					}
					break;
				case 2:		// FTP protocol
					{
						String strIP		= curWebData.ip;
						UInt uPort			= curWebData.port;
						String strUsername	= FTP_USERNAME;
						String strPassword	= FTP_PASSWORD;
						String strPath		= curWebData.path;
						String strFilename	= curWebData.file;
						String strLocalFilename	= LOCAL_TRACKER_LIST_FILE;

						DeleteFile(strLocalFilename.c_str());
						DeleteFile(_T("patch\\srvlistf2.txt"));
						DeleteFile(_T("patch\\zonelist.txt"));
						DeleteFile(_T("patch\\fileversion.txt"));
						bSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
						bSuccess = bSuccess && FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), _T("srvlistf2.txt"), _T("patch\\srvlistf2.txt"));
						bSuccess = bSuccess && FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), _T("zonelist.txt"), _T("patch\\zonelist.txt"));
						bSuccess = bSuccess && FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), _T("fileversion.txt"), _T("patch\\fileversion.txt"));
					}
					break;
				}
			}while( !bSuccess && m_trackerList.size() > m_tryTrackerList.size() );
			m_tryTrackerList.clear();
			if( bSuccess  )
				break;
		}
	}else
	{
		// debug mode -> compulsory change allowed
		bSuccess = TRUE;
	}

	// if successfully download and integrated Tracker setting, set status to Free Navigation;
	// if not, pop an message.
	if( bSuccess && 
		GetConfigureServerList()->ReloadConfigure() && 
		GetConfigureZoneList()->ReloadConfigure() &&
		GetConfigureVersion()->ReloadConfigure()
		)
	{
		// Server assignment to Zone
		ServerInfoList& serverInfoList = *GetConfigureServerList()->GetServerInfoList();
		for( Index i = C_INDEX(0); i < serverInfoList.size(); ++i )
		{
			ServerInfo& info = serverInfoList[i];
			for( Index j = C_INDEX(0); j < info.zoneList.size(); ++j )
			{
				UInt zoneid = info.zoneList[j];
				Zone* zone = GetConfigureZoneList()->GetZone(zoneid);
				if( zone )
					zone->RegisterServer(&info);
			}
		}
		GetUIManagerZoneList()->LoadContent(GetConfigureZoneList()->GetZoneMap());
		GetUIManagerRecommList()->LoadContent(GetConfigureServerList(), GetConfigureZoneList());

		// load default zone server list
		UInt32 zoneid = 1;
		SetSelectedZone(*GetConfigureZoneList()->GetZone(zoneid));
		if( GetSelectedZone() )
		{
			ServerInfoList zoneSrvList;
			zoneSrvList.clear();
			GetSelectedZone()->GetServerList(zoneSrvList);
			GetUIManagerServerList()->LoadContent(&zoneSrvList);
			// 2. jump to selected zone
		}
		SetPatchCurrentVersion(GetPatchCurrentVersion());

		SetCurStatus(PATCHER_FREENAVIGATE);
		m_curMainDlg->m_startGame.UnlockStatus();;
		m_curMainDlg->m_startGame.RedrawWindow();

		LookupDefaultServerSetting();
	}
	else
	{
		GetMainDialog()->MessageBox(
			GetStringTable()->Get(_T("MSG_TRACKER_DOWNLOAD_FAILED")).c_str(),
			GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
			);
		exit(0);
		return FALSE;
	}
	
	SetInitTrackerLoaded();
	return TRUE;
}

WebAccessData CQGLPatcherApp::SelectTrackerData()
{
	if( m_trackerList.size() > 0 )
	{
		UInt total = (UInt)m_trackerList.size();
		Reuben::Math::Random rand;
		rand.Randomize();
		UInt selno = 0;
		if( m_tryTrackerList.size()==0 )
			selno = rand.GetRandom(total);
		else
		{
			selno = (m_tryTrackerList[m_tryTrackerList.size()-1] + 1) % total;
		}
		m_tryTrackerList.push_back(selno);
		return m_trackerList[selno];
	}else
	{
		WebAccessData temp;
		temp.file	= DEFAULT_TRACKER_FILE;
		temp.ip		= DEFAULT_TRACKER_IP;
		temp.path	= DEFAULT_TRACKER_PATH;
		temp.port	= DEFAULT_TRACKER_PORT;
		temp.type	= DEFAULT_TRACKER_TYPE;
		return temp;
	}
}

BOOL CQGLPatcherApp::DownloadPatchSummary()
{
	SetCurStatus(PATCHER_SUMMARY_DOWNLOADING);
	m_curMainDlg->m_startGamePatch.LockStatus(3);
	m_curMainDlg->m_startGamePatch.RedrawWindow();

	BOOL bSumSuccess	= FALSE;
	BOOL bFileSuccess	= FALSE;

	LoadSummaryDownloadLink(GetSelectedServer());

	ResetSummarySelection();
	if( !GetConfigureApp()->m_debug_ui )
	{
		while( !(bSumSuccess && bFileSuccess) && GetSummarySelection()!=-1 )
		{
			bSumSuccess		= FALSE;
			bFileSuccess	= FALSE;

			// Summary Download Selection (temp!!!)
			WebAccessData curWebData	= m_serverSummaryList[GetSummarySelection()];
			WebAccessData curFileList	= m_serverFileList[GetSummarySelection()];
			WebAccessData curRevertList	= m_serverRevertList[GetSummarySelection()];
			SummarySelectionChange();

			while(true)
			{
				switch( curWebData.type )
				{
				case 1:	// HTTP
					{
						// Summary download
						{
							String strIP		= curWebData.ip;
							UInt uPort			= curWebData.port;
							String strUsername	= _T("");
							String strPassword	= _T("");
							String strPath		= curWebData.path;
							String strFilename	= curWebData.file;
							String strLocalFilename	= LOCAL_SUMMARY_LIST_FILE;

							DeleteFile(strLocalFilename.c_str());
							bSumSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
						}
						// FileList download
						if( bSumSuccess )
						{
							String strIP		= curWebData.ip;
							UInt uPort			= curWebData.port;
							String strUsername	= _T("");
							String strPassword	= _T("");
							String strPath		= curWebData.path;
							String strFilename	= curFileList.file;
							String strLocalFilename	= LOCAL_FILELIST_FILE;

							DeleteFile(strLocalFilename.c_str());
							bFileSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
						}
					}
					break;
				case 2:	// FTP
					{
						// Summary download
						{
							String strIP		= curWebData.ip;
							UInt uPort			= curWebData.port;
							String strUsername	= FTP_USERNAME;
							String strPassword	= FTP_PASSWORD;
							String strPath		= curWebData.path;
							String strFilename	= curWebData.file;
							String strLocalFilename	= LOCAL_SUMMARY_LIST_FILE;

							DeleteFile(strLocalFilename.c_str());
							bSumSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
						}
						// FileList download
						if( bSumSuccess )
						{
							String strIP		= curWebData.ip;
							UInt uPort			= curWebData.port;
							String strUsername	= FTP_USERNAME;
							String strPassword	= FTP_PASSWORD;
							String strPath		= curWebData.path;
							String strFilename	= curFileList.file;
							String strLocalFilename	= LOCAL_FILELIST_FILE;

							DeleteFile(strLocalFilename.c_str());
							bFileSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
						}
					}
					break;
				}
				if( bSumSuccess && bFileSuccess )
					break;
			}
		}
	}else
	{
		// debug mode -> compulsory change allowed
		bSumSuccess		= TRUE;
		bFileSuccess	= TRUE;
	}

	if( bSumSuccess && bFileSuccess )
	{
		LoadSummaryConfig();

		GetMainDialog()->RedrawSerVerDisplay();
		GetMainDialog()->RedrawDownloadStatusText();
		// 1. validate version and CRC now
		// 2. if it's the latest, valid -> pass!
		//    if it's older than the server version, find summary for current version and CRC in filelist
		//		if it matches -> patch Up!
		//		else
		//			DL revert list
		//			compare VersionTable.GDX and revert list to see what to patch.  If not match, again
		//	  if it's newer than the server version
		//		DL revert list
		//		compare VersionTable.GDX and revert list to see what to patch.  If not match, again

		bool bIsLatestVersion = (GetConfigureApp()->m_patcherversioncrc==m_patchSummaryVersionCRC || GetConfigureApp()->m_patcherversioncrc==0 ) &&
			GetConfigureApp()->m_patcherversion==GetPatchTargetVersion();

		TRACE_INFODTL_4(GLOBAL_LOGGER, _F("localVersion %d, localCRC %x, remoteVersion %d, remoteCRC %x"),
			GetConfigureApp()->m_patcherversion, GetConfigureApp()->m_patcherversioncrc,
			GetPatchTargetVersion(), m_patchSummaryVersionCRC
			);

		if( bIsLatestVersion )
		{
			LoadPatchUpFileList(GetPatchCurrentVersion(), GetPatchTargetVersion());
			LoadSummaryDownloadLink(GetSelectedServer());
		}else
		{
			TCHAR tempstring[100];
			bool bIsInSummaryTree = false;
			{
				String accessVariable, groupVariable;
				groupVariable = _T("FILESUMMARY");

				// find filename
				String currentFilename;
				ZeroMemory(tempstring, 100*sizeof(TCHAR));
				accessVariable.Format(_T("patch%08i"), GetPatchCurrentVersion());
				GetPrivateProfileString(groupVariable.c_str(), accessVariable.c_str(), _T(""), tempstring, 100, LOCAL_SUMMARY_LIST_FILE);
				currentFilename = tempstring;

				UInt32 patchVersionCRC = 0;
				// find patch support version CRC
				ZeroMemory(tempstring, 100*sizeof(TCHAR));
				GetPrivateProfileString(currentFilename.c_str(), _T("vcrc"), _T(""), tempstring, 100, LOCAL_FILELIST_FILE);
				_stscanf(tempstring, _T("%x"), &patchVersionCRC);

				// confirmed that current version is NOT REVERTED (i.e. CRC(current) = CRC(file in main stream) )
				bIsInSummaryTree = (GetConfigureApp()->m_patcherversioncrc==patchVersionCRC || GetConfigureApp()->m_patcherversioncrc==0) &&
					GetConfigureApp()->m_patcherversion <= GetPatchTargetVersion();

				if( GetPatchCurrentVersion()==GetPatchTargetVersion() )
				{
					TRACE_INFODTL_1(GLOBAL_LOGGER, _F("remoteFileInfo StartFromVersion %d(top), but CRC different"), 
						GetPatchCurrentVersion());
				}else
				{
					TRACE_INFODTL_4(GLOBAL_LOGGER, _F("remoteFileInfo StartFromVersion %d, StartFromCRC %x, StartFromFilename %s, isInSummaryTree %d"), 
						GetPatchCurrentVersion(), patchVersionCRC, currentFilename.c_str(), bIsInSummaryTree ? 1 : 0
						);
				}
			}
			if( bIsInSummaryTree )
			{
				LoadPatchUpFileList(GetPatchCurrentVersion(), GetPatchTargetVersion());
				LoadSummaryDownloadLink(GetSelectedServer());
			}else
			{
				TRACE_INFODTL(GLOBAL_LOGGER, _F("Patch Revert start"));

				// 1. DL revert list
				BOOL bRevertSuccess;
				ResetSummarySelection();
				if( !GetConfigureApp()->m_debug_ui )
				{
					bRevertSuccess	= FALSE;
					while( !bRevertSuccess && GetSummarySelection()!=-1 )
					{
						// Summary Download Selection (temp!!!)
						WebAccessData curWebData	= m_serverSummaryList[GetSummarySelection()];
						WebAccessData curFileList	= m_serverFileList[GetSummarySelection()];
						WebAccessData curRevertList	= m_serverRevertList[GetSummarySelection()];
						SummarySelectionChange();

						while(true)
						{
							switch( curWebData.type )
							{
							case 1:	// HTTP
								{
									// Revertlist download
									{
										String strIP		= curRevertList.ip;
										UInt uPort			= curRevertList.port;
										String strUsername	= _T("");
										String strPassword	= _T("");
										String strPath		= curRevertList.path;
										String strFilename	= curRevertList.file;
										String strLocalFilename	= LOCAL_REVERT_LIST_FILE;

										DeleteFile(strLocalFilename.c_str());
										bRevertSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
									}
								}
								break;
							case 2:	// FTP
								{
									// Revertlist download
									{
										String strIP		= curRevertList.ip;
										UInt uPort			= curRevertList.port;
										String strUsername	= FTP_USERNAME;
										String strPassword	= FTP_PASSWORD;
										String strPath		= curRevertList.path;
										String strFilename	= curRevertList.file;
										String strLocalFilename	= LOCAL_REVERT_LIST_FILE;

										DeleteFile(strLocalFilename.c_str());
										bRevertSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
									}
								}
								break;
							}
							if( bRevertSuccess )
								break;
						}	// while true
					}	// while Summary(getselection)
				}else
				{
					// debug mode -> compulsory change allowed
					bRevertSuccess	= TRUE;
				}
				// 2. find patch down tree
				RevertInfoMap revertMap;

				UInt32 counter = 1;
				LPTSTR lpszReturnBuffer;
				lpszReturnBuffer = new TCHAR[MAX_NAMES_BIG];
				TCHAR* pNextSection = NULL;
				GetPrivateProfileSectionNames(lpszReturnBuffer, MAX_NAMES_BIG, LOCAL_REVERT_LIST_FILE);
				pNextSection = lpszReturnBuffer;
				{
					String filename(pNextSection);
					RevertInfo dummy;
					revertMap.insert(std::make_pair(filename, dummy));
				}
				while (*pNextSection != 0x00)
				{
					pNextSection = pNextSection + _tcslen(pNextSection) + 1;
					if(*pNextSection != 0x00)
					{
						if( _tcsicmp(pNextSection, _T("FINISH")) )
						{
							String filename(pNextSection);
							RevertInfo dummy;
							revertMap.insert(std::make_pair(filename, dummy));
						}
					}
				}
				delete [] lpszReturnBuffer;

				// revert data loaded into memory
				TCHAR strCRC[10];
				for( RevertInfoMap::iterator itr = revertMap.begin(); itr != revertMap.end(); ++itr )
				{
					CONST String& filename = itr->first;
					RevertInfo& info = itr->second;

					ZeroMemory(strCRC, 10*sizeof(TCHAR));
					info.from		= GetPrivateProfileInt(filename.c_str(), _T("from"), 0, LOCAL_REVERT_LIST_FILE);
					info.to			= GetPrivateProfileInt(filename.c_str(), _T("to"), 0, LOCAL_REVERT_LIST_FILE);
					GetPrivateProfileString(filename.c_str(), _T("vcrcf"), _T(""), strCRC, 10, LOCAL_REVERT_LIST_FILE);
					info.fromCRC	= 0;
					_stscanf(strCRC, _T("%x"), &info.fromCRC);
					GetPrivateProfileString(filename.c_str(), _T("vcrct"), _T(""), strCRC, 10, LOCAL_REVERT_LIST_FILE);
					info.toCRC		= 0;
					_stscanf(strCRC, _T("%x"), &info.toCRC);
				}

				// From > to, e.g. From66->To64
				UInt32 tempFrom, tempTo;
				UInt32 tempFromCRC, tempToCRC;
				CGDXFile gdxFile;
				gdxFile.LoadFile(VERSION_TABLE_FILENAME);
				tempFrom		= GetConfigureApp()->m_patcherversion;
				tempFromCRC		= GetConfigureApp()->m_patcherversioncrc;
				tempTo			= gdxFile.GetFrom(tempFrom);
				tempToCRC		= gdxFile.GetCRCFrom(tempFrom);

				UInt32 summaryFinalVer = GetPatchTargetVersion();
				UInt32 summaryFinalCRC = m_patchSummaryVersionCRC;

				// Breakloop criteria 1: final version is part of my history
				UInt32 testCounter = 0;
				while( tempFrom!=summaryFinalVer || tempFromCRC!=summaryFinalCRC )
				{
					TRACE_INFODTL_4(GLOBAL_LOGGER, _F("Searching Update History %d(%x)->%d(%x)"),
						tempTo, tempToCRC, tempFrom, tempFromCRC
						);
					// find an appropriate REVERT record
					bool bFound = false;
					for( RevertInfoMap::iterator itr = revertMap.begin(); itr != revertMap.end(); ++itr )
					{
						CONST String& filename = itr->first;
						RevertInfo& info = itr->second;

						if( info.from==tempFrom && info.fromCRC==tempFromCRC && info.to==tempTo )
						{
							TRACE_INFODTL(GLOBAL_LOGGER, _F("Found"));

							// insert it into download list
							FileSummary fs;
							fs.filename = filename;
							GetPrivateProfileString(filename.c_str(), _T("crc"), _T(""), strCRC, 10, LOCAL_REVERT_LIST_FILE);
							fs.filehash	= 0;
							_stscanf(strCRC, _T("%x"), &fs.filehash);
							fs.filesize	= GetPrivateProfileInt(filename.c_str(), _T("size"), 0, LOCAL_REVERT_LIST_FILE);
							fs.version	= GetPrivateProfileInt(filename.c_str(), _T("from"), 0, LOCAL_REVERT_LIST_FILE);
							m_patchDownFileList.push_back(fs);

							// expected CRC and Version after revert
							tempFrom	= info.to;
							tempFromCRC	= info.toCRC;
							tempTo		= gdxFile.GetFrom(tempFrom);
							tempToCRC	= gdxFile.GetCRCFrom(tempFrom);

							bFound = true;
							break;
						}
					}
					if( !bFound )
					{
						TRACE_ERRORDTL_4(GLOBAL_LOGGER, _FE("Current Revert-To not found, Update History %d(%x)->%d(%x)"),
							tempTo, tempToCRC, tempFrom, tempFromCRC
							);
						break;
					}

					// Breakloop criteria 2: part of my history has reached part of the summary
					UInt32 summaryVCRC = 0;
					{
						String patchVersion;
						patchVersion.Format(_T("patch%08d"), tempFrom);
						GetPrivateProfileString(_T("FILESUMMARY"), patchVersion.c_str(), _T(""), tempstring, 100, LOCAL_SUMMARY_LIST_FILE);
						GetPrivateProfileString(tempstring, _T("vcrc"), _T("00000000"), strCRC, 10, LOCAL_FILELIST_FILE); 
						_stscanf(strCRC, _T("%x"), &summaryVCRC);
					}
					bool bIsCurrentRevertToPartOfSummary = (summaryVCRC==tempFromCRC);
					if( bIsCurrentRevertToPartOfSummary )
					{
						TRACE_INFODTL(GLOBAL_LOGGER, _F("Current Revert-To is part of File Summary"));
						break;
					}else
					{
						TRACE_INFODTL_8(GLOBAL_LOGGER, _F("Current Revert-To is not part of FileSummary, continue Patch down, expected %d(%x)->%d(%x), found %d(%x)->%d(%x)"),
							tempTo, tempToCRC, tempFrom, tempFromCRC, 
							tempTo, tempToCRC, tempFrom, summaryVCRC
							);
					}

					if( testCounter > 100000 )
					{
						GetMainDialog()->MessageBox(_T("Due to patch version file corruption, patcher stopped here."),
							GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
							);
						return FALSE;
					}
				}
				// 3. find patch up tree
				UInt32 versionAfterDown = tempFrom;
				LoadPatchUpFileList(versionAfterDown, GetPatchTargetVersion());
			}	// current file crc not match (revert taken place)
		}
		GetMainDialog()->RedrawSerVerDisplay();
		GetMainDialog()->RedrawDownloadStatusText();
		theApp.SetCurStatus(PATCHER_FREENAVIGATE);
		m_curMainDlg->m_startGamePatch.UnlockStatus();
		m_curMainDlg->m_startGamePatch.RedrawWindow();
		return TRUE;
	}else
	{
		GetMainDialog()->MessageBox(
			GetStringTable()->Get(_T("MSG_SERVERLIST_LOAD_FAILED")).c_str(),
			GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
			);
		return FALSE;
	}
}

BOOL CQGLPatcherApp::DownloadAllPatchFile()
{
	if( m_patchUpFileList.empty() && m_patchDownFileList.empty() )
		return TRUE;

	// overall statistics
	UInt32 totalsize = 0;
	for( Index i = C_INDEX(0); i < m_patchUpFileList.size(); ++i )
	{
		FileSummary& fs = m_patchUpFileList[i];
		totalsize += fs.filesize;
	}
	for( Index i = C_INDEX(0); i < m_patchDownFileList.size(); ++i )
	{
		FileSummary& fs = m_patchDownFileList[i];
		totalsize += fs.filesize;
	}
	GetProgressMonitor()->SetOverallSize(totalsize);

	DownloadMethod dm;
	BOOL succeeded = FALSE;
	ResetPatchProtocolList();
	while( !succeeded && SelectPatchProtocol(dm) )
	{
		switch( dm.type )
		{
		case 1: // HTTP
		case 2: // FTP
			{
				BOOL bForAllSucceed = TRUE;
				for( Index i = C_INDEX(0); i < m_patchDownFileList.size(); ++i )
				{
					if( m_patchDownFileList[i].version==GetPatchCurrentVersion() &&
						(m_patchDownFileList[i].versionCRC==GetConfigureApp()->m_patcherversioncrc ||
						GetConfigureApp()->m_patcherversioncrc!=0)
						)
					{
						// Download file
						SetDownloadPos(i);
						UInt revertVersion = GetPatchCurrentVersion();
						FileSummary& fs = m_patchDownFileList[i];
						SetDLStatus(PATCHER_DL_STATUSDL);

						BOOL bSuccess = FALSE;
						switch( dm.type )
						{
						case 1: // HTTP
							{
								String strIP		= dm.sourceip;
								UInt uPort			= dm.port;
								String strUsername	= _T("");
								String strPassword	= _T("");
								String strPath		= dm.path;
								String strFilename	= fs.filename;
								String strLocalFilename;
								strLocalFilename.Format(_T("patch\\%s"), fs.filename.c_str());
								GetProgressMonitor()->SetStateDownload();
								GetProgressMonitor()->SetNewDownload(fs.filename.c_str(), strLocalFilename.c_str());
								GetProgressMonitor()->SetFileTotalSize(fs.filesize);
								// app download
								BOOL bDownloadIt = TRUE, bRedownloadIt = FALSE;
								PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
								if( bRedownloadIt )
									DeleteFile(strLocalFilename.c_str());
								if( bDownloadIt )
								{
									bSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
									if( bSuccess )
									{
										PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
										// if the resumed data is corrupted (probably caused by previous part), redo download once
										if( bRedownloadIt )
											DeleteFile(strLocalFilename.c_str());
										if( bDownloadIt )
											bSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
									}
								}else	// fully downloaded.  Resume or download skipped.
									bSuccess = TRUE;
							}
							break;
						case 2: // FTP
							{
								String strIP		= dm.sourceip;
								UInt uPort			= dm.port;
								String strUsername	= FTP_USERNAME;
								String strPassword	= FTP_PASSWORD;
								String strPath		= dm.path;
								String strFilename	= fs.filename;
								String strLocalFilename;
								strLocalFilename.Format(_T("patch\\%s"), fs.filename.c_str());
								GetProgressMonitor()->SetStateDownload();
								GetProgressMonitor()->SetNewDownload(fs.filename.c_str(), strLocalFilename.c_str());
								GetProgressMonitor()->SetFileTotalSize(fs.filesize);
								// app download
								BOOL bDownloadIt = TRUE, bRedownloadIt = FALSE;
								PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
								if( bRedownloadIt )
									DeleteFile(strLocalFilename.c_str());
								if( bDownloadIt )
								{
									bSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
									if( bSuccess )
									{
										PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
										// if the resumed data is corrupted (probably caused by previous part), redo download once
										if( bRedownloadIt )
											DeleteFile(strLocalFilename.c_str());
										if( bDownloadIt )
											bSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
									}
								}else	// fully downloaded.  Resume or download skipped.
									bSuccess = TRUE;
							}
							break;
						}
						if( bSuccess )
						{
							SetDLStatus(PATCHER_DL_STATUSIN);
							String strLocalFilename;
							strLocalFilename.Format(_T("patch\\%s"), fs.filename.c_str());
							struct _stat st;
							_tstat(strLocalFilename.c_str(), &st);
							GetProgressMonitor()->SetStateUpdate();
							if( !DecompressPatchFile(fs) )	// version update completed (install succeed && no patcher update)
								SetInterrupted();
							GetProgressMonitor()->ChangeOverallFinished(st.st_size);
							GetProgressMonitor()->SetFileCurrentSize((UInt32)0);
						}else
						{
							bForAllSucceed = FALSE;
							SetDLError();
						}
					}
					String major, suffix;
					UInt32 majorSince = 0, suffixSince = 0;
					theApp.GetConfigureVersion()->GetMajorVersion(GetPatchCurrentVersion(), major, majorSince);
					theApp.GetConfigureVersion()->GetSuffixVersion(GetPatchCurrentVersion(), suffix, suffixSince);
					m_curVersion.Format(_T("%s.%i%s"), major.c_str(), GetPatchCurrentVersion()-majorSince+1, suffix.c_str());
					GetMainDialog()->RedrawCurVerDisplay();
					if( IsInterrupted() )
						break;
				}	// for PatchDownFileList
				if( !IsInterrupted() )
				{
					for( Index i = C_INDEX(0); i < m_patchUpFileList.size(); ++i )
					{
						if( m_patchUpFileList[i].version==GetPatchCurrentVersion() &&
							(m_patchUpFileList[i].versionCRC==GetConfigureApp()->m_patcherversioncrc ||
							GetConfigureApp()->m_patcherversioncrc==0)
							)
						{
							// Download file
							SetDownloadPos(i);
							UInt revertVersion = GetPatchCurrentVersion();
							FileSummary& fs = m_patchUpFileList[i];
							SetDLStatus(PATCHER_DL_STATUSDL);

							BOOL bSuccess = FALSE;
							switch( dm.type )
							{
							case 1: // HTTP
								{
									String strIP		= dm.sourceip;
									UInt uPort			= dm.port;
									String strUsername	= _T("");
									String strPassword	= _T("");
									String strPath		= dm.path;
									String strFilename	= fs.filename;
									String strLocalFilename;
									strLocalFilename.Format(_T("patch\\%s"), fs.filename.c_str());
									GetProgressMonitor()->SetStateDownload();
									GetProgressMonitor()->SetNewDownload(fs.filename.c_str(), strLocalFilename.c_str());
									GetProgressMonitor()->SetFileTotalSize(fs.filesize);
									// app download
									BOOL bDownloadIt = TRUE, bRedownloadIt = FALSE;
									PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
									if( bRedownloadIt )
										DeleteFile(strLocalFilename.c_str());
									if( bDownloadIt )
									{
										bSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
										if( bSuccess )
										{
											PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
											// if the resumed data is corrupted (probably caused by previous part), redo download once
											if( bRedownloadIt )
												DeleteFile(strLocalFilename.c_str());
											if( bDownloadIt )
												bSuccess = HTTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
										}
									}else	// fully downloaded.  Resume or download skipped.
										bSuccess = TRUE;
								}
								break;
							case 2: // FTP
								{
									String strIP		= dm.sourceip;
									UInt uPort			= dm.port;
									String strUsername	= FTP_USERNAME;
									String strPassword	= FTP_PASSWORD;
									String strPath		= dm.path;
									String strFilename	= fs.filename;
									String strLocalFilename;
									strLocalFilename.Format(_T("patch\\%s"), fs.filename.c_str());
									GetProgressMonitor()->SetStateDownload();
									GetProgressMonitor()->SetNewDownload(fs.filename.c_str(), strLocalFilename.c_str());
									GetProgressMonitor()->SetFileTotalSize(fs.filesize);
									// app download
									BOOL bDownloadIt = TRUE, bRedownloadIt = FALSE;
									PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
									if( bRedownloadIt )
										DeleteFile(strLocalFilename.c_str());
									if( bDownloadIt )
									{
										bSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
										if( bSuccess )
										{
											PatcherPatchFileCheck(strLocalFilename.c_str(), fs.filesize, fs.filehash, bDownloadIt, bRedownloadIt);
											// if the resumed data is corrupted (probably caused by previous part), redo download once
											if( bRedownloadIt )
												DeleteFile(strLocalFilename.c_str());
											if( bDownloadIt )
												bSuccess = FTPDownload(strIP.c_str(), uPort, strUsername.c_str(), strPassword.c_str(), strPath.c_str(), strFilename.c_str(), strLocalFilename.c_str());
										}
									}else	// fully downloaded.  Resume or download skipped.
										bSuccess = TRUE;
								}
								break;
							}
							if( bSuccess )
							{
								SetDLStatus(PATCHER_DL_STATUSIN);
								String strLocalFilename;
								strLocalFilename.Format(_T("patch\\%s"), fs.filename.c_str());
								struct _stat st;
								_tstat(strLocalFilename.c_str(), &st);
								GetProgressMonitor()->SetStateUpdate();
								if( !DecompressPatchFile(fs) )	// version update completed (install succeed && no patcher update)
									SetInterrupted();
								GetProgressMonitor()->ChangeOverallFinished(st.st_size);
								GetProgressMonitor()->SetFileCurrentSize((UInt32)0);
							}else
							{
								bForAllSucceed = FALSE;
								SetDLError();
							}
							GetProgressMonitor()->SetStateStop();
						}
						String major, suffix;
						UInt32 majorSince = 0, suffixSince = 0;
						theApp.GetConfigureVersion()->GetMajorVersion(GetPatchCurrentVersion(), major, majorSince);
						theApp.GetConfigureVersion()->GetSuffixVersion(GetPatchCurrentVersion(), suffix, suffixSince);
						m_curVersion.Format(_T("%s.%i%s"), major.c_str(), GetPatchCurrentVersion()-majorSince+1, suffix.c_str());
						GetMainDialog()->RedrawCurVerDisplay();
						if( IsInterrupted() )
							break;
					}	// for PatchUpFileList
					SetDownloadPos((UInt)m_patchUpFileList.size());
					m_errorList.clear();
					if( bForAllSucceed )
						succeeded = TRUE;
				}
			}
			break;
		case 3: // P2P (BitTorrent)
			break;
		}
	}
	SetDLStatus(PATCHER_DL_STATUSSTOP);
	GetProgressMonitor()->SetStateStop();
	return succeeded;
}

BOOL CQGLPatcherApp::DecompressPatchFile(FileSummary filesummary)
{
	BOOL succeed = TRUE;

	// Installer extraction
	// to trash possible outcomes:
	// case 1: total err
	// => exit code!=0 => update stop
	// case 2: extract err
	// => exit code!=0 => update stop
	// case 3: extract ok, folder err
	// => exit code may be = 0, but UpdatePatch detected folder error => update halts
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	String execCommand;
	TCHAR execParam[MAX_PATH];
	ZeroMemory(execParam, MAX_PATH * sizeof(TCHAR));
	BOOL commandSucceed;
	execCommand.Format(_T("%s\\%s"), m_patchDir.c_str(), filesummary.filename.c_str());
	_stprintf(execParam, _T("%s /S /D=%s"), execCommand.c_str(), m_trashDir.c_str());
	commandSucceed = CreateProcess(execCommand.c_str(), execParam, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if( commandSucceed )
	{
		// Wait until child process exits.
		DWORD retValue = WaitForSingleObject( pi.hProcess, INFINITE );
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		// check if disk full by finding the instruction file
		String cmdFilename;
		cmdFilename.Format(_T("%s\\%s"), m_trashDir.c_str(), POST_PATCH_INSTRUCTION_FILE);
		if( File::IsExists(cmdFilename.c_str()) )
		{
			// if patcher is updated in this version, set interrupt and let update.exe copy Patcher and Patcher-Res, update version number
			if( IsPatcherUpdateExist() || IsPatcherResUpdateExist() )
				SetInterrupted();

			ClearPreviousTempPatch();	// to ensure that only patch files in current version are updated

			// no matter patcher update or not, RUN UpdatePatch to update GDF and version
			// copy files to install folder
			String srcFolder, destFolder;
			srcFolder = m_trashDir.c_str();
			destFolder = m_currentDir.c_str();
			BOOL bExceptionFound = FALSE;
			if( RecursiveMoveExceptPatcher(srcFolder.c_str(), destFolder.c_str(), _T(""), bExceptionFound) )
			{
				// call UpdatePatch (or bat file calling this)
				BOOL openCmdFound = FALSE;
				File f0;
				try
				{
					String cmdFilename;
					cmdFilename.Format(_T("%s\\%s"), m_currentDir.c_str(), POST_PATCH_INSTRUCTION_FILE);
					f0.Open(cmdFilename.c_str(), File::MODE_READ_EXISTING);
					if( f0.IsOpened() )
					{
						UInt filelength = (UInt32)f0.GetLength();
						if( filelength > 0 )
						{
							openCmdFound = TRUE;
							BYTE* cmdbytes = SafeAllocate(BYTE, filelength + 1);
							ZeroMemory(cmdbytes, filelength + 1);
							f0.Read(cmdbytes, filelength);

							// RUN UpdatePatch to update GDF and version
							String localcommand = (char*)cmdbytes;
							int firstspace = (int)localcommand.find_first_of(_T(" "));
							execCommand.Format(_T("%s\\%s"), m_currentDir.c_str(), localcommand.Left(firstspace).c_str());
							ZeroMemory(execParam, MAX_PATH * sizeof(TCHAR));
							_stprintf(execParam, _T("%s %s"), execCommand.c_str(), localcommand.Right(localcommand.GetLength() - firstspace - 1).c_str());
							ZeroMemory( &si, sizeof(si) );
							si.cb = sizeof(si);
							ZeroMemory( &pi, sizeof(pi) );
							commandSucceed = CreateProcess(execCommand.c_str(), execParam, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
							if( commandSucceed )
							{
								// Wait until child process exits.
								WaitForSingleObject( pi.hProcess, INFINITE );
								CloseHandle( pi.hProcess );
								CloseHandle( pi.hThread );

								int oldver = GetPatchCurrentVersion();
								ReadPatchCurrentVersion();
								if( oldver==GetPatchCurrentVersion() )
								{
									SetDLError();
									succeed = FALSE;
								}else
								{
									execCommand.Format(_T("%s\\%s"), m_patchDir.c_str(), filesummary.filename.c_str());
									DeleteFile(execCommand.c_str());
								}
							}else
							{
								SetDLError();
								succeed = FALSE;
							}
							SafeDeallocate(cmdbytes);
						}
						f0.Close();
						DeleteFile(cmdFilename.c_str());
					}
					if( !openCmdFound )
					{
						int oldver = GetPatchCurrentVersion();
						ReadPatchCurrentVersion();
						if( oldver==GetPatchCurrentVersion() )
						{
							SetDLError();
							succeed = FALSE;
						}
					}
				}catch(...)
				{
					SetDLError();
					succeed = FALSE;
					TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::DecompressPatchFile Error (1) [_instcmd.txt open catch] File=%s, version=%i"), filesummary.filename.c_str(), filesummary.version);
				}
			}else
			{
				SetDLError();
				succeed = FALSE;
				TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::DecompressPatchFile Error (2) [recursive move failed] File=%s, version=%i"), filesummary.filename.c_str(), filesummary.version);
			}
		}else
		{
			SetDLError();
			succeed = FALSE;
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::DecompressPatchFile Error (3) [_instcmd.txt not found, probably disk full] File=%s, version=%i"), filesummary.filename.c_str(), filesummary.version);
		}
	}else
	{
		SetDLError();
		succeed = FALSE;
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CQGLPatcherApp::DecompressPatchFile Error (4) [exec cmd failed] Cmd=%s"), execCommand.c_str());
	}
	DeleteFile(DEFAULT_UPDATEPATCH_FILE);

	return succeed;
}

BOOL CQGLPatcherApp::MergePatchToPatch(PCSTRING masterfile, PCSTRING transfile)
{
	BOOL succeed = TRUE;
	String s1 = masterfile;
	String s2 = transfile;
	String d1 = masterfile;
	GDFPackageMaker packageMaker;
	if( !packageMaker.ExecuteMerge(s1.c_str(), s2.c_str(), FALSE) )
		succeed = FALSE;
	DeleteFile(transfile);
	return succeed;
}

VOID CQGLPatcherApp::StartDownloadProcess()
{
	if( !DownloadAllPatchFile() )
	{
		GetMainDialog()->MessageBox(
			GetStringTable()->Get(_T("MSG_DOWNLOAD_FILE_ERR")).c_str(),
			GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
			);
		GetMainDialog()->ReleaseConfirmDownloadUI();
		ResetDLError();
		ResetInterrupted();
		return;
	}

	if( IsDLError() )
	{
		GetMainDialog()->MessageBox(
			GetStringTable()->Get(_T("MSG_DOWNLOAD_FILE_ERR")).c_str(),
			GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
			);
		GetMainDialog()->ReleaseConfirmDownloadUI();
		ResetDLError();
		ResetInterrupted();
		return;
	}else if( IsInterrupted() )
	{
		// interrupt detected, due to Patcher found (system failure does not reach here)
		QGLCreateProcess(GetPatcherAppFilename().c_str());
		return;
	}else
	{
		// no interrupt detected, patch process completed
		SaveSelectConfiguration();
		QGLCreateProcess(GetGameAppFilename().c_str());
	}
}

BOOL CQGLPatcherApp::SelectPatchProtocol(DownloadMethod& dm)
{
	TRACE_ENSURE(m_patchFileSelectionList);

	UInt32 selected = m_patchFileSelectionList->Get();
	if( selected == -1 )
		return FALSE;

	dm = m_patchMethod[selected];
	return TRUE;
}

VOID CQGLPatcherApp::ResetPatchProtocolList()
{
	TRACE_ENSURE(m_patchFileSelectionList);
	m_patchFileSelectionList->Reset();
}

VOID CQGLPatcherApp::LoadSummaryConfig()
{
	FreeSummaryConfig();

	String groupVariable, accessVariable;
	TCHAR tempstring[100];

	groupVariable = _T("PROTOCOL");
	accessVariable = _T("count");
	m_patchProtocolCount = GetPrivateProfileInt(_T("PROTOCOL"), _T("count"), 9999,LOCAL_SUMMARY_LIST_FILE);
	for( Index i = C_INDEX(0); i < m_patchProtocolCount; ++i )
	{
		DownloadMethod dm;
		accessVariable.Format(_T("protocol%i"), i+1);		dm.type = GetPrivateProfileInt(groupVariable.c_str(), accessVariable.c_str(), 0, LOCAL_SUMMARY_LIST_FILE);
		accessVariable.Format(_T("sourceip%i"), i+1);		GetPrivateProfileString(groupVariable.c_str(), accessVariable.c_str(), _T(""), tempstring, 100, LOCAL_SUMMARY_LIST_FILE);		dm.sourceip = tempstring;
		accessVariable.Format(_T("port%i"), i+1);			dm.port = GetPrivateProfileInt(groupVariable.c_str(), accessVariable.c_str(), 0, LOCAL_SUMMARY_LIST_FILE);
		accessVariable.Format(_T("path%i"), i+1);			GetPrivateProfileString(groupVariable.c_str(), accessVariable.c_str(), _T(""), tempstring, 100, LOCAL_SUMMARY_LIST_FILE);		dm.path = tempstring;
		accessVariable.Format(_T("priority%i"), i+1);		dm.priority = GetPrivateProfileInt(groupVariable.c_str(), accessVariable.c_str(), 0, LOCAL_SUMMARY_LIST_FILE);
		m_patchMethod.push_back(dm);
	}
	// set SelectionList according to priority, for selection use
	{
		if( m_patchFileSelectionList )
			SafeDeleteObject(m_patchFileSelectionList);

		UInt32Vector indexList;
		for( Index i = C_INDEX(0); i < m_patchMethod.size(); ++i )
			indexList.push_back(i);

		DownloadMethodList tempSortList = m_patchMethod;
		for( Index i = C_INDEX(0); i < tempSortList.size() - 1; ++i )
		{
			for( Index j = 0; j < tempSortList.size() - i - 1; ++j )
			{
				DownloadMethod& dm0 = tempSortList[j];
				DownloadMethod& dm1 = tempSortList[j+1];
				if( dm0.priority < dm1.priority )
				{
					DownloadMethod temp = dm0;
					dm0 = dm1;
					dm1 = temp;

					UInt32 tempint = indexList[j];
					indexList[j] = indexList[j+1];
					indexList[j+1] = tempint;
				}
			}
		}
		UInt32Vector priorityList;
		for( Index i = C_INDEX(0); i < indexList.size(); ++i )
			priorityList.push_back(m_patchMethod[indexList[i]].priority);
		m_patchFileSelectionList = SafeCreateObject2(CSelectionList, indexList, priorityList);
	}

	groupVariable = _T("FILESUMMARY");
	accessVariable = _T("version");
	UInt targetversion = GetPrivateProfileInt(groupVariable.c_str(), accessVariable.c_str(), 0, LOCAL_SUMMARY_LIST_FILE);
	TCHAR strCRC[10];
	ZeroMemory(strCRC, 10*sizeof(TCHAR));
	accessVariable = _T("versionCRC");
	GetPrivateProfileString(groupVariable.c_str(), accessVariable.c_str(), _T(""), strCRC, 10, LOCAL_SUMMARY_LIST_FILE);
	m_patchSummaryVersionCRC = 0;
	_stscanf(strCRC, _T("%x"), &m_patchSummaryVersionCRC);

	SetPatchTargetVersion(targetversion);

}

VOID CQGLPatcherApp::LoadPatchUpFileList(UInt32 patchUpStart, UInt32 patchUpEnd)
{
	String accessVariable;
	String groupVariable(_T("FILESUMMARY"));
	TCHAR tempstring[100];
	for( Index i = patchUpStart; i < patchUpEnd; ++i )
	{
		FileSummary fs;
		fs.version = i;
		ZeroMemory(tempstring, 100*sizeof(TCHAR));
		// filename
		accessVariable.Format(_T("patch%08i"), i);	GetPrivateProfileString(groupVariable.c_str(), accessVariable.c_str(), _T(""), tempstring, 100, LOCAL_SUMMARY_LIST_FILE);
		fs.filename.Format(_T("%s"), tempstring);
		// CRC
		accessVariable.Format(_T("crc"), i);		GetPrivateProfileString(fs.filename.c_str(), accessVariable.c_str(), _T(""), tempstring, 9, LOCAL_FILELIST_FILE);
		_stscanf(tempstring, _T("%x"), &fs.filehash);
		// Version CRC
		accessVariable.Format(_T("vcrc"), i);		GetPrivateProfileString(fs.filename.c_str(), accessVariable.c_str(), _T(""), tempstring, 9, LOCAL_FILELIST_FILE);
		_stscanf(tempstring, _T("%x"), &fs.versionCRC);
		// file size
		accessVariable.Format(_T("size"), i);		fs.filesize = GetPrivateProfileInt(fs.filename.c_str(), accessVariable.c_str(), 0, LOCAL_FILELIST_FILE);
		if( fs.filesize > 0 )
			m_patchUpFileList.push_back(fs);
	}
}

BOOL CQGLPatcherApp::LoadSummaryDownloadLink(UInt32 ServerNo)
{
	m_serverSummaryList.clear();
	m_serverFileList.clear();
	m_serverRevertList.clear();
	ServerInfoList& serverList = *GetConfigureServerList()->GetServerInfoList();
	for( Index i = C_INDEX(0); i < serverList.size(); ++i )
	{
		ServerInfo& serverInfo = serverList[i];
		if( serverInfo.id==ServerNo )
		{
			for( Index j = C_INDEX(0); j < serverInfo.downloadLinkList.size(); ++j )
			{
				WebAccessData webdata;
				ConvertURLtoComponents(
					serverInfo.downloadLinkList[j].c_str(), webdata.ip, webdata.port, webdata.type, webdata.path, webdata.file
					);

				webdata.file = _T("summary.txt");
				m_serverSummaryList.push_back(webdata);
				webdata.file = _T("filelist.txt");
				m_serverFileList.push_back(webdata);
				webdata.file = _T("revertlst.txt");
				m_serverRevertList.push_back(webdata);
			}
			break;
		}
	}
	return TRUE;
}

VOID CQGLPatcherApp::FreeSummaryConfig()
{
	m_patchMethod.clear();
	m_patchUpFileList.clear();
	m_patchProtocolCount = 0;
	SetPatchTargetVersion(0);
}

void CQGLPatcherApp::InitLogger()
{
	Reuben::Utility::ConfigFile config(CONFIGURATION_INI_SYS_FILE);
	gGlobalLogger = SafeCreateObject1(Logger, _T("QGLPatcher"));
	gGlobalLogger->Open(config);

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void CQGLPatcherApp::CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

VOID CQGLPatcherApp::RecursiveMove(CONST String SrcFolder, CONST String TargetFolder)
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
			if( !MoveUnzipFile(FullPath, target) )
			{
				TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::RecursiveMove Error (1) [cannot move file from %s to %s]"), FullPath.c_str(), target.c_str());
				String text;
				text.Format(GetStringTable()->Get(_T("MSG_FILEMOVE_ERR")).c_str(), FullPath.c_str(), target.c_str());
				GetMainDialog()->MessageBox(
					text.c_str(),
					GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
					);
			}
		}
	}
	RemoveDirectory(SrcFolder.c_str());
}

// Return TRUE if succeeded, FALSE if failed.  bException is TRUE when PATCHER file found, FALSE if not.
BOOL CQGLPatcherApp::RecursiveMoveExceptPatcher(LPCTSTR SrcFolder, LPCTSTR TargetFolder, LPCTSTR RelativePath, BOOL& bExceptionFound)
{
	BOOL bIsExceptionFound	= FALSE;
	BOOL bSuccess			= TRUE;

	BlockVector blocklist;
	String tempword;
	tempword = _T(".");
	blocklist.push_back(tempword);
	tempword = _T("..");
	blocklist.push_back(tempword);

	String NewSearchCriteria;
	NewSearchCriteria.Format(_T("%s\\*.*"), SrcFolder);

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = FindFirstFile(NewSearchCriteria.c_str(), &FileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL fFinished		= FALSE;
		while (!fFinished) 
		{ 
			// delete found files
			BOOL found			= FALSE;
			for( Index i = C_INDEX(0); i < blocklist.size(); ++i )
			{
				if( blocklist[i].CompareNoCase(FileData.cFileName)==0 )
				{
					found = TRUE;
					break;
				}
			}
			if( !found )
			{
				// if current folder is "RES" and some file is found, set Patcher-file as Found.
				if( ::_tcsicmp(RelativePath, PATCHERRESNAME_FILE)==0 )
				{
					bIsExceptionFound = TRUE;
				}else
				{
					String NewRelativePath;
					if( _tcslen(RelativePath) > 0 )
						NewRelativePath.Format(_T("%s\\%s"), RelativePath, FileData.cFileName);
					else
						NewRelativePath.Format(_T("%s"), FileData.cFileName);
					String NewSrcFolder;
					NewSrcFolder.Format(_T("%s\\%s"), SrcFolder, FileData.cFileName);
					DWORD attr = GetFileAttributes(NewSrcFolder.c_str());
					if( attr & FILE_ATTRIBUTE_DIRECTORY )
					{
						// Recursive loop on Next Directory
						BOOL bSubExceptFound = FALSE;
						String NewTargetFolder;
						NewTargetFolder.Format(_T("%s\\%s"), TargetFolder, FileData.cFileName);
						// Move folder
						if( !RecursiveMoveExceptPatcher(NewSrcFolder.c_str(), NewTargetFolder.c_str(), NewRelativePath.c_str(), bSubExceptFound) )
							bSuccess = FALSE;
						if( bSubExceptFound )
							bIsExceptionFound = TRUE;
					}else
					{
						if( NewRelativePath.CompareNoCase(GetThisAppFilename().c_str())==0 )
						{
							bIsExceptionFound = TRUE;
							// Don't move anything
						}else
						{
							// Move File to TargetFolder
							String param1 = NewSrcFolder;
							String param2 = TargetFolder;
							if( !MoveUnzipFile(param1, param2) )
							{
								ErrorMap err;
								err.src		= NewSrcFolder;
								err.dest	= TargetFolder;
								m_errorList.push_back(err);
								bSuccess = FALSE;
							}
						}
					}
				}
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
	// if Patcher file is NOT FOUND and ALL MOVE actions are successful, remove directory.
	if( !bIsExceptionFound && bSuccess )
		RemoveDirectory(SrcFolder);
	bExceptionFound = bIsExceptionFound;
	return bSuccess;
}

BOOL CQGLPatcherApp::MoveUnzipFile(CONST String SrcPath, CONST String TargetFolder)
{
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
	
	if( !MoveFileEx(SrcPath.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) )
		return FALSE;
	
	return TRUE;
}

BOOL CQGLPatcherApp::CreateFolder(LPCTSTR szFolder) // Copy from Unzipper.cpp
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
	free(szPath);
	abc.Replace(_T("/"), _T("\\"));
	TCHAR* p = (TCHAR*)_tcsrchr(abc.c_str(), '\\');

	if (p) 
	{
		// The parent is a dir, not a drive
		*p = '\0';
			
		// if can't create parent
		if (!CreateFolder(abc.c_str()))
		{
			return FALSE;
		}
	}
	if (!::CreateDirectory(szFolder, NULL)) 
		return FALSE;
	
	return TRUE;
}

void CQGLPatcherApp::RemoveDirectoryRecursive(CString szPath)
{
	// Removes the folder szPath after removing all its
	// contents including its sub-folders and their contents
	// 
	// The following block {..} limits the scope of ff
	{
		CFileFind ff;
		CString path = szPath;

		if(path.Right(1) != "\\")
			path += "\\";

		path += "*.*";

		BOOL res = ff.FindFile(path);

		while(res)
		{
			res = ff.FindNextFile();
			if (ff.IsDots())
				continue;

			if (ff.IsDirectory())
			{
				path = ff.GetFilePath();
				RemoveDirectoryRecursive(path);
				RemoveDirectory(path);
			}
			else
				DeleteFile(ff.GetFilePath());
		}
	}

	// Remove szPath itself...

	// ...Now everything under folder szPath has been removed
	// and since ff is now out of scope so it no longer has
	// open a handle to folder szPath

	RemoveDirectory(szPath);
}

BOOL CQGLPatcherApp::IsPatcherUpdateExist()
{
	String buf;
	buf.Format(_T("%s\\%s"), m_trashDir.c_str(), _T("QGLPatcherU.exe"));
	if( ::IsFileExist(buf.c_str()) )
		return TRUE;
	return FALSE;
}

BOOL CQGLPatcherApp::IsPatcherResUpdateExist()
{
	String buf;
	buf.Format(_T("%s\\%s"), m_trashDir.c_str(), _T("res"));
	if( ::IsFileExist(buf.c_str()) )
		return TRUE;
	return FALSE;
}

VOID CQGLPatcherApp::SaveSelectConfiguration()
{
	if( !theApp.GetConfigureServerList()->GetServerInfoList() )
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CQGLPatcherApp::SaveSelectConfiguration() Error (1) [server list null]"));
		return;
	}

	// Save setting [patch.ini]
	// enter Proxy IP value to Config\Setting.ini
	ServerInfoList& serverList = *theApp.GetConfigureServerList()->GetServerInfoList();
	for( Index i = C_INDEX(0); i < serverList.size(); ++i )
	{
		ServerInfo& server = serverList[i];
		if( server.id==GetSelectedServer() )
		{
			theApp.GetConfigureApp()->SaveLastSelection(server.id, server.name.c_str(), GetSelectedZone()->GetName().c_str());
			break;
		}
	}

	// Save setting to QGL Game Config. [Config.ini]
	// removal of existing Proxy IP values in Config\Setting.ini
	UInt32 numExist = 0;
	UInt32 port = 1;
	while( port!=0 )
	{
		String strAttrib;
		strAttrib.Format(_T("ProxyPort%d"), numExist+1);
		port = GetPrivateProfileInt(_T("Client"), strAttrib.c_str(), 0, CONFIGURATION_GAME_INI_FILE);
		if( port )
		{
			String strip(_T("0.0.0.0"));
			String strport(_T("0"));
			strAttrib.Format(_T("ProxyIP%d"), numExist+1);
			WritePrivateProfileString(_T("Client"), strAttrib.c_str(), strip.c_str(), CONFIGURATION_GAME_INI_FILE);
			strAttrib.Format(_T("proxyPort%d"), numExist+1);
			WritePrivateProfileString(_T("Client"), strAttrib.c_str(), strport.c_str(), CONFIGURATION_GAME_INI_FILE);
		}
		++numExist;
	}

	// enter Proxy IP value to Config\Setting.ini
	for( Index i = C_INDEX(0); i < serverList.size(); ++i )
	{
		ServerInfo& server = serverList[i];
		if( server.id==GetSelectedServer() )
		{
			ProxyIPList& proxyList = server.proxyList;
			for( Index j = C_INDEX(0); j < proxyList.size(); ++j )
			{
				ProxyIP& proxy = proxyList[j];

				String strip, strport;
				strip.Format(_T("%d.%d.%d.%d"), proxy.pos1, proxy.pos2, proxy.pos3, proxy.pos4);
				strport.Format(_T("%d"), proxy.port);

				String strAttrib;
				strAttrib.Format(_T("ProxyIP%d"), j+1);
				WritePrivateProfileString(_T("Client"), strAttrib.c_str(), strip.c_str(), CONFIGURATION_GAME_INI_FILE);
				strAttrib.Format(_T("proxyPort%d"), j+1);
				WritePrivateProfileString(_T("Client"), strAttrib.c_str(), strport.c_str(), CONFIGURATION_GAME_INI_FILE);
			}
			break;
		}
	}
}

VOID CQGLPatcherApp::GetCurrentFolderInfo()
{
	TCHAR pBuf[MAX_PATH];
	GetModuleFileName(0, pBuf, MAX_PATH);
	String fullPath = pBuf;
	int lastpos = (int)fullPath.find_last_of(_T("\\"));
	m_currentDir = fullPath.Left(lastpos).c_str();
	m_trashDir.Format(_T("%s\\%s"), m_currentDir.c_str(), FULL_INSTALL_PATH_TEMP);
	m_patchDir.Format(_T("%s\\%s"), m_currentDir.c_str(), FULL_INSTALL_PATH_PATCH);
}

VOID CQGLPatcherApp::CreateDirStructure()
{
	_tmkdir(m_patchDir.c_str());
	_tmkdir(m_trashDir.c_str());
}

VOID CQGLPatcherApp::ClearPreviousTempPatch()
{
	if( m_currentDir.IsEmpty() )
		GetCurrentFolderInfo();

	String clearDir;
	clearDir.Empty();
	clearDir.Format(_T("%s\\_*.gdf.patch"), m_currentDir.c_str());

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = FindFirstFile(clearDir.c_str(), &FileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL fFinished = FALSE;

		while (!fFinished) 
		{ 
			// delete found files
			String delFile;
			delFile.Empty();
			delFile.Format(_T("%s\\%s"), m_currentDir.c_str(), FileData.cFileName);
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
}

UInt32 CQGLPatcherApp::GetSummarySelection()
{
	if( m_serverSummaryList.size()==0 || m_serverFileList.size()==0 || m_serverRevertList.size()==0 )	// invalid input, return failed
		return -1;
	if( m_serverSummaryList.size() != m_serverFileList.size() || m_serverRevertList.size()!=m_serverFileList.size()  )
		return -1;

	if( m_summaryTimes >= m_serverSummaryList.size() )		// tried all possible selection, but all failed
		return -1;

	if( m_summaryStart==-1 )		// if initial number is -1, get 1 number from seed
	{
		Reuben::Math::Random rand;
		rand.Randomize();
		m_summaryStart	= rand.GetRandom((Int)m_serverSummaryList.size());
	}

	return ((m_summaryStart + m_summaryTimes) % m_serverSummaryList.size());
}

VOID CQGLPatcherApp::SummarySelectionChange()
{
	m_summaryTimes += 1;
}

VOID CQGLPatcherApp::ResetSummarySelection()
{
	m_summaryTimes	= 0;
	m_summaryStart	= -1;
}

BOOL CQGLPatcherApp::FTPDownload(LPCTSTR cszIP, UInt uPort, LPCTSTR cszUsername, LPCTSTR cszPassword, LPCTSTR cszPath, LPCTSTR cszFilename, LPCTSTR cszLocalFilename)
{
	String strUrlPath;
	strUrlPath.Format(_T("ftp://%s/%s/%s"), cszIP, cszPath, cszFilename);

	CFTPClient pFtpClient;
	return pFtpClient.Download(cszLocalFilename, strUrlPath.c_str(), cszUsername, cszPassword);;
}

BOOL CQGLPatcherApp::HTTPDownload(LPCTSTR cszIP, UInt uPort, LPCTSTR cszUsername, LPCTSTR cszPassword, LPCTSTR cszPath, LPCTSTR cszFilename, LPCTSTR cszLocalFilename)
{
	String strUrlPath;
	strUrlPath.Format(_T("http://%s/%s/%s"), cszIP, cszPath, cszFilename);

	CHTTPClient pHttpClient;
	return pHttpClient.Download(cszLocalFilename, strUrlPath.c_str(), cszUsername, cszPassword);;
}

BOOL CQGLPatcherApp::FTPDownload_Testing()
{
	// ftp://ftp.microsoft.com/bussys/1394/OHCI/Draft_Specs/ohci1206.pdf		should succeed
	TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test2 start"));
	if( !FTPDownload(_T("ftp.microsoft.com"), 21, NULL, NULL, _T("bussys/1394/OHCI/Draft_Specs"), _T("ohci1206.pdf"), _T("ohci1206.pdf")) )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test2 failed"));
	// ftp://ftp.cse.cuhk.edu.hk/pub1/csc3230/DINOSAUR_REPORT_99610203.zip		should fail
	TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test3 start"));
	if( !FTPDownload(_T("ftp.cse.cuhk.edu.hk"), 21, NULL, NULL, _T("pub1/csc3230"), _T("DINOSAUR_REPORT_99610203.zip"), _T("DINOSAUR_REPORT_99610203.zip")) )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test3 failed"));

	TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Tests finished"));
	return TRUE;
}

BOOL CQGLPatcherApp::HTTPDownload_Testing()
{
	//// http://ftp.chg.ru/pub/opensuse/distribution/11.0/iso/dvd/openSUSE-11.0-DVD-i386.iso		should succeed
	//TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test4 start"));
	//if( !HTTPDownload(_T("ftp.chg.ru"), 80, NULL, NULL, _T("pub/opensuse/distribution/11.0/iso/dvd"), _T("openSUSE-11.0-DVD-i386.iso"), _T("openSUSE-11.0-DVD-i386.iso")) )
	//	TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test4 failed"));

	// http://download.winzip.com/winzip120.exe
	TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test4 start"));
	if( !HTTPDownload(_T("download.winzip.com"), 80, NULL, NULL, _T(""), _T("winzip120.exe"), _T("winzip120.exe")) )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test4 failed"));
	//  http://www.drweb.de/img/christmas-wallpaper-4.jpg		should succeed
	TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test5 start"));
	if( !HTTPDownload(_T("www.drweb.de"), 80, NULL, NULL, _T("img"), _T("christmas-wallpaper-4.jpg"), _T("christmas-wallpaper-4.jpg")) )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Test5 failed"));

	TRACE_ERRORDTL(GLOBAL_LOGGER, _T("Tests finished"));
	return TRUE;
}

ZoneMap CQGLPatcherApp::GetDisplayMapZone()
{
	ZoneMap zonemap;
	zonemap.clear();
	GetUIManagerZoneList()->GetCurPageContent(zonemap);
	return zonemap;
}

ServerInfoPtrList CQGLPatcherApp::GetDisplayServerInfoList()
{
	ServerInfoPtrList serverList;
	if( GetSelectedZone() )
		GetSelectedZone()->GetServerList(serverList);
	return serverList;
}

BOOL CQGLPatcherApp::CheckPatcherValid(UInt32 errId, String& errString)
{
	errId = 0;
	errString.Empty();
	while( true )
	{
		if( !IsWritable(_T("QGLPU.bin")) )
		{
			errString	= _T("QGLPU.bin is running.");
			errId		= 2;
			TRACE_INFODTL_1(GLOBAL_LOGGER, _T("CheckPatcherValid (1) [obstacle] errString=%s"), errString.c_str());
			break;
		}
		if( !IsWritable(DEFAULT_UPDATEPATCH_FILE) )
		{
			errString.Format(_T("%s is running"), DEFAULT_UPDATEPATCH_FILE);
			errId		= 3;
			TRACE_INFODTL_1(GLOBAL_LOGGER, _T("CheckPatcherValid (2) [obstacle] errString=%s"), errString.c_str());
			break;
		}
		if( !IsWritable(_T("UpdateU.bin")) )
		{
			errString	= _T("UpdateU.bin is running.");
			errId		= 4;
			TRACE_INFODTL_1(GLOBAL_LOGGER, _T("CheckPatcherValid (3) [obstacle] errString=%s"), errString.c_str());
			break;
		}
		if( !IsWritable(_T(".\\Data\\Data.gdf")) )
		{
			errString	= _T("Data.gdf cannot be written.");
			errId		= 4;
			TRACE_INFODTL_1(GLOBAL_LOGGER, _T("CheckPatcherValid (4) [obstacle] errString=%s"), errString.c_str());
			break;
		}
		break;
	}
	if( errId!=0 || !errString.IsEmpty() )
		return FALSE;
	return TRUE;
}

BOOL CQGLPatcherApp::IsWritable(PCSTRING pName)
{
	TRACE_ENSURE(pName);

	FILE* f0;
	if( (f0 = _tfopen(pName, _T("a")))!=NULL )
	{
		fclose(f0);
		return TRUE;
	}
	return FALSE;
}

BOOL CQGLPatcherApp::SingleTest(PCSTRING szName)
{
	HANDLE hMutex = CreateMutex (NULL, TRUE, szName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		return FALSE;
	}
	return TRUE;
}

VOID CQGLPatcherApp::LookupDefaultServer(PCSTRING serverName)
{
	if( GetConfigureServerList()->GetServerInfoList() )
	{
		ServerInfoList& serverInfoList = *GetConfigureServerList()->GetServerInfoList();
		for( Index i = C_INDEX(0); i < serverInfoList.size(); ++i )
		{
			ServerInfo& info = serverInfoList[i];
			if( !_tcsicmp(serverName, info.name.c_str()) )
			{
				theApp.SetDefaultServer(info.id);
				break;
			}
		}
	}
}

VOID CQGLPatcherApp::LookupDefaultZone(PCSTRING zoneName)
{
	if( GetConfigureZoneList()->GetZoneMap() )
	{
		ZoneMap& zoneMap = *GetConfigureZoneList()->GetZoneMap();
		for( ZoneMap::iterator itr = zoneMap.begin(); itr != zoneMap.end(); ++itr )
		{
			String& listName = itr->second.GetName();
			UInt32 zoneId = itr->first;
			if( !listName.CompareNoCase(zoneName) )
			{
				theApp.SetDefaultZone(zoneId);
				break;
			}
		}
	}
}

VOID CQGLPatcherApp::LookupDefaultServerSetting()
{
	if( GetSelectedServer()==NO_SERVER_SELECTED )
	{
		// Set Default Zone and Server
		LookupDefaultServer(GetConfigureApp()->m_lastservername.c_str());
		LookupDefaultZone(GetConfigureApp()->m_lastzonename.c_str());
		if( GetDefaultServer()!=NO_SERVER_SELECTED )
		{
			SetSelectedServer(GetDefaultServer());
			if( GetConfigureZoneList()->GetZone(GetDefaultZone()) )
				SetSelectedZone(*GetConfigureZoneList()->GetZone(GetDefaultZone()));
			// validate server and its relationship with zone,
			Zone& zone = *GetSelectedZone();
			ServerInfoList serverInfoList;
			zone.GetServerList(serverInfoList);
			for( Index i = C_INDEX(0); i < serverInfoList.size(); ++i )
			{
				ServerInfo& server = serverInfoList[i];
				if( server.id==GetSelectedServer() )
				{
					// SetDefaultExists only when
					// 1a) Server is found in Config;
					// 1b) Zone is found in Config;
					// 2a) Server is found in download list;
					// 2b) Zone is found in download list;
					// 3) Server is found in corresponding Zone saved in Config.
					SetDefaultExists();
					break;
				}
			}
			// if invalid, assign that server to the first zone contained it
			if( !GetDefaultExists() )
			{
				ZoneMap& zoneMap = *GetConfigureZoneList()->GetZoneMap();
				for( ZoneMap::iterator itr = zoneMap.begin(); itr != zoneMap.end(); ++itr )
				{
					UInt zoneId = itr->first;
					Zone& zone = itr->second;

					ServerInfoList serverInfoList;
					zone.GetServerList(serverInfoList);
					BOOL bFound = FALSE;
					for( Index i = C_INDEX(0); i < serverInfoList.size(); ++i )
					{
						ServerInfo& server = serverInfoList[i];
						if( server.id==GetSelectedServer() )
						{
							bFound = TRUE;
							break;
						}
					}
					if( bFound )	// another instance of zone is found to contain this server
					{
						SetSelectedZone(zone);
						break;
					}
				}
			}
		}else	// no server found -> default select the first
		{
			if( GetConfigureZoneList()->GetZoneMap() &&
				GetConfigureZoneList()->GetZoneMap()->size() > 0 )
			{
				// Set first zone as Selected Zone
				Zone& zone = GetConfigureZoneList()->GetZoneMap()->begin()->second;
				SetSelectedZone(zone);
				// Set first zone's first server as Selected Server
				ServerInfoList serverInfoList;
				zone.GetServerList(serverInfoList);
				if( serverInfoList.size() > 0 )
					SetSelectedServer(serverInfoList[0].id);
			}else
			{
				TRACE_WARNDTL(GLOBAL_LOGGER, _T("CQGLPatcherDlg::ShowPage Warning (1) [zonelist null or empty]"));
			}
		}
	}
}

VOID CQGLPatcherApp::UpdateProgressMonitor()
{
	// check the current status of download and update
	struct _stat st;
	_tstat(GetProgressMonitor()->GetFullFilePath().c_str(), &st);
	GetProgressMonitor()->SetFileCurrentSize(st.st_size);
}

VOID CQGLPatcherApp::PatcherPatchFileCheck(PCSTRING filename, UInt32 expectedSize, UInt32 expectedCRC, BOOL& bDownloadIt, BOOL& bRedownloadIt)
{
	bDownloadIt = TRUE;
	bRedownloadIt = FALSE;
	if( Reuben::Platform::File::IsExists(filename) )
	{
		struct _stat st;
		_tstat(filename, &st);
		if( (UInt32)st.st_size > expectedSize )
		{
			bRedownloadIt = TRUE;
		}else if( st.st_size==expectedSize )
		{
			UInt32 crc = 0;
			Reuben::Math::CRC32(filename, crc);
			if( crc==expectedCRC )
			{
				bDownloadIt = FALSE;
			}else
			{
				bRedownloadIt = TRUE;
			}
		}
	}
}

VOID CQGLPatcherApp::SetPatchTargetVersion(UInt ver)
{
	m_patchTargetVersion = ver;
	if( ver )
	{
		String major, suffix;
		UInt32 majorSince = 0, suffixSince = 0;
		theApp.GetConfigureVersion()->GetMajorVersion(ver, major, majorSince);
		theApp.GetConfigureVersion()->GetSuffixVersion(ver, suffix, suffixSince);
		m_serVersion.Format(_T("%s.%i%s"), major.c_str(), ver-majorSince+1, suffix.c_str());
	}else
	{
		m_serVersion = _T("--");
	}
};

VOID CQGLPatcherApp::SetPatchCurrentVersion(UInt ver)
{
	m_patchCurrentVersion = ver;
	String major, suffix;
	UInt32 majorSince = 0, suffixSince = 0;
	theApp.GetConfigureVersion()->GetMajorVersion(ver, major, majorSince);
	theApp.GetConfigureVersion()->GetSuffixVersion(ver, suffix, suffixSince);
	m_curVersion.Format(_T("%s.%i%s"), major.c_str(), ver-majorSince+1, suffix.c_str());
};

BOOL CQGLPatcherApp::QGLCreateProcess(PCSTRING appName)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	return CreateProcess(appName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
