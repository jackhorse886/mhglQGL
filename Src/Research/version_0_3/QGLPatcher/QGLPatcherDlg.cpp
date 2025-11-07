// QGLPatcherDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "QGLPatcher.h"
#include "QGLPatcherDlg.h"
#include ".\qglpatcherdlg.h"

#define PATCHER_WINDOW_SIZE_WIDTH				528
#define PATCHER_WINDOW_SIZE_HEIGHT				396

#define PATCHER_COMPANY_OFFICIAL_WEBSITE		_T("http://mg.51hit.com/")
#define PATCHER_QGL_OFFICIAL_WEBSITE			_T("http://mg.51hit.com/")
#define PATCHER_QGL_POINT_WEBSITE				_T("http://register.zijiewang.com/action/login.do")
#define PATCHER_QGL_ACCOUNT_WEBSITE				_T("http://register.zijiewang.com/action/login.do")
#define PATCHER_QGL_REGISTRATION_WEBSITE		_T("http://register.zijiewang.com/action/login.do")
#define PATCHER_QGL_PATCH_WEBSITE				_T("http://mg.51hit.com/Download.html")

#define PATCHER_WEBBROWSER1_OFFSET_X			31
#define PATCHER_WEBBROWSER1_OFFSET_Y			80
#define PATCHER_WEBBROWSER1_WIDTH				467
#define PATCHER_WEBBROWSER1_HEIGHT				258
#define PATCHER_WEBBROWSER2_OFFSET_X			264
#define PATCHER_WEBBROWSER2_OFFSET_Y			80
#define PATCHER_WEBBROWSER2_WIDTH				234
#define PATCHER_WEBBROWSER2_HEIGHT				258
#define PATCHER_WEBBROWSER3_OFFSET_X			31
#define PATCHER_WEBBROWSER3_OFFSET_Y			80
#define PATCHER_WEBBROWSER3_WIDTH				463
#define PATCHER_WEBBROWSER3_HEIGHT				224

#define PATCHER_DEFAULT_BUTTON_WIDTH			82
#define PATCHER_DEFAULT_BUTTON_HEIGHT			23
#define PATCHER_SERVER_BUTTON_WIDTH				107
#define PATCHER_SERVER_BUTTON_HEIGHT			40

#define PATCHER_IMAGE_WEB_BACKGROUND			_T("res/NewsBg.bmp")
#define PATCHER_IMAGE_WEB_BUTTON1				_T("res/NewsGoOfficeNormal.bmp")
#define PATCHER_IMAGE_WEB_BUTTON2				_T("res/NewsGoAccountNormal.bmp")
#define PATCHER_IMAGE_WEB_BUTTON3				_T("res/NewsGoRegisterNormal.bmp")
#define PATCHER_IMAGE_WEB_BUTTON4				_T("res/NewsPointNormal.bmp")
#define PATCHER_IMAGE_WEB_BUTTON5				_T("res/NewsExitNormal.bmp")
#define PATCHER_IMAGE_WEB_BUTTON6				_T("res/NewsStartNormal.bmp")
#define PATCHER_IMAGE_WEB_BUTTON1_OVER			_T("res/NewsGoOfficeOn.bmp")
#define PATCHER_IMAGE_WEB_BUTTON2_OVER			_T("res/NewsGoAccountOn.bmp")
#define PATCHER_IMAGE_WEB_BUTTON3_OVER			_T("res/NewsGoRegisterOn.bmp")
#define PATCHER_IMAGE_WEB_BUTTON4_OVER			_T("res/NewsPointOn.bmp")
#define PATCHER_IMAGE_WEB_BUTTON5_OVER			_T("res/NewsExitOn.bmp")
#define PATCHER_IMAGE_WEB_BUTTON6_OVER			_T("res/NewsStartOn.bmp")
#define PATCHER_IMAGE_WEB_BUTTON1_DOWN			_T("res/NewsGoOfficeDown.bmp")
#define PATCHER_IMAGE_WEB_BUTTON2_DOWN			_T("res/NewsGoAccountDown.bmp")
#define PATCHER_IMAGE_WEB_BUTTON3_DOWN			_T("res/NewsGoRegisterDown.bmp")
#define PATCHER_IMAGE_WEB_BUTTON4_DOWN			_T("res/NewsPointDown.bmp")
#define PATCHER_IMAGE_WEB_BUTTON5_DOWN			_T("res/NewsExitDown.bmp")
#define PATCHER_IMAGE_WEB_BUTTON6_DOWN			_T("res/NewsStartDown.bmp")

#define PATCHER_IMAGE_PATCH_BACKGROUND			_T("res/PatchBg.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_DL_DOWN		_T("res/PatchDLDown.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_DL			_T("res/PatchDLNormal.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_DL_OVER		_T("res/PatchDLOn.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_EXIT_DOWN	_T("res/PatchExitDown.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_EXIT			_T("res/PatchExitNormal.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_EXIT_OVER	_T("res/PatchExitOn.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_START_DOWN	_T("res/PatchStartDown.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_START		_T("res/PatchStartNormal.bmp")
#define PATCHER_IMAGE_PATCH_BUTTON_START_OVER	_T("res/PatchStartOn.bmp")
#define PATCHER_IMAGE_DOWNLOADING_A				_T("res/PatchDLa.bmp")
#define PATCHER_IMAGE_DOWNLOADING_B				_T("res/PatchDLb.bmp")
#define PATCHER_IMAGE_DOWNLOADING_C				_T("res/PatchDLc.bmp")
#define PATCHER_IMAGE_DOWNLOADING_D				_T("res/PatchDLd.bmp")
#define PATCHER_IMAGE_INSTALLING_A				_T("res/PatchIna.bmp")
#define PATCHER_IMAGE_INSTALLING_B				_T("res/PatchInb.bmp")
#define PATCHER_IMAGE_INSTALLING_C				_T("res/PatchInc.bmp")
#define PATCHER_IMAGE_INSTALLING_D				_T("res/PatchInd.bmp")
#define PATCHER_IMAGE_EMPTY						_T("res/PatchEmp.bmp")

#define PATCHER_IMAGE_SERVER_BACKGROUND			_T("res/ServerBg.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT_DOWN	_T("res/ServerDefaultSlotBgDown.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT		_T("res/ServerDefaultSlotBgNormal.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT_OVER	_T("res/ServerDefaultSlotBgOn.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_NEXT_DOWN	_T("res/ServerNextDown.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_NEXT		_T("res/ServerNextNormal.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_NEXT_OVER	_T("res/ServerNextOn.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_PREV_DOWN	_T("res/ServerPrevDown.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_PREV		_T("res/ServerPrevNormal.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_PREV_OVER	_T("res/ServerPrevOn.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT1_DOWN	_T("res/ServerSlot1BgDown.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT1		_T("res/ServerSlot1BgNormal.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT1_OVER	_T("res/ServerSlot1BgOn.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT2_DOWN	_T("res/ServerSlot2BgDown.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT2		_T("res/ServerSlot2BgNormal.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT2_OVER	_T("res/ServerSlot2BgOn.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT3_DOWN	_T("res/ServerSlot3BgDown.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT3		_T("res/ServerSlot3BgNormal.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT3_OVER	_T("res/ServerSlot3BgOn.bmp")
#define PATCHER_IMAGE_SERVER_BUTTON_SLOT4		_T("res/ServerSlot4BgNormal.bmp")
#define PATCHER_IMAGE_SERVER_PAGEUP_DOWN		_T("res/ServerPageUpDown.bmp")
#define PATCHER_IMAGE_SERVER_PAGEUP				_T("res/ServerPageUpNormal.bmp")
#define PATCHER_IMAGE_SERVER_PAGEUP_OVER		_T("res/ServerPageUpOn.bmp")
#define PATCHER_IMAGE_SERVER_PAGEDOWN_DOWN		_T("res/ServerPageDownDown.bmp")
#define PATCHER_IMAGE_SERVER_PAGEDOWN			_T("res/ServerPageDownNormal.bmp")
#define PATCHER_IMAGE_SERVER_PAGEDOWN_OVER		_T("res/ServerPageDownOn.bmp")

#define MAX_SERVER_BUTTON						20

#define TRANSPARENTCOLOR						RGB(0, 255, 0);

// CQGLPatcherDlg 對話方塊



CQGLPatcherDlg::CQGLPatcherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQGLPatcherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

CQGLPatcherDlg::~CQGLPatcherDlg()
{
	if( m_bmpWebBg )		SafeDeleteObject(m_bmpWebBg);
	if( m_bmpServerBg )		SafeDeleteObject(m_bmpServerBg);
	if( m_bmpPatchBg )		SafeDeleteObject(m_bmpPatchBg);
	for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
	{
		if( m_bmpDL[i] )	SafeDeleteObject(m_bmpDL[i]);
		if( m_bmpIN[i] )	SafeDeleteObject(m_bmpIN[i]);
	}
	if( m_bmpEmp )			SafeDeleteObject(m_bmpEmp);

}

void CQGLPatcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_UPPER, m_browser1);
	DDX_Control(pDX, IDC_EXPLORER_CENTER, m_browser3);
	DDX_Control(pDX, IDC_BUTTON_GOOFFICIAL, m_goOfficial);
	DDX_Control(pDX, IDC_BUTTON_ACCOUNT, m_goAccount);
	DDX_Control(pDX, IDC_BUTTON_REGISTRATION, m_goRegistration);
	DDX_Control(pDX, IDC_BUTTON_POINT, m_goPoint);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_exitGame);
	DDX_Control(pDX, IDC_BUTTON_START, m_startGame);
	DDX_Control(pDX, IDC_BUTTON_SER1, m_serverChoice[0]);
	DDX_Control(pDX, IDC_BUTTON_SER2, m_serverChoice[1]);
	DDX_Control(pDX, IDC_BUTTON_SER3, m_serverChoice[2]);
	DDX_Control(pDX, IDC_BUTTON_SER4, m_serverChoice[3]);
	DDX_Control(pDX, IDC_BUTTON_SER5, m_serverChoice[4]);
	DDX_Control(pDX, IDC_BUTTON_SER6, m_serverChoice[5]);
	DDX_Control(pDX, IDC_BUTTON_SER7, m_serverChoice[6]);
	DDX_Control(pDX, IDC_BUTTON_SER8, m_serverChoice[7]);
	DDX_Control(pDX, IDC_BUTTON_SER9, m_serverChoice[8]);
	DDX_Control(pDX, IDC_BUTTON_SER10, m_serverChoice[9]);
	DDX_Control(pDX, IDC_BUTTON_SER11, m_serverChoice[10]);
	DDX_Control(pDX, IDC_BUTTON_SER12, m_serverChoice[11]);
	DDX_Control(pDX, IDC_BUTTON_SER13, m_serverChoice[12]);
	DDX_Control(pDX, IDC_BUTTON_SER14, m_serverChoice[13]);
	DDX_Control(pDX, IDC_BUTTON_SER15, m_serverChoice[14]);
	DDX_Control(pDX, IDC_BUTTON_SER16, m_serverChoice[15]);
	DDX_Control(pDX, IDC_BUTTON_SER17, m_serverChoice[16]);
	DDX_Control(pDX, IDC_BUTTON_SER18, m_serverChoice[17]);
	DDX_Control(pDX, IDC_BUTTON_SER19, m_serverChoice[18]);
	DDX_Control(pDX, IDC_BUTTON_SER20, m_serverChoice[19]);
	DDX_Control(pDX, IDC_BUTTON_DL, m_startPatch);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_serverNext);
	DDX_Control(pDX, IDC_BUTTON_PREV, m_serverPrev);
	DDX_Control(pDX, IDC_BUTTON_CURSERVER, m_serverCur);
	DDX_Control(pDX, IDC_BUTTON_PAGEUP, m_serverPageUp);
	DDX_Control(pDX, IDC_BUTTON_PAGEDOWN, m_serverPageDown);
	DDX_Control(pDX, IDC_PROGRESS_BIG, m_process_big);
	DDX_Control(pDX, IDC_PROGRESS_SMALL, m_process_small);
}

BEGIN_MESSAGE_MAP(CQGLPatcherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_GOOFFICIAL, OnBnClickedButtonGoofficial)
	ON_BN_CLICKED(IDC_BUTTON_REGISTRATION, OnBnClickedButtonRegistration)
	ON_BN_CLICKED(IDC_BUTTON_ACCOUNT, OnBnClickedButtonAccount)
	ON_BN_CLICKED(IDC_BUTTON_POINT, OnBnClickedButtonPoint)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_SER1, OnBnClickedButtonSer1)
	ON_BN_CLICKED(IDC_BUTTON_SER2, OnBnClickedButtonSer2)
	ON_BN_CLICKED(IDC_BUTTON_SER3, OnBnClickedButtonSer3)
	ON_BN_CLICKED(IDC_BUTTON_SER4, OnBnClickedButtonSer4)
	ON_BN_CLICKED(IDC_BUTTON_SER5, OnBnClickedButtonSer5)
	ON_BN_CLICKED(IDC_BUTTON_SER6, OnBnClickedButtonSer6)
	ON_BN_CLICKED(IDC_BUTTON_SER7, OnBnClickedButtonSer7)
	ON_BN_CLICKED(IDC_BUTTON_SER8, OnBnClickedButtonSer8)
	ON_BN_CLICKED(IDC_BUTTON_SER9, OnBnClickedButtonSer9)
	ON_BN_CLICKED(IDC_BUTTON_SER10, OnBnClickedButtonSer10)
	ON_BN_CLICKED(IDC_BUTTON_SER11, OnBnClickedButtonSer11)
	ON_BN_CLICKED(IDC_BUTTON_SER12, OnBnClickedButtonSer12)
	ON_BN_CLICKED(IDC_BUTTON_SER13, OnBnClickedButtonSer13)
	ON_BN_CLICKED(IDC_BUTTON_SER14, OnBnClickedButtonSer14)
	ON_BN_CLICKED(IDC_BUTTON_SER15, OnBnClickedButtonSer15)
	ON_BN_CLICKED(IDC_BUTTON_SER16, OnBnClickedButtonSer16)
	ON_BN_CLICKED(IDC_BUTTON_SER17, OnBnClickedButtonSer17)
	ON_BN_CLICKED(IDC_BUTTON_SER18, OnBnClickedButtonSer18)
	ON_BN_CLICKED(IDC_BUTTON_SER19, OnBnClickedButtonSer19)
	ON_BN_CLICKED(IDC_BUTTON_SER20, OnBnClickedButtonSer20)
	ON_BN_CLICKED(IDC_BUTTON_DL, OnBnClickedButtonDl)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_CURSERVER, OnBnClickedButtonSerCur)
	ON_BN_CLICKED(IDC_BUTTON_PAGEDOWN, OnBnClickedButtonPagedown)
	ON_BN_CLICKED(IDC_BUTTON_PAGEUP, OnBnClickedButtonPageup)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVERLINE, OnCbnSelchangeComboServerline)
END_MESSAGE_MAP()


// CQGLPatcherDlg 訊息處理常式

BOOL CQGLPatcherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	time(&m_PrevTime);

	if( theApp.GetStringTable() )
	{
		String title = theApp.GetStringTable()->Get(_T("APP_QGL_PATCHER_CAPTION"));
		SetWindowText(title.c_str());
	}else
	{
		SetWindowText(_T("QGL Auto Update"));
	}
	UInt iX = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (PATCHER_WINDOW_SIZE_WIDTH / 2);
	UInt iY = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (PATCHER_WINDOW_SIZE_HEIGHT / 2);
	SetWindowPos(&wndTop, iX, iY, PATCHER_WINDOW_SIZE_WIDTH, PATCHER_WINDOW_SIZE_HEIGHT, NULL); 

	m_browser1.SetWindowPos(GetWindow(0), PATCHER_WINDOW_SIZE_WIDTH, PATCHER_WINDOW_SIZE_HEIGHT, 1, 1, 0);
//	m_browser1.ShowWindow(SW_HIDE);
	m_browser3.ShowWindow(SW_HIDE);
//	m_browser1.Navigate(PATCHER_COMPANY_OFFICIAL_WEBSITE,NULL,NULL,NULL,NULL);
//	m_browser1.SetWindowPos(GetWindow(0), PATCHER_WEBBROWSER1_OFFSET_X, PATCHER_WEBBROWSER1_OFFSET_Y, PATCHER_WEBBROWSER1_WIDTH, PATCHER_WEBBROWSER1_HEIGHT, 0);
//	m_browser3.Navigate(PATCHER_QGL_ACCOUNT_WEBSITE,NULL,NULL,NULL,NULL);
//	m_browser3.SetWindowPos(GetWindow(0), PATCHER_WEBBROWSER3_OFFSET_X, PATCHER_WEBBROWSER3_OFFSET_Y, PATCHER_WEBBROWSER3_WIDTH, PATCHER_WEBBROWSER3_HEIGHT, 0);
	
	GetDlgItem(IDC_BUTTON_GOOFFICIAL)->SetWindowPos(GetWindow(IDC_BUTTON_GOOFFICIAL), 30, 43, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
	GetDlgItem(IDC_BUTTON_REGISTRATION)->SetWindowPos(GetWindow(IDC_BUTTON_REGISTRATION), 121, 43, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
	GetDlgItem(IDC_BUTTON_ACCOUNT)->SetWindowPos(GetWindow(IDC_BUTTON_ACCOUNT), 211, 43, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
	GetDlgItem(IDC_BUTTON_POINT)->SetWindowPos(GetWindow(IDC_BUTTON_POINT), 301, 43, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
	UInt x, y;
	x =  43;	y = 104;
	GetDlgItem(IDC_BUTTON_SER1)->SetWindowPos(GetWindow(IDC_BUTTON_SER1), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER2)->SetWindowPos(GetWindow(IDC_BUTTON_SER2), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER3)->SetWindowPos(GetWindow(IDC_BUTTON_SER3), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER4)->SetWindowPos(GetWindow(IDC_BUTTON_SER4), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x =  43;	y += 48;
	GetDlgItem(IDC_BUTTON_SER5)->SetWindowPos(GetWindow(IDC_BUTTON_SER5), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER6)->SetWindowPos(GetWindow(IDC_BUTTON_SER6), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER7)->SetWindowPos(GetWindow(IDC_BUTTON_SER7), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER8)->SetWindowPos(GetWindow(IDC_BUTTON_SER8), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x =  43;	y += 48;
	GetDlgItem(IDC_BUTTON_SER9)->SetWindowPos(GetWindow(IDC_BUTTON_SER9), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER10)->SetWindowPos(GetWindow(IDC_BUTTON_SER10), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER11)->SetWindowPos(GetWindow(IDC_BUTTON_SER11), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER12)->SetWindowPos(GetWindow(IDC_BUTTON_SER12), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x =  43;	y += 48;
	GetDlgItem(IDC_BUTTON_SER13)->SetWindowPos(GetWindow(IDC_BUTTON_SER13), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER14)->SetWindowPos(GetWindow(IDC_BUTTON_SER14), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER15)->SetWindowPos(GetWindow(IDC_BUTTON_SER15), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER16)->SetWindowPos(GetWindow(IDC_BUTTON_SER16), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x =  43;	y += 48;
	GetDlgItem(IDC_BUTTON_SER17)->SetWindowPos(GetWindow(IDC_BUTTON_SER17), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER18)->SetWindowPos(GetWindow(IDC_BUTTON_SER18), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER19)->SetWindowPos(GetWindow(IDC_BUTTON_SER19), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);
	x += 110;
	GetDlgItem(IDC_BUTTON_SER20)->SetWindowPos(GetWindow(IDC_BUTTON_SER20), x, y, PATCHER_SERVER_BUTTON_WIDTH, PATCHER_SERVER_BUTTON_HEIGHT, 0);

	GetDlgItem(IDC_BUTTON_PAGEUP)->SetWindowPos(GetWindow(IDC_BUTTON_PAGEUP), 490, 218, 25, 63, 0);
	GetDlgItem(IDC_BUTTON_PAGEDOWN)->SetWindowPos(GetWindow(IDC_BUTTON_PAGEDOWN), 490, 281, 25, 63, 0);

	CComboBox* cb = (CComboBox*)(GetDlgItem(IDC_COMBO_SERVERLINE));
	if(cb)
		cb->SetCurSel(theApp.GetSelectedLine());

	HidePage(2);
	HidePage(3);
	ShowPage(1);

	AfxBeginThread(startDownloadTracker, 0);

	m_bmpWebBg		= NULL;
	m_bmpServerBg	= NULL;
	m_bmpPatchBg	= NULL;

	for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
	{
		m_bmpDL[i]	= NULL;
		m_bmpIN[i]	= NULL;
	}
	m_bmpEmp		= NULL;

	BOOL loadBmpSucceed = LoadBitmapResource();

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，以便繪製圖示。
// 對於使用文件/檢視模式的 MFC 應用程式，框架會自動完成此作業。

void CQGLPatcherDlg::OnPaint() 
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
		CPaintDC dc(this);              // device context for painting
		CDC dcMem;                  // memory device context
		dcMem.CreateCompatibleDC(&dc);

		// Select the bmp into the tmp memory DC
		switch( theApp.GetCurPage() )
		{
		case PATCHER_PAGE1:
			{
				CBitmap* pOldBmp = (CBitmap*) dcMem.SelectObject(m_bmpWebBg);
				BITMAP bmpInfo;
				m_bmpWebBg->GetBitmap(&bmpInfo);
				DrawBitmap( (CMemDC*)&dc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMem, 0, 0);
				dcMem.SelectObject(pOldBmp);
			}
			break;
		case PATCHER_PAGE2:
			{
				CBitmap* pOldBmp = (CBitmap*) dcMem.SelectObject(m_bmpServerBg);
				BITMAP bmpInfo;
				m_bmpServerBg->GetBitmap(&bmpInfo);
				DrawBitmap( (CMemDC*)&dc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMem, 0, 0);
				dcMem.SelectObject(pOldBmp);
			}
			break;
		case PATCHER_PAGE3:
			{
				CBitmap* pOldBmp = (CBitmap*) dcMem.SelectObject(m_bmpPatchBg);
				BITMAP bmpInfo;
				m_bmpPatchBg->GetBitmap(&bmpInfo);
				DrawBitmap( (CMemDC*)&dc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMem, 0, 0);

				time_t now;
				time(&now);
				CBitmap* pStatusBmp;
				switch( theApp.GetDLStatus() )
				{
				case PATCHER_DL_STATUSDL:
					pStatusBmp = (CBitmap*) dcMem.SelectObject(m_bmpDL[(now-m_PrevTime)%4]);
					DrawBitmap( (CMemDC*)&dc, 30, 350, 99, 34, &dcMem, 0, 0 );
					break;
				case PATCHER_DL_STATUSIN:
					pStatusBmp = (CBitmap*) dcMem.SelectObject(m_bmpIN[(now-m_PrevTime)%4]);
					DrawBitmap( (CMemDC*)&dc, 30, 350, 99, 34, &dcMem, 0, 0 );
					break;
				case PATCHER_DL_STATUSEMPTY:
					pStatusBmp = (CBitmap*) dcMem.SelectObject(m_bmpEmp);
					DrawBitmap( (CMemDC*)&dc, 30, 350, 99, 34, &dcMem, 0, 0 );
					break;
				};

				dcMem.SelectObject(pOldBmp);

				dc.SetBkMode(TRANSPARENT);
				dc.SetTextColor(RGB(255,255,0));
				RECT tempbox;
				tempbox.left = 94;	tempbox.top = 38;	tempbox.right = 208;	tempbox.bottom = 59;
				dc.DrawText(theApp.m_curVersion.c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
				tempbox.left = 294;	tempbox.top = 38;	tempbox.right = 409;	tempbox.bottom = 59;
				dc.DrawText(theApp.m_serVersion.c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
			}
			break;
		}
	}
}

void CQGLPatcherDlg::OnSize(UINT nType, int cx, int cy)
{
	// Load the image
	HBITMAP m_hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), PATCHER_IMAGE_WEB_BACKGROUND, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (m_hBitmap == NULL)
	{
		MessageBox(_T("Error loading bitmap"));
		return;
	}

	//Get information about the bitmap..
	GetObject(m_hBitmap, sizeof(m_bmpWebBg), &m_bmpWebBg);	// Get info about the bitmap 
	// Put the bitmap into a memory device context
	CPaintDC dc(this);
	//get a memory dc object
	CDC dcMem;
	//create a compatible dc
	dcMem.CreateCompatibleDC(&dc);	// Select the bitmap into the in-memory DC
	//Select the bitmap into the dc
	CBitmap* pOldBitmap = dcMem.SelectObject(CBitmap::FromHandle(m_hBitmap));
	//Create a couple of region objects.
/*	CRgn crRgn, crRgnTmp;
	//create an empty region
	crRgn.CreateRectRgn(0, 0, 0, 0);
	//Create a region from a bitmap with transparency colour of Purple
	COLORREF crTransparent = TRANSPARENTCOLOR;	
	int iX = 0;
	for (int iY = 0; iY < PATCHER_WINDOW_SIZE_HEIGHT; iY++)
	{
		do
		{
			//skip over transparent pixels at start of lines.
			while (iX <= PATCHER_WINDOW_SIZE_WIDTH && dcMem.GetPixel(iX, iY) == crTransparent)
				iX++;
			//remember this pixel
			int iLeftX = iX;
			//now find first non transparent pixel
			while (iX <= PATCHER_WINDOW_SIZE_WIDTH && dcMem.GetPixel(iX, iY) != crTransparent)
				++iX;
			//create a temp region on this info
			crRgnTmp.CreateRectRgn(iLeftX, iY, iX, iY+1);
			//combine into main region.
			crRgn.CombineRgn(&crRgn, &crRgnTmp, RGN_OR);
			//delete the temp region for next pass (otherwise you'll get an ASSERT)
			crRgnTmp.DeleteObject();
		}while(iX < PATCHER_WINDOW_SIZE_WIDTH);
		iX = 0;
	}
	//Centre it on current desktop
	SetWindowRgn(crRgn, TRUE);
*/
	// Free resources.
	dcMem.SelectObject(pOldBitmap);	// Put the original bitmap back (prevents memory leaks)
	dcMem.DeleteDC();
//	crRgn.DeleteObject();

	DeleteObject(m_hBitmap);	//not really need but what the heck.
}

//當使用者拖曳最小化視窗時，系統呼叫這個功能取得游標顯示。
HCURSOR CQGLPatcherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQGLPatcherDlg::OnBnClickedButtonGoofficial()
{
	NavigateToBlank(PATCHER_COMPANY_OFFICIAL_WEBSITE);
}

void CQGLPatcherDlg::OnBnClickedButtonRegistration()
{
	NavigateToBlank(PATCHER_QGL_REGISTRATION_WEBSITE);
}

void CQGLPatcherDlg::OnBnClickedButtonAccount()
{
	NavigateToBlank(PATCHER_QGL_ACCOUNT_WEBSITE);
}

void CQGLPatcherDlg::OnBnClickedButtonPoint()
{
	NavigateToBlank(PATCHER_QGL_POINT_WEBSITE);
}

void CQGLPatcherDlg::NavigateToBlank(String address)
{
	_variant_t strVar("_blank");
	VARIANT targetflame;
		targetflame.vt = VT_BSTR;
		targetflame.bstrVal = strVar.bstrVal;

	m_browser1.Navigate(address.c_str(),NULL,&targetflame,NULL,NULL);
}

BOOL CQGLPatcherDlg::LoadFileBitmap(CBitmap* pBmp, LPCTSTR szFilename)
{
	pBmp->DeleteObject();
	return pBmp->Attach(LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
}

void CQGLPatcherDlg::DrawBitmap(CMemDC* pDC, int xStart, int yStart, int wWidth, int wHeight, CDC* pTmpDC, int xSource, int ySource)
{
	CDC hdcMem;
	hdcMem.CreateCompatibleDC(NULL);
	pDC->BitBlt(xStart, yStart, wWidth, wHeight, pTmpDC, xSource, ySource,SRCCOPY);
	hdcMem.DeleteDC();
}

void CQGLPatcherDlg::OnDestroy()
{
	CDialog::OnDestroy();

   // TODO: Add your message handler code here
	m_bmpWebBg->DeleteObject();
	m_bmpServerBg->DeleteObject();
	m_bmpPatchBg->DeleteObject();
	for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
	{
		m_bmpIN[i]->DeleteObject();
		m_bmpDL[i]->DeleteObject();
	}
	m_bmpEmp->DeleteObject();
}

void CQGLPatcherDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SendMessage(WM_SYSCOMMAND, SC_MOVE|0x0002);        
}

void CQGLPatcherDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	switch( theApp.GetCurPage() )
	{
	case PATCHER_PAGE1:
		switch( theApp.GetCurStatus() )
		{
		case PATCHER_TRACKER_DOWNLOADING:
			MessageBox(
				theApp.GetStringTable()->Get(_T("MSG_TRACKER_DOWNLOADING")).c_str(),
				theApp.GetStringTable()->Get(_T("INFO_GENERAL")).c_str()
				);
			break;
		case PATCHER_FREENAVIGATE:
			ChangePage(PATCHER_PAGE2);
			break;
		}
		break;
	case PATCHER_PAGE2:
		ChangePage(PATCHER_PAGE3);
		break;
	case PATCHER_PAGE3:
		switch( theApp.GetCurStatus() )
		{
		case PATCHER_FREENAVIGATE:
			ConfirmDownload();
			break;
		case PATCHER_SUMMARY_DOWNLOADING:
			MessageBox(
				theApp.GetStringTable()->Get(_T("MSG_SERVERLIST_DOWNLOADING")).c_str(),
				theApp.GetStringTable()->Get(_T("INFO_GENERAL")).c_str()
				);
			break;
		}
		break;
	}
}

void CQGLPatcherDlg::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	switch( theApp.GetCurPage() )
	{
	case PATCHER_PAGE1:
		OnCancel();
		break;
	case PATCHER_PAGE2:
		ChangePage(PATCHER_PAGE1);
		break;
	case PATCHER_PAGE3:
		ChangePage(PATCHER_PAGE2);
		break;
	}
}

void CQGLPatcherDlg::ChangePage(unsigned int pageno)
{
	switch( pageno )
	{
	case PATCHER_PAGE3:
		AfxBeginThread(startDownloadSummary, 0);
	case PATCHER_PAGE1:
	case PATCHER_PAGE2:
		HidePage(theApp.GetCurPage());
		theApp.SetCurPage(pageno);
		ShowPage(theApp.GetCurPage());
		RedrawWindow();
		break;
	}
}

void CQGLPatcherDlg::HidePage(unsigned int pageno)
{
	switch( pageno )
	{
	case PATCHER_PAGE1:
		GetDlgItem(IDC_BUTTON_GOOFFICIAL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ACCOUNT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_REGISTRATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_POINT)->ShowWindow(SW_HIDE);
//		::ShowWindow(GetDlgItem(IDC_EXPLORER_UPPER)->GetSafeHwnd(), SW_HIDE);
		GetDlgItem(IDC_BUTTON_EXIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_HIDE);
		break;
	case PATCHER_PAGE2:
		GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_NEXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_CURSERVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_SERVERLINE)->ShowWindow(SW_HIDE);
		for( Index i = C_INDEX(0); i < C_INDEX(MAX_SERVER_BUTTON); ++i )
			m_serverChoice[i].ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_PAGEUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_PAGEDOWN)->ShowWindow(SW_HIDE);
		break;
	case PATCHER_PAGE3:
//		::ShowWindow(GetDlgItem(IDC_EXPLORER_CENTER)->GetSafeHwnd(), SW_HIDE);
		GetDlgItem(IDC_BUTTON_DL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PROGRESS_BIG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PROGRESS_SMALL)->ShowWindow(SW_HIDE);
		break;
	}
}

void CQGLPatcherDlg::ShowPage(unsigned int pageno)
{
	switch( pageno )
	{
	case PATCHER_PAGE1:
		GetDlgItem(IDC_BUTTON_EXIT)->SetWindowPos(GetWindow(IDC_BUTTON_EXIT), 327, 353, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
		GetDlgItem(IDC_BUTTON_START)->SetWindowPos(GetWindow(IDC_BUTTON_START), 418, 353, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
		GetDlgItem(IDC_BUTTON_GOOFFICIAL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_ACCOUNT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_REGISTRATION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_POINT)->ShowWindow(SW_SHOW);
//		::ShowWindow(GetDlgItem(IDC_EXPLORER_UPPER)->GetSafeHwnd(), SW_SHOW);
		GetDlgItem(IDC_BUTTON_EXIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_SHOW);
		break;
	case PATCHER_PAGE2:
		{
			GetDlgItem(IDC_BUTTON_PREV)->SetWindowPos(GetWindow(IDC_BUTTON_PREV), 320, 355, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
			GetDlgItem(IDC_BUTTON_NEXT)->SetWindowPos(GetWindow(IDC_BUTTON_NEXT), 409, 355, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
			GetDlgItem(IDC_BUTTON_CURSERVER)->SetWindowPos(GetWindow(IDC_BUTTON_CURSERVER), 32, 44, 90, 36, 0);
			GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_NEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_CURSERVER)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_COMBO_SERVERLINE)->ShowWindow(SW_SHOW);

			GetDlgItem(IDC_BUTTON_PAGEUP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_PAGEDOWN)->ShowWindow(SW_SHOW);

			UInt totalpage = (UInt)(theApp.m_serverData.size() + MAX_SERVER_BUTTON - 1) / MAX_SERVER_BUTTON;
			SetTotalServerPage(totalpage);
			UInt curpage = theApp.GetSelectedServer() / MAX_SERVER_BUTTON;
			SetCurServerPage(curpage);
			RedrawCurServerSelectionPage();

			CComboBox* pCb = (CComboBox*)(GetDlgItem(IDC_COMBO_SERVERLINE));
			if( pCb )
			{
				pCb->ResetContent();
				ServerData* pSd = theApp.GetServerInfo(theApp.GetSelectedServer());	// server
				if( pSd )
				{
					for( Index i = C_INDEX(0); i < pSd->server.size(); ++i )	// line
					{
						LineCombination& linedata = pSd->server[i];
						pCb->AddString(linedata.lineName.c_str());
					}
					UInt selindex = (theApp.GetSelectedLine() >= pSd->server.size()) ? 0 : theApp.GetSelectedLine();
					pCb->SetCurSel(selindex);
					theApp.SetSelectedLine(selindex);
				}
			}
		}
		break;
	case PATCHER_PAGE3:
//		::ShowWindow(GetDlgItem(IDC_EXPLORER_CENTER)->GetSafeHwnd(), SW_SHOW);
		GetDlgItem(IDC_BUTTON_DL)->SetWindowPos(GetWindow(IDC_BUTTON_DL), 326, 353, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
		GetDlgItem(IDC_BUTTON_PREV)->SetWindowPos(GetWindow(IDC_BUTTON_PREV), 235, 353, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
		GetDlgItem(IDC_BUTTON_START)->SetWindowPos(GetWindow(IDC_BUTTON_START), 415, 353, PATCHER_DEFAULT_BUTTON_WIDTH, PATCHER_DEFAULT_BUTTON_HEIGHT, 0);
		GetDlgItem(IDC_PROGRESS_BIG)->SetWindowPos(GetWindow(IDC_PROGRESS_BIG), 98, 312, 395, 11, 0);
		GetDlgItem(IDC_PROGRESS_SMALL)->SetWindowPos(GetWindow(IDC_PROGRESS_SMALL), 98, 330, 395, 11, 0);
		GetDlgItem(IDC_BUTTON_DL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_PROGRESS_BIG)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_PROGRESS_SMALL)->ShowWindow(SW_SHOW);
		break;
	}
}

BOOL CQGLPatcherDlg::LoadBitmapResource()
{
	BOOL success	= TRUE;
	m_bmpWebBg		= SafeCreateObject(CBitmap);
	m_bmpServerBg	= SafeCreateObject(CBitmap);
	m_bmpPatchBg	= SafeCreateObject(CBitmap);
	for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
	{
		m_bmpDL[i]	= SafeCreateObject(CBitmap);
		m_bmpIN[i]	= SafeCreateObject(CBitmap);
	}
	m_bmpEmp		= SafeCreateObject(CBitmap);
	if (!LoadFileBitmap(m_bmpWebBg, PATCHER_IMAGE_WEB_BACKGROUND))
	{
		SafeDeleteObject(m_bmpWebBg);
		m_bmpWebBg		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpServerBg, PATCHER_IMAGE_SERVER_BACKGROUND))
	{
		SafeDeleteObject(m_bmpServerBg);
		m_bmpServerBg	= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpPatchBg, PATCHER_IMAGE_PATCH_BACKGROUND))
	{
		SafeDeleteObject(m_bmpPatchBg);
		m_bmpPatchBg	= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpDL[0], PATCHER_IMAGE_DOWNLOADING_A))
	{
		SafeDeleteObject(m_bmpDL[0]);
		m_bmpDL[0]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpDL[1], PATCHER_IMAGE_DOWNLOADING_B))
	{
		SafeDeleteObject(m_bmpDL[1]);
		m_bmpDL[1]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpDL[2], PATCHER_IMAGE_DOWNLOADING_C))
	{
		SafeDeleteObject(m_bmpDL[2]);
		m_bmpDL[2]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpDL[3], PATCHER_IMAGE_DOWNLOADING_D))
	{
		SafeDeleteObject(m_bmpDL[3]);
		m_bmpDL[3]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpIN[0], PATCHER_IMAGE_INSTALLING_A))
	{
		SafeDeleteObject(m_bmpIN[0]);
		m_bmpIN[0]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpIN[1], PATCHER_IMAGE_INSTALLING_B))
	{
		SafeDeleteObject(m_bmpIN[1]);
		m_bmpIN[1]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpIN[2], PATCHER_IMAGE_INSTALLING_C))
	{
		SafeDeleteObject(m_bmpIN[2]);
		m_bmpIN[2]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpIN[3], PATCHER_IMAGE_INSTALLING_D))
	{
		SafeDeleteObject(m_bmpIN[3]);
		m_bmpIN[3]		= NULL;
		success			= FALSE;
	}
	if (!LoadFileBitmap(m_bmpEmp, PATCHER_IMAGE_EMPTY))
	{
		SafeDeleteObject(m_bmpEmp);
		m_bmpEmp		= NULL;
		success			= FALSE;
	}

	if( !m_goOfficial.LoadBitmap(PATCHER_IMAGE_WEB_BUTTON1, PATCHER_IMAGE_WEB_BUTTON1_OVER, PATCHER_IMAGE_WEB_BUTTON1_DOWN) )
		success = FALSE;
	if( !m_goAccount.LoadBitmap(PATCHER_IMAGE_WEB_BUTTON2, PATCHER_IMAGE_WEB_BUTTON2_OVER, PATCHER_IMAGE_WEB_BUTTON2_DOWN) )
		success = FALSE;
	if( !m_goRegistration.LoadBitmap(PATCHER_IMAGE_WEB_BUTTON3, PATCHER_IMAGE_WEB_BUTTON3_OVER, PATCHER_IMAGE_WEB_BUTTON3_DOWN) )
		success = FALSE;
	if( !m_goPoint.LoadBitmap(PATCHER_IMAGE_WEB_BUTTON4, PATCHER_IMAGE_WEB_BUTTON4_OVER, PATCHER_IMAGE_WEB_BUTTON4_DOWN) )
		success = FALSE;
	if( !m_exitGame.LoadBitmap(PATCHER_IMAGE_WEB_BUTTON5, PATCHER_IMAGE_WEB_BUTTON5_OVER, PATCHER_IMAGE_WEB_BUTTON5_DOWN) )
		success = FALSE;
	if( !m_startGame.LoadBitmap(PATCHER_IMAGE_WEB_BUTTON6, PATCHER_IMAGE_WEB_BUTTON6_OVER, PATCHER_IMAGE_WEB_BUTTON6_DOWN) )
		success = FALSE;

	if( !m_serverCur.LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_SLOT, PATCHER_IMAGE_SERVER_BUTTON_SLOT, PATCHER_IMAGE_SERVER_BUTTON_SLOT) )
		success = FALSE;
	if( !m_serverPageUp.LoadBitmap(PATCHER_IMAGE_SERVER_PAGEUP, PATCHER_IMAGE_SERVER_PAGEUP_OVER, PATCHER_IMAGE_SERVER_PAGEUP_DOWN) )
		success = FALSE;
	if( !m_serverPageDown.LoadBitmap(PATCHER_IMAGE_SERVER_PAGEDOWN, PATCHER_IMAGE_SERVER_PAGEDOWN_OVER, PATCHER_IMAGE_SERVER_PAGEDOWN_DOWN) )
		success = FALSE;
//	if( !LoadServerButtonBitmapResource() )
//		success = FALSE;

	if( !m_startPatch.LoadBitmap(PATCHER_IMAGE_PATCH_BUTTON_DL, PATCHER_IMAGE_PATCH_BUTTON_DL_OVER, PATCHER_IMAGE_PATCH_BUTTON_DL_DOWN) )
		success = FALSE;
	if( !m_serverNext.LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_NEXT, PATCHER_IMAGE_SERVER_BUTTON_NEXT_OVER, PATCHER_IMAGE_SERVER_BUTTON_NEXT_DOWN) )
		success = FALSE;
	if( !m_serverPrev.LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_PREV, PATCHER_IMAGE_SERVER_BUTTON_PREV_OVER, PATCHER_IMAGE_SERVER_BUTTON_PREV_DOWN) )
		success = FALSE;

	return success;
}

BOOL CQGLPatcherDlg::LoadServerButtonBitmapResource(UInt* status, UInt noServerInPage)
{
	BOOL success = TRUE;
	for( Index i = C_INDEX(0); i < noServerInPage; ++i )
	{
		switch( status[i] )
		{
		case 0:
			if( !m_serverChoice[i].LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_SLOT1, PATCHER_IMAGE_SERVER_BUTTON_SLOT1_OVER, PATCHER_IMAGE_SERVER_BUTTON_SLOT1_DOWN) )
				success = FALSE;
			break;
		case 1:
			if( !m_serverChoice[i].LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_SLOT2, PATCHER_IMAGE_SERVER_BUTTON_SLOT2_OVER, PATCHER_IMAGE_SERVER_BUTTON_SLOT2_DOWN) )
				success = FALSE;
			break;
		case 2:
			if( !m_serverChoice[i].LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_SLOT3, PATCHER_IMAGE_SERVER_BUTTON_SLOT3_OVER, PATCHER_IMAGE_SERVER_BUTTON_SLOT3_DOWN) )
				success = FALSE;
			break;
		case 3:
			if( !m_serverChoice[i].LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_SLOT4, PATCHER_IMAGE_SERVER_BUTTON_SLOT4, PATCHER_IMAGE_SERVER_BUTTON_SLOT4) )
				success = FALSE;
			break;
		}
	}
	for( Index i = noServerInPage; i < C_INDEX(MAX_SERVER_BUTTON); ++i )
	{
		if( !m_serverChoice[i].LoadBitmap(PATCHER_IMAGE_SERVER_BUTTON_SLOT4, PATCHER_IMAGE_SERVER_BUTTON_SLOT4, PATCHER_IMAGE_SERVER_BUTTON_SLOT4) )
			success = FALSE;
	}
	return success;
}

void CQGLPatcherDlg::OnBnClickedButtonSer1()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(0);
}

void CQGLPatcherDlg::OnBnClickedButtonSer2()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(1);
}

void CQGLPatcherDlg::OnBnClickedButtonSer3()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(2);
}

void CQGLPatcherDlg::OnBnClickedButtonSer4()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(3);
}

void CQGLPatcherDlg::OnBnClickedButtonSer5()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(4);
}

void CQGLPatcherDlg::OnBnClickedButtonSer6()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(5);
}

void CQGLPatcherDlg::OnBnClickedButtonSer7()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(6);
}

void CQGLPatcherDlg::OnBnClickedButtonSer8()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(7);
}

void CQGLPatcherDlg::OnBnClickedButtonSer9()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(8);
}

void CQGLPatcherDlg::OnBnClickedButtonSer10()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(9);
}

void CQGLPatcherDlg::OnBnClickedButtonSer11()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(10);
}

void CQGLPatcherDlg::OnBnClickedButtonSer12()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(11);
}

void CQGLPatcherDlg::OnBnClickedButtonSer13()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(12);
}

void CQGLPatcherDlg::OnBnClickedButtonSer14()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(13);
}

void CQGLPatcherDlg::OnBnClickedButtonSer15()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(14);
}

void CQGLPatcherDlg::OnBnClickedButtonSer16()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(15);
}

void CQGLPatcherDlg::OnBnClickedButtonSer17()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(16);
}

void CQGLPatcherDlg::OnBnClickedButtonSer18()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(17);
}

void CQGLPatcherDlg::OnBnClickedButtonSer19()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(18);
}

void CQGLPatcherDlg::OnBnClickedButtonSer20()
{
	// TODO: Add your control notification handler code here
	SelectServerAction(19);
}

void CQGLPatcherDlg::OnBnClickedButtonDl()
{
	NavigateToBlank(PATCHER_QGL_PATCH_WEBSITE);
}

void CQGLPatcherDlg::OnBnClickedButtonNext()
{
	// TODO: Add your control notification handler code here
	switch( theApp.GetCurPage() )
	{
	case PATCHER_PAGE1:
		ChangePage(PATCHER_PAGE2);
		break;
	case PATCHER_PAGE2:
		ChangePage(PATCHER_PAGE3);
		break;
	case PATCHER_PAGE3:
		OnOK();
		break;
	}
}

void CQGLPatcherDlg::OnBnClickedButtonPrev()
{
	// TODO: Add your control notification handler code here
	switch( theApp.GetCurPage() )
	{
	case PATCHER_PAGE1:
		OnCancel();
		break;
	case PATCHER_PAGE2:
		ChangePage(PATCHER_PAGE1);
		break;
	case PATCHER_PAGE3:
		switch( theApp.GetCurStatus() )
		{
		case PATCHER_FREENAVIGATE:
			ChangePage(PATCHER_PAGE2);
			break;
		}
		break;
	}
}

void CQGLPatcherDlg::OnBnClickedButtonSerCur()
{
	// TODO: Add your control notification handler code here
}

VOID CQGLPatcherDlg::SelectServerAction(UInt serverNo)
{
	m_serverChoice[theApp.GetSelectedServer() % MAX_SERVER_BUTTON].UnlockStatus();
	m_serverChoice[theApp.GetSelectedServer() % MAX_SERVER_BUTTON].RedrawWindow();

	theApp.SetSelectedServer(serverNo + GetCurServerPage() * MAX_SERVER_BUTTON);
	m_serverChoice[theApp.GetSelectedServer() % MAX_SERVER_BUTTON].LockStatus(3);
	m_serverChoice[theApp.GetSelectedServer() % MAX_SERVER_BUTTON].RedrawWindow();

	// Line content change
	CComboBox* pCb = (CComboBox*)(GetDlgItem(IDC_COMBO_SERVERLINE));
	if( pCb )
	{
		pCb->ResetContent();
		ServerData* pSd = theApp.GetServerInfo(theApp.GetSelectedServer());	// server
		if( pSd )
		{
			for( Index i = C_INDEX(0); i < pSd->server.size(); ++i )	// line
			{
				LineCombination& linedata = pSd->server[i];
				pCb->AddString(linedata.lineName.c_str());
			}
			UInt selindex = (theApp.GetSelectedLine() >= pSd->server.size()) ? 0 : theApp.GetSelectedLine();
			pCb->SetCurSel(selindex);
			theApp.SetSelectedLine(selindex);
		}
	}
}

void CQGLPatcherDlg::OnBnClickedButtonPagedown()
{
	// TODO: Add your control notification handler code here
	SetCurServerPage(GetCurServerPage()+1);
	RedrawCurServerSelectionPage();
}

void CQGLPatcherDlg::OnBnClickedButtonPageup()
{
	// TODO: Add your control notification handler code here
	SetCurServerPage(GetCurServerPage()-1);
	RedrawCurServerSelectionPage();
}

VOID CQGLPatcherDlg::RedrawCurServerSelectionPage()
{
	m_serverCur.SetText(theApp.m_serverData[theApp.GetDefaultServer()].name);

	UInt maxServerInPage = MAX_SERVER_BUTTON;
	if( theApp.m_serverData.size() - GetCurServerPage() * MAX_SERVER_BUTTON < MAX_SERVER_BUTTON )
		maxServerInPage = (UInt)theApp.m_serverData.size() - GetCurServerPage() * MAX_SERVER_BUTTON;
	UInt* status = SafeAllocate(UInt, maxServerInPage);
	for( Index i = C_INDEX(0); i < maxServerInPage ; ++i )
	{
		UInt curserver = GetCurServerPage() * MAX_SERVER_BUTTON + i;
		if( theApp.m_serverData[curserver].status==0 )
			status[i] = 4; // 0:normal, 1:ok, 2:busy, 3:disabled
		else if( theApp.m_serverData[curserver].status==1 )
			status[i] = 0;
	}
	LoadServerButtonBitmapResource(status, maxServerInPage);
	SafeDeallocate(status);

	for( Index i = C_INDEX(0); i < C_INDEX(MAX_SERVER_BUTTON); ++i )
	{
		m_serverChoice[i].ShowWindow(SW_SHOW);
		m_serverChoice[i].UnlockStatus();
		UInt curserver = GetCurServerPage() * MAX_SERVER_BUTTON + i;
		if( i < maxServerInPage )
			m_serverChoice[i].SetText(theApp.m_serverData[curserver].name);
		else
		{
			String dummy;
			dummy.Empty();
			m_serverChoice[i].SetText(dummy);
		}
	}
	if( theApp.GetSelectedServer() / MAX_SERVER_BUTTON==GetCurServerPage() )
		m_serverChoice[theApp.GetSelectedServer() % MAX_SERVER_BUTTON].LockStatus(3);
	if( GetCurServerPage()==0 )
		m_serverPageUp.LockStatus(3);
	else
		m_serverPageUp.UnlockStatus();
	if( GetCurServerPage()==GetTotalServerPage()-1 )
		m_serverPageDown.LockStatus(3);
	else
		m_serverPageDown.UnlockStatus();

	for( Index i = C_INDEX(0); i < C_INDEX(MAX_SERVER_BUTTON); ++i )
		m_serverChoice[i].RedrawWindow();
	m_serverPageDown.RedrawWindow();
	m_serverPageUp.RedrawWindow();
}

VOID CQGLPatcherDlg::ConfirmDownload()
{
	theApp.SetCurStatus(PATCHER_DOWNLOAD);
	m_startGame.LockStatus(3);
	m_startGame.RedrawWindow();
	m_serverPrev.LockStatus(3);
	m_serverPrev.RedrawWindow();

	AfxBeginThread(startDownload, 0);
	AfxBeginThread(startDownloadTimer, 0);
}

VOID CQGLPatcherDlg::ReleaseConfirmDownloadUI()
{
	theApp.SetCurStatus(PATCHER_FREENAVIGATE);
	m_startGame.UnlockStatus();
	m_startGame.RedrawWindow();
	m_serverPrev.UnlockStatus();
	m_serverPrev.RedrawWindow();
}

UINT CQGLPatcherDlg::startDownloadTracker(LPVOID pParam)
{
	if( theApp.DownloadTrackerIpIni()==FALSE )
		exit(0);
	return 0;
}

UINT CQGLPatcherDlg::startDownloadSummary(LPVOID pParam)
{
	theApp.DownloadPatchSummary();
	return 0;
}

UINT CQGLPatcherDlg::startDownload(LPVOID pParam) // thread for download
{
	theApp.StartDownloadProcess();
	exit(0);
	return 0;
}

UINT CQGLPatcherDlg::startDownloadTimer(LPVOID pParam) // thread for GUI (progress bar)
{
	UInt total = theApp.GetDownloadTotal();
	UInt pos = theApp.GetDownloadPos();
	theApp.GetMainDialog()->m_process_big.SetRange(0, (short)total);
	UInt fTotal = theApp.GetFileDownloadTotal();
	UInt fPos = theApp.GetFileDownloadPos();

	RECT rect;
	rect.left = 30;	rect.top = 350; rect.right = 129, rect.bottom = 384;
	while(theApp.GetCurStatus()==PATCHER_DOWNLOAD)
	{
		// Total Progress Bar
		total = theApp.GetDownloadTotal();
		theApp.GetMainDialog()->m_process_big.SetRange(0, (short)total);
		pos = theApp.GetDownloadPos();
		theApp.GetMainDialog()->m_process_big.SetPos(pos);
		// Per File Progress Bar
		fTotal = theApp.GetFileDownloadTotal();
		theApp.GetMainDialog()->m_process_small.SetRange(0, (short)fTotal);
		fPos = theApp.GetFileDownloadPos();
		theApp.GetMainDialog()->m_process_small.SetPos(fPos);
		theApp.GetMainDialog()->RedrawWindow(&rect);
		::Sleep(500);
	}

	// Total Progress Bar
	total = theApp.GetDownloadTotal();
	theApp.GetMainDialog()->m_process_big.SetRange(0, (short)total);
	pos = theApp.GetDownloadPos();
	theApp.GetMainDialog()->m_process_big.SetPos(pos);
	// Per File Progress Bar
	fTotal = theApp.GetFileDownloadTotal();
	theApp.GetMainDialog()->m_process_small.SetRange(0, (short)fTotal);
	fPos = theApp.GetFileDownloadPos();
	theApp.GetMainDialog()->m_process_small.SetPos(fPos);

	theApp.SetCurStatus(PATCHER_FREENAVIGATE);
	theApp.GetMainDialog()->m_startGame.UnlockStatus();
	theApp.GetMainDialog()->m_startGame.RedrawWindow();
	theApp.GetMainDialog()->m_serverPrev.UnlockStatus();
	theApp.GetMainDialog()->m_serverPrev.RedrawWindow();

	return 0;
}

VOID CQGLPatcherDlg::RedrawSerVerDisplay()
{
	RECT tempbox;
	tempbox.left = 294;	tempbox.top = 38;	tempbox.right = 409;	tempbox.bottom = 59;
	RedrawWindow(&tempbox);
}

VOID CQGLPatcherDlg::RedrawCurVerDisplay()
{
	RECT tempbox;
	tempbox.left = 94;	tempbox.top = 38;	tempbox.right = 207;	tempbox.bottom = 59;
	RedrawWindow(&tempbox);
}

void CQGLPatcherDlg::OnCbnSelchangeComboServerline()
{
	// TODO: Add your control notification handler code here
	CComboBox* cb = (CComboBox*)(GetDlgItem(IDC_COMBO_SERVERLINE));
	theApp.SetSelectedLine(cb->GetCurSel());
}
