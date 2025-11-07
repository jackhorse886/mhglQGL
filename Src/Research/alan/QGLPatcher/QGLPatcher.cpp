// QGLPatcher.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "common.h"
#include "QGLPatcher.h"
#include "QGLPatcherDlg.h"
#include "GDFPackager/GDFPackageMaker.h"
#include "GDFPackager/CFindFile.h"
#include "Reuben/Math/Random.h"
#include "Misc/Stackwalker.h"
#include "Reuben/Math/CRC.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#define STREAM_BUF_MAX	65536
#define MAX_LIMIT_TRY 3
#define MAX_LIMIT_DELAY 2000

// CQGLPatcherApp

BEGIN_MESSAGE_MAP(CQGLPatcherApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

DWORD WINAPI DLHttpFileFunction(IN LPVOID vThreadParm);

typedef struct
{
	String		urlstring;
	CHttpFile*	pFile;
	CHttpConnection* pHttpConnection;
} PARM;

// CQGLPatcherApp 建構

CQGLPatcherApp::CQGLPatcherApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
	m_stringTable.Load(GAMESTRINGTABLE_LOCALIZATION_FILENAME);
	InitLogger();
}

CQGLPatcherApp::~CQGLPatcherApp()
{
	FreeGeneralConfig();
	FreeTrackerConfig();
	FreeServerListConfig();
	FreeSummaryConfig();
	CloseLogger();
}


// 僅有的一個 CQGLPatcherApp 物件

CQGLPatcherApp theApp;


// CQGLPatcherApp 初始設定

BOOL CQGLPatcherApp::InitInstance()
{
	if (!Reuben::System::Initialize())
		return (-2);

	REUBEN_SYSTEM_MEMORY_END_LEAK_CHECK();
	InitAllocCheck(ACOutput_XML);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 標準初始設定
	// 如果您不使用這些功能並且想減少最後完成的可執行檔大小，您可以從下列
	// 程式碼移除不需要的初始化常式，變更儲存設定值的登錄機碼
	// TODO: 您應該適度修改此字串 (例如，公司名稱或組織名稱)
//	SetRegistryKey(_T("本機 AppWizard 所產生的應用程式"));

	SetDefaultServer(0);
	SetDefaultLine(0);
	SetSelectedServer(0);
	SetSelectedLine(0);
	m_curVersion		= _T("");
	m_serVersion		= _T("--");
	SetPatchCurrentVersion(0);
	m_interrupted		= FALSE;
	m_dlerror			= FALSE;

	m_pSession			= NULL;
	pHttpConnection		= NULL;
	pFtpConnection		= NULL;
	m_pTCPClient		= NULL;
	m_trackerList.clear();

	SetDownloadPos(0);

	if( !LoadGeneralConfig() )
		return FALSE;

	CreateDirStructure();

	if( !LoadTrackerConfig() )
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("QGLPatcher::Init Error (1) [Tracker not found.  Default is used instead.]"));

	m_errorList.clear();

	if( !RecursiveMoveUnlessPatcher(m_trashDir.c_str(), m_currentDir.c_str()) )
	{
		RemoveDirectoryRecursive(m_trashDir.c_str());	// remove temp trash folder

		SetCurPage(PATCHER_PAGE1);
		SetCurStatus(PATCHER_FREENAVIGATE);

		CQGLPatcherDlg dlg;
		m_curMainDlg = &dlg;
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

		Reuben::System::Uninitialize();
		REUEBN_SYSTEM_MEMORY_REGISTER_LEAK_REPORT(GLOBAL_LOGGER.GetLogFileName().c_str());

		DeInitAllocCheck();
		REUBEN_SYSTEM_MEMORY_END_LEAK_CHECK();

		// 因為已經關閉對話方塊，傳回 FALSE，所以我們會結束應用程式，
		// 而非提示開始應用程式的訊息。
	}
	else	// exit to show error or restart
	{
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
		}else
		{
			ShellExecute(0, _T("open"), GetPatcherAppFilename().c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
	m_errorList.clear();
	return FALSE;
}

BOOL CQGLPatcherApp::ReadPatchCurrentVersion()
{
	TCHAR pBuf[MAX_PATH];

	GetPrivateProfileString(_T("APP"), _T("patcherversion"), _T("\0"), pBuf, MAX_PATH, CONFIGURATION_INI_FILE);
	m_curVersion = pBuf;
	UInt ver = _ttoi(pBuf);
	if( ver==0 )
		return FALSE;

	SetPatchCurrentVersion(ver);
	return TRUE;
}

BOOL CQGLPatcherApp::LoadGeneralConfig()
{
	FreeGeneralConfig();

	GetCurrentFolderInfo();

	UInt uBuf;
	TCHAR pBuf[MAX_PATH];
	String accessVariable;

	BOOL success = TRUE;

	if( !ReadPatchCurrentVersion() )
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("QGLPatcher::LoadGeneralConfig Error (1) [patch.ini, APP[patcherversion] reading error]"));
		GetMainDialog()->MessageBox(
			GetStringTable()->Get(_T("MSG_CONFIGURATION_ERR")).c_str(),
			GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
			);
	}

	GetPrivateProfileString(_T("APP"), _T("startgame"), _T("QGL.exe"), pBuf, MAX_PATH, CONFIGURATION_INI_FILE);
	SetGameAppFilename(pBuf);
	GetPrivateProfileString(_T("APP"), _T("startpatcher"), _T("UPDATE.exe"), pBuf, MAX_PATH, CONFIGURATION_INI_FILE);
	SetPatcherAppFilename(pBuf);
	GetPrivateProfileString(_T("APP"), _T("startrestart"), _T("QGLPatcher.exe"), pBuf, MAX_PATH, CONFIGURATION_INI_FILE);
	SetThisAppFilename(pBuf);

	accessVariable.Format(_T("lastserver"));	GetPrivateProfileString(_T("APP"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, CONFIGURATION_INI_FILE);
	m_defaultServerIP = pBuf;
	accessVariable.Format(_T("lastport"));		uBuf = GetPrivateProfileInt(_T("APP"), accessVariable.c_str(), 0, CONFIGURATION_INI_FILE);
	m_defaultServerPort = uBuf;

	if( !success )
		FreeGeneralConfig();
	
	return success;
}

BOOL CQGLPatcherApp::LoadServerListConfig()
{
	FreeServerListConfig();

	BOOL succeed = TRUE;
	BOOL serverCompleted;

	String accessVariable;
	UInt uBuf;
	TCHAR pBuf[MAX_PATH];

	UInt sizeOfServer = GetPrivateProfileInt(_T("SERVERLIST"), _T("count"), 9999, LOCAL_TRACKER_LIST_FILE);
	if( sizeOfServer==9999 ) return FALSE;

	for( Index i = C_INDEX(0); i < sizeOfServer; ++i )
	{
		serverCompleted = TRUE;

		ServerData tempserver;
		// server basic info
		if( serverCompleted )
		{
			accessVariable.Format(_T("name%i"), i+1);		GetPrivateProfileString(_T("SERVERLIST"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, LOCAL_TRACKER_LIST_FILE);
			tempserver.name = pBuf;
			accessVariable.Format(_T("status%i"), i+1);		uBuf = GetPrivateProfileInt(_T("SERVERLIST"), accessVariable.c_str(), 9999, LOCAL_TRACKER_LIST_FILE);
			tempserver.status = uBuf;
			if( uBuf==9999 ) 
				serverCompleted = FALSE;
		}
		// server connection (line) info
		if( serverCompleted )
		{
			accessVariable.Format(_T("linecount%i"), i+1);		
			UInt linecount = GetPrivateProfileInt(_T("SERVERLIST"), accessVariable.c_str(), 9999, LOCAL_TRACKER_LIST_FILE);
			if( linecount==9999 )
			{
				serverCompleted = FALSE;
			}else
			{
				for( Index j = C_INDEX(0); j < linecount; ++j )
				{
					LineCombination templine;
					accessVariable.Format(_T("linename%i_%i"), i+1, j+1);		GetPrivateProfileString(_T("SERVERLIST"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, LOCAL_TRACKER_LIST_FILE);
					templine.lineName = pBuf;
					accessVariable.Format(_T("lineip%i_%i"), i+1, j+1);			GetPrivateProfileString(_T("SERVERLIST"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, LOCAL_TRACKER_LIST_FILE);
					templine.ip = pBuf;
					accessVariable.Format(_T("lineport%i_%i"), i+1, j+1);		uBuf = GetPrivateProfileInt(_T("SERVERLIST"), accessVariable.c_str(), 0, LOCAL_TRACKER_LIST_FILE);
					templine.port = uBuf;
					tempserver.server.push_back(templine);
				}
			}
		}
		if( serverCompleted )
			m_serverData.push_back(tempserver);
		
		succeed = succeed && serverCompleted;
	}
	// set selected server
	for( Index i = C_INDEX(0); i < sizeOfServer; ++i )
	{
		UInt linecount = (UInt)m_serverData[i].server.size();
		for( Index j = C_INDEX(0); j < linecount; ++j )
		{
			if( m_serverData[i].server[j].ip.Compare(m_defaultServerIP.c_str())==0 && m_serverData[i].server[j].port==m_defaultServerPort )
			{
				SetDefaultServer(i);
				SetSelectedServer(i);
				SetDefaultLine(j);
				SetSelectedLine(j);
			}
		}
	}
	return succeed;
}

BOOL CQGLPatcherApp::LoadTrackerConfig()
{
	BOOL succeed = TRUE;

	TCHAR pBuf[MAX_PATH];
	String accessVariable;
	// tracker IP address configuration
	UInt trackerCount = GetPrivateProfileInt(_T("TRACKERIP"), _T("count"), 0, CONFIGURATION_INI_FILE);
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
			GetPrivateProfileString(_T("TRACKERIP"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, CONFIGURATION_INI_FILE);
			String fullAddress = pBuf;

			String ipaddress, path, file;
			UInt port, type;
			ConvertURLtoComponents(fullAddress, ipaddress, port, type, path, file);

			// last. gather all information up and store
			webdata.type	= type;
			webdata.ip		= ipaddress;
			webdata.port	= port;
			webdata.path	= path;
			webdata.file	= file;
			m_trackerList.push_back(webdata);
		}
	}
	return succeed;
}

VOID CQGLPatcherApp::ConvertURLtoComponents(String& fulladdress, String& ipaddress, UInt& port, UInt& type, String& path, String& file)
{
	String ipWithPort;
	String protocol;

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
		WritePrivateProfileString(_T("TRACKERIP"), _T("count"), tempvar.c_str(), CONFIGURATION_INI_FILE);
		for( Index i = C_INDEX(0); i < trackerCount; ++i )
		{
			accessVariable.Format(_T("server%i"), i+1);
			GetPrivateProfileString(_T("TRACKERIP"), accessVariable.c_str(), _T("\0"), tempip, 100, LOCAL_TRACKER_LIST_FILE);
			WritePrivateProfileString(_T("TRACKERIP"), accessVariable.c_str(), tempip, CONFIGURATION_INI_FILE);
		}
	}
	return success;
}


BOOL CQGLPatcherApp::DownloadTrackerIpIni()
{
	SetCurStatus(PATCHER_TRACKER_DOWNLOADING);

	String flag_dont_cache;
	flag_dont_cache.Format(_T("%i"), INTERNET_FLAG_DONT_CACHE);

	BOOL bSuccess = FALSE;
	WebAccessData curWebData;
	UInt32 retries = 0;
	while(true)
	{
		do
		{
			curWebData = SelectTrackerData();
			switch( curWebData.type )
			{
			case 1:		// HTTP protocol
				{
					m_pSession = SafeCreateObject6(CInternetSessionEx, AfxGetApp()->m_pszAppName, 1, INTERNET_OPEN_TYPE_DIRECT, _T("0"), _T("0"), INTERNET_FLAG_DONT_CACHE);
					//	m_pSession->hWnd	= g_hWnd;
					m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 15);
					m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);
					m_pSession->EnableStatusCallback();

					pHttpConnection		= NULL;
					try
					{
						pHttpConnection = m_pSession->GetHttpConnection(curWebData.ip.c_str(), curWebData.port, FTP_USERNAME, FTP_PASSWORD);
						if( pHttpConnection )
						{
							String finalpath;
							if( curWebData.path.GetLength() )
								finalpath.Format(_T("/%s/%s"), curWebData.path.c_str(), curWebData.file.c_str());
							else
								finalpath.Format(_T("/%s"), curWebData.file.c_str());
							CHttpFile* pFile1;
							//pFile1->AddRequestHeaders(_T("User-Agent: GetWebFile/1.0\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
							pFile1 = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, finalpath.c_str());

							PARM parm;
							parm.urlstring			= finalpath;
							parm.pHttpConnection	= pHttpConnection;
							parm.pFile				= pFile1;

							HANDLE   hThread; 
							DWORD    dwThreadID;
							hThread = ::CreateThread(
								NULL,            // Pointer to thread security attributes 
								0,               // Initial thread stack size, in bytes 
								DLHttpFileFunction,  // Pointer to thread function 
								&parm,     // The argument for the new thread
								0,               // Creation flags 
								&dwThreadID      // Pointer to returned thread identifier 
								); 

							// Wait for the call to InternetConnect in worker function to complete
							UInt dwTimeout = 5000; // in milliseconds
							if ( WaitForSingleObject ( hThread, dwTimeout ) == WAIT_TIMEOUT )
							{
								pFile1->Abort();
	//							WaitForSingleObject ( hThread, INFINITE );
							}else
							{
								DWORD msg;
								pFile1->QueryInfo(HTTP_QUERY_STATUS_CODE, msg, NULL);
								if( msg/100==2 ) // 200, 201... 299 are all successful
								{
									UInt filesize = (UInt)pFile1->GetLength();

									if( filesize > 0 )
									{
										BYTE* buf = SafeAllocate(BYTE, 65536 * sizeof(BYTE));
										File file;
										try
										{
											file.Open(LOCAL_TRACKER_LIST_FILE, File::MODE_WRITE);
											if (file.IsOpened())
											{
												while( pFile1->Read(buf, filesize) )
												{
													file.Write(buf, filesize);
													filesize = (UInt)pFile1->GetLength();
												}
												file.Close();
												CopyTrackerConfig();
												bSuccess = TRUE; // break the loop
											}
										}
										catch(FileException Er)
										{
											TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadTrackerIpIni (1) [exception caught when writing file %s]"), LOCAL_TRACKER_LIST_FILE);
											file.Close();
										}
										SafeDeallocate(buf);
									}
								}
							}
							pFile1->Close();
							if( pFile1 ) delete pFile1;
							pHttpConnection->Close();
							if( pHttpConnection ) delete pHttpConnection;
							pHttpConnection = NULL;
						}
					}catch (...)
					{
						TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadTrackerIpIni (2) [exception caught]"));
						if( pHttpConnection )
						{
							pHttpConnection->Close();
							delete pHttpConnection;
							pHttpConnection = NULL;
						}
					}
					m_pSession->Close();
					SafeDeleteObject(m_pSession);
					m_pSession = NULL;
				}
				break;
			case 2:		// FTP protocol
				{
					m_pSession = SafeCreateObject6(CInternetSessionEx, AfxGetApp()->m_pszAppName, 1, INTERNET_OPEN_TYPE_DIRECT, _T("0"), _T("0"), INTERNET_FLAG_DONT_CACHE);
					//	m_pSession->hWnd	= g_hWnd;
					m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 15);
					m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);
					m_pSession->EnableStatusCallback();

					pFtpConnection		= NULL;
					try
					{
						pFtpConnection = m_pSession->GetFtpConnection(curWebData.ip.c_str(), FTP_USERNAME, FTP_PASSWORD, curWebData.port, 0);
						if( pFtpConnection )
						{
							pFtpConnection->SetCurrentDirectory(curWebData.path.c_str());
							CInternetFile* pFile1 = NULL;
							try
							{
								pFile1 = pFtpConnection->OpenFile(curWebData.file.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 1);
							}catch(...)
							{
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("QGLPatcher::DownloadTrackerIpIni Error (3) [FTP open file exception] File=%s"), curWebData.file.c_str());
							}
							if( pFile1 )
							{
								ULONGLONG usize		= pFile1->GetLength();
								UINT getlen	= 0;
								ULONGLONG bufoffset = 0;
								UINT readlen = 0;
								BYTE* buf = SafeAllocate(BYTE, STREAM_BUF_MAX);
								File file;
								try
								{
									file.Open(LOCAL_TRACKER_LIST_FILE, File::MODE_WRITE);
									if (file.IsOpened())
									{
										while( bufoffset < usize )
										{
											readlen = STREAM_BUF_MAX;
											if( bufoffset + readlen > usize )
												readlen = (UINT)usize - (UINT)bufoffset;
											bufoffset += readlen;

											getlen = pFile1->Read(buf, readlen);

											if( getlen==readlen )
												file.Write(buf, (Count)readlen);
											else
												TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("QGLPatcher::DownloadTrackerIpIni Error (4) [FTP file read size not match] File=%s"), curWebData.file.c_str());
										}
										file.Close();
										CopyTrackerConfig();
										bSuccess = TRUE; // break the loop
									}
								}
								catch(FileException Er)
								{
									TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadTrackerIpIni (5) [exception caught when writing file %s]"), LOCAL_TRACKER_LIST_FILE);
									file.Close();
								}
								SafeDeallocate(buf);

								pFile1->Close();
								if( pFile1 ) delete pFile1;
							}
								
							pFtpConnection->Close();
							if( pFtpConnection ) delete pFtpConnection;
							pFtpConnection = NULL;
						}
					}catch (...)
					{
						TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadTrackerIpIni (6) [ftp exception caught]"));
						if( pFtpConnection )
						{
							pFtpConnection->Close();
							delete pFtpConnection;
							pFtpConnection = NULL;
						}
					}
					m_pSession->Close();
					SafeDeleteObject(m_pSession);
					m_pSession = NULL;
				}
				break;
			}
		}while( !bSuccess && m_trackerList.size() > m_tryTrackerList.size() );
		m_tryTrackerList.clear();
		if( !bSuccess && retries < MAX_LIMIT_TRY )
		{
			retries++;
			Sleep(MAX_LIMIT_DELAY);
		}else
			break;
	}

	// if successfully download and integrated Tracker setting, set status to Free Navigation;
	// if not, pop an message.
	if( bSuccess )
	{
		LoadServerListConfig();
		SetCurStatus(PATCHER_FREENAVIGATE);
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

	String flag_dont_cache;
	flag_dont_cache.Format(_T("%i"), INTERNET_FLAG_DONT_CACHE);

	BOOL bSuccess = TRUE;

	LoadSummaryDownloadLink(GetSelectedServer());

	// Summary Download Selection (temp!!!)
	WebAccessData curWebData	= m_serverSummaryList[0];
	WebAccessData curFileList	= m_serverFileList[0];
	UInt32 retries = 0;
	while(true)
	{
		switch( curWebData.type )
		{
		case 1:	// HTTP
			{
				m_pSession = SafeCreateObject6(CInternetSessionEx, AfxGetApp()->m_pszAppName, 1, INTERNET_OPEN_TYPE_DIRECT, _T("0"), _T("0"), INTERNET_FLAG_DONT_CACHE);
				//	m_pSession->hWnd	= g_hWnd;
				m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 500);
				m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);
				m_pSession->EnableStatusCallback();

				pHttpConnection = NULL;

				try{
					pHttpConnection = m_pSession->GetHttpConnection(curWebData.ip.c_str(), curWebData.port, FTP_USERNAME, FTP_PASSWORD);
					if( pHttpConnection )
					{
						String finalpath;
						if( curWebData.path.GetLength() )
							finalpath.Format(_T("/%s/%s"), curWebData.path.c_str(), curWebData.file.c_str());
						else
							finalpath.Format(_T("/%s"), curWebData.file.c_str());
						CHttpFile* pFile1 = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, finalpath.c_str());
						//pFile1->AddRequestHeaders(_T("User-Agent: GetWebFile/1.0\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);

						BOOL getDocSuccess = pFile1->SendRequest(_T(""));

						DWORD msg;
						pFile1->QueryInfo(HTTP_QUERY_STATUS_CODE, msg, NULL);
						if( (msg+1)/100==2 )
						{
							UInt filesize = (UInt)pFile1->GetLength();

							if( filesize > 0 )
							{
								BYTE* buf = SafeAllocate(BYTE, 65536 * sizeof(BYTE));
								File file;
								try
								{
									file.Open(LOCAL_SUMMARY_LIST_FILE, File::MODE_WRITE);
									if (file.IsOpened())
									{
										while( pFile1->Read(buf, filesize) )
										{
											file.Write(buf, filesize);
											filesize = (UInt)pFile1->GetLength();
										}
										file.Close();
									}
								}
								catch(FileException Er)
								{
									bSuccess = FALSE;
									file.Close();
								}
								SafeDeallocate(buf);
							}
						}
						else
						{
							GetMainDialog()->MessageBox(
								GetStringTable()->Get(_T("MSG_SERVERLIST_DOWNLOAD_FAILED")).c_str(), 
								GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
								);
							bSuccess = FALSE;
						}
						pFile1->Close();
						pHttpConnection->Close();
						if( pFile1 ) delete pFile1;
						if( pHttpConnection ) delete pHttpConnection;
						pHttpConnection = NULL;
					}
				}catch (...)
				{
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchSummary (1) [exception caught using HTTP connection]"));
					bSuccess = FALSE;
				}

				try{
					pHttpConnection = m_pSession->GetHttpConnection(curFileList.ip.c_str(), curFileList.port, FTP_USERNAME, FTP_PASSWORD);
					if( pHttpConnection )
					{
						String finalpath;
						if( curFileList.path.GetLength() )
							finalpath.Format(_T("/%s/%s"), curFileList.path.c_str(), curFileList.file.c_str());
						else
							finalpath.Format(_T("/%s"), curFileList.file.c_str());
						CHttpFile* pFile1 = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, finalpath.c_str());
						//pFile1->AddRequestHeaders(_T("User-Agent: GetWebFile/1.0\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);

						BOOL getDocSuccess = pFile1->SendRequest(_T(""));

						DWORD msg;
						pFile1->QueryInfo(HTTP_QUERY_STATUS_CODE, msg, NULL);
						if( (msg+1)/100==2 )
						{
							UInt filesize = (UInt)pFile1->GetLength();

							if( filesize > 0 )
							{
								BYTE* buf = SafeAllocate(BYTE, 65536 * sizeof(BYTE));
								File file;
								try
								{
									file.Open(LOCAL_FILELIST_FILE, File::MODE_WRITE);
									if (file.IsOpened())
									{
										while( pFile1->Read(buf, filesize) )
										{
											file.Write(buf, filesize);
											filesize = (UInt)pFile1->GetLength();
										}
										file.Close();
									}
								}
								catch(FileException Er)
								{
									bSuccess = FALSE;
									file.Close();
								}
								SafeDeallocate(buf);
							}
						}
						else
						{
							GetMainDialog()->MessageBox(
								GetStringTable()->Get(_T("MSG_SERVERLIST_DOWNLOAD_FAILED")).c_str(), 
								GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
								);
							bSuccess = FALSE;
						}
						pFile1->Close();
						pHttpConnection->Close();
						if( pFile1 ) delete pFile1;
						if( pHttpConnection ) delete pHttpConnection;
						pHttpConnection = NULL;
					}
				}catch (...)
				{
					bSuccess = FALSE;
				}
				if( bSuccess )
				{
					if( !LoadSummaryConfig() )
						bSuccess = FALSE;
				}
				SafeDeleteObject(m_pSession);
				m_pSession = NULL;
			}
			break;
		case 2:	// FTP
			{
				m_pSession = SafeCreateObject6(CInternetSessionEx, AfxGetApp()->m_pszAppName, 1, INTERNET_OPEN_TYPE_DIRECT, _T("0"), _T("0"), INTERNET_FLAG_DONT_CACHE);
				//	m_pSession->hWnd	= g_hWnd;
				m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 500);
				m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);
				m_pSession->EnableStatusCallback();

				pFtpConnection = NULL;

				try{
					pFtpConnection = m_pSession->GetFtpConnection(curWebData.ip.c_str(), FTP_USERNAME, FTP_PASSWORD, curWebData.port, 0);
					pFtpConnection->SetCurrentDirectory(curWebData.path.c_str());
					CInternetFile* pFile1;
					if( pFtpConnection )
					{
						ULONGLONG usize;
						UINT getlen;
						ULONGLONG bufoffset;
						UINT readlen;
						BYTE* buf = SafeAllocate(BYTE, STREAM_BUF_MAX);
						File file;
						if( bSuccess )
						{
							// SUMMARY file
							pFile1		= pFtpConnection->OpenFile(curWebData.file.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 1);
							usize		= pFile1->GetLength();
							getlen		= 0;
							bufoffset	= 0;
							readlen		= 0;
							try
							{
								file.Open(LOCAL_SUMMARY_LIST_FILE, File::MODE_WRITE);
								if (file.IsOpened())
								{
									while( bufoffset < usize )
									{
										readlen = STREAM_BUF_MAX;
										if( bufoffset + readlen > usize )
											readlen = (UINT)usize - (UINT)bufoffset;
										bufoffset += readlen;

										getlen = pFile1->Read(buf, readlen);

										if( getlen==readlen )
											file.Write(buf, (Count)readlen);
										else
										{
											TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("QGLPatcher::DownloadPatchSummary Error (1) [FTP file read size not match] File=%s"), curWebData.file.c_str());
											bSuccess = FALSE;
										}
									}
									file.Close();
								}
							}
							catch(FileException Er)
							{
								bSuccess = FALSE;
								file.Close();
							}
							pFile1->Close();
							if( pFile1 ) delete pFile1;
						}
						if( bSuccess )
						{
							// FILELIST file
							pFile1		= pFtpConnection->OpenFile(curFileList.file.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 1);
							usize		= pFile1->GetLength();
							getlen		= 0;
							bufoffset	= 0;
							readlen		= 0;
							try
							{
								file.Open(LOCAL_FILELIST_FILE, File::MODE_WRITE);
								if (file.IsOpened())
								{
									while( bufoffset < usize )
									{
										readlen = STREAM_BUF_MAX;
										if( bufoffset + readlen > usize )
											readlen = (UINT)usize - (UINT)bufoffset;
										bufoffset += readlen;

										getlen = pFile1->Read(buf, readlen);

										if( getlen==readlen )
											file.Write(buf, (Count)readlen);
										else
										{
											TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("QGLPatcher::DownloadPatchSummary Error (2) [FTP file read size not match] File=%s"), curFileList.file.c_str());
											bSuccess = FALSE;
										}
									}
									file.Close();
								}
							}
							catch(FileException Er)
							{
								bSuccess = FALSE;
								file.Close();
							}
							pFile1->Close();
							if( pFile1 ) delete pFile1;
							if( bSuccess )
							{
								if( !LoadSummaryConfig() )
									bSuccess = FALSE;
							}
						}
						SafeDeallocate(buf);
						pFtpConnection->Close();
						if( pFtpConnection ) delete pFtpConnection;
						pFtpConnection = NULL;
					}
				}catch (...)
				{
					bSuccess = FALSE;
				}
				SafeDeleteObject(m_pSession);
				m_pSession = NULL;
			}
			break;
		}
		if( !bSuccess && retries < MAX_LIMIT_TRY )
		{
			retries++;
			Sleep(MAX_LIMIT_DELAY);
		}else
			break;
	}

	theApp.SetCurStatus(PATCHER_FREENAVIGATE);
	if( bSuccess )
	{
		GetMainDialog()->RedrawSerVerDisplay();
	}else
	{
		GetMainDialog()->MessageBox(
			GetStringTable()->Get(_T("MSG_SERVERLIST_LOAD_FAILED")).c_str(),
			GetStringTable()->Get(_T("ERR_GENERAL")).c_str()
			);
	}
	
	return bSuccess;
}

BOOL CQGLPatcherApp::DownloadAllPatchFile()
{
	DownloadMethod dm = SelectPatchProtocol();
	BOOL succeeded = TRUE;

	String flag_dont_cache;
	flag_dont_cache.Format(_T("%i"), INTERNET_FLAG_DONT_CACHE);
	switch( dm.type )
	{
	case 1: // HTTP
		{
			m_pSession = SafeCreateObject6(CInternetSessionEx, AfxGetApp()->m_pszAppName, 1, INTERNET_OPEN_TYPE_DIRECT, _T("0"), _T("0"), INTERNET_FLAG_DONT_CACHE);
			//	m_pSession->hWnd	= g_hWnd;
			m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 500);
			m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);
			m_pSession->EnableStatusCallback();

			try{
				pHttpConnection = m_pSession->GetHttpConnection(dm.sourceip.c_str(), dm.port, FTP_USERNAME, FTP_PASSWORD);
				if( pHttpConnection )
				{
					// For each version in Summary list
					for( Index i = C_INDEX(0); i < m_patchFileSummary.size(); ++i )
					{
						if( m_patchFileSummary[i].version==GetPatchCurrentVersion() )
						{
							// Download file
							SetDownloadPos(i);
							UInt revertVersion = GetPatchCurrentVersion();
							FileSummary fs = m_patchFileSummary[i];
							SetDLStatus(PATCHER_DL_STATUSDL);
							if( DownloadPatchFile(dm, fs) )	// check if download proceeded successfully
							{
								SetDLStatus(PATCHER_DL_STATUSIN);
								if( !DecompressPatchFile(fs) )	// version update completed (install succeed && no patcher update)
									SetInterrupted();
							}else
							{
								succeeded = FALSE;
								SetDLError();
							}
							SetDLStatus(PATCHER_DL_STATUSEMPTY);
						}
						GetMainDialog()->RedrawCurVerDisplay();
						if( IsInterrupted() )
							break;
					}
					SetDownloadPos((UInt)m_patchFileSummary.size());
					pHttpConnection->Close();
					if( pHttpConnection ) delete pHttpConnection;
					pHttpConnection = NULL;
					m_errorList.clear();
				}
			}catch(...)
			{
				TRACE_ERRORDTL(GLOBAL_LOGGER, _T("QGLPatcher::DownloadAllPatchFile Error (1) [HTTP exception caught]"));
				succeeded = FALSE;
			}
			m_pSession->Close();
			SafeDeleteObject(m_pSession);
			m_pSession = NULL;
		}
		break;
	case 2: // FTP
		{
			m_pSession = SafeCreateObject6(CInternetSessionEx, AfxGetApp()->m_pszAppName, 1, INTERNET_OPEN_TYPE_DIRECT, _T("0"), _T("0"), INTERNET_FLAG_DONT_CACHE);
			//	m_pSession->hWnd	= g_hWnd;
			m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 500);
			m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);
			m_pSession->EnableStatusCallback();

			try{
				pFtpConnection = m_pSession->GetFtpConnection(dm.sourceip.c_str(), FTP_USERNAME, FTP_PASSWORD, dm.port, 0);
				if( pFtpConnection )
				{
					pFtpConnection->SetCurrentDirectory(dm.path.c_str());
					// Download Each File
					for( Index i = C_INDEX(0); i < m_patchFileSummary.size(); ++i )
					{
						if( m_patchFileSummary[i].version==GetPatchCurrentVersion() )
						{
							// Download file
							SetDownloadPos(i);
							UInt revertVersion = GetPatchCurrentVersion();
							FileSummary fs = m_patchFileSummary[i];
							SetDLStatus(PATCHER_DL_STATUSDL);
							if( DownloadPatchFile(dm, fs) )	// check if download proceeded successfully
							{
								SetDLStatus(PATCHER_DL_STATUSIN);
								if( !DecompressPatchFile(fs) )	// version update completed (install succeed && no patcher update)
									SetInterrupted();
							}else
							{
								succeeded = FALSE;
								SetDLError();
							}
							SetDLStatus(PATCHER_DL_STATUSEMPTY);
						}
						GetMainDialog()->RedrawCurVerDisplay();
						if( IsInterrupted() )
							break;
					}
					SetDownloadPos((UInt)m_patchFileSummary.size());
					pFtpConnection->Close();
					if( pFtpConnection ) delete pFtpConnection;
					pFtpConnection = NULL;
					m_errorList.clear();
				}
			}catch(...)
			{
				TRACE_ERRORDTL(GLOBAL_LOGGER, _T("QGLPatcher::DownloadAllPatchFile Error (2) [FTP exception caught]"));
				succeeded = FALSE;
			}
			m_pSession->Close();
			SafeDeleteObject(m_pSession);
			m_pSession = NULL;
		}
		break;
	case 3: // P2P (BitTorrent)
		break;
	}
	return succeeded;
}

BOOL CQGLPatcherApp::DownloadPatchFile(DownloadMethod downloadmethod, FileSummary filesummary)
{
	BOOL bSuccess = TRUE;
	UInt32 localFileSize = 0;
	String localfilepath;
	localfilepath.Format(_T("patch\\%s"), filesummary.filename.c_str());
	UInt32 retries = 0;
	BOOL localFound = FALSE;
	while(true)
	{
		// Download process
		switch( downloadmethod.type )
		{
		case 1:	// HTTP
			{
				if( File::IsExists(localfilepath.c_str()) )
				{
					WIN32_FIND_DATA FileData; 
					HANDLE hSearch = FindFirstFile(localfilepath.c_str(), &FileData);
					localFileSize = (UInt32)FileData.nFileSizeLow;
					if( filesummary.filesize==localFileSize )
					{
						// file CRC check
						UInt32 crc = 0;
						Reuben::Math::CRC32::Calculate(localfilepath.c_str(), crc);
						if( crc==filesummary.filehash )
						{
							FileDownloadStatus filestatus;
							filestatus.curDLpos			= localFileSize;
							filestatus.totalDLsize		= localFileSize;
							filestatus.curMergeIndex	= 0;
							SetFileDownloadInfo(filestatus);
							localFound = TRUE;
						}
					}
					if( !localFound )
						DeleteFile(localfilepath.c_str());
				}
				if( !localFound )
				{
					String finalpath;
					if( downloadmethod.path.GetLength() )
						finalpath.Format(_T("/%s/%s"), downloadmethod.path.c_str(), filesummary.filename.c_str());
					else
						finalpath.Format(_T("/%s"), filesummary.filename.c_str());
					CHttpFile* pFile1 = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, finalpath.c_str());
					//pFile1->AddRequestHeaders(_T("User-Agent: GetWebFile/1.0\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
					BOOL getDocSuccess = pFile1->SendRequest(_T(""));
					DWORD msg;
					pFile1->QueryInfo(HTTP_QUERY_STATUS_CODE, msg, NULL);
					if( msg/100==2 ) // 200, 201... 299 are all successful
					{
						UInt filesize = (UInt)pFile1->GetLength();

						if( filesize > 0 )
						{
							try
							{
								FileDownloadStatus filestatus;
								filestatus.curDLpos			= 0;
								filestatus.totalDLsize		= filesize;
								filestatus.curMergeIndex	= 0;
								SetFileDownloadInfo(filestatus);

								File file;
								file.Open(localfilepath.c_str(), File::MODE_WRITE);
								if (file.IsOpened())
								{
									BYTE* buf = SafeAllocate(BYTE, 1024);
									UInt bytesTrans = 0;
									while( bytesTrans = pFile1->Read(buf, 1024) )
									{
										file.Write(buf, bytesTrans);
										filestatus.curDLpos		+= bytesTrans;
										SetFileDownloadInfo(filestatus);
									}
									SafeDeallocate(buf);
									localFileSize = (UInt32)file.GetLength();
									file.Close();
								}
							}
							catch(FileException Er)
							{
								bSuccess = FALSE;
								TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (1) [File exception caught] File=%s"), localfilepath.c_str());
							}
						}	// File Length check (!=0)
					}else	// Response check (200)
					{
						bSuccess = FALSE;
						TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (2) [Webserver Error Response] File=%s, MsgCode=%i"), localfilepath.c_str(), msg);
					}
					pFile1->Close();
					if( pFile1 ) delete pFile1;
				}
			}
			break;
		case 2:	// FTP
			{
				if( File::IsExists(localfilepath.c_str()) )
				{
					WIN32_FIND_DATA FileData; 
					HANDLE hSearch = FindFirstFile(localfilepath.c_str(), &FileData);
					localFileSize = (UInt32)FileData.nFileSizeLow;
					if( filesummary.filesize==localFileSize )
					{
						// file CRC check
						UInt32 crc = 0;
						Reuben::Math::CRC32::Calculate(localfilepath.c_str(), crc);
						if( crc==filesummary.filehash )
						{
							FileDownloadStatus filestatus;
							filestatus.curDLpos			= localFileSize;
							filestatus.totalDLsize		= localFileSize;
							filestatus.curMergeIndex	= 0;
							SetFileDownloadInfo(filestatus);
							localFound = TRUE;
						}
					}
					if( !localFound )
						DeleteFile(localfilepath.c_str());
				}
				if( !localFound )
				{
					CInternetFile* pFile1 = NULL;
					try
					{
						pFile1 = pFtpConnection->OpenFile(filesummary.filename.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 1);
					}catch(...)
					{
						bSuccess = FALSE;
						TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (6) [FTP Open File exception] File=%s"), localfilepath.c_str());
					}
					if( pFile1 )
					{
						BYTE* buf = SafeAllocate(BYTE, STREAM_BUF_MAX);
						try
						{
							File file;
							file.Open(localfilepath.c_str(), File::MODE_WRITE);
							if (file.IsOpened())
							{
								FileDownloadStatus filestatus;
								filestatus.curDLpos			= 0;
								filestatus.totalDLsize		= filesummary.filesize;
								filestatus.curMergeIndex	= 0;
								SetFileDownloadInfo(filestatus);

								UINT byteTrans;
								while (byteTrans = pFile1->Read(buf, STREAM_BUF_MAX))
								{
									filestatus.curDLpos += byteTrans;
									SetFileDownloadInfo(filestatus);
									file.Write(buf, (Count)byteTrans);
								}

								localFileSize = (UInt32)file.GetLength();
								file.Close();
							}
						}catch(CInternetException* pEx)
						{
							bSuccess = FALSE;

							TCHAR sz[1024];
							pEx->GetErrorMessage(sz, 1024);
							TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (7) [Internet exception caught] File=%s, ErrorMsg=%s"), localfilepath.c_str(), sz);
						}catch(...)
						{
							bSuccess = FALSE;
							TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (3) [File exception caught] File=%s"), localfilepath.c_str());
						}
						SafeDeallocate(buf);
						pFile1->Close();
						if( pFile1 ) delete pFile1;
					}
				}
			}
			break;
		default:
			bSuccess = FALSE;
			break;
		}
		// File Validation
		if( bSuccess )
		{
			if( !localFound )
			{
				if( filesummary.filesize!=localFileSize )
				{
					// file size check
					bSuccess = FALSE;
					TRACE_ERRORDTL_3(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (4) [Length-check failed] File=%s, expectedLength=%u, fileLength=%u"), localfilepath.c_str(), filesummary.filesize, localFileSize);
				}else
				{
					// file CRC check
					UInt32 crc = 0;
					Reuben::Math::CRC32::Calculate(localfilepath.c_str(), crc);
					if( crc!=filesummary.filehash )
					{
						bSuccess = FALSE;
						TRACE_ERRORDTL_3(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (5) [CRC failed] File=%s, expectedCRC=%x, fileCRC=%x"), localfilepath.c_str(), filesummary.filehash, crc);
					}
				}
			}
		}
		if( !bSuccess && retries < MAX_LIMIT_TRY )
		{
			retries++;
			Sleep(MAX_LIMIT_DELAY);
		}else
			break;
	}
	if( !bSuccess )
	{
		SetDLError();
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CQGLPatcherApp::DownloadPatchFile Error (6) [Retries exceeded]"));
	}
	return bSuccess;
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
		cmdFilename.Format(_T("%s\\%s"), m_currentDir.c_str(), POST_PATCH_INSTRUCTION_FILE);
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
			RecursiveMoveUnlessPatcher(srcFolder, destFolder);

			// call UpdatePatch (or bat file calling this)
			BOOL openCmdFound = FALSE;
			File f0;
			try
			{
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
			TRACE_ERRORDTL_2(GLOBAL_LOGGER, _T("CQGLPatcherApp::DecompressPatchFile Error (2) [_instcmd.txt not found, probably disk full] File=%s, version=%i"), filesummary.filename.c_str(), filesummary.version);
		}
	}else
	{
		SetDLError();
		succeed = FALSE;
		TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CQGLPatcherApp::DecompressPatchFile Error (3) [exec cmd failed] Cmd=%s"), execCommand.c_str());
	}

	return succeed;
}

BOOL CQGLPatcherApp::MergePatchToPatch(String& masterfile, String& transfile)
{
	BOOL succeed = TRUE;
	String s1 = masterfile;
	String s2 = transfile;
	String d1 = masterfile;
	GDFPackageMaker packageMaker;
	if( !packageMaker.ExecuteMerge(s1, s2, FALSE) )
		succeed = FALSE;
	DeleteFile(transfile.c_str());
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
		ShellExecute(0, _T("open"), GetPatcherAppFilename().c_str(), NULL, NULL, SW_SHOWNORMAL);
		return;
	}else
	{
		// no interrupt detected, patch process completed
		SaveSelectConfiguration();
		ShellExecute(0, _T("open"), GetGameAppFilename().c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

DownloadMethod CQGLPatcherApp::SelectPatchProtocol()
{
	UInt selected = 0;

	for( Index i = C_INDEX(0); i < m_patchMethod.size() - 1; ++i )
	{
		if( m_patchMethod[i].priority < m_patchMethod[i+1].priority )
			selected = i;
	}
	return m_patchMethod[selected];
}

BOOL CQGLPatcherApp::LoadSummaryConfig()
{
	FreeSummaryConfig();

	BOOL success = TRUE;
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

	groupVariable = _T("FILESUMMARY");
	accessVariable = _T("version");
	UInt targetversion = GetPrivateProfileInt(groupVariable.c_str(), accessVariable.c_str(), 0, LOCAL_SUMMARY_LIST_FILE);
	SetPatchTargetVersion(targetversion);
	m_serVersion.Format(_T("%i"), GetPatchTargetVersion());

	for( Index i = GetPatchCurrentVersion(); i < GetPatchTargetVersion(); ++i )
	{
		FileSummary fs;
		fs.version = i;
		// filename
		accessVariable.Format(_T("patch%08i"), i);	GetPrivateProfileString(groupVariable.c_str(), accessVariable.c_str(), _T(""), tempstring, 100, LOCAL_SUMMARY_LIST_FILE);
		fs.filename.Format(_T("%s"), tempstring);
		// CRC
		accessVariable.Format(_T("crc"), i);		GetPrivateProfileString(fs.filename.c_str(), accessVariable.c_str(), _T(""), tempstring, 9, LOCAL_FILELIST_FILE);
		_stscanf(tempstring, _T("%x"), &fs.filehash);
		// file size
		accessVariable.Format(_T("size"), i);		fs.filesize = GetPrivateProfileInt(fs.filename.c_str(), accessVariable.c_str(), 0, LOCAL_FILELIST_FILE);
		if( fs.filesize > 0 )
			m_patchFileSummary.push_back(fs);
	}

	if( !success )
		FreeSummaryConfig();
	
	return success;
}

BOOL CQGLPatcherApp::LoadSummaryDownloadLink(UInt32 ServerNo)
{
	FreeSummaryDownloadLink();

	BOOL serverCompleted = TRUE;
	// find Summary File Download links
	TCHAR pBuf[MAX_PATH];
	String accessVariable;
	accessVariable.Format(_T("downloadcount%i"), ServerNo+1);
	UInt linkCount = GetPrivateProfileInt(_T("SERVERLIST"), accessVariable.c_str(), 0, LOCAL_TRACKER_LIST_FILE);
	if( linkCount==0 )
	{
		serverCompleted = FALSE;
	}else
	{
		for( Index j = C_INDEX(0); j < linkCount; ++j )
		{
			WebAccessData webdata;
			String ipaddress, path, file;
			String fullAddress;
			UInt port, type;

			accessVariable.Format(_T("downloadlink%i_%i"), ServerNo+1, j+1);
			GetPrivateProfileString(_T("SERVERLIST"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, LOCAL_TRACKER_LIST_FILE);
			fullAddress = pBuf;

			ConvertURLtoComponents(fullAddress, ipaddress, port, type, path, file);

			webdata.type	= type;
			webdata.ip		= ipaddress;
			webdata.port	= port;
			webdata.path	= path;
			webdata.file	= file;
			m_serverSummaryList.push_back(webdata);

			accessVariable.Format(_T("downloadfile%i_%i"), ServerNo+1, j+1);
			GetPrivateProfileString(_T("SERVERLIST"), accessVariable.c_str(), _T("\0"), pBuf, MAX_PATH, LOCAL_TRACKER_LIST_FILE);
			fullAddress = pBuf;

			ConvertURLtoComponents(fullAddress, ipaddress, port, type, path, file);

			webdata.type	= type;
			webdata.ip		= ipaddress;
			webdata.port	= port;
			webdata.path	= path;
			webdata.file	= file;
			m_serverFileList.push_back(webdata);
		}
	}
	return serverCompleted;
}

VOID CQGLPatcherApp::FreeSummaryDownloadLink()
{
	m_serverSummaryList.clear();
	m_serverFileList.clear();
}

VOID CQGLPatcherApp::FreeSummaryConfig()
{
	m_patchMethod.clear();
	m_patchFileSummary.clear();
	m_patchProtocolCount = 0;
	SetPatchTargetVersion(0);
}

void CQGLPatcherApp::InitLogger()
{
	Reuben::Utility::ConfigFile config(CONFIGURATION_INI_FILE);
	gGlobalLogger = SafeCreateObject1(Logger, _T("QGLPatcher"));
	gGlobalLogger->Open(config);

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void CQGLPatcherApp::CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

VOID CQGLPatcherApp::WritePatchCurrentVersion()
{
	String version, lastModifiedDate;

	version.Format(_T("%i"), GetPatchCurrentVersion());

	SYSTEMTIME st;	GetLocalTime(&st);
	TCHAR formattime[20], formatdate[20];
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &st, _T("dd'-'MM'-'yyyy"), formatdate, 20);
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &st, _T("HH':'mm"), formattime, 20);
	lastModifiedDate.Format(_T("%s %s"), formatdate, formattime);

	WritePrivateProfileString(_T("APP"), _T("patcherversion"), version.c_str(), CONFIGURATION_INI_FILE);
	WritePrivateProfileString(_T("APP"), _T("lastmodifieddate"), lastModifiedDate.c_str(), CONFIGURATION_INI_FILE);
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

BOOL CQGLPatcherApp::RecursiveMoveUnlessPatcher(CONST String SrcFolder, CONST String TargetFolder)
{
	BOOL bIsExceptionFound = FALSE;

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
			
			if( FullPath.CompareNoCase(_T("patch\\trash\\res"))!=0 )				////////////////////////// Exception case for patcher
				if( RecursiveMoveUnlessPatcher(FullPath, target) )
					bIsExceptionFound = TRUE;
			else
				bIsExceptionFound = TRUE;
		}else
		{
			target = TargetFolder;
			String srcfile;
			srcfile.Format(_T("patch\\trash\\%s"), GetThisAppFilename().c_str());
			if( FullPath.CompareNoCase(srcfile.c_str())!=0 )	////////////////////////// Exception case for patcher
				if( !MoveUnzipFile(FullPath, target) )
				{
					String tempdest;
					ErrorMap err;
					UInt lastpos		= (UInt)FullPath.find_last_of(_T("\\"));
					if( target.GetLength() )
						tempdest.Format(_T("%s\\%s"), target.c_str(), FullPath.Right(FullPath.GetLength()-lastpos-1).c_str());
					else
						tempdest = FullPath.Right(FullPath.GetLength()-lastpos-1).c_str();
					err.src				= FullPath;
					err.dest			= tempdest;
					m_errorList.push_back(err);
				}
			else
				bIsExceptionFound = TRUE;
		}
	}
	// if failed, it has non-deleted folder
	RemoveDirectory(SrcFolder.c_str());
	return bIsExceptionFound;
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
	TCHAR* p = _tcsrchr(abc.c_str(), '\\');

	if (p) 
	{
		// The parent is a dir, not a drive
		*p = '\0';
			
		// if can't create parent
		if (!CreateFolder(abc.c_str()))
		{
			return FALSE;
		}
		free(szPath);
	}
	free(p);
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
	ServerData &lb = m_serverData[GetSelectedServer()];
	if( GetSelectedServer() < m_serverData.size() && GetSelectedLine() < m_serverData[GetSelectedServer()].server.size() )
	{
		String tempname;

		tempname = m_serverData[GetSelectedServer()].server[GetSelectedLine()].ip.c_str();
		WritePrivateProfileString(_T("APP"), _T("lastserver"), tempname.c_str(), CONFIGURATION_INI_FILE);			// last modified server
		WritePrivateProfileString(_T("Client"), _T("ProxyIP"), tempname.c_str(), CONFIGURATION_GAME_INI_FILE);		// game server ip

		tempname.Format(_T("%i"), m_serverData[GetSelectedServer()].server[GetSelectedLine()].port);
		WritePrivateProfileString(_T("APP"), _T("lastport"), tempname.c_str(), CONFIGURATION_INI_FILE);				// last modified port
		WritePrivateProfileString(_T("Client"), _T("ProxyPort"), tempname.c_str(), CONFIGURATION_GAME_INI_FILE);	// game server port
	}
}

DWORD WINAPI DLHttpFileFunction(IN LPVOID vThreadParm)
{
	PARM* var = (PARM*)vThreadParm;

	try{
        BOOL getDocSuccess = var->pFile->SendRequest(_T(""));
	}catch(...)
	{
	}
	return 0;
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
	}
}