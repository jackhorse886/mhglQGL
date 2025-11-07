#pragma once
#include "library\mfcgridctrl\mfcgridctrl\gridctrl.h"


// CQGLSettingDlg dialog

class CQGLSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CQGLSettingDlg)

public:
	CQGLSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQGLSettingDlg();

// Dialog Data
	enum { IDD = IDD_QGLPUBLISH_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnGvnEndLabelEditSetting(NMHDR *pNMHDR, LRESULT* pResult);

private:
	VOID InitializeGridCtrl();
	VOID ValidateSaveButton();

public:
	CGridCtrl m_pSettingGridCtrl;
	afx_msg void OnBnClickedSaveclose();
};
