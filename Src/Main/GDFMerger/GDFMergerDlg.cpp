// GDFMergerDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GDFMerger.h"
#include "GDFMergerDlg.h"
#include ".\gdfmergerdlg.h"

#include "Merger.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#define CONFIG_SOURCE_FILENAME1	_T("Data\\Data.gdf")
#define CONFIG_SOURCE_FILENAME2	_T("Patch\\patch.gdf")
#define CONFIG_TARGET_FILENAME	_T("Data\\Data.gdf")

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


// CGDFMergerDlg 對話方塊



CGDFMergerDlg::CGDFMergerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGDFMergerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGDFMergerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGDFMergerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CGDFMergerDlg 訊息處理常式

BOOL CGDFMergerDlg::OnInitDialog()
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
	// Initialization of variables
	m_status = APP_STATUS_IDLE;
	this->SetDlgItemText(IDC_SRC1, CONFIG_SOURCE_FILENAME1);
	this->SetDlgItemText(IDC_SRC2, CONFIG_SOURCE_FILENAME2);
	this->SetDlgItemText(IDC_DEST, CONFIG_TARGET_FILENAME);

	
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CGDFMergerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGDFMergerDlg::OnPaint() 
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
HCURSOR CGDFMergerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGDFMergerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
//	OnOK();
}

void CGDFMergerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	Merger m;
	CString asrc1, asrc2, adest;
	this->GetDlgItemText(IDC_SRC1, asrc1);
	this->GetDlgItemText(IDC_SRC2, asrc2);
	this->GetDlgItemText(IDC_DEST, adest);

	String src1 = asrc1;
	String src2 = asrc2;
	String dest = adest;
	ChangeStatus(APP_STATUS_BUSY);
	m.Merge(src1.c_str(), src2.c_str(), dest.c_str());
	ChangeStatus(APP_STATUS_IDLE);

//	OnOK();
}

VOID CGDFMergerDlg::ChangeStatus(INT newstatus)
{
	if( m_status==APP_STATUS_IDLE && newstatus==APP_STATUS_BUSY )
	{
		m_status = APP_STATUS_BUSY;
		GetDlgItem(IDC_SRC1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SRC2)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEST)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	}
	else if( m_status==APP_STATUS_BUSY && newstatus==APP_STATUS_IDLE )
	{
		m_status = APP_STATUS_IDLE;
		GetDlgItem(IDC_SRC1)->EnableWindow(TRUE);
		GetDlgItem(IDC_SRC2)->EnableWindow(TRUE);
		GetDlgItem(IDC_DEST)->EnableWindow(TRUE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	}
}