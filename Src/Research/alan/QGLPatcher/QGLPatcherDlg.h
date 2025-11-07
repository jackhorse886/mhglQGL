// QGLPatcherDlg.h : 標頭檔 
//

#pragma once
#include "MemDC.h"
#include "explorer_upper.h"
#include "afxwin.h"
#include "HoverButton.h"
#include "afxcmn.h"

// CQGLPatcherDlg 對話方塊
class CQGLPatcherDlg : public CDialog
{
// 建構
public:
	CQGLPatcherDlg(CWnd* pParent = NULL);	// 標準建構函式
	~CQGLPatcherDlg();

// 對話方塊資料
	enum { IDD = IDD_QGLPATCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Official News webpage
	CExplorer_upper m_browser1;
	// System update news
	CExplorer_upper m_browser2;
	// Advertisement
	CExplorer_upper m_browser3;

	afx_msg void OnBnClickedButtonGoofficial();
	afx_msg void OnBnClickedButtonRegistration();
	afx_msg void OnBnClickedButtonAccount();
	afx_msg void OnBnClickedButtonPoint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonSer1();
	afx_msg void OnBnClickedButtonSer2();
	afx_msg void OnBnClickedButtonSer3();
	afx_msg void OnBnClickedButtonSer4();
	afx_msg void OnBnClickedButtonSer5();
	afx_msg void OnBnClickedButtonSer6();
	afx_msg void OnBnClickedButtonSer7();
	afx_msg void OnBnClickedButtonSer8();
	afx_msg void OnBnClickedButtonSer9();
	afx_msg void OnBnClickedButtonSer10();
	afx_msg void OnBnClickedButtonSer11();
	afx_msg void OnBnClickedButtonSer12();
	afx_msg void OnBnClickedButtonSer13();
	afx_msg void OnBnClickedButtonSer14();
	afx_msg void OnBnClickedButtonSer15();
	afx_msg void OnBnClickedButtonSer16();
	afx_msg void OnBnClickedButtonSer17();
	afx_msg void OnBnClickedButtonSer18();
	afx_msg void OnBnClickedButtonSer19();
	afx_msg void OnBnClickedButtonSer20();
	afx_msg void OnBnClickedButtonDl();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonSerCur();
	afx_msg void OnBnClickedButtonPagedown();
	afx_msg void OnBnClickedButtonPageup();
	afx_msg void OnCbnSelchangeComboServerline();

	// Called by App to update Version field
	VOID RedrawSerVerDisplay();
	VOID RedrawCurVerDisplay();
	VOID ReleaseConfirmDownloadUI();

private:
	// Initialization
	BOOL LoadServerButtonBitmapResource(UInt* status, UInt noServerInPage);
	BOOL LoadBitmapResource();

	// Browser URL
	void NavigateToBlank(String address);

	// Bitmap control
	BOOL LoadFileBitmap(CBitmap* pBmp, LPCTSTR szFilename);
	void DrawBitmap (CMemDC* pDC, int xStart, int yStart, int wWidth, int wHeight, CDC* pTmpDC, int xSource = 0, int ySource = 0);

	// Page Navigation
	void ChangePage(unsigned int pageno);
	void HidePage(unsigned int pageno);
	void ShowPage(unsigned int pageno);

	// Server Page (2) control
	VOID SetTotalServerPage(UInt totalpage)		{m_totalServerPage = totalpage;};
	UInt GetTotalServerPage()					{return m_totalServerPage;};
	VOID SetCurServerPage(UInt serverpage)
	{
		if( serverpage>=0 && serverpage < GetTotalServerPage() )
			m_curServerPage = serverpage;
	};
	UInt GetCurServerPage()						{return m_curServerPage;};
	VOID SelectServerAction(UInt serverNo);
	VOID RedrawCurServerSelectionPage();

	// Begin Thread Process
	static UINT startDownload(LPVOID pParam);
	static UINT startDownloadTimer(LPVOID pParam);
	static UINT startDownloadTracker(LPVOID pParam);
	static UINT startDownloadSummary(LPVOID pParam);

	// Download and patch start
	VOID ConfirmDownload();

public:
	CHoverButton	m_exitGame;
	CHoverButton	m_startGame;
	CHoverButton	m_startPatch;

	CHoverButton	m_goOfficial;
	CHoverButton	m_goAccount;
	CHoverButton	m_goRegistration;
	CHoverButton	m_goPoint;
	CHoverButton	m_serverChoice[20];
	CHoverButton	m_serverCur;
	CHoverButton	m_serverNext;
	CHoverButton	m_serverPrev;
	CHoverButton	m_serverPageUp;
	CHoverButton	m_serverPageDown;

	CProgressCtrl	m_process_small;
	CProgressCtrl	m_process_big;

private:
	CBitmap*		m_bmpWebBg;
	CBitmap*		m_bmpServerBg;
	CBitmap*		m_bmpPatchBg;
	CBitmap*		m_bmpDL[4];
	CBitmap*		m_bmpIN[4];
	CBitmap*		m_bmpEmp;

	UInt			m_curServerPage; // 0-based page number
	UInt			m_totalServerPage; // (n + 19) / 20

	time_t			m_PrevTime;
};
