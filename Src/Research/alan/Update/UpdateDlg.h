// UpdateDlg.h : 標頭檔 
//

#pragma once
#include "GDFPackager\GDFCommon.h"

// CUpdateDlg 對話方塊
class CUpdateDlg : public CDialog
{
// 建構
public:
	CUpdateDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_UPDATE_DIALOG };

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

private:
	static UINT StartCopyProcess(LPVOID pParam);
	VOID RecursiveMove(CONST String SrcFolder, CONST String TargetFolder);
public:
	VOID CopyProcess();
};
