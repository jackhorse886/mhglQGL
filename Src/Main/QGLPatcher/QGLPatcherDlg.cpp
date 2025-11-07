// QGLPatcherDlg.cpp : ��@��
//

#include "stdafx.h"
#include "QGLPatcher.h"
#include "QGLPatcherDlg.h"
#include ".\qglpatcherdlg.h"
#include <sys/stat.h>

#define PATCHER_WINDOW_SIZE_WIDTH				800
#define PATCHER_WINDOW_SIZE_HEIGHT				532

#define PATCHER_IMAGE_WEB_BACKGROUND			_T("res/NewsBg.bmp")
#define PATCHER_IMAGE_PATCH_BACKGROUND			_T("res/PatchBg.bmp")
#define PATCHER_IMAGE_SERVER_BACKGROUND			_T("res/ServerBg.bmp")

#define MAX_SERVER_BUTTON						60

#define TRANSPARENTCOLOR						RGB(0, 255, 0);

// CQGLPatcherDlg ��ܤ��



CQGLPatcherDlg::CQGLPatcherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQGLPatcherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_bManualSelected = FALSE;
	m_initTrackerDlg = NULL;
}

CQGLPatcherDlg::~CQGLPatcherDlg()
{
	if( m_bmpWebBg )		SafeDeleteObject(m_bmpWebBg);
	if( m_bmpServerBg )		SafeDeleteObject(m_bmpServerBg);
	if( m_bmpPatchBg )		SafeDeleteObject(m_bmpPatchBg);
	if( m_initTrackerDlg )
		SafeDeleteObject(m_initTrackerDlg);
}

void CQGLPatcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_UPPER, m_browser1);
	DDX_Control(pDX, IDC_EXPLORER_CENTER, m_browser3);
	DDX_Control(pDX, IDC_EXPLORER_RIGHT, m_browser4);
	DDX_Control(pDX, IDC_BUTTON_GOOFFICIAL, m_goOfficial);
	DDX_Control(pDX, IDC_BUTTON_ACCOUNT, m_goAccount);
	DDX_Control(pDX, IDC_BUTTON_REGISTRATION, m_goRegistration);
	DDX_Control(pDX, IDC_BUTTON_POINT, m_goPoint);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_exitGame);
	DDX_Control(pDX, IDC_BUTTON_START, m_startGame);
	DDX_Control(pDX, IDC_BUTTON_PATCHSTART, m_startGamePatch);
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
	DDX_Control(pDX, IDC_BUTTON_SER21, m_serverChoice[20]);
	DDX_Control(pDX, IDC_BUTTON_SER22, m_serverChoice[21]);
	DDX_Control(pDX, IDC_BUTTON_SER23, m_serverChoice[22]);
	DDX_Control(pDX, IDC_BUTTON_SER24, m_serverChoice[23]);
	DDX_Control(pDX, IDC_BUTTON_SER25, m_serverChoice[24]);
	DDX_Control(pDX, IDC_BUTTON_SER26, m_serverChoice[25]);
	DDX_Control(pDX, IDC_BUTTON_SER27, m_serverChoice[26]);
	DDX_Control(pDX, IDC_BUTTON_SER28, m_serverChoice[27]);
	DDX_Control(pDX, IDC_BUTTON_SER29, m_serverChoice[28]);
	DDX_Control(pDX, IDC_BUTTON_SER30, m_serverChoice[29]);
	DDX_Control(pDX, IDC_BUTTON_SER31, m_serverChoice[30]);
	DDX_Control(pDX, IDC_BUTTON_SER32, m_serverChoice[31]);
	DDX_Control(pDX, IDC_BUTTON_SER33, m_serverChoice[32]);
	DDX_Control(pDX, IDC_BUTTON_SER34, m_serverChoice[33]);
	DDX_Control(pDX, IDC_BUTTON_SER35, m_serverChoice[34]);
	DDX_Control(pDX, IDC_BUTTON_SER36, m_serverChoice[35]);
	DDX_Control(pDX, IDC_BUTTON_SER37, m_serverChoice[36]);
	DDX_Control(pDX, IDC_BUTTON_SER38, m_serverChoice[37]);
	DDX_Control(pDX, IDC_BUTTON_SER39, m_serverChoice[38]);
	DDX_Control(pDX, IDC_BUTTON_SER40, m_serverChoice[39]);
	DDX_Control(pDX, IDC_BUTTON_SER41, m_serverChoice[40]);
	DDX_Control(pDX, IDC_BUTTON_SER42, m_serverChoice[41]);
	DDX_Control(pDX, IDC_BUTTON_SER43, m_serverChoice[42]);
	DDX_Control(pDX, IDC_BUTTON_SER44, m_serverChoice[43]);
	DDX_Control(pDX, IDC_BUTTON_SER45, m_serverChoice[44]);
	DDX_Control(pDX, IDC_BUTTON_SER46, m_serverChoice[45]);
	DDX_Control(pDX, IDC_BUTTON_SER47, m_serverChoice[46]);
	DDX_Control(pDX, IDC_BUTTON_SER48, m_serverChoice[47]);
	DDX_Control(pDX, IDC_BUTTON_SER49, m_serverChoice[48]);
	DDX_Control(pDX, IDC_BUTTON_SER50, m_serverChoice[49]);
	DDX_Control(pDX, IDC_BUTTON_SER51, m_serverChoice[50]);
	DDX_Control(pDX, IDC_BUTTON_SER52, m_serverChoice[51]);
	DDX_Control(pDX, IDC_BUTTON_SER53, m_serverChoice[52]);
	DDX_Control(pDX, IDC_BUTTON_SER54, m_serverChoice[53]);
	DDX_Control(pDX, IDC_BUTTON_SER55, m_serverChoice[54]);
	DDX_Control(pDX, IDC_BUTTON_SER56, m_serverChoice[55]);
	DDX_Control(pDX, IDC_BUTTON_SER57, m_serverChoice[56]);
	DDX_Control(pDX, IDC_BUTTON_SER58, m_serverChoice[57]);
	DDX_Control(pDX, IDC_BUTTON_SER59, m_serverChoice[58]);
	DDX_Control(pDX, IDC_BUTTON_SER60, m_serverChoice[59]);
	DDX_Control(pDX, IDC_BUTTON_DL, m_startPatch);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_serverNext);
	DDX_Control(pDX, IDC_BUTTON_PREV, m_serverPrev);
	DDX_Control(pDX, IDC_BUTTON_CURSERVER, m_serverCur);
	DDX_Control(pDX, IDC_BUTTON_PAGEUP, m_serverPageUp);
	DDX_Control(pDX, IDC_BUTTON_PAGEDOWN, m_serverPageDown);
	DDX_Control(pDX, IDC_BUTTON_ZPAGEUP, m_serverZPageUp);
	DDX_Control(pDX, IDC_BUTTON_ZPAGEDOWN, m_serverZPageDown);
	DDX_Control(pDX, IDC_PROGRESS_BIG, m_process_big);
	DDX_Control(pDX, IDC_PROGRESS_SMALL, m_process_small);
	DDX_Control(pDX, IDC_BUTTON_OFFICIALSITE, m_officialSite);
	DDX_Control(pDX, IDC_BUTTON_CUSTOMERSERVICE, m_customerService);
}

BEGIN_MESSAGE_MAP(CQGLPatcherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_PATCHSTART, OnBnClickedButtonPatchStart)
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
	ON_BN_CLICKED(IDC_BUTTON_SER21, OnBnClickedButtonSer21)
	ON_BN_CLICKED(IDC_BUTTON_SER22, OnBnClickedButtonSer22)
	ON_BN_CLICKED(IDC_BUTTON_SER23, OnBnClickedButtonSer23)
	ON_BN_CLICKED(IDC_BUTTON_SER24, OnBnClickedButtonSer24)
	ON_BN_CLICKED(IDC_BUTTON_SER25, OnBnClickedButtonSer25)
	ON_BN_CLICKED(IDC_BUTTON_SER26, OnBnClickedButtonSer26)
	ON_BN_CLICKED(IDC_BUTTON_SER27, OnBnClickedButtonSer27)
	ON_BN_CLICKED(IDC_BUTTON_SER28, OnBnClickedButtonSer28)
	ON_BN_CLICKED(IDC_BUTTON_SER29, OnBnClickedButtonSer29)
	ON_BN_CLICKED(IDC_BUTTON_SER30, OnBnClickedButtonSer30)
	ON_BN_CLICKED(IDC_BUTTON_SER31, OnBnClickedButtonSer31)
	ON_BN_CLICKED(IDC_BUTTON_SER32, OnBnClickedButtonSer32)
	ON_BN_CLICKED(IDC_BUTTON_SER33, OnBnClickedButtonSer33)
	ON_BN_CLICKED(IDC_BUTTON_SER34, OnBnClickedButtonSer34)
	ON_BN_CLICKED(IDC_BUTTON_SER35, OnBnClickedButtonSer35)
	ON_BN_CLICKED(IDC_BUTTON_SER36, OnBnClickedButtonSer36)
	ON_BN_CLICKED(IDC_BUTTON_SER37, OnBnClickedButtonSer37)
	ON_BN_CLICKED(IDC_BUTTON_SER38, OnBnClickedButtonSer38)
	ON_BN_CLICKED(IDC_BUTTON_SER39, OnBnClickedButtonSer39)
	ON_BN_CLICKED(IDC_BUTTON_SER40, OnBnClickedButtonSer40)
	ON_BN_CLICKED(IDC_BUTTON_SER41, OnBnClickedButtonSer41)
	ON_BN_CLICKED(IDC_BUTTON_SER42, OnBnClickedButtonSer42)
	ON_BN_CLICKED(IDC_BUTTON_SER43, OnBnClickedButtonSer43)
	ON_BN_CLICKED(IDC_BUTTON_SER44, OnBnClickedButtonSer44)
	ON_BN_CLICKED(IDC_BUTTON_SER45, OnBnClickedButtonSer45)
	ON_BN_CLICKED(IDC_BUTTON_SER46, OnBnClickedButtonSer46)
	ON_BN_CLICKED(IDC_BUTTON_SER47, OnBnClickedButtonSer47)
	ON_BN_CLICKED(IDC_BUTTON_SER48, OnBnClickedButtonSer48)
	ON_BN_CLICKED(IDC_BUTTON_SER49, OnBnClickedButtonSer49)
	ON_BN_CLICKED(IDC_BUTTON_SER50, OnBnClickedButtonSer50)
	ON_BN_CLICKED(IDC_BUTTON_SER51, OnBnClickedButtonSer51)
	ON_BN_CLICKED(IDC_BUTTON_SER52, OnBnClickedButtonSer52)
	ON_BN_CLICKED(IDC_BUTTON_SER53, OnBnClickedButtonSer53)
	ON_BN_CLICKED(IDC_BUTTON_SER54, OnBnClickedButtonSer54)
	ON_BN_CLICKED(IDC_BUTTON_SER55, OnBnClickedButtonSer55)
	ON_BN_CLICKED(IDC_BUTTON_SER56, OnBnClickedButtonSer56)
	ON_BN_CLICKED(IDC_BUTTON_SER57, OnBnClickedButtonSer57)
	ON_BN_CLICKED(IDC_BUTTON_SER58, OnBnClickedButtonSer58)
	ON_BN_CLICKED(IDC_BUTTON_SER59, OnBnClickedButtonSer59)
	ON_BN_CLICKED(IDC_BUTTON_SER60, OnBnClickedButtonSer60)
	ON_BN_CLICKED(IDC_BUTTON_DL, OnBnClickedButtonDl)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_CURSERVER, OnBnClickedButtonSerCur)
	ON_BN_CLICKED(IDC_BUTTON_PAGEDOWN, OnBnClickedButtonPagedown)
	ON_BN_CLICKED(IDC_BUTTON_PAGEUP, OnBnClickedButtonPageup)
	ON_BN_CLICKED(IDC_BUTTON_ZPAGEDOWN, OnBnClickedButtonZPagedown)
	ON_BN_CLICKED(IDC_BUTTON_ZPAGEUP, OnBnClickedButtonZPageup)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVERLINE, OnCbnSelchangeComboServerline)
	ON_BN_CLICKED(IDC_BUTTON_GO1, OnBnClickedButtonGo1)
	ON_BN_CLICKED(IDC_BUTTON_GO2, OnBnClickedButtonGo2)
	ON_BN_CLICKED(IDC_BUTTON_GO3, OnBnClickedButtonGo3)
	ON_BN_CLICKED(IDC_BUTTON_GO4, OnBnClickedButtonGo4)
	ON_BN_CLICKED(IDC_BUTTON_GO5, OnBnClickedButtonGo5)
	ON_BN_CLICKED(IDC_BUTTON_GO6, OnBnClickedButtonGo6)
END_MESSAGE_MAP()


// CQGLPatcherDlg �T���B�z�`��

BOOL CQGLPatcherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO: �b���[�J�B�~����l�]�w
	time(&m_PrevTime);

	String filename(Reuben::System::GetApplicationFileName());
	if( !theApp.SingleTest(filename.c_str()) )
	{
		String errOutput;
		errOutput.Format(
			_T("Invalid patching environment.\nProgram terminated abnormally.\nAnother instance is running.")
			);
		MessageBox(errOutput.c_str());
		exit(-1);
	}

	if( theApp.GetStringTable() )
	{
		String title = theApp.GetStringTable()->Get(_T("APP_QGL_PATCHER_CAPTION"));
		SetWindowText(title.c_str());
	}else
	{
		SetWindowText(_T("QGL Auto Update"));
	}

	m_browser1.Navigate(theApp.GetConfigure()->m_newsExplorer.url.c_str(),NULL,NULL,NULL,NULL);
	m_browser1.SetWindowPos(GetWindow(0), 
		theApp.GetConfigure()->m_newsExplorer.x, theApp.GetConfigure()->m_newsExplorer.y, 
		theApp.GetConfigure()->m_newsExplorer.w, theApp.GetConfigure()->m_newsExplorer.h, 
		0);
	m_browser3.Navigate(theApp.GetConfigure()->m_patchExplorer.url.c_str(),NULL,NULL,NULL,NULL);
	m_browser3.SetWindowPos(GetWindow(0), 
		theApp.GetConfigure()->m_patchExplorer.x, theApp.GetConfigure()->m_patchExplorer.y, 
		theApp.GetConfigure()->m_patchExplorer.w, theApp.GetConfigure()->m_patchExplorer.h, 
		0);
	m_browser4.Navigate(theApp.GetConfigure()->m_eventExplorer.url.c_str(),NULL,NULL,NULL,NULL);
	m_browser4.SetWindowPos(GetWindow(0), 
		theApp.GetConfigure()->m_eventExplorer.x, theApp.GetConfigure()->m_eventExplorer.y, 
		theApp.GetConfigure()->m_eventExplorer.w, theApp.GetConfigure()->m_eventExplorer.h, 
		0);

	m_curServerPage = 0;

	UINT nCmd;
	nCmd = IDC_BUTTON_START;
	GetDlgItem(nCmd)->SetWindowPos(GetWindow(nCmd), 
		theApp.GetConfigure()->m_newsStart.x, theApp.GetConfigure()->m_newsStart.y, theApp.GetConfigure()->m_newsStart.w, theApp.GetConfigure()->m_newsStart.h, 0);
	nCmd = IDC_BUTTON_PATCHSTART;
	GetDlgItem(nCmd)->SetWindowPos(GetWindow(nCmd), 
		theApp.GetConfigure()->m_patchStart.x, theApp.GetConfigure()->m_patchStart.y, theApp.GetConfigure()->m_patchStart.w, theApp.GetConfigure()->m_patchStart.h, 0);

	for( Index i = C_INDEX(0); i < theApp.GetConfigure()->m_linkCount; ++i )
	{
		switch(i)
		{
		case 0:		nCmd = IDC_BUTTON_GO1;	break;
		case 1:		nCmd = IDC_BUTTON_GO2;	break;
		case 2:		nCmd = IDC_BUTTON_GO3;	break;
		case 3:		nCmd = IDC_BUTTON_GO4;	break;
		case 4:		nCmd = IDC_BUTTON_GO5;	break;
		case 5:		nCmd = IDC_BUTTON_GO6;	break;
			// for more cases, added here manually
		default:	nCmd = 0;				break;
		}
		if( nCmd != 0 )
		{
			LinkInfo& info = theApp.GetConfigure()->m_linkInfo[i];
			GetDlgItem(nCmd)->SetWindowPos(GetWindow(nCmd), info.x, info.y, info.w, info.h, 0);
		}
	}
	for( Index i = C_INDEX(0); i < C_INDEX(MAX_SERVER_BUTTON); ++i )
		m_serverChoice[i].ShowWindow(SW_SHOW);
	for( Index k = C_INDEX(0); k < C_INDEX(3); ++k )
	{
		ZoneInfo* ptrzoneinfo = NULL;
		switch( k )
		{
		case 0:	ptrzoneinfo = &theApp.GetConfigure()->m_zoneinfo1;	break;
		case 1:	ptrzoneinfo = &theApp.GetConfigure()->m_zoneinfo2;	break;
		case 2:	ptrzoneinfo = &theApp.GetConfigure()->m_zoneinfo3;	break;
		}
		ZoneInfo& zoneinfo = *ptrzoneinfo;
		for( Index j = C_INDEX(0); j < zoneinfo.y.size(); ++j )
		{
			for( Index i = C_INDEX(0); i < zoneinfo.x.size(); ++i )
			{
				UInt posindex = j * zoneinfo.x.size() + i + (k*20);
				UInt formIdx = 0;
				UInt x, y;
				UInt type = (UInt)zoneinfo.zoneType;
				UInt w = 0, h = 0;
				switch( type )
				{
				case 0:
					w = theApp.GetConfigure()->m_zoneButtonTable.w;
					h = theApp.GetConfigure()->m_zoneButtonTable.h;
					break;
				case 1:
					w = theApp.GetConfigure()->m_serverButtonTable.w;
					h = theApp.GetConfigure()->m_serverButtonTable.h;
					break;
				}
				x = zoneinfo.x[i];
				y = zoneinfo.y[j];
				switch( posindex )
				{
				case 0:		formIdx = IDC_BUTTON_SER1;	break;
				case 1:		formIdx = IDC_BUTTON_SER2;	break;
				case 2:		formIdx = IDC_BUTTON_SER3;	break;
				case 3:		formIdx = IDC_BUTTON_SER4;	break;
				case 4:		formIdx = IDC_BUTTON_SER5;	break;
				case 5:		formIdx = IDC_BUTTON_SER6;	break;
				case 6:		formIdx = IDC_BUTTON_SER7;	break;
				case 7:		formIdx = IDC_BUTTON_SER8;	break;
				case 8:		formIdx = IDC_BUTTON_SER9;	break;
				case 9:		formIdx = IDC_BUTTON_SER10;	break;
				case 10:	formIdx = IDC_BUTTON_SER11;	break;
				case 11:	formIdx = IDC_BUTTON_SER12;	break;
				case 12:	formIdx = IDC_BUTTON_SER13;	break;
				case 13:	formIdx = IDC_BUTTON_SER14;	break;
				case 14:	formIdx = IDC_BUTTON_SER15;	break;
				case 15:	formIdx = IDC_BUTTON_SER16;	break;
				case 16:	formIdx = IDC_BUTTON_SER17;	break;
				case 17:	formIdx = IDC_BUTTON_SER18;	break;
				case 18:	formIdx = IDC_BUTTON_SER19;	break;
				case 19:	formIdx = IDC_BUTTON_SER20;	break;
				case 20:	formIdx = IDC_BUTTON_SER21;	break;
				case 21:	formIdx = IDC_BUTTON_SER22;	break;
				case 22:	formIdx = IDC_BUTTON_SER23;	break;
				case 23:	formIdx = IDC_BUTTON_SER24;	break;
				case 24:	formIdx = IDC_BUTTON_SER25;	break;
				case 25:	formIdx = IDC_BUTTON_SER26;	break;
				case 26:	formIdx = IDC_BUTTON_SER27;	break;
				case 27:	formIdx = IDC_BUTTON_SER28;	break;
				case 28:	formIdx = IDC_BUTTON_SER29;	break;
				case 29:	formIdx = IDC_BUTTON_SER30;	break;
				case 30:	formIdx = IDC_BUTTON_SER31;	break;
				case 31:	formIdx = IDC_BUTTON_SER32;	break;
				case 32:	formIdx = IDC_BUTTON_SER33;	break;
				case 33:	formIdx = IDC_BUTTON_SER34;	break;
				case 34:	formIdx = IDC_BUTTON_SER35;	break;
				case 35:	formIdx = IDC_BUTTON_SER36;	break;
				case 36:	formIdx = IDC_BUTTON_SER37;	break;
				case 37:	formIdx = IDC_BUTTON_SER38;	break;
				case 38:	formIdx = IDC_BUTTON_SER39;	break;
				case 39:	formIdx = IDC_BUTTON_SER40;	break;
				case 40:	formIdx = IDC_BUTTON_SER41;	break;
				case 41:	formIdx = IDC_BUTTON_SER42;	break;
				case 42:	formIdx = IDC_BUTTON_SER43;	break;
				case 43:	formIdx = IDC_BUTTON_SER44;	break;
				case 44:	formIdx = IDC_BUTTON_SER45;	break;
				case 45:	formIdx = IDC_BUTTON_SER46;	break;
				case 46:	formIdx = IDC_BUTTON_SER47;	break;
				case 47:	formIdx = IDC_BUTTON_SER48;	break;
				case 48:	formIdx = IDC_BUTTON_SER49;	break;
				case 49:	formIdx = IDC_BUTTON_SER50;	break;
				case 50:	formIdx = IDC_BUTTON_SER51;	break;
				case 51:	formIdx = IDC_BUTTON_SER52;	break;
				case 52:	formIdx = IDC_BUTTON_SER53;	break;
				case 53:	formIdx = IDC_BUTTON_SER54;	break;
				case 54:	formIdx = IDC_BUTTON_SER55;	break;
				case 55:	formIdx = IDC_BUTTON_SER56;	break;
				case 56:	formIdx = IDC_BUTTON_SER57;	break;
				case 57:	formIdx = IDC_BUTTON_SER58;	break;
				case 58:	formIdx = IDC_BUTTON_SER59;	break;
				case 69:	formIdx = IDC_BUTTON_SER60;	break;
				}
				GetDlgItem(formIdx)->SetWindowPos(GetWindow(formIdx), x, y, w, h, 0);
			}
		}
	}

	GetDlgItem(IDC_BUTTON_CURSERVER)->SetWindowPos(GetWindow(IDC_BUTTON_CURSERVER), theApp.GetConfigure()->m_serverDefault.x, theApp.GetConfigure()->m_serverDefault.y, theApp.GetConfigure()->m_serverDefault.w, theApp.GetConfigure()->m_serverDefault.h, 0);
	GetDlgItem(IDC_BUTTON_PAGEUP)->SetWindowPos(GetWindow(IDC_BUTTON_PAGEUP), theApp.GetConfigure()->m_serverServerUp.x, theApp.GetConfigure()->m_serverServerUp.y, theApp.GetConfigure()->m_serverServerUp.w, theApp.GetConfigure()->m_serverServerUp.h, 0);
	GetDlgItem(IDC_BUTTON_PAGEDOWN)->SetWindowPos(GetWindow(IDC_BUTTON_PAGEDOWN), theApp.GetConfigure()->m_serverServerDown.x, theApp.GetConfigure()->m_serverServerDown.y, theApp.GetConfigure()->m_serverServerDown.w, theApp.GetConfigure()->m_serverServerDown.h, 0);
	GetDlgItem(IDC_BUTTON_ZPAGEUP)->SetWindowPos(GetWindow(IDC_BUTTON_ZPAGEUP), theApp.GetConfigure()->m_serverZoneUp.x, theApp.GetConfigure()->m_serverZoneUp.y, theApp.GetConfigure()->m_serverZoneUp.w, theApp.GetConfigure()->m_serverZoneUp.h, 0);
	GetDlgItem(IDC_BUTTON_ZPAGEDOWN)->SetWindowPos(GetWindow(IDC_BUTTON_ZPAGEDOWN), theApp.GetConfigure()->m_serverZoneDown.x, theApp.GetConfigure()->m_serverZoneDown.y, theApp.GetConfigure()->m_serverZoneDown.w, theApp.GetConfigure()->m_serverZoneDown.h, 0);
	m_process_big.SetRange(0, (short)255);
	m_process_small.SetRange(0, (short)255);

	HidePage(2);
	HidePage(3);
	ShowPage(1);

	AfxBeginThread(startDownloadTracker, this);

	m_bmpWebBg		= NULL;
	m_bmpServerBg	= NULL;
	m_bmpPatchBg	= NULL;

	BOOL loadBmpSucceed = LoadBitmapResource();

	UInt centerX = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (PATCHER_WINDOW_SIZE_WIDTH / 2);
	UInt centerY = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (PATCHER_WINDOW_SIZE_HEIGHT / 2);
	SetWindowPos(&wndTop, centerX, centerY, PATCHER_WINDOW_SIZE_WIDTH, PATCHER_WINDOW_SIZE_HEIGHT, NULL); 

	if( !theApp.IsInitTrackerLoaded() )
	{
		m_initTrackerDlg = SafeCreateObject(CInitTrackerDialog);
		m_initTrackerDlg->Create(IDD_DIALOG_INIT);
		m_initTrackerDlg->ShowWindow(1);
	}
	
	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A�H�Kø�s�ϥܡC
// ���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A�ج[�|�۰ʧ������@�~�C

void CQGLPatcherDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
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
				dc.SetBkMode(TRANSPARENT);
				dc.SetTextColor(RGB(0,0,0));

				CFont font;
				VERIFY(font.CreateFont(
					16,                        // nHeight
					0,                         // nWidth
					0,                         // nEscapement
					0,                         // nOrientation
					FW_NORMAL,                 // nWeight
					FALSE,                     // bItalic
					FALSE,                     // bUnderline
					0,                         // cStrikeOut
					ANSI_CHARSET,              // nCharSet
					OUT_DEFAULT_PRECIS,        // nOutPrecision
					CLIP_DEFAULT_PRECIS,       // nClipPrecision
					DEFAULT_QUALITY,           // nQuality
					DEFAULT_PITCH | FF_ROMAN,  // nPitchAndFamily
					_T("Times New Roman")));                 // lpszFacename
				CFont* def_font = dc.SelectObject(&font);

				RECT tempbox;
				// default server content
				tempbox.left	= theApp.GetConfigure()->m_serverDefaultContent.x;
				tempbox.top		= theApp.GetConfigure()->m_serverDefaultContent.y;
				tempbox.right	= tempbox.left + theApp.GetConfigure()->m_serverDefaultContent.w;
				tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_serverDefaultContent.h;
				String text;
				text.Format(_T("%s-%s"), theApp.GetConfigureApp()->m_lastzonename.c_str(), theApp.GetConfigureApp()->m_lastservername.c_str());
				dc.DrawText(text.c_str(), &tempbox, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

				dc.SelectObject(def_font);
				font.DeleteObject(); 
			}
			break;
		case PATCHER_PAGE3:
			{
				CBitmap* pOldBmp = (CBitmap*) dcMem.SelectObject(m_bmpPatchBg);
				BITMAP bmpInfo;
				m_bmpPatchBg->GetBitmap(&bmpInfo);
				DrawBitmap( (CMemDC*)&dc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMem, 0, 0);

				dcMem.SelectObject(pOldBmp);

				dc.SetBkMode(TRANSPARENT);
				dc.SetTextColor(RGB(255,255,0));
				
				RECT tempbox;
				// Current Client version
				tempbox.left	= theApp.GetConfigure()->m_patchCurVer.x;
				tempbox.top		= theApp.GetConfigure()->m_patchCurVer.y;
				tempbox.right	= tempbox.left + theApp.GetConfigure()->m_patchCurVer.w;
				tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_patchCurVer.h;
				dc.DrawText(theApp.m_curVersion.c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
				// Server version
				tempbox.left	= theApp.GetConfigure()->m_patchTarVer.x;
				tempbox.top		= theApp.GetConfigure()->m_patchTarVer.y;
				tempbox.right	= tempbox.left + theApp.GetConfigure()->m_patchTarVer.w;
				tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_patchTarVer.h;
				dc.DrawText(theApp.m_serVersion.c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);

				dc.SetTextColor(RGB(0,0,255));
				// Calender
				//String msg;
				//time_t now;
				//time(&now);
				//tm* nowtime = localtime(&now);
				//tempbox.left	= theApp.GetConfigure()->m_patchYear.x;
				//tempbox.top		= theApp.GetConfigure()->m_patchYear.y;
				//tempbox.right	= tempbox.left + theApp.GetConfigure()->m_patchYear.w;
				//tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_patchYear.h;
				//msg.Format(_T("%d"), nowtime->tm_year + 1900);
				//dc.DrawText(msg.c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
				//tempbox.left	= theApp.GetConfigure()->m_patchMonth.x;
				//tempbox.top		= theApp.GetConfigure()->m_patchMonth.y;
				//tempbox.right	= tempbox.left + theApp.GetConfigure()->m_patchMonth.w;
				//tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_patchMonth.h;
				//msg.Format(_T("%d"), nowtime->tm_mon + 1);
				//dc.DrawText(msg.c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
				//tempbox.left	= theApp.GetConfigure()->m_patchDay.x;
				//tempbox.top		= theApp.GetConfigure()->m_patchDay.y;
				//tempbox.right	= tempbox.left + theApp.GetConfigure()->m_patchDay.w;
				//tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_patchDay.h;
				//msg.Format(_T("%d"), nowtime->tm_mday);
				//dc.DrawText(msg.c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
				//tempbox.left	= theApp.GetConfigure()->m_patchWeekday.x;
				//tempbox.top		= theApp.GetConfigure()->m_patchWeekday.y;
				//tempbox.right	= tempbox.left + theApp.GetConfigure()->m_patchWeekday.w;
				//tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_patchWeekday.h;
				//msg.Format(_T("MSG_PATCH_CALENDAR_WEEK%d"), nowtime->tm_wday);
				//StringA parseString(msg.c_str());
				//dc.DrawText(theApp.GetStringTable()->Get(parseString.c_str()).c_str(), &tempbox, DT_CENTER | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
				String msg;
				switch( theApp.GetDLStatus() )
				{
				case PATCHER_DL_STATUSSTOP:
					{
						dc.SetTextColor(RGB(255,0,0));
						CFont font;
						VERIFY(font.CreateFont(
							14,                        // nHeight
							0,                         // nWidth
							0,                         // nEscapement
							0,                         // nOrientation
							FW_NORMAL,                 // nWeight
							FALSE,                     // bItalic
							FALSE,                     // bUnderline
							0,                         // cStrikeOut
							ANSI_CHARSET,              // nCharSet
							OUT_DEFAULT_PRECIS,        // nOutPrecision
							CLIP_DEFAULT_PRECIS,       // nClipPrecision
							DEFAULT_QUALITY,           // nQuality
							DEFAULT_PITCH | FF_ROMAN,  // nPitchAndFamily
							_T("Times New Roman")));                 // lpszFacename
						CFont* def_font = dc.SelectObject(&font);
						if( theApp.GetPatchCurrentVersion()==theApp.GetPatchTargetVersion() )
						{
							if( theApp.GetConfigureApp()->m_patcherversioncrc==theApp.GetPatchSummaryVersionCRC() || theApp.GetConfigureApp()->m_patcherversioncrc==0 )
							{
								tempbox.left	= theApp.GetConfigure()->m_progressText1.x;
								tempbox.top		= theApp.GetConfigure()->m_progressText1.y;
								tempbox.right	= tempbox.left + theApp.GetConfigure()->m_progressText1.w;
								tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_progressText1.h;
								dc.DrawText(theApp.GetStringTable()->Get("MSG_PATCH_VERSION_LATEST").c_str(), &tempbox, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
							}else
							{
								tempbox.left	= theApp.GetConfigure()->m_progressText1.x;
								tempbox.top		= theApp.GetConfigure()->m_progressText1.y;
								tempbox.right	= tempbox.left + theApp.GetConfigure()->m_progressText1.w;
								tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_progressText1.h;
								dc.DrawText(theApp.GetStringTable()->Get("MSG_PATCH_VERSION_NEEDREVERT").c_str(), &tempbox, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
							}
						}else if( theApp.GetPatchCurrentVersion()>=theApp.GetPatchTargetVersion() && theApp.GetPatchTargetVersion() > 0 )
						{
							tempbox.left	= theApp.GetConfigure()->m_progressText1.x;
							tempbox.top		= theApp.GetConfigure()->m_progressText1.y;
							tempbox.right	= tempbox.left + theApp.GetConfigure()->m_progressText1.w;
							tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_progressText1.h;
							dc.DrawText(theApp.GetStringTable()->Get("MSG_PATCH_VERSION_NEEDREVERT").c_str(), &tempbox, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
						}
						font.DeleteObject(); 
					}
					break;
				case PATCHER_DL_STATUSIN:
					tempbox.left	= theApp.GetConfigure()->m_progressText1.x;
					tempbox.top		= theApp.GetConfigure()->m_progressText1.y;
					tempbox.right	= tempbox.left + theApp.GetConfigure()->m_progressText1.w;
					tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_progressText1.h;
					dc.DrawText(theApp.GetStringTable()->Get("MSG_UPDATE_PROGRESS_INSTALL").c_str(), &tempbox, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
					break;
				case PATCHER_DL_STATUSDL:
				case PATCHER_DL_STATUSNOTIFY:
					tempbox.left	= theApp.GetConfigure()->m_progressText1.x;
					tempbox.top		= theApp.GetConfigure()->m_progressText1.y;
					tempbox.right	= tempbox.left + theApp.GetConfigure()->m_progressText1.w;
					tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_progressText1.h;
					msg.Format(_T("(%dKB/%dKB)"), theApp.GetProgressMonitor()->GetOverallCur()/1024, theApp.GetProgressMonitor()->GetOverallTotal()/1024);
					dc.DrawText(msg.c_str(), &tempbox, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
					tempbox.left	= theApp.GetConfigure()->m_progressText2.x;
					tempbox.top		= theApp.GetConfigure()->m_progressText2.y;
					tempbox.right	= tempbox.left + theApp.GetConfigure()->m_progressText2.w;
					tempbox.bottom	= tempbox.top + theApp.GetConfigure()->m_progressText2.h;
					msg.Format(_T("(%dKB/%dKB)"), theApp.GetProgressMonitor()->GetFileCurrentSize()/1024, theApp.GetProgressMonitor()->GetFileTotalSize()/1024);
					dc.DrawText(msg.c_str(), &tempbox, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER);
					break;
				}
			}
			break;
		}
	}
}

void CQGLPatcherDlg::OnSize(UINT nType, int cx, int cy)
{
	HBITMAP m_hBitmap;

	// Load the image
	switch(theApp.GetCurPage())
	{
	case 1:
		m_hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), PATCHER_IMAGE_WEB_BACKGROUND, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (m_hBitmap == NULL)
		{
			MessageBox(_T("Error loading bitmap"));
			return;
		}
		break;
	case 2:
		m_hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), PATCHER_IMAGE_SERVER_BACKGROUND, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (m_hBitmap == NULL)
		{
			MessageBox(_T("Error loading bitmap"));
			return;
		}
		break;
	case 3:
		m_hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), PATCHER_IMAGE_PATCH_BACKGROUND, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (m_hBitmap == NULL)
		{
			MessageBox(_T("Error loading bitmap"));
			return;
		}
		break;
	default:
		break;
	}

	// Put the bitmap into a memory device context
	CPaintDC dc(this);
	//get a memory dc object
	CDC dcMem;
	//create a compatible dc
	dcMem.CreateCompatibleDC(&dc);	// Select the bitmap into the in-memory DC
	//Select the bitmap into the dc
	CBitmap* pOldBitmap = dcMem.SelectObject(CBitmap::FromHandle(m_hBitmap));

	//Create a couple of region objects.
	CRgn crRgn, crRgnTmp;
	// load from stream
	BYTE* stream = NULL;
	UInt numOfBytes = 0;
	String cacheFilename, bgFilename;
	StringA cacheFilenameA, bgFilenameA;
	switch( theApp.GetCurPage() )
	{
	case 1:
		bgFilename = PATCHER_IMAGE_WEB_BACKGROUND;
		break;
	case 2:
		bgFilename = PATCHER_IMAGE_SERVER_BACKGROUND;
		break;
	case 3:
		bgFilename = PATCHER_IMAGE_PATCH_BACKGROUND;
		break;
	}
	cacheFilename.Format(_T("%s.blk"), bgFilename.c_str());
	File pFile;
	if( pFile.IsExists(cacheFilename.c_str()) )
	{
		pFile.Open(cacheFilename.c_str(), File::MODE_READ_EXISTING);
		if( pFile.IsOpened() )
		{
			pFile.SetPosition(0, Reuben::Platform::File::MOVE_BEGIN);
			BlkHeader header;
			struct stat st;
			cacheFilenameA = cacheFilename.c_str();
			stat(cacheFilenameA.c_str(), &st);
			if( st.st_size > sizeof(BlkHeader) )
			{
				struct stat stOriginal;
				bgFilenameA = bgFilename.c_str();
				stat(bgFilenameA.c_str(), &stOriginal);
				pFile.Read((BYTE*)&header, sizeof(BlkHeader));
				if( header.filesize==stOriginal.st_size && header.fileTime==stOriginal.st_mtime )
				{
					if( header.byteRegion > 0 )
					{
						numOfBytes = header.byteRegion * sizeof(RECT);
						stream = SafeAllocate(BYTE, numOfBytes);
						numOfBytes = pFile.Read((BYTE*)stream, numOfBytes);
					}
				}
			}
			pFile.Close();
		}
	}
	if( !stream || !crRgn.CreateFromData(NULL, numOfBytes, (RGNDATA*)stream) )
	{
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
		
		struct stat st;
		bgFilenameA = bgFilename.c_str();
		stat(bgFilenameA.c_str(), &st);

		BlkHeader header;
		memcpy(header.magic, ".BLK", 4);
		header.version	= 1;
		header.fileTime = st.st_mtime;
		header.filesize = st.st_size;
		UInt numberOfByte = crRgn.GetRegionData(NULL, 0);
		header.byteRegion = numberOfByte;
		RGNDATA* rgndata = (RGNDATA*)SafeAllocate(BYTE, numberOfByte);
		crRgn.GetRegionData(rgndata, numberOfByte);

		pFile.Open(cacheFilename.c_str(), File::MODE_WRITE);
		if( pFile.IsOpened() )
		{
			pFile.SetLength(0);
			pFile.Write((BYTE*)&header, sizeof(BlkHeader));
			pFile.Write((BYTE*)rgndata, numberOfByte);
			pFile.Close();
		}
		if( rgndata )
			SafeDeallocate(rgndata);
	}
	//Centre it on current desktop
	SetWindowRgn(crRgn, TRUE);

	// Free resources.
	if( stream )
		SafeDeallocate(stream);
	dcMem.SelectObject(pOldBitmap);	// Put the original bitmap back (prevents memory leaks)
	dcMem.DeleteDC();
	crRgn.DeleteObject();

	DeleteObject(m_hBitmap);	//not really need but what the heck.
}

//���ϥΪ̩즲�̤p�Ƶ����ɡA�t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CQGLPatcherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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
		if( theApp.GetSelectedServer()==NO_SERVER_SELECTED )
			break;
		AfxBeginThread(startDownloadSummary, 0);
	case PATCHER_PAGE1:
	case PATCHER_PAGE2:
		HidePage(theApp.GetCurPage());
		theApp.SetCurPage(pageno);
		ShowPage(theApp.GetCurPage());
		RedrawWindow();
		break;
	}
	PostMessage(WM_SIZE);
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
		GetDlgItem(IDC_BUTTON_OFFICIALSITE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_CUSTOMERSERVICE)->ShowWindow(SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EXPLORER_UPPER)->GetSafeHwnd(), SW_HIDE);
		GetDlgItem(IDC_BUTTON_EXIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_HIDE);
		break;
	case PATCHER_PAGE2:
		GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_NEXT)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_BUTTON_CURSERVER)->ShowWindow(SW_HIDE);
		for( Index i = C_INDEX(0); i < C_INDEX(60); ++i )
			m_serverChoice[i].ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_PAGEUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_PAGEDOWN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ZPAGEUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ZPAGEDOWN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_CURSERVER)->ShowWindow(SW_HIDE);
		break;
	case PATCHER_PAGE3:
		::ShowWindow(GetDlgItem(IDC_EXPLORER_CENTER)->GetSafeHwnd(), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EXPLORER_RIGHT)->GetSafeHwnd(), SW_HIDE);
		GetDlgItem(IDC_BUTTON_DL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PROGRESS_BIG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PROGRESS_SMALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_PATCHSTART)->ShowWindow(SW_HIDE);
		break;
	}
}

void CQGLPatcherDlg::ShowPage(unsigned int pageno)
{
	switch( pageno )
	{
	case PATCHER_PAGE1:
		GetDlgItem(IDC_BUTTON_EXIT)->SetWindowPos(GetWindow(IDC_BUTTON_EXIT), theApp.GetConfigure()->m_newsExit.x, theApp.GetConfigure()->m_newsExit.y, theApp.GetConfigure()->m_newsExit.w, theApp.GetConfigure()->m_newsExit.h, 0);
		GetDlgItem(IDC_BUTTON_START)->SetWindowPos(GetWindow(IDC_BUTTON_START), 
			theApp.GetConfigure()->m_newsStart.x, theApp.GetConfigure()->m_newsStart.y, theApp.GetConfigure()->m_newsStart.w, theApp.GetConfigure()->m_newsStart.h, 0);
		GetDlgItem(IDC_BUTTON_GOOFFICIAL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_ACCOUNT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_REGISTRATION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_POINT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_OFFICIALSITE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CUSTOMERSERVICE)->ShowWindow(SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_EXPLORER_UPPER)->GetSafeHwnd(), SW_SHOW);
		GetDlgItem(IDC_BUTTON_EXIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_SHOW);
		break;
	case PATCHER_PAGE2:
		{
			GetDlgItem(IDC_BUTTON_PREV)->SetWindowPos(GetWindow(IDC_BUTTON_PREV), theApp.GetConfigure()->m_serverPrev.x, theApp.GetConfigure()->m_serverPrev.y, theApp.GetConfigure()->m_serverPrev.w, theApp.GetConfigure()->m_serverPrev.h, 0);
			GetDlgItem(IDC_BUTTON_NEXT)->SetWindowPos(GetWindow(IDC_BUTTON_NEXT), theApp.GetConfigure()->m_serverNext.x, theApp.GetConfigure()->m_serverNext.y, theApp.GetConfigure()->m_serverNext.w, theApp.GetConfigure()->m_serverNext.h, 0);
			GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_NEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_PAGEUP)->SetWindowPos(GetWindow(IDC_BUTTON_PAGEUP), theApp.GetConfigure()->m_serverServerUp.x, theApp.GetConfigure()->m_serverServerUp.y, theApp.GetConfigure()->m_serverServerUp.w, theApp.GetConfigure()->m_serverServerUp.h, 0);
			GetDlgItem(IDC_BUTTON_PAGEDOWN)->SetWindowPos(GetWindow(IDC_BUTTON_PAGEDOWN), theApp.GetConfigure()->m_serverServerDown.x, theApp.GetConfigure()->m_serverServerDown.y, theApp.GetConfigure()->m_serverServerDown.w, theApp.GetConfigure()->m_serverServerDown.h, 0);
			GetDlgItem(IDC_BUTTON_PAGEUP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_PAGEDOWN)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_ZPAGEUP)->SetWindowPos(GetWindow(IDC_BUTTON_ZPAGEUP), theApp.GetConfigure()->m_serverZoneUp.x, theApp.GetConfigure()->m_serverZoneUp.y, theApp.GetConfigure()->m_serverZoneUp.w, theApp.GetConfigure()->m_serverZoneUp.h, 0);
			GetDlgItem(IDC_BUTTON_ZPAGEDOWN)->SetWindowPos(GetWindow(IDC_BUTTON_ZPAGEDOWN), theApp.GetConfigure()->m_serverZoneDown.x, theApp.GetConfigure()->m_serverZoneDown.y, theApp.GetConfigure()->m_serverZoneDown.w, theApp.GetConfigure()->m_serverZoneDown.h, 0);
			GetDlgItem(IDC_BUTTON_ZPAGEUP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_ZPAGEDOWN)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_CURSERVER)->ShowWindow(SW_SHOW);

			// change zone and server page if it's the 1st time to press button or initialization stage
			if( !GetManualSelected() )
			{
				if( theApp.GetDefaultExists() )
				{
//					// set recommendation position
//					theApp.GetUIManagerRecommList()->SetSelect(*theApp.GetSelectedServer());
					// jump zone page
					theApp.GetUIManagerZoneList()->SetSelect(*theApp.GetSelectedZone());
					theApp.GetUIManagerZoneList()->JumpSelectPage();
					// jump server page
					ServerInfoList zoneSrvList;
					zoneSrvList.clear();
					theApp.GetSelectedZone()->GetServerList(zoneSrvList);
					theApp.GetUIManagerServerList()->LoadContent(&zoneSrvList);
					theApp.GetUIManagerServerList()->SetSelect(theApp.GetSelectedServer());
					theApp.GetUIManagerServerList()->JumpSelectPage();
				}
			}
			RedrawCurServerSelectionPage();
		}
		break;
	case PATCHER_PAGE3:
		::ShowWindow(GetDlgItem(IDC_EXPLORER_CENTER)->GetSafeHwnd(), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_EXPLORER_RIGHT)->GetSafeHwnd(), SW_SHOW);
		GetDlgItem(IDC_BUTTON_DL)->SetWindowPos(GetWindow(IDC_BUTTON_DL), theApp.GetConfigure()->m_patchLink.x, theApp.GetConfigure()->m_patchLink.y, theApp.GetConfigure()->m_patchLink.w, theApp.GetConfigure()->m_patchLink.h, 0);
		GetDlgItem(IDC_BUTTON_PREV)->SetWindowPos(GetWindow(IDC_BUTTON_PREV), theApp.GetConfigure()->m_patchExit.x, theApp.GetConfigure()->m_patchExit.y, theApp.GetConfigure()->m_patchExit.w, theApp.GetConfigure()->m_patchExit.h, 0);
		GetDlgItem(IDC_BUTTON_PATCHSTART)->SetWindowPos(GetWindow(IDC_BUTTON_START), theApp.GetConfigure()->m_patchStart.x, theApp.GetConfigure()->m_patchStart.y, theApp.GetConfigure()->m_patchStart.w, theApp.GetConfigure()->m_patchStart.h, 0);
		GetDlgItem(IDC_PROGRESS_BIG)->SetWindowPos(GetWindow(IDC_PROGRESS_BIG), theApp.GetConfigure()->m_patchProgress1.x, theApp.GetConfigure()->m_patchProgress1.y, theApp.GetConfigure()->m_patchProgress1.w, theApp.GetConfigure()->m_patchProgress1.h, 0);
		GetDlgItem(IDC_PROGRESS_SMALL)->SetWindowPos(GetWindow(IDC_PROGRESS_SMALL), theApp.GetConfigure()->m_patchProgress2.x, theApp.GetConfigure()->m_patchProgress2.y, theApp.GetConfigure()->m_patchProgress2.w, theApp.GetConfigure()->m_patchProgress2.h, 0);
		GetDlgItem(IDC_BUTTON_DL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_PREV)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_PATCHSTART)->ShowWindow(SW_SHOW);
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
	CHoverButton* pHButton;
	for( Index i = C_INDEX(0); i < theApp.GetConfigure()->m_linkCount; ++i )
	{
		switch( i )
		{
		case 0:		pHButton = &m_goOfficial;		break;
		case 1:		pHButton = &m_goAccount;		break;
		case 2:		pHButton = &m_goRegistration;	break;
		case 3:		pHButton = &m_goPoint;			break;
		case 4:		pHButton = &m_officialSite;		break;
		case 5:		pHButton = &m_customerService;	break;
		default:	pHButton = NULL;				break;
		}
		LinkInfo& info = theApp.GetConfigure()->m_linkInfo[i];
		if( pHButton )
		{
			String path1, path2, path3;
			path1.Format(_T("res\\%s"), info.btnImage1.c_str());
			path2.Format(_T("res\\%s"), info.btnImage2.c_str());
			path3.Format(_T("res\\%s"), info.btnImage3.c_str());
			if( !pHButton->LoadBitmap(path1.c_str(), path2.c_str(), path3.c_str()) )
				success = FALSE;
		}
	}
	if( !m_exitGame.LoadBitmap(theApp.GetConfigure()->m_newsExit.btnImage1.c_str(), theApp.GetConfigure()->m_newsExit.btnImage2.c_str(), theApp.GetConfigure()->m_newsExit.btnImage3.c_str()) )
		success = FALSE;
	if( !m_startGame.LoadBitmap(theApp.GetConfigure()->m_newsStart.btnImage1.c_str(), theApp.GetConfigure()->m_newsStart.btnImage2.c_str(), theApp.GetConfigure()->m_newsStart.btnImage3.c_str()) )
		success = FALSE;
	if( !m_startGamePatch.LoadBitmap(theApp.GetConfigure()->m_patchStart.btnImage1.c_str(), theApp.GetConfigure()->m_patchStart.btnImage2.c_str(), theApp.GetConfigure()->m_patchStart.btnImage3.c_str()) )
		success = FALSE;

	if( !m_serverCur.LoadBitmap(theApp.GetConfigure()->m_serverDefault.btnImage1.c_str(), theApp.GetConfigure()->m_serverDefault.btnImage2.c_str(), theApp.GetConfigure()->m_serverDefault.btnImage3.c_str()) )
		success = FALSE;
	if( !m_serverPageUp.LoadBitmap(theApp.GetConfigure()->m_serverServerUp.btnImage1.c_str(), theApp.GetConfigure()->m_serverServerUp.btnImage2.c_str(), theApp.GetConfigure()->m_serverServerUp.btnImage3.c_str()) )
		success = FALSE;
	if( !m_serverPageDown.LoadBitmap(theApp.GetConfigure()->m_serverServerDown.btnImage1.c_str(), theApp.GetConfigure()->m_serverServerDown.btnImage2.c_str(), theApp.GetConfigure()->m_serverServerDown.btnImage3.c_str()) )
		success = FALSE;
	if( !m_serverZPageUp.LoadBitmap(theApp.GetConfigure()->m_serverZoneUp.btnImage1.c_str(), theApp.GetConfigure()->m_serverZoneUp.btnImage2.c_str(), theApp.GetConfigure()->m_serverZoneUp.btnImage3.c_str()) )
		success = FALSE;
	if( !m_serverZPageDown.LoadBitmap(theApp.GetConfigure()->m_serverZoneDown.btnImage1.c_str(), theApp.GetConfigure()->m_serverZoneDown.btnImage2.c_str(), theApp.GetConfigure()->m_serverZoneDown.btnImage3.c_str()) )
		success = FALSE;
//	if( !LoadServerButtonBitmapResource() )
//		success = FALSE;

	if( !m_startPatch.LoadBitmap(theApp.GetConfigure()->m_patchLink.btnImage1.c_str(), theApp.GetConfigure()->m_patchLink.btnImage2.c_str(), theApp.GetConfigure()->m_patchLink.btnImage3.c_str()) )
		success = FALSE;
	if( !m_serverNext.LoadBitmap(theApp.GetConfigure()->m_serverNext.btnImage1.c_str(), theApp.GetConfigure()->m_serverNext.btnImage2.c_str(), theApp.GetConfigure()->m_serverNext.btnImage3.c_str()) )
		success = FALSE;
	if( !m_serverPrev.LoadBitmap(theApp.GetConfigure()->m_serverPrev.btnImage1.c_str(), theApp.GetConfigure()->m_serverPrev.btnImage2.c_str(), theApp.GetConfigure()->m_serverPrev.btnImage3.c_str()) )
		success = FALSE;

	return success;
}

BOOL CQGLPatcherDlg::LoadServerButtonBitmapResource(UInt* status, UInt noServerInPage)
{
	BOOL bSuccess = TRUE;
	UInt zone1count = (UInt)theApp.GetConfigure()->m_zoneinfo1.x.size() * theApp.GetConfigure()->m_zoneinfo1.y.size();
	for( Index i = C_INDEX(0); i < zone1count; ++i )
	{
		switch( status[i] )
		{
		case 1:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[0].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[0].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[0].c_str()
				) )
				bSuccess = FALSE;
			break;
		case 2:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[1].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[1].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[1].c_str()
				) )
				bSuccess = FALSE;
			break;
		case 3:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[2].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[2].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[2].c_str()
				) )
				bSuccess = FALSE;
			break;
		case 0:
		default:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[3].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[3].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[3].c_str()
				) )
				bSuccess = FALSE;
			break;
		}
	}
	for( Index i = noServerInPage; i < zone1count; ++i )
	{
		if( !m_serverChoice[i].LoadBitmap(
			theApp.GetConfigure()->m_serverButtonTable.btnImage1[3].c_str(),
			theApp.GetConfigure()->m_serverButtonTable.btnImage2[3].c_str(),
			theApp.GetConfigure()->m_serverButtonTable.btnImage3[3].c_str()
			) )
			bSuccess = FALSE;
	}

	UInt zone2count = (UInt)theApp.GetConfigure()->m_zoneinfo2.x.size() * theApp.GetConfigure()->m_zoneinfo2.y.size();
	for( Index i = C_INDEX(20); i < zone2count + 20; ++i )
	{
		switch( status[i] )
		{
		case 0:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_zoneButtonTable.btnImage1.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage2.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage3.c_str()
				) )
				bSuccess = FALSE;
			break;
		case 1:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_zoneButtonTable.btnImage1.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage2.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage3.c_str()
				) )
				bSuccess = FALSE;
			break;
		case 2:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_zoneButtonTable.btnImage1.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage2.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage3.c_str()
				) )
				bSuccess = FALSE;
			break;
		case 3:
		default:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_zoneButtonTable.btnImage3.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage3.c_str(),
				theApp.GetConfigure()->m_zoneButtonTable.btnImage3.c_str()
				) )
				bSuccess = FALSE;
			break;
		}
	}
	for( Index i = noServerInPage + 20; i < zone2count + 20; ++i )
	{
		if( !m_serverChoice[i].LoadBitmap(
			theApp.GetConfigure()->m_zoneButtonTable.btnImage1.c_str(),
			theApp.GetConfigure()->m_zoneButtonTable.btnImage2.c_str(),
			theApp.GetConfigure()->m_zoneButtonTable.btnImage3.c_str()
			) )
			bSuccess = FALSE;
	}

	UInt zone3count = (UInt)theApp.GetConfigure()->m_zoneinfo3.x.size() * theApp.GetConfigure()->m_zoneinfo3.y.size();
	for( Index i = C_INDEX(40); i < zone3count + 40; ++i )
	{
		switch( status[i] )
		{
		case 1:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[0].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[0].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[0].c_str()
				) )
				bSuccess = FALSE;
			break;
		case 2:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[1].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[1].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[1].c_str()
				) )
				bSuccess = FALSE;
			break;
		case 3:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[2].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[2].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[2].c_str()
				) )
				bSuccess = FALSE;
			break;
		case 0:
		default:
			if( !m_serverChoice[i].LoadBitmap(
				theApp.GetConfigure()->m_serverButtonTable.btnImage1[3].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage2[3].c_str(),
				theApp.GetConfigure()->m_serverButtonTable.btnImage3[3].c_str()
				) )
				bSuccess = FALSE;
			break;
		}
	}
	for( Index i = noServerInPage + 40; i < zone3count + 40; ++i )
	{
		if( !m_serverChoice[i].LoadBitmap(
			theApp.GetConfigure()->m_serverButtonTable.btnImage1[3].c_str(),
			theApp.GetConfigure()->m_serverButtonTable.btnImage2[3].c_str(),
			theApp.GetConfigure()->m_serverButtonTable.btnImage3[3].c_str()
			) )
			bSuccess = FALSE;
	}

	return bSuccess;
}

void CQGLPatcherDlg::OnBnClickedButtonSer1()	{	SelectServerAction(0);	}
void CQGLPatcherDlg::OnBnClickedButtonSer2()	{	SelectServerAction(1);	}
void CQGLPatcherDlg::OnBnClickedButtonSer3()	{	SelectServerAction(2);	}
void CQGLPatcherDlg::OnBnClickedButtonSer4()	{	SelectServerAction(3);	}
void CQGLPatcherDlg::OnBnClickedButtonSer5()	{	SelectServerAction(4);	}
void CQGLPatcherDlg::OnBnClickedButtonSer6()	{	SelectServerAction(5);	}
void CQGLPatcherDlg::OnBnClickedButtonSer7()	{	SelectServerAction(6);	}
void CQGLPatcherDlg::OnBnClickedButtonSer8()	{	SelectServerAction(7);	}
void CQGLPatcherDlg::OnBnClickedButtonSer9()	{	SelectServerAction(8);	}
void CQGLPatcherDlg::OnBnClickedButtonSer10()	{	SelectServerAction(9);	}
void CQGLPatcherDlg::OnBnClickedButtonSer11()	{	SelectServerAction(10);	}
void CQGLPatcherDlg::OnBnClickedButtonSer12()	{	SelectServerAction(11);	}
void CQGLPatcherDlg::OnBnClickedButtonSer13()	{	SelectServerAction(12);	}
void CQGLPatcherDlg::OnBnClickedButtonSer14()	{	SelectServerAction(13);	}
void CQGLPatcherDlg::OnBnClickedButtonSer15()	{	SelectServerAction(14);	}
void CQGLPatcherDlg::OnBnClickedButtonSer16()	{	SelectServerAction(15);	}
void CQGLPatcherDlg::OnBnClickedButtonSer17()	{	SelectServerAction(16);	}
void CQGLPatcherDlg::OnBnClickedButtonSer18()	{	SelectServerAction(17);	}
void CQGLPatcherDlg::OnBnClickedButtonSer19()	{	SelectServerAction(18);	}
void CQGLPatcherDlg::OnBnClickedButtonSer20()	{	SelectServerAction(19);	}
void CQGLPatcherDlg::OnBnClickedButtonSer21()	{	SelectServerAction(20);	}
void CQGLPatcherDlg::OnBnClickedButtonSer22()	{	SelectServerAction(21);	}
void CQGLPatcherDlg::OnBnClickedButtonSer23()	{	SelectServerAction(22);	}
void CQGLPatcherDlg::OnBnClickedButtonSer24()	{	SelectServerAction(23);	}
void CQGLPatcherDlg::OnBnClickedButtonSer25()	{	SelectServerAction(24);	}
void CQGLPatcherDlg::OnBnClickedButtonSer26()	{	SelectServerAction(25);	}
void CQGLPatcherDlg::OnBnClickedButtonSer27()	{	SelectServerAction(26);	}
void CQGLPatcherDlg::OnBnClickedButtonSer28()	{	SelectServerAction(27);	}
void CQGLPatcherDlg::OnBnClickedButtonSer29()	{	SelectServerAction(28);	}
void CQGLPatcherDlg::OnBnClickedButtonSer30()	{	SelectServerAction(29);	}
void CQGLPatcherDlg::OnBnClickedButtonSer31()	{	SelectServerAction(30);	}
void CQGLPatcherDlg::OnBnClickedButtonSer32()	{	SelectServerAction(31);	}
void CQGLPatcherDlg::OnBnClickedButtonSer33()	{	SelectServerAction(32);	}
void CQGLPatcherDlg::OnBnClickedButtonSer34()	{	SelectServerAction(33);	}
void CQGLPatcherDlg::OnBnClickedButtonSer35()	{	SelectServerAction(34);	}
void CQGLPatcherDlg::OnBnClickedButtonSer36()	{	SelectServerAction(35);	}
void CQGLPatcherDlg::OnBnClickedButtonSer37()	{	SelectServerAction(36);	}
void CQGLPatcherDlg::OnBnClickedButtonSer38()	{	SelectServerAction(37);	}
void CQGLPatcherDlg::OnBnClickedButtonSer39()	{	SelectServerAction(38);	}
void CQGLPatcherDlg::OnBnClickedButtonSer40()	{	SelectServerAction(39);	}
void CQGLPatcherDlg::OnBnClickedButtonSer41()	{	SelectServerAction(40);	}
void CQGLPatcherDlg::OnBnClickedButtonSer42()	{	SelectServerAction(41);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer43()	{	SelectServerAction(42);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer44()	{	SelectServerAction(43);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer45()	{	SelectServerAction(44);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer46()	{	SelectServerAction(45);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer47()	{	SelectServerAction(46);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer48()	{	SelectServerAction(47);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer49()	{	SelectServerAction(48);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer50()	{	SelectServerAction(49);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer51()	{	SelectServerAction(50);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer52()	{	SelectServerAction(51);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer53()	{	SelectServerAction(52);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer54()	{	SelectServerAction(53);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer55()	{	SelectServerAction(54);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer56()	{	SelectServerAction(55);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer57()	{	SelectServerAction(56);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer58()	{	SelectServerAction(57);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer59()	{	SelectServerAction(58);	SetManualSelected(); }
void CQGLPatcherDlg::OnBnClickedButtonSer60()	{	SelectServerAction(59);	SetManualSelected(); }

void CQGLPatcherDlg::OnBnClickedButtonDl()
{
	NavigateToBlank(theApp.GetConfigure()->m_patchLink.url.c_str());
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
	if( theApp.GetDefaultExists() )
	{
		theApp.SetSelectedServer(NO_SERVER_SELECTED);
		theApp.LookupDefaultZone(theApp.GetConfigureApp()->m_lastzonename.c_str());
		theApp.LookupDefaultServer(theApp.GetConfigureApp()->m_lastservername.c_str());
		theApp.LookupDefaultServerSetting();
		// jump zone page
		theApp.GetUIManagerZoneList()->SetSelect(*theApp.GetSelectedZone());
		theApp.GetUIManagerZoneList()->JumpSelectPage();
		// jump server page
		ServerInfoList zoneSrvList;
		zoneSrvList.clear();
		theApp.GetSelectedZone()->GetServerList(zoneSrvList);
		theApp.GetUIManagerServerList()->LoadContent(&zoneSrvList);
		theApp.GetUIManagerServerList()->SetSelect(theApp.GetSelectedServer());
		theApp.GetUIManagerServerList()->JumpSelectPage();

		RedrawCurServerSelectionPage();
	}
	// no action for else case
}

VOID CQGLPatcherDlg::SelectServerAction(UInt serverNo)
{
	if( serverNo < 20 )
	{
		ServerInfo serverInfo;
		if( theApp.GetUIManagerRecommList()->GetServerInfoBySlot(serverInfo, serverNo) )
		{
			theApp.SetSelectedServer(serverInfo.id);
			if( serverInfo.zoneId!=-1 )
			{
				// if server found on current zone, no page change needed
				bool found = false;
				if( theApp.GetSelectedZone() )
				{
					Zone* zone = theApp.GetSelectedZone();
					if( zone )
					{
						ServerInfoList serverList;
						zone->GetServerList(serverList);
						if( serverList.size() )
						{
							for( Index i = C_INDEX(0); i < serverList.size(); ++i )
							{
								ServerInfo& info = serverList[i];
								if( info.id==serverInfo.id )
								{
									found = true;
									break;
								}
							}
						}
					}
				}
				if( !found )
				{
					// jump zone page
					theApp.SetSelectedZone(*theApp.GetConfigureZoneList()->GetZone(serverInfo.zoneId));
					theApp.GetUIManagerZoneList()->JumpSelectPage();
					// jump server page
					ServerInfoList zoneSrvList;
					zoneSrvList.clear();
					theApp.GetSelectedZone()->GetServerList(zoneSrvList);
					theApp.GetUIManagerServerList()->LoadContent(&zoneSrvList);
					theApp.GetUIManagerServerList()->SetSelect(theApp.GetSelectedServer());
					theApp.GetUIManagerServerList()->JumpSelectPage();
				}
			}
			RedrawCurServerSelectionPage();
		}
	}else if( serverNo < 40 )
	{
		UInt zoneIdx = serverNo - 20;
		ZoneMap zonemap = theApp.GetDisplayMapZone();
		UInt32 counter = 0;
		if( zoneIdx < zonemap.size() )
		{
			ZoneMap::iterator itr = zonemap.begin();
			for( ; counter < zoneIdx; ++itr, ++counter );
			Zone& zone = itr->second;
			theApp.SetSelectedZone(zone);
			if( theApp.GetSelectedZone() )
			{
				ServerInfoList zoneSrvList;
				zoneSrvList.clear();
				theApp.GetSelectedZone()->GetServerList(zoneSrvList);
				theApp.GetUIManagerServerList()->LoadContent(&zoneSrvList);
				// server list page reset
				theApp.GetUIManagerServerList()->SetSelect(0);
				theApp.GetUIManagerServerList()->JumpSelectPage();
			}
			RedrawCurServerSelectionPage();
		}
	}else if( serverNo < 60 )
	{
		ServerInfo serverInfo;
		if( theApp.GetUIManagerServerList()->GetServerInfoBySlot(serverInfo, serverNo - 40) )
		{
			theApp.SetSelectedServer(serverInfo.id);
			RedrawCurServerSelectionPage();
		}
	}
}

void CQGLPatcherDlg::OnBnClickedButtonPagedown()
{
	theApp.GetUIManagerServerList()->SetNextPage();
	RedrawCurServerSelectionPage();
}

void CQGLPatcherDlg::OnBnClickedButtonPageup()
{
	theApp.GetUIManagerServerList()->SetPrevPage();
	RedrawCurServerSelectionPage();
}

void CQGLPatcherDlg::OnBnClickedButtonZPagedown()
{
	theApp.GetUIManagerZoneList()->SetNextPage();
	RedrawCurServerSelectionPage();
}

void CQGLPatcherDlg::OnBnClickedButtonZPageup()
{
	theApp.GetUIManagerZoneList()->SetPrevPage();
	RedrawCurServerSelectionPage();
}

VOID CQGLPatcherDlg::RedrawCurServerSelectionPage()
{
	// temp variables
	UInt displayStatus[MAX_SERVER_BUTTON];			// status = 0:normal, 1:busy, 2:jam, 3:disabled, 4:no item
	ZeroMemory(displayStatus, sizeof(UInt) * MAX_SERVER_BUTTON);
	ZoneMap zoneMap;
	ServerInfoList recommList, serverList;
	
	// 00-19 - button setting
	theApp.GetUIManagerRecommList()->GetPageContent(recommList);
	for( Index i = C_INDEX(0); i < recommList.size(); ++i )
	{
		ServerInfo& info = recommList[i];
		displayStatus[i] = info.status;
	}
	for( Index i = (UInt32)recommList.size(); i < C_INDEX(20); ++i )
		displayStatus[i] = 4;
	// 00-19 - Text assignment
	for( Index i = C_INDEX(0); i < recommList.size(); ++i )
	{
		ServerInfo& info = recommList[i];
		if( info.status==0 )
			m_serverChoice[i].LockStatus(3);
		else
			m_serverChoice[i].UnlockStatus();
		m_serverChoice[i].SetText(info.fullName_cache);
		if( info.id==theApp.GetSelectedServer() )
			m_serverChoice[i].LockStatus(3);
	}
	for( Index i = (UInt32)recommList.size(); i < theApp.GetUIManagerRecommList()->GetTotalSlot(); ++i )
	{
		String dummy;
		dummy.Empty();
		m_serverChoice[i].SetText(dummy);
		m_serverChoice[i].ShowWindow(SW_HIDE);
	}

	// 20-39 - button setting
	theApp.GetUIManagerZoneList()->GetCurPageContent(zoneMap);
	for( Index i = C_INDEX(20); i < zoneMap.size() + 20; ++i )
		displayStatus[i] = 0;
	for( Index i = (UInt32)zoneMap.size() + 20; i < C_INDEX(40); ++i )
		displayStatus[i] = 4;
	// 20-39 - Text assignment
	UInt counter = 0;
	for( ZoneMap::iterator itr = zoneMap.begin(); itr != zoneMap.end(); ++itr )
	{
		Zone& zone = itr->second;
		m_serverChoice[counter + 20].SetText(zone.GetName());
		m_serverChoice[counter + 20].UnlockStatus();
		if( zone.GetName().CompareNoCase(theApp.GetSelectedZone()->GetName().c_str())==0 )
			m_serverChoice[counter + 20].LockStatus(3);
		++counter;
	}
	for( Index i = counter; i < theApp.GetUIManagerZoneList()->GetPageSlot(); ++i )
	{
		String dummy;
		dummy.Empty();
		m_serverChoice[i + 20].SetText(dummy);
		m_serverChoice[i + 20].ShowWindow(SW_HIDE);
	}

	// 40-59 - button setting
	theApp.GetUIManagerServerList()->GetCurPageContent(serverList);
	for( Index i = C_INDEX(0); i < serverList.size(); ++i )
	{
		ServerInfo& serverInfo = serverList[i];
		displayStatus[i + 40] = serverInfo.status;
	}
	for( Index i = (UInt32)serverList.size(); i < C_INDEX(20); ++i )
		displayStatus[i + 40] = 4;
	// 40-59 - Text assignmeznt
	for( Index i = C_INDEX(0); i < serverList.size(); ++i )
	{
		ServerInfo& serverInfo = serverList[i];
		if( serverInfo.status==0 )
			m_serverChoice[i + 40].LockStatus(3);
		else
			m_serverChoice[i + 40].UnlockStatus();
		m_serverChoice[i + 40].SetText(serverInfo.name);
		if( serverInfo.id==theApp.GetSelectedServer() )
			m_serverChoice[i + 40].LockStatus(3);
	}
	for( Index i = (UInt32)serverList.size(); i < theApp.GetUIManagerZoneList()->GetTotalSlot(); ++i )
	{
		String dummy;
		dummy.Empty();
		m_serverChoice[i + 40].SetText(dummy);
		m_serverChoice[i + 40].ShowWindow(SW_HIDE);
	}
	// load bitmap according to Busy-status
	LoadServerButtonBitmapResource(displayStatus, MAX_SERVER_BUTTON);

	// button redraw
	UInt listcount;
	listcount = (UInt)theApp.GetConfigure()->m_zoneinfo1.x.size() * theApp.GetConfigure()->m_zoneinfo1.y.size();
	for( Index i = C_INDEX(0); i < listcount; ++i )
	{
		if( displayStatus[i]!=4 )
			m_serverChoice[i].ShowWindow(SW_SHOW);
		else
			m_serverChoice[i].ShowWindow(SW_HIDE);

	}
	listcount = (UInt)theApp.GetConfigure()->m_zoneinfo2.x.size() * theApp.GetConfigure()->m_zoneinfo2.y.size();
	for( Index i = C_INDEX(20); i < listcount + 20; ++i )
	{
		if( displayStatus[i]!=4 )
			m_serverChoice[i].ShowWindow(SW_SHOW);
		else
			m_serverChoice[i].ShowWindow(SW_HIDE);
	}
	listcount = (UInt)theApp.GetConfigure()->m_zoneinfo3.x.size() * theApp.GetConfigure()->m_zoneinfo3.y.size();
	for( Index i = C_INDEX(40); i < listcount + 40; ++i )
	{
		if( displayStatus[i]!=4 )
			m_serverChoice[i].ShowWindow(SW_SHOW);
		else
			m_serverChoice[i].ShowWindow(SW_HIDE);
	}

	for( Index i = C_INDEX(0); i < C_INDEX(MAX_SERVER_BUTTON); ++i )
		m_serverChoice[i].RedrawWindow();

	if( theApp.GetDefaultExists() )
		m_serverCur.UnlockStatus();
	else
		m_serverCur.LockStatus(3);
	m_serverCur.RedrawWindow();

	// zone list page up/down assignment
	if( theApp.GetUIManagerZoneList()->HasPrevPage() )
		m_serverZPageUp.UnlockStatus();
	else
		m_serverZPageUp.LockStatus(3);
	if( theApp.GetUIManagerZoneList()->HasNextPage() )
		m_serverZPageDown.UnlockStatus();
	else
		m_serverZPageDown.LockStatus(3);
	// server list page up/down assignment
	if( theApp.GetUIManagerServerList()->HasPrevPage() )
		m_serverPageUp.UnlockStatus();
	else
		m_serverPageUp.LockStatus(3);
	if( theApp.GetUIManagerServerList()->HasNextPage() )
		m_serverPageDown.UnlockStatus();
	else
		m_serverPageDown.LockStatus(3);

	m_serverZPageUp.RedrawWindow();
	m_serverZPageDown.RedrawWindow();
	m_serverPageUp.RedrawWindow();
	m_serverPageDown.RedrawWindow();
}

VOID CQGLPatcherDlg::ConfirmDownload()
{
	theApp.SetCurStatus(PATCHER_DOWNLOAD);
	m_startGamePatch.LockStatus(3);
	m_startGamePatch.RedrawWindow();
	m_serverPrev.LockStatus(3);
	m_serverPrev.RedrawWindow();

	// ensure that startDownload status is not STOP for loop check
	theApp.SetDLStatus(PATCHER_DL_STATUSNOTIFY);
	AfxBeginThread(startDownload, 0);
	AfxBeginThread(startUpdateProgress, this);
}

VOID CQGLPatcherDlg::ReleaseConfirmDownloadUI()
{
	theApp.SetCurStatus(PATCHER_FREENAVIGATE);
	m_startGamePatch.UnlockStatus();
	m_startGamePatch.RedrawWindow();
	m_serverPrev.UnlockStatus();
	m_serverPrev.RedrawWindow();
}

UINT CQGLPatcherDlg::startDownloadTracker(LPVOID pParam)
{
	if( theApp.DownloadTrackerIpIni()==FALSE )
		exit(0);

	if( theApp.GetConfigureApp()->m_debug_ui )
		Sleep(10000);

	CQGLPatcherDlg* dlg = (CQGLPatcherDlg*)pParam;
	if( dlg && dlg->m_initTrackerDlg )
		dlg->m_initTrackerDlg->ShowWindow(0);
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

UINT CQGLPatcherDlg::startUpdateProgress(LPVOID pParam)
{
	CQGLPatcherDlg* dlg = (CQGLPatcherDlg*)pParam;
	while( theApp.GetDLStatus()==PATCHER_DL_STATUSNOTIFY )
		Sleep(200);
	while( theApp.GetDLStatus()!=PATCHER_DL_STATUSSTOP )
	{
		theApp.UpdateProgressMonitor();
		dlg->RedrawDownloadStatus();
		Sleep(200);
	}
	theApp.UpdateProgressMonitor();
	dlg->RedrawDownloadStatus();

	dlg->m_startGame.UnlockStatus();
	dlg->m_startGame.RedrawWindow();
	dlg->m_serverPrev.UnlockStatus();
	dlg->m_serverPrev.RedrawWindow();
	return 0;
}

VOID CQGLPatcherDlg::RedrawSerVerDisplay()
{
	RECT tempbox;
	tempbox.left	= theApp.GetConfigure()->m_patchTarVer.x;
	tempbox.top		= theApp.GetConfigure()->m_patchTarVer.y;
	tempbox.right	= tempbox.left	+ theApp.GetConfigure()->m_patchTarVer.w;
	tempbox.bottom	= tempbox.top	+ theApp.GetConfigure()->m_patchTarVer.h;
	RedrawWindow(&tempbox);
}

VOID CQGLPatcherDlg::RedrawCurVerDisplay()
{
	RECT tempbox;
	tempbox.left	= theApp.GetConfigure()->m_patchCurVer.x;
	tempbox.top		= theApp.GetConfigure()->m_patchCurVer.y;
	tempbox.right	= tempbox.left	+ theApp.GetConfigure()->m_patchCurVer.w;
	tempbox.bottom	= tempbox.top	+ theApp.GetConfigure()->m_patchCurVer.h;
	RedrawWindow(&tempbox);
}

VOID CQGLPatcherDlg::RedrawDownloadStatusText()
{
	RECT tempbox;
	// Progress Text Total
	tempbox.left	= theApp.GetConfigure()->m_progressText1.x;
	tempbox.top		= theApp.GetConfigure()->m_progressText1.y;
	tempbox.right	= tempbox.left	+ theApp.GetConfigure()->m_progressText1.w;
	tempbox.bottom	= tempbox.top	+ theApp.GetConfigure()->m_progressText1.h;
	RedrawWindow(&tempbox);
	// Progress Text File
	tempbox.left	= theApp.GetConfigure()->m_progressText2.x;
	tempbox.top		= theApp.GetConfigure()->m_progressText2.y;
	tempbox.right	= tempbox.left	+ theApp.GetConfigure()->m_progressText2.w;
	tempbox.bottom	= tempbox.top	+ theApp.GetConfigure()->m_progressText2.h;
	RedrawWindow(&tempbox);
}

VOID CQGLPatcherDlg::RedrawDownloadStatus()
{
	RedrawDownloadStatusText();
	RECT tempbox;
	// Total Progress Bar
	m_process_big.SetRange(0, (short)255);
	m_process_big.SetPos(theApp.GetProgressMonitor()->GetOverallCur()/1000*255/(theApp.GetProgressMonitor()->GetOverallTotal()/1000));
	tempbox.left	= theApp.GetConfigure()->m_patchProgress1.x;
	tempbox.top		= theApp.GetConfigure()->m_patchProgress1.y;
	tempbox.right	= tempbox.left	+ theApp.GetConfigure()->m_patchProgress1.w;
	tempbox.bottom	= tempbox.top	+ theApp.GetConfigure()->m_patchProgress1.h;
	RedrawWindow(&tempbox);
	// Per File Progress Bar
	m_process_small.SetRange(0, (short)255);
	m_process_small.SetPos(theApp.GetProgressMonitor()->GetFileCurrentSize()/1000*255/(theApp.GetProgressMonitor()->GetFileTotalSize()/1000));
	tempbox.left	= theApp.GetConfigure()->m_patchProgress2.x;
	tempbox.top		= theApp.GetConfigure()->m_patchProgress2.y;
	tempbox.right	= tempbox.left	+ theApp.GetConfigure()->m_patchProgress2.w;
	tempbox.bottom	= tempbox.top	+ theApp.GetConfigure()->m_patchProgress2.h;
	RedrawWindow(&tempbox);
}

void CQGLPatcherDlg::OnCbnSelchangeComboServerline()
{
}

void CQGLPatcherDlg::OnBnClickedButtonGo1()
{
	UInt pos = 0;
	if( (UInt)theApp.GetConfigure()->m_linkInfo.size() >= pos+1 )
		NavigateToBlank(theApp.GetConfigure()->m_linkInfo[pos].url.c_str());
}

void CQGLPatcherDlg::OnBnClickedButtonGo2()
{
	UInt pos = 1;
	if( (UInt)theApp.GetConfigure()->m_linkInfo.size() >= pos+1 )
		NavigateToBlank(theApp.GetConfigure()->m_linkInfo[pos].url.c_str());
}

void CQGLPatcherDlg::OnBnClickedButtonGo3()
{
	UInt pos = 2;
	if( (UInt)theApp.GetConfigure()->m_linkInfo.size() >= pos+1 )
		NavigateToBlank(theApp.GetConfigure()->m_linkInfo[pos].url.c_str());
}

void CQGLPatcherDlg::OnBnClickedButtonGo4()
{
	UInt pos = 3;
	if( (UInt)theApp.GetConfigure()->m_linkInfo.size() >= pos+1 )
		NavigateToBlank(theApp.GetConfigure()->m_linkInfo[pos].url.c_str());
}

void CQGLPatcherDlg::OnBnClickedButtonGo5()
{
	UInt pos = 4;
	if( (UInt)theApp.GetConfigure()->m_linkInfo.size() >= pos+1 )
		NavigateToBlank(theApp.GetConfigure()->m_linkInfo[pos].url.c_str());
}

void CQGLPatcherDlg::OnBnClickedButtonGo6()
{
	UInt pos = 5;
	if( (UInt)theApp.GetConfigure()->m_linkInfo.size() >= pos+1 )
		NavigateToBlank(theApp.GetConfigure()->m_linkInfo[pos].url.c_str());
}

void CQGLPatcherDlg::OnBnClickedButtonPatchStart()
{
	switch( theApp.GetCurStatus() )
	{
	case PATCHER_FREENAVIGATE:
		{
			// Environment checking
			String errString;
			UInt32 errId = 0;
			if( theApp.GetPatchCurrentVersion()!=theApp.GetPatchTargetVersion() && !theApp.CheckPatcherValid(errId, errString) )
			{
				String errOutput;
				errOutput.Format(
					_T("Invalid patching environment.\n\n%s"),
					errString.c_str()
					);
				MessageBox(errOutput.c_str());
			}else
			{
				ConfirmDownload();
			}
		}
		break;
	case PATCHER_SUMMARY_DOWNLOADING:
		MessageBox(
			theApp.GetStringTable()->Get(_T("MSG_SERVERLIST_DOWNLOADING")).c_str(),
			theApp.GetStringTable()->Get(_T("INFO_GENERAL")).c_str()
			);
		break;
	}
}
