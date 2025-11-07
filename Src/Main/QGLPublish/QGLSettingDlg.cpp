// QGLSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QGLPublish.h"
#include "QGLSettingDlg.h"
#include ".\qglsettingdlg.h"


// CQGLSettingDlg dialog

IMPLEMENT_DYNAMIC(CQGLSettingDlg, CDialog)
CQGLSettingDlg::CQGLSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQGLSettingDlg::IDD, pParent)
{
}

CQGLSettingDlg::~CQGLSettingDlg()
{
}

BOOL CQGLSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitializeGridCtrl();
	ValidateSaveButton();
	return TRUE;
}

void CQGLSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETTINGGRIDCTRL, m_pSettingGridCtrl);
}


BEGIN_MESSAGE_MAP(CQGLSettingDlg, CDialog)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_SETTINGGRIDCTRL, OnGvnEndLabelEditSetting)
	ON_BN_CLICKED(IDC_SAVECLOSE, OnBnClickedSaveclose)
END_MESSAGE_MAP()


// CQGLSettingDlg message handlers

VOID CQGLSettingDlg::InitializeGridCtrl()
{
	// Set Row and Col
	m_pSettingGridCtrl.SetRowCount(17+1);
	m_pSettingGridCtrl.SetColumnCount(2);
	m_pSettingGridCtrl.SetFixedRowCount(1);
	m_pSettingGridCtrl.SetFixedColumnCount(1);
	// Set Column Width
	m_pSettingGridCtrl.SetColumnWidth(0, 120);
	m_pSettingGridCtrl.SetColumnWidth(1, 340);
	// Set Title
	m_pSettingGridCtrl.SetItemText(0, 0, _T("Name"));
	m_pSettingGridCtrl.SetItemText(0, 1, _T("Value"));
	// Set State and Control Type of 1st Col
	for ( int i = 0; i < 2; ++i )
	{
		m_pSettingGridCtrl.SetItemState(0, i, m_pSettingGridCtrl.GetItemState(0, i) | GVIS_READONLY);
		m_pSettingGridCtrl.SetCellType(0, i, RUNTIME_CLASS(CGridCell));
	}
	UInt32 counter = 0;
	m_pSettingGridCtrl.SetItemText(1, 0, _T("Packaging Path"));
	m_pSettingGridCtrl.SetItemText(1, 1, theApp.GetConfigure()->path_single.c_str());
	m_pSettingGridCtrl.SetItemText(2, 0, _T("Merging Path"));
	m_pSettingGridCtrl.SetItemText(2, 1, theApp.GetConfigure()->path_batch.c_str());
	m_pSettingGridCtrl.SetItemText(3, 0, _T("Version Backup Path"));
	m_pSettingGridCtrl.SetItemText(3, 1, theApp.GetConfigure()->path_replace.c_str());
	m_pSettingGridCtrl.SetItemText(4, 0, _T("File Control Path"));
	m_pSettingGridCtrl.SetItemText(4, 1, theApp.GetConfigure()->path_complete.c_str());
	m_pSettingGridCtrl.SetItemText(5, 0, _T("Product Path"));
	m_pSettingGridCtrl.SetItemText(5, 1, theApp.GetConfigure()->path_product.c_str());
	m_pSettingGridCtrl.SetItemText(6, 0, _T("Last Source Path"));
	m_pSettingGridCtrl.SetItemText(6, 1, theApp.GetConfigure()->path_lastSource.c_str());
	m_pSettingGridCtrl.SetItemText(7, 0, _T("Last Source SVN check"));
	m_pSettingGridCtrl.SetItemText(7, 1, theApp.GetConfigure()->path_lastSourceSvn.c_str());
	m_pSettingGridCtrl.SetItemText(8, 0, _T("Patch Template Path"));
	m_pSettingGridCtrl.SetItemText(8, 1, theApp.GetConfigure()->path_template.c_str());
	m_pSettingGridCtrl.SetItemText(9, 0, _T("NSIS file path"));
	m_pSettingGridCtrl.SetItemText(9, 1, theApp.GetConfigure()->path_makensis_file.c_str());
	m_pSettingGridCtrl.SetItemText(10, 0, _T("SvnWCRew file path"));
	m_pSettingGridCtrl.SetItemText(10, 1, theApp.GetConfigure()->path_subwcrev_file.c_str());
	m_pSettingGridCtrl.SetItemText(11, 0, _T("Revert Packaging file path"));
	m_pSettingGridCtrl.SetItemText(11, 1, theApp.GetConfigure()->path_revertSingle.c_str());
	m_pSettingGridCtrl.SetItemText(12, 0, _T("Revert Product file path"));
	m_pSettingGridCtrl.SetItemText(12, 1, theApp.GetConfigure()->path_revertProduct.c_str());
	m_pSettingGridCtrl.SetItemText(13, 0, _T("FTP Summary file path"));
	m_pSettingGridCtrl.SetItemText(13, 1, theApp.GetConfigure()->path_ftp_summary_file.c_str());
	m_pSettingGridCtrl.SetItemText(14, 0, _T("FTP Filelist file path"));
	m_pSettingGridCtrl.SetItemText(14, 1, theApp.GetConfigure()->path_ftp_filelist_file.c_str());
	m_pSettingGridCtrl.SetItemText(15, 0, _T("FTP Revertlist file path"));
	m_pSettingGridCtrl.SetItemText(15, 1, theApp.GetConfigure()->path_ftp_revertlist_file.c_str());
	m_pSettingGridCtrl.SetItemText(16, 0, _T("FTP Folder"));
	m_pSettingGridCtrl.SetItemText(16, 1, theApp.GetConfigure()->path_ftp.c_str());
	m_pSettingGridCtrl.SetItemText(17, 0, _T("FTP Version File"));
	m_pSettingGridCtrl.SetItemText(17, 1, theApp.GetConfigure()->path_ftp_version_file.c_str());
}

VOID CQGLSettingDlg::ValidateSaveButton()
{
	BOOL bChanged = FALSE;
	if( theApp.GetConfigure()->path_single.CompareNoCase(m_pSettingGridCtrl.GetItemText(1, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_batch.CompareNoCase(m_pSettingGridCtrl.GetItemText(2, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_replace.CompareNoCase(m_pSettingGridCtrl.GetItemText(3, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_complete.CompareNoCase(m_pSettingGridCtrl.GetItemText(4, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_product.CompareNoCase(m_pSettingGridCtrl.GetItemText(5, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_lastSource.CompareNoCase(m_pSettingGridCtrl.GetItemText(6, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_lastSourceSvn.CompareNoCase(m_pSettingGridCtrl.GetItemText(7, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_template.CompareNoCase(m_pSettingGridCtrl.GetItemText(8, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_makensis_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(9, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_subwcrev_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(10, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_revertSingle.CompareNoCase(m_pSettingGridCtrl.GetItemText(11, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_revertProduct.CompareNoCase(m_pSettingGridCtrl.GetItemText(12, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_ftp_summary_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(13, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_ftp_filelist_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(14, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_ftp_revertlist_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(15, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_ftp.CompareNoCase(m_pSettingGridCtrl.GetItemText(16, 1)) )
		bChanged = TRUE;
	if( theApp.GetConfigure()->path_ftp_version_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(17, 1)) )
		bChanged = TRUE;
	if( bChanged )
		GetDlgItem(IDC_SAVECLOSE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_SAVECLOSE)->EnableWindow(FALSE);
}

void CQGLSettingDlg::OnGvnEndLabelEditSetting(NMHDR *pNMHDR, LRESULT* pResult)
{
	ValidateSaveButton();
}
void CQGLSettingDlg::OnBnClickedSaveclose()
{
	if( theApp.GetConfigure()->path_single.CompareNoCase(m_pSettingGridCtrl.GetItemText(1, 1)) )
		theApp.GetConfigure()->SaveSinglePath(m_pSettingGridCtrl.GetItemText(1, 1));
	if( theApp.GetConfigure()->path_batch.CompareNoCase(m_pSettingGridCtrl.GetItemText(2, 1)) )
		theApp.GetConfigure()->SaveBatchPath(m_pSettingGridCtrl.GetItemText(2, 1));
	if( theApp.GetConfigure()->path_replace.CompareNoCase(m_pSettingGridCtrl.GetItemText(3, 1)) )
		theApp.GetConfigure()->SaveReplacePath(m_pSettingGridCtrl.GetItemText(3, 1));
	if( theApp.GetConfigure()->path_complete.CompareNoCase(m_pSettingGridCtrl.GetItemText(4, 1)) )
		theApp.GetConfigure()->SaveCompletePath(m_pSettingGridCtrl.GetItemText(4, 1));
	if( theApp.GetConfigure()->path_product.CompareNoCase(m_pSettingGridCtrl.GetItemText(5, 1)) )
		theApp.GetConfigure()->SaveProductPath(m_pSettingGridCtrl.GetItemText(5, 1));
	if( theApp.GetConfigure()->path_lastSource.CompareNoCase(m_pSettingGridCtrl.GetItemText(6, 1)) )
		theApp.GetConfigure()->SaveLastSourcePath(m_pSettingGridCtrl.GetItemText(6, 1));
	if( theApp.GetConfigure()->path_lastSourceSvn.CompareNoCase(m_pSettingGridCtrl.GetItemText(7, 1)) )
		theApp.GetConfigure()->SaveLastSourceSvnPath(m_pSettingGridCtrl.GetItemText(7, 1));
	if( theApp.GetConfigure()->path_template.CompareNoCase(m_pSettingGridCtrl.GetItemText(8, 1)) )
		theApp.GetConfigure()->SaveTemplatePath(m_pSettingGridCtrl.GetItemText(8, 1));
	if( theApp.GetConfigure()->path_makensis_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(9, 1)) )
		theApp.GetConfigure()->SaveMakeNsisFile(m_pSettingGridCtrl.GetItemText(9, 1));
	if( theApp.GetConfigure()->path_subwcrev_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(10, 1)) )
		theApp.GetConfigure()->SaveSubWCRevFile(m_pSettingGridCtrl.GetItemText(10, 1));
	if( theApp.GetConfigure()->path_revertSingle.CompareNoCase(m_pSettingGridCtrl.GetItemText(11, 1)) )
		theApp.GetConfigure()->SaveRevertPath(m_pSettingGridCtrl.GetItemText(11, 1));
	if( theApp.GetConfigure()->path_revertProduct.CompareNoCase(m_pSettingGridCtrl.GetItemText(12, 1)) )
		theApp.GetConfigure()->SaveRevertProductPath(m_pSettingGridCtrl.GetItemText(12, 1));
	if( theApp.GetConfigure()->path_ftp_summary_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(13, 1)) )
		theApp.GetConfigure()->SaveFTPSummaryFile(m_pSettingGridCtrl.GetItemText(13, 1));
	if( theApp.GetConfigure()->path_ftp_filelist_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(14, 1)) )
		theApp.GetConfigure()->SaveFTPFilelistFile(m_pSettingGridCtrl.GetItemText(14, 1));
	if( theApp.GetConfigure()->path_ftp_revertlist_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(15, 1)) )
		theApp.GetConfigure()->SaveFTPRevertlistFile(m_pSettingGridCtrl.GetItemText(15, 1));
	if( theApp.GetConfigure()->path_ftp.CompareNoCase(m_pSettingGridCtrl.GetItemText(16, 1)) )
		theApp.GetConfigure()->SaveFTPPath(m_pSettingGridCtrl.GetItemText(16, 1));
	if( theApp.GetConfigure()->path_ftp_version_file.CompareNoCase(m_pSettingGridCtrl.GetItemText(17, 1)) )
		theApp.GetConfigure()->SaveFTPVersionFile(m_pSettingGridCtrl.GetItemText(17, 1));
	OnOK();
}
