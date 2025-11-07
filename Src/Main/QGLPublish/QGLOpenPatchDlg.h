#pragma once

struct OptionData
{
	String display;
	UInt32 revision;
	UInt32 version;
};

typedef StlMap<UInt32, OptionData, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, OptionData> > > OptionDataMap;

// CQGLOpenPatchDlg dialog

class CQGLOpenPatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CQGLOpenPatchDlg)

public:
	CQGLOpenPatchDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQGLOpenPatchDlg();
	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_QGLPUBLISH_OPENPATCHVERSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	VOID GetDirectoryList();

private:
	OptionDataMap m_optionMap;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblchangeOpenList();
};
