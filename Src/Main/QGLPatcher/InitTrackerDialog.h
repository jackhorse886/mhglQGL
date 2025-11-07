// InitTrackerDialog.h : 標頭檔 
//

#pragma once
#include "MemDC.h"
#include "afxwin.h"
#include "afxcmn.h"

// CInitTrackerDialog 對話方塊
class CInitTrackerDialog : public CDialog
{
// 建構
public:
	CInitTrackerDialog(CWnd* pParent = NULL);	// 標準建構函式
	~CInitTrackerDialog();

// 對話方塊資料
	enum { IDD = IDD_DIALOG_INIT };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();

private:
	BOOL LoadBitmapResource();
	BOOL LoadFileBitmap(CBitmap* pBmp, LPCTSTR szFilename);
	void DrawBitmap(CMemDC* pDC, int xStart, int yStart, int wWidth, int wHeight, CDC* pTmpDC, int xSource, int ySource);

private:
	CBitmap* m_bmpInitBg;
};
