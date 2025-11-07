#pragma once


// CQGLNewPatchDlg dialog

class CQGLNewPatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CQGLNewPatchDlg)

public:
	CQGLNewPatchDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQGLNewPatchDlg();
	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_QGLPUBLISH_NEWVERSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSvn();
};
