// GDFMergerDlg.h : 標頭檔 
//

#pragma once

enum APP_STATUS
{
	APP_INVALID = 0,
	APP_STATUS_IDLE,
	APP_STATUS_BUSY
};

// CGDFMergerDlg 對話方塊
class CGDFMergerDlg : public CDialog
{
// 建構
public:
	CGDFMergerDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_GDFMERGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();

	VOID ChangeStatus(INT newstatus);
	INT m_status;
};
