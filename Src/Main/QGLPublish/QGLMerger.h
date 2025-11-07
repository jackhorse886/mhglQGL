#pragma once


// CQGLMerger dialog

class CQGLMerger : public CDialog
{
	DECLARE_DYNAMIC(CQGLMerger)

public:
	CQGLMerger(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQGLMerger();

// Dialog Data
	enum { IDD = IDD_QGLPUBLISH_MERGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedCheckIntegrate();
	afx_msg void OnEditSrcDest();
	afx_msg void OnEditDestDest();
	afx_msg void OnEditFinalDest();

private:
	void Refresh();
};
