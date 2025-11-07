// QGLPublishDlg.h : 標頭檔 
//

#pragma once

#include <MFCGridCtrl/GridCtrl.h>

// CQGLPublishDlg 對話方塊
class CQGLPublishDlg : public CDialog
{
// 建構
public:
	CQGLPublishDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_QGLPUBLISH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;
	CMenu m_menu;
	CStatusBar m_bar;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHelpAbout();
	afx_msg void OnToolsQglversionmerger();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpenexistingversion();
	afx_msg void OnFileExit();
	afx_msg void OnOptionDefaultsetting();
	afx_msg void OnGvnEndLabelEditSrcGridCtrl(NMHDR *pNMHDR, LRESULT* pResult);
	afx_msg void OnGvnEndLabelEditPackGridCtrl(NMHDR *pNMHDR, LRESULT* pResult);
	afx_msg void OnGvnEndLabelEditFileGridCtrl(NMHDR *pNMHDR, LRESULT* pResult);

private:
	void InitSrcGridCtrl();
	void InitPackGridCtrl();
	void InitFileGridCtrl();
	VOID UpdateSrcGridCtrl();
	VOID UpdateProductDisplay();
	VOID UpdatePackGridCtrl();
	VOID UpdateFileGridCtrl();
	VOID UpdatePackList();
	VOID ChangePackList();
	VOID TransferSetEnabled(BOOL Enabled);

public:
	CGridCtrl m_pSrcGridCtrl;
	CGridCtrl m_pPackGridCtrl;
	CGridCtrl m_pFileGridCtrl;
	afx_msg void OnCbnSelchangeListpack();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedGdfpackToright();
	afx_msg void OnBnClickedIdcGdfpackToleft();
	afx_msg void OnBnClickedFileToright();
	afx_msg void OnBnClickedFileToleft();

private:
	LineFilenameMap m_packLineMap;
	LineFilenameMap m_fileLineMap;
	LineFilenameMap m_srcLineMap;
public:
	afx_msg void OnFileClose();
};
