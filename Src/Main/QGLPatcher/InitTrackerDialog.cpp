// QGLPatcherDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "QGLPatcher.h"
#include "InitTrackerDialog.h"

#define INIT_WINDOW_SIZE_WIDTH				320
#define INIT_WINDOW_SIZE_HEIGHT				100

#define PATCHER_IMAGE_WAITING_BACKGROUND		_T("res/waiting.bmp")

#define TRANSPARENTCOLOR						RGB(0, 255, 0);

// CInitTrackerDialog 對話方塊



CInitTrackerDialog::CInitTrackerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInitTrackerDialog::IDD, pParent)
{
	m_bmpInitBg = NULL;
}

CInitTrackerDialog::~CInitTrackerDialog()
{
	if( m_bmpInitBg )		SafeDeleteObject(m_bmpInitBg);
}

void CInitTrackerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInitTrackerDialog, CDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CInitTrackerDialog 訊息處理常式

BOOL CInitTrackerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	LoadBitmapResource();

	UInt centerX = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (INIT_WINDOW_SIZE_WIDTH / 2);
	UInt centerY = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (INIT_WINDOW_SIZE_HEIGHT / 2);
	SetWindowPos(&wndTop, centerX, centerY, INIT_WINDOW_SIZE_WIDTH, INIT_WINDOW_SIZE_HEIGHT, NULL); 

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，以便繪製圖示。
// 對於使用文件/檢視模式的 MFC 應用程式，框架會自動完成此作業。

void CInitTrackerDialog::OnPaint() 
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
		CBitmap* pOldBmp = (CBitmap*) dcMem.SelectObject(m_bmpInitBg);
		BITMAP bmpInfo;
		m_bmpInitBg->GetBitmap(&bmpInfo);
		DrawBitmap( (CMemDC*)&dc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMem, 0, 0);
		dcMem.SelectObject(pOldBmp);
	}
}

BOOL CInitTrackerDialog::LoadFileBitmap(CBitmap* pBmp, LPCTSTR szFilename)
{
	pBmp->DeleteObject();
	return pBmp->Attach(LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
}

void CInitTrackerDialog::DrawBitmap(CMemDC* pDC, int xStart, int yStart, int wWidth, int wHeight, CDC* pTmpDC, int xSource, int ySource)
{
	CDC hdcMem;
	hdcMem.CreateCompatibleDC(NULL);
	pDC->BitBlt(xStart, yStart, wWidth, wHeight, pTmpDC, xSource, ySource,SRCCOPY);
	hdcMem.DeleteDC();
}

void CInitTrackerDialog::OnDestroy()
{
	CDialog::OnDestroy();

   // TODO: Add your message handler code here
	m_bmpInitBg->DeleteObject();
}

BOOL CInitTrackerDialog::LoadBitmapResource()
{
	m_bmpInitBg = SafeCreateObject(CBitmap);
	if (!LoadFileBitmap(m_bmpInitBg, PATCHER_IMAGE_WAITING_BACKGROUND))
	{
		SafeDeleteObject(m_bmpInitBg);
		m_bmpInitBg = NULL;
		return FALSE;
	}

	return TRUE;
}
