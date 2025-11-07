// QGLPatcherDlg.h : 標頭檔 
//

#pragma once
#include "MemDC.h"
#include "explorer_upper.h"
#include "afxwin.h"
#include "HoverButton.h"
#include "InitTrackerDialog.h"
#include "afxcmn.h"

struct BlkHeader
{
	char magic[4];
	UInt32 version;
	UInt32 byteRegion;
	UInt64 filesize;
	UInt64 fileTime;
};

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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Official News webpage
	CExplorer_upper m_browser1;
	// Advertisement
	CExplorer_upper m_browser3;
	CExplorer_upper m_browser4;

	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonPatchStart();
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
	afx_msg void OnBnClickedButtonSer21();
	afx_msg void OnBnClickedButtonSer22();
	afx_msg void OnBnClickedButtonSer23();
	afx_msg void OnBnClickedButtonSer24();
	afx_msg void OnBnClickedButtonSer25();
	afx_msg void OnBnClickedButtonSer26();
	afx_msg void OnBnClickedButtonSer27();
	afx_msg void OnBnClickedButtonSer28();
	afx_msg void OnBnClickedButtonSer29();
	afx_msg void OnBnClickedButtonSer30();
	afx_msg void OnBnClickedButtonSer31();
	afx_msg void OnBnClickedButtonSer32();
	afx_msg void OnBnClickedButtonSer33();
	afx_msg void OnBnClickedButtonSer34();
	afx_msg void OnBnClickedButtonSer35();
	afx_msg void OnBnClickedButtonSer36();
	afx_msg void OnBnClickedButtonSer37();
	afx_msg void OnBnClickedButtonSer38();
	afx_msg void OnBnClickedButtonSer39();
	afx_msg void OnBnClickedButtonSer40();
	afx_msg void OnBnClickedButtonSer41();
	afx_msg void OnBnClickedButtonSer42();
	afx_msg void OnBnClickedButtonSer43();
	afx_msg void OnBnClickedButtonSer44();
	afx_msg void OnBnClickedButtonSer45();
	afx_msg void OnBnClickedButtonSer46();
	afx_msg void OnBnClickedButtonSer47();
	afx_msg void OnBnClickedButtonSer48();
	afx_msg void OnBnClickedButtonSer49();
	afx_msg void OnBnClickedButtonSer50();
	afx_msg void OnBnClickedButtonSer51();
	afx_msg void OnBnClickedButtonSer52();
	afx_msg void OnBnClickedButtonSer53();
	afx_msg void OnBnClickedButtonSer54();
	afx_msg void OnBnClickedButtonSer55();
	afx_msg void OnBnClickedButtonSer56();
	afx_msg void OnBnClickedButtonSer57();
	afx_msg void OnBnClickedButtonSer58();
	afx_msg void OnBnClickedButtonSer59();
	afx_msg void OnBnClickedButtonSer60();
	afx_msg void OnBnClickedButtonDl();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonSerCur();
	afx_msg void OnBnClickedButtonPageup();
	afx_msg void OnBnClickedButtonPagedown();
	afx_msg void OnBnClickedButtonZPageup();
	afx_msg void OnBnClickedButtonZPagedown();
	afx_msg void OnCbnSelchangeComboServerline();

	// Called by App to update Version field
	VOID RedrawSerVerDisplay();
	VOID RedrawCurVerDisplay();
	VOID RedrawDownloadStatus();
	VOID RedrawDownloadStatusText();
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
	VOID SelectServerAction(UInt serverNo);
	VOID RedrawCurServerSelectionPage();

	// Begin Thread Process
	static UINT startDownload(LPVOID pParam);
	static UINT startDownloadTracker(LPVOID pParam);
	static UINT startDownloadSummary(LPVOID pParam);
	static UINT startUpdateProgress(LPVOID pParam);

	// Download and patch start
	VOID ConfirmDownload();

	// human interaction
	BOOL GetManualSelected()					{ return m_bManualSelected; };
	VOID SetManualSelected()					{ m_bManualSelected = TRUE; };

public:
	CHoverButton	m_exitGame;
	CHoverButton	m_startGame;
	CHoverButton	m_startGamePatch;
	CHoverButton	m_startPatch;

	CHoverButton	m_goOfficial;
	CHoverButton	m_goAccount;
	CHoverButton	m_goRegistration;
	CHoverButton	m_goPoint;
	CHoverButton	m_officialSite;
	CHoverButton	m_customerService;
	CHoverButton	m_serverChoice[60];
	CHoverButton	m_serverCur;
	CHoverButton	m_serverNext;
	CHoverButton	m_serverPrev;
	CHoverButton	m_serverPageUp;
	CHoverButton	m_serverPageDown;
	CHoverButton	m_serverZPageUp;
	CHoverButton	m_serverZPageDown;

	CProgressCtrl	m_process_small;
	CProgressCtrl	m_process_big;

private:
	CBitmap*		m_bmpWebBg;
	CBitmap*		m_bmpServerBg;
	CBitmap*		m_bmpPatchBg;

	UInt			m_curServerPage; // 0-based page number
	UInt			m_totalServerPage; // (n + 19) / 20

	time_t			m_PrevTime;

	BOOL			m_bManualSelected;

public:
	afx_msg void OnBnClickedButtonGo1();
	afx_msg void OnBnClickedButtonGo2();
	afx_msg void OnBnClickedButtonGo3();
	afx_msg void OnBnClickedButtonGo4();
	afx_msg void OnBnClickedButtonGo5();
	afx_msg void OnBnClickedButtonGo6();

public:
	CInitTrackerDialog* m_initTrackerDlg;
};
