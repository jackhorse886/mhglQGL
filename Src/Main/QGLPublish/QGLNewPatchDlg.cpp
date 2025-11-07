// QGLNewPatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QGLPublish.h"
#include "QGLNewPatchDlg.h"
#include ".\qglnewpatchdlg.h"


// CQGLNewPatchDlg dialog

IMPLEMENT_DYNAMIC(CQGLNewPatchDlg, CDialog)
CQGLNewPatchDlg::CQGLNewPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQGLNewPatchDlg::IDD, pParent)
{
}

CQGLNewPatchDlg::~CQGLNewPatchDlg()
{
}

void CQGLNewPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CQGLNewPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	theApp.CheckVersionInfo();
	String strLatestVer;
	strLatestVer.Format(_T("%i"), theApp.GetLatestVersion()+1);
	GetDlgItem(IDC_NEW_NEWVERSION)->SetWindowText(strLatestVer.c_str());
	String strNewVer;
	strNewVer.Format(_T("%i"), 0);
	GetDlgItem(IDC_NEW_NEWREVISION)->SetWindowText(strNewVer.c_str());
	String majorVersion;
	theApp.GetConfigure()->GetMajorVersion(theApp.GetLatestVersion()+1, majorVersion);
	GetDlgItem(IDC_EDIT_MAJORVERSION)->SetWindowText(majorVersion.c_str());
	return TRUE;
}

BEGIN_MESSAGE_MAP(CQGLNewPatchDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SVN, OnBnClickedButtonSvn)
END_MESSAGE_MAP()


// CQGLNewPatchDlg message handlers

void CQGLNewPatchDlg::OnBnClickedOk()
{
	TCHAR strText[MAX_PATH];
	UInt32 patchVersion, patchRevision;
	ZeroMemory(strText, MAX_PATH * sizeof(TCHAR));
	GetDlgItemText(IDC_NEW_NEWVERSION, strText, MAX_PATH);
	patchVersion = _ttoi(strText);
	ZeroMemory(strText, MAX_PATH * sizeof(TCHAR));
	GetDlgItemText(IDC_NEW_NEWREVISION, strText, MAX_PATH);
	patchRevision = _ttoi(strText);
	theApp.SetTargetVersion(patchVersion, patchRevision);
	String target;
	target.Format(_T("%s\\%i"), theApp.GetConfigure()->path_single.c_str(), theApp.GetTargetVersion());
	theApp.SetTargetFolder(target.c_str());
	ZeroMemory(strText, MAX_PATH * sizeof(TCHAR));
	GetDlgItemText(IDC_EDIT_MAJORVERSION, strText, MAX_PATH);
	theApp.SetMajorVersion(strText);
	OnOK();
}

void CQGLNewPatchDlg::OnBnClickedButtonSvn()
{
	// generation
	theApp.GenerateSVNRevision();
	theApp.GetConfigureSVN()->LoadConfigure();
	// display
	String revision;
	revision.Format(_T("%d"), theApp.GetConfigureSVN()->svnRevision);
	SetDlgItemText(IDC_NEW_NEWREVISION, revision.c_str());
}
