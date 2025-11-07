// QGLPublishDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "QGLPublish.h"
#include "QGLPublishDlg.h"
#include "QGLMerger.h"
#include "QGLNewPatchDlg.h"
#include "QGLOpenPatchDlg.h"
#include "QGLSettingDlg.h"
#include ".\qglpublishdlg.h"

#define PATCHER_WINDOW_SIZE_WIDTH	800
#define PATCHER_WINDOW_SIZE_HEIGHT	562

static UINT BASED_CODE indicators[] =
{
    ID_INDICATOR_STATUS,
    ID_INDICATOR_PROGRESS,
    ID_INDICATOR_PERCENT,
};

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQGLPublishDlg 對話方塊



CQGLPublishDlg::CQGLPublishDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQGLPublishDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQGLPublishDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRIDCTRL_SRC, m_pSrcGridCtrl);
	DDX_Control(pDX, IDC_GRIDCTRL_PACK, m_pPackGridCtrl);
	DDX_Control(pDX, IDC_GRIDCTRL_FILE, m_pFileGridCtrl);
}

BEGIN_MESSAGE_MAP(CQGLPublishDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_TOOLS_QGLVERSIONMERGER, OnToolsQglversionmerger)
	ON_COMMAND(ID_FILE_NEW32771, OnFileNew)
	ON_COMMAND(ID_FILE_OPENEXISTINGVERSION, OnFileOpenexistingversion)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_OPTION_DEFAULTSETTING, OnOptionDefaultsetting)
	ON_CBN_SELCHANGE(IDC_LISTPACK, OnCbnSelchangeListpack)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_GDFPACK_TORIGHT, OnBnClickedGdfpackToright)
	ON_BN_CLICKED(IDC_IDC_GDFPACK_TOLEFT, OnBnClickedIdcGdfpackToleft)
	ON_BN_CLICKED(IDC_FILE_TORIGHT, OnBnClickedFileToright)
	ON_BN_CLICKED(IDC_FILE_TOLEFT, OnBnClickedFileToleft)
	ON_COMMAND(ID_FILE_CLOSE32773, OnFileClose)
END_MESSAGE_MAP()


// CQGLPublishDlg 訊息處理常式

BOOL CQGLPublishDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 將 "關於..." 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	m_menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_menu);
	m_bar.Create(this);
	m_bar.SetIndicators(indicators,3); //Set the number of panes 
	CRect rect;
	GetClientRect(&rect);
	//Size the two panes
	m_bar.SetPaneInfo(0,ID_INDICATOR_STATUS, SBPS_STRETCH,0);      
	m_bar.SetPaneInfo(1,ID_INDICATOR_PROGRESS,SBPS_NORMAL,100);
	m_bar.SetPaneInfo(2,ID_INDICATOR_PERCENT,SBPS_NORMAL,100);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,ID_INDICATOR_PERCENT);

	UInt centerX = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (PATCHER_WINDOW_SIZE_WIDTH / 2);
	UInt centerY = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (PATCHER_WINDOW_SIZE_HEIGHT / 2);
	SetWindowPos(&wndTop, centerX, centerY, PATCHER_WINDOW_SIZE_WIDTH, PATCHER_WINDOW_SIZE_HEIGHT, NULL); 

	InitSrcGridCtrl();
	InitPackGridCtrl();
	InitFileGridCtrl();
	UpdateSrcGridCtrl();
	UpdateProductDisplay();

	TransferSetEnabled(FALSE);

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

VOID CQGLPublishDlg::UpdateSrcGridCtrl()
{
	SetDlgItemText(IDC_SRCFOLDER, theApp.GetSrcFolder().c_str());
	m_srcLineMap.clear();
	CONST FileDisplayMap& fileMap = theApp.GetSrcFileMap();
	m_pSrcGridCtrl.SetRowCount((UInt32)fileMap.size()+1);
	UInt32 counter = 1;
	for( FileDisplayMap::const_iterator itr = fileMap.begin(); itr != fileMap.end(); ++itr, ++counter )
	{
		m_pSrcGridCtrl.SetItemText(counter, 0, itr->second.displayFilename.c_str());
		const FileProperty& fp = itr->second;
		switch( fp.status )
		{
		case FILE_PROPERTY_STATUS_UNVERSIONED:
			m_pSrcGridCtrl.SetItemText(counter, 1, _T("[Unversioned]"));
			break;
		case FILE_PROPERTY_STATUS_NOCHANGE:
			m_pSrcGridCtrl.SetItemText(counter, 1, _T("[No change]"));
			break;
		case FILE_PROPERTY_STATUS_CHANGED:
			m_pSrcGridCtrl.SetItemText(counter, 1, _T("[Changed]"));
			break;
		}
		m_srcLineMap.insert(std::make_pair(counter, itr->second.displayFilename));
	}
}

VOID CQGLPublishDlg::UpdateProductDisplay()
{
	// update Product info display (from and to)
	String prevMajorVersion, targetMajorVersion;
	theApp.GetConfigure()->GetMajorVersion(theApp.GetPrevVersion(), prevMajorVersion);
	targetMajorVersion = theApp.GetMajorVersion();

	String display;
	display.Format(
		_T("%s.%i(%i) -> %s.%i(%i)"),
		prevMajorVersion.c_str(),
		theApp.GetPrevRevision(), theApp.GetPrevVersion(),
		targetMajorVersion.c_str(),
		theApp.GetTargetRevision(), theApp.GetTargetVersion()
		);
	SetDlgItemText(IDC_PRODUCT_INFO, display.c_str());	

	// Set a list of .patch file
	UpdatePackList();

	// Display upper "pack" panel and lower "file" panel
	UpdatePackGridCtrl();
	UpdateFileGridCtrl();
}

VOID CQGLPublishDlg::UpdatePackList()
{
	CComboBox* pCb = (CComboBox*)(GetDlgItem(IDC_LISTPACK));
	if( !pCb )
		return;

	for (int i = 0; i < pCb->GetCount(); ++i)
		pCb->DeleteString( i );
	// Default selected is 0
	// Delete every other item from the combo box.
	UInt32 setcur = 0;
	FileDisplayMap fileMap = theApp.GetTargetFileMap();
	if( fileMap.size() > 0 )
	{
		// open existing version
		for( FileDisplayMap::iterator itr = fileMap.begin(); itr != fileMap.end(); ++itr )
		{
			const String& filename = itr->second.displayFilename;
			if( filename.Right(10).CompareNoCase(_T(".gdf.patch"))==0 )
			{
				pCb->AddString(filename.c_str());
				if( filename.CompareNoCase(theApp.GetSelectedPackFile().c_str())==0 )	// _Data.gdf.patch is default
					setcur = pCb->GetCount()-1;
			}
		}
	}else
	{
		// new patch
		CComboBox* pCb = (CComboBox*)(GetDlgItem(IDC_LISTPACK));
		pCb->AddString(_T("_Data.gdf.patch"));
	}
	pCb->SetCurSel(setcur);
}

VOID CQGLPublishDlg::ChangePackList()
{
	// Usage: On Change Pack List -> Call it -> Update Pack Cell

	CComboBox* pCb = (CComboBox*)(GetDlgItem(IDC_LISTPACK));
	if( !pCb )
		return;

	CString tempstring;
	pCb->GetLBText(pCb->GetCurSel(), tempstring);
	String fullPath;
	fullPath.Format(_T("%s\\%s"), theApp.GetTargetFolder().c_str(), tempstring);
	theApp.SetSelectedPackFile(fullPath.c_str());
}

VOID CQGLPublishDlg::UpdatePackGridCtrl()
{
	// init
	CONST FileDisplayMap& fileMap = theApp.GetTargetPackMap();
	PatchChangeVector changeVector = theApp.GetGdfChangeList();
	m_packLineMap.clear();
	// gridcell line calculation
	UInt32 addDelCounter = 0;
	for( Index i = C_INDEX(0); i < changeVector.size(); ++i )
	{
		switch( changeVector[i].type )
		{
		case PATCH_CHANGE_ADD:
		case PATCH_CHANGE_DELETE:
			++addDelCounter;
			break;
		}
	}
	UInt32 UINumberOfRows = 1 + (UInt32)fileMap.size() + addDelCounter;
	m_pPackGridCtrl.SetRowCount(UINumberOfRows);
	// show content
	UInt32 counter = 1;
	for( FileDisplayMap::const_iterator itr = fileMap.begin(); itr != fileMap.end(); ++itr, ++counter )
	{
		const String& filename = itr->second.displayFilename;
		String strType(_T(""));
		for( Index i = C_INDEX(0); i < changeVector.size(); ++i )
		{
			PatchChange& patch = changeVector[i];
			if( !filename.CompareNoCase(patch.filename.c_str()) )
			{
				strType = _T("[Updated]");
				break;
			}
		}
		m_pPackGridCtrl.SetItemText(counter, 0, itr->second.displayFilename.c_str());
		m_pPackGridCtrl.SetItemText(counter, 1, strType.c_str());
		m_packLineMap.insert(std::make_pair(counter, filename));
	}
	for( Index i = C_INDEX(0); i < changeVector.size(); ++i )
	{
		String strType;
		switch(changeVector[i].type)
		{
		case PATCH_CHANGE_ADD:
			strType = _T("[Add]");
			m_pPackGridCtrl.SetItemText(counter, 0, changeVector[i].filename.c_str());
			m_pPackGridCtrl.SetItemText(counter, 1, strType.c_str());
			m_packLineMap.insert(std::make_pair(counter, changeVector[i].filename));
			++counter;
			break;
		case PATCH_CHANGE_DELETE:
			strType = _T("[Delete]");
			m_pPackGridCtrl.SetItemText(counter, 0, changeVector[i].filename.c_str());
			m_pPackGridCtrl.SetItemText(counter, 1, strType.c_str());
			m_packLineMap.insert(std::make_pair(counter, changeVector[i].filename));
			++counter;
			break;
		}
	}
	m_pPackGridCtrl.RedrawWindow();
}

VOID CQGLPublishDlg::UpdateFileGridCtrl()
{
	// init
	FileDisplayMap fileMap = theApp.GetTargetFileMap();
	PatchChangeVector changeVector = theApp.GetFileChangeList();
	m_fileLineMap.clear();
	// gridcell line calculation
	UInt32 addDelCounter = 0;
	for( Index i = C_INDEX(0); i < changeVector.size(); ++i )
	{
		switch( changeVector[i].type )
		{
		case PATCH_CHANGE_ADD:
		case PATCH_CHANGE_DELETE:
			++addDelCounter;
			break;
		}
	}
	UInt32 UINumberOfRows = 1 + (UInt32)fileMap.size() + addDelCounter;
	m_pFileGridCtrl.SetRowCount(UINumberOfRows);
	// show content
	UInt32 counter = 1;
	for( FileDisplayMap::iterator itr = fileMap.begin(); itr != fileMap.end(); ++itr, ++counter )
	{
		const String& filename = itr->second.displayFilename;
		String strType(_T(""));
		for( Index i = C_INDEX(0); i < changeVector.size(); ++i )
		{
			PatchChange& patch = changeVector[i];
			if( !filename.CompareNoCase(patch.filename.c_str()) )
			{
				strType = _T("[Updated]");
				break;
			}
		}
		m_pFileGridCtrl.SetItemText(counter, 0, itr->second.displayFilename.c_str());
		m_pFileGridCtrl.SetItemText(counter, 1, strType.c_str());
		m_fileLineMap.insert(std::make_pair(counter, filename));
	}
	for( Index i = C_INDEX(0); i < changeVector.size(); ++i )
	{
		String strType;
		PatchChange& change = changeVector[i];
		switch(change.type)
		{
		case PATCH_CHANGE_ADD:
			strType = _T("[Add]");
			m_pFileGridCtrl.SetItemText(counter, 0, change.filename.c_str());
			m_pFileGridCtrl.SetItemText(counter, 1, strType.c_str());
			m_fileLineMap.insert(std::make_pair(counter, change.filename));
			++counter;
			break;
		case PATCH_CHANGE_DELETE:
			strType = _T("[Delete]");
			m_pFileGridCtrl.SetItemText(counter, 0, change.filename.c_str());
			m_pFileGridCtrl.SetItemText(counter, 1, strType.c_str());
			m_fileLineMap.insert(std::make_pair(counter, change.filename));
			++counter;
			break;
		}
	}
	m_pFileGridCtrl.RedrawWindow();
}

void CQGLPublishDlg::InitSrcGridCtrl()
{
	// Set Row and Col
	m_pSrcGridCtrl.SetRowCount(1);
	m_pSrcGridCtrl.SetColumnCount(2);
	m_pSrcGridCtrl.SetFixedRowCount(1);
	m_pSrcGridCtrl.SetFixedColumnCount(0);
	// Set Column Width
	m_pSrcGridCtrl.SetColumnWidth(0, 233);
	m_pSrcGridCtrl.SetColumnWidth(1, 80);
	// Set Title
	m_pSrcGridCtrl.SetItemText(0, 0, _T("Filename"));
	m_pSrcGridCtrl.SetItemText(0, 1, _T("Status"));
	// Set State and Control Type of 1st Col
	for ( int i = 0; i < 2; ++i )
	{
		m_pSrcGridCtrl.SetItemState(0, i, m_pSrcGridCtrl.GetItemState(0, i) | GVIS_READONLY);
		m_pSrcGridCtrl.SetCellType(0, i, RUNTIME_CLASS(CGridCell));
	}
	TransferSetEnabled(FALSE);
}

void CQGLPublishDlg::InitPackGridCtrl()
{
	// Set Row and Col
	m_pPackGridCtrl.SetRowCount(1);
	m_pPackGridCtrl.SetColumnCount(2);
	m_pPackGridCtrl.SetFixedRowCount(1);
	m_pPackGridCtrl.SetFixedColumnCount(0);
	// Set Column Width
	m_pPackGridCtrl.SetColumnWidth(0, 233);
	m_pPackGridCtrl.SetColumnWidth(1, 80);
	// Set Title
	m_pPackGridCtrl.SetItemText(0, 0, _T("Filename"));
	m_pPackGridCtrl.SetItemText(0, 1, _T("Status"));
	// Set State and Control Type of 1st Col
	for ( int i = 0; i < 2; ++i )
	{
		m_pPackGridCtrl.SetItemState(0, i, m_pPackGridCtrl.GetItemState(0, i) | GVIS_READONLY);
		m_pPackGridCtrl.SetCellType(0, i, RUNTIME_CLASS(CGridCell));
	}
}

void CQGLPublishDlg::InitFileGridCtrl()
{
	// Set Row and Col
	m_pFileGridCtrl.SetRowCount(1);
	m_pFileGridCtrl.SetColumnCount(2);
	m_pFileGridCtrl.SetFixedRowCount(1);
	m_pFileGridCtrl.SetFixedColumnCount(0);
	// Set Column Width
	m_pFileGridCtrl.SetColumnWidth(0, 233);
	m_pFileGridCtrl.SetColumnWidth(1, 80);
	// Set Title
	m_pFileGridCtrl.SetItemText(0, 0, _T("Filename"));
	m_pFileGridCtrl.SetItemText(0, 1, _T("Status"));
	// Set State and Control Type of 1st Col
	for ( int i = 0; i < 2; ++i )
	{
		m_pFileGridCtrl.SetItemState(0, i, m_pFileGridCtrl.GetItemState(0, i) | GVIS_READONLY);
		m_pFileGridCtrl.SetCellType(0, i, RUNTIME_CLASS(CGridCell));
	}
	m_pFileGridCtrl.SortItems(0, TRUE);
}

void CQGLPublishDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，以便繪製圖示。
// 對於使用文件/檢視模式的 MFC 應用程式，框架會自動完成此作業。

void CQGLPublishDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//當使用者拖曳最小化視窗時，系統呼叫這個功能取得游標顯示。
HCURSOR CQGLPublishDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQGLPublishDlg::OnHelpAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CQGLPublishDlg::OnToolsQglversionmerger()
{
	CQGLMerger dlg;
	dlg.DoModal();
}

void CQGLPublishDlg::OnFileNew()
{
	CQGLNewPatchDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		TransferSetEnabled(TRUE);
		// on click ok, SetTargetVersion(version, revision) and SetTargetFolder is set
		InitPackGridCtrl();
		InitFileGridCtrl();
		theApp.CheckVersionInfo();		// check latest and prev revision
		theApp.ClearChangeFileMap();
		theApp.ClearChangeGdfMap();
		String gdfFilename;
		gdfFilename.Format(_T("%s\\_Data.gdf.patch"), theApp.GetTargetFolder().c_str());
		theApp.SetSelectedPackFile(gdfFilename.c_str());
		theApp.UpdateFileFolderList();
		theApp.UpdatePackFolderList();
		UpdateProductDisplay();
	}
}

void CQGLPublishDlg::OnFileOpenexistingversion()
{
	CQGLOpenPatchDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		TransferSetEnabled(TRUE);
		// on click ok, SetTargetVersion and SetTargetFolder is set, except Target Revision is 0;
		InitPackGridCtrl();
		InitFileGridCtrl();
		UInt32 targetRev, supVer, supRev;
		theApp.ReadPatchVer(theApp.GetTargetVersion(), targetRev, supVer, supRev);
		theApp.SetTargetVersion(theApp.GetTargetVersion(), targetRev);// read current revision
		theApp.CheckVersionInfo();		// check latest and prev revision
		theApp.ClearChangeFileMap();
		theApp.ClearChangeGdfMap();
		String gdfFilename;
		gdfFilename.Format(_T("%s\\_Data.gdf.patch"), theApp.GetTargetFolder().c_str());
		theApp.SetSelectedPackFile(gdfFilename.c_str());
		theApp.UpdateFileFolderList();
		theApp.UpdatePackFolderList();
		UpdateProductDisplay();
	}
}

void CQGLPublishDlg::OnFileExit()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CQGLPublishDlg::OnOptionDefaultsetting()
{
	CQGLSettingDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		theApp.UpdateQGLPublishSetting();
		theApp.UpdateSrcFolderList();
		UpdateSrcGridCtrl();
		String gdfFilename;
		gdfFilename.Format(_T("%s\\_Data.gdf.patch"), theApp.GetTargetFolder().c_str());
		theApp.SetSelectedPackFile(gdfFilename.c_str());
		theApp.UpdateFileFolderList();
		theApp.UpdatePackFolderList();
		UpdateProductDisplay();
	}
}

void CQGLPublishDlg::OnGvnEndLabelEditSrcGridCtrl(NMHDR *pNMHDR, LRESULT* pResult)
{
}

void CQGLPublishDlg::OnGvnEndLabelEditPackGridCtrl(NMHDR *pNMHDR, LRESULT* pResult)
{
}

void CQGLPublishDlg::OnGvnEndLabelEditFileGridCtrl(NMHDR *pNMHDR, LRESULT* pResult)
{
}

void CQGLPublishDlg::OnCbnSelchangeListpack()
{
	// TODO: Add your control notification handler code here
}

void CQGLPublishDlg::OnBnClickedButton3()
{
	// Default>> button
	// Move the "Changed" files to GDF and Direct-copy panel
	theApp.ClearChangeFileMap();
	theApp.ClearChangeGdfMap();
	CONST FileDisplayMap& filemap = theApp.GetSrcFileMap();
	for( FileDisplayMap::const_iterator itr = filemap.begin(); itr != filemap.end(); ++itr )
	{
		const String& filename = itr->first;
		const FileProperty& fp = itr->second;

		switch( fp.status )
		{
		case FILE_PROPERTY_STATUS_UNVERSIONED:
		case FILE_PROPERTY_STATUS_CHANGED:
				{
					// rules distinguish what is GDF, what is Direct Copy
					String extension(filename.Right(4).c_str());
					if(
						!extension.CompareNoCase(_T(".blk")) ||
						!extension.CompareNoCase(_T(".csv")) || 
						!extension.CompareNoCase(_T(".dat")) || 
						!extension.CompareNoCase(_T(".ent")) || 
						!extension.CompareNoCase(_T(".lua")) || 
						!extension.CompareNoCase(_T(".map")) || 
						!extension.CompareNoCase(_T(".pak")) || 
						!extension.CompareNoCase(_T(".pic")) || 
						!extension.CompareNoCase(_T(".ttc")) || 
						!extension.CompareNoCase(_T(".ttf")) || 
						!extension.CompareNoCase(_T(".xml"))
						)
					{
						BOOL bAdded = theApp.AddChangeGdfMap(filename.c_str());
					}else if(
						!extension.CompareNoCase(_T(".bin")) ||
						!extension.CompareNoCase(_T(".bmp")) ||
						!extension.CompareNoCase(_T(".dll")) ||
						!extension.CompareNoCase(_T(".ini")) ||
						!extension.CompareNoCase(_T(".ogg")) ||
						!extension.CompareNoCase(_T(".wav"))
						)
					{
						BOOL bAdded = theApp.AddChangeFileMap(filename.c_str());
					}
				}
			break;
		}
	}
	UpdateProductDisplay();
}

void CQGLPublishDlg::OnBnClickedOk()
{
	// "Execute!"
	theApp.ExecutePatch();

	// refresh and reopen current
	theApp.CheckVersionInfo();
//	theApp.UpdateSrcFolderList();
//	UpdateSrcGridCtrl();
	String gdfFilename;
	gdfFilename.Format(_T("%s\\_Data.gdf.patch"), theApp.GetTargetFolder().c_str());
	theApp.SetSelectedPackFile(gdfFilename.c_str());
	theApp.UpdateFileFolderList();
	theApp.UpdatePackFolderList();
	UpdateProductDisplay();

	MessageBox(_T("Export finished"), _T("Info"), MB_OK);
}

void CQGLPublishDlg::OnBnClickedGdfpackToright()
{
	// "GDF Pack >"
	// move selected cells from left to right
	CCellRange cellRange = m_pSrcGridCtrl.GetSelectedCellRange();
	for( int i = cellRange.GetMinRow(); i <= cellRange.GetMaxRow(); ++i )
	{
		LineFilenameMap::iterator itr = m_srcLineMap.find(i);
		if( itr != m_fileLineMap.end() )
			theApp.AddChangeGdfMap(itr->second.c_str());
	}
	UpdatePackGridCtrl();
}

void CQGLPublishDlg::OnBnClickedIdcGdfpackToleft()
{
	// "GDF Pack <"
	// move selected cells from right to left
	CCellRange cellRange = m_pPackGridCtrl.GetSelectedCellRange();
	for( int i = cellRange.GetMinRow(); i <= cellRange.GetMaxRow(); ++i )
	{
		LineFilenameMap::iterator itr = m_packLineMap.find(i);
		if( itr != m_packLineMap.end() )
			theApp.DeleteChangeGdfMap(itr->second.c_str());
	}
	UpdatePackGridCtrl();
}

void CQGLPublishDlg::OnBnClickedFileToright()
{
	// "File >"
	// move selected cells from left to right
	CCellRange cellRange = m_pSrcGridCtrl.GetSelectedCellRange();
	for( int i = cellRange.GetMinRow(); i <= cellRange.GetMaxRow(); ++i )
	{
		LineFilenameMap::iterator itr = m_srcLineMap.find(i);
		if( itr != m_fileLineMap.end() )
			theApp.AddChangeFileMap(itr->second.c_str());
	}
	UpdateFileGridCtrl();
}

void CQGLPublishDlg::OnBnClickedFileToleft()
{
	// "File <"
	// move selected cells from right to left
	CCellRange cellRange = m_pFileGridCtrl.GetSelectedCellRange();
	for( int i = cellRange.GetMinRow(); i <= cellRange.GetMaxRow(); ++i )
	{
		LineFilenameMap::iterator itr = m_fileLineMap.find(i);
		if( itr != m_fileLineMap.end() )
			theApp.DeleteChangeFileMap(itr->second.c_str());
	}
	UpdateFileGridCtrl();
}

void CQGLPublishDlg::OnFileClose()
{
	InitPackGridCtrl();
	InitFileGridCtrl();

	theApp.NewPublishVersion();
	TransferSetEnabled(FALSE);

	UpdateProductDisplay();
}

VOID CQGLPublishDlg::TransferSetEnabled(BOOL Enabled)
{
	((CButton*)(GetDlgItem(IDC_BUTTON3)))->EnableWindow(Enabled);
	((CButton*)(GetDlgItem(IDC_GDFPACK_TORIGHT)))->EnableWindow(Enabled);
	((CButton*)(GetDlgItem(IDC_IDC_GDFPACK_TOLEFT)))->EnableWindow(Enabled);
	((CButton*)(GetDlgItem(IDC_FILE_TORIGHT)))->EnableWindow(Enabled);
	((CButton*)(GetDlgItem(IDC_FILE_TOLEFT)))->EnableWindow(Enabled);
	((CButton*)(GetDlgItem(IDOK)))->EnableWindow(Enabled);
}
