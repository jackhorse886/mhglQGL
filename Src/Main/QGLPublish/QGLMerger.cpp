// QGLMerger.cpp : implementation file
//

#include "stdafx.h"
#include "QGLPublish.h"
#include "QGLMerger.h"
#include ".\qglmerger.h"


// CQGLMerger dialog

IMPLEMENT_DYNAMIC(CQGLMerger, CDialog)
CQGLMerger::CQGLMerger(CWnd* pParent /*=NULL*/)
	: CDialog(CQGLMerger::IDD, pParent)
{
}

CQGLMerger::~CQGLMerger()
{
}

void CQGLMerger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQGLMerger, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CHECK_INTEGRATE, OnBnClickedCheckIntegrate)
	ON_EN_CHANGE(IDC_TEXT_SRC_DEST, OnEditSrcDest)
	ON_EN_CHANGE(IDC_TEXT_DEST_DEST, OnEditDestDest)
	ON_EN_CHANGE(IDC_TEXT_FINAL_DEST, OnEditFinalDest)
END_MESSAGE_MAP()


// CQGLMerger message handlers

void CQGLMerger::OnBnClickedOk()
{
    UINT srcDestVer = GetDlgItemInt(IDC_TEXT_SRC_DEST);
    UINT destDestVer = GetDlgItemInt(IDC_TEXT_DEST_DEST);
	int bCheck = ((CButton*)GetDlgItem(IDC_CHECK_INTEGRATE))->GetCheck();
	if( !bCheck )
	{
		theApp.ExecuteNormalMerge(srcDestVer, destDestVer);
		MessageBox(_T("Merge Succeeded"));
	}else
	{
		UINT finalDestVer = GetDlgItemInt(IDC_TEXT_FINAL_DEST);
		// e.g. Merge from (1->2..5->6 ==> 1->2 == 1->2,2->3,3->4,4->5,5->6)
		theApp.ExecuteIntegrateMerge(srcDestVer, destDestVer, finalDestVer);
		MessageBox(_T("Merge Succeeded"));
	}
}

void CQGLMerger::OnBnClickedCancel()
{
	OnCancel();
}

void CQGLMerger::OnBnClickedRefresh()
{
	Refresh();
}

void CQGLMerger::OnBnClickedCheckIntegrate()
{
	int bCheck = ((CButton*)GetDlgItem(IDC_CHECK_INTEGRATE))->GetCheck();
	if( bCheck )
		((CEdit*)GetDlgItem(IDC_TEXT_FINAL_DEST))->SetReadOnly(FALSE);
	else
		((CEdit*)GetDlgItem(IDC_TEXT_FINAL_DEST))->SetReadOnly(TRUE);
	Refresh();
}

void CQGLMerger::Refresh()
{
	// show UI component with respective values
	UInt32 srcSupVer, srcSupRev, srcDestVer, srcDestRev;
	UInt32 destSupVer, destSupRev, destDestVer, destDestRev;
    srcDestVer = GetDlgItemInt(IDC_TEXT_SRC_DEST);
	theApp.ReadPatchVer(srcDestVer, srcDestRev, srcSupVer, srcSupRev);
    destDestVer = GetDlgItemInt(IDC_TEXT_DEST_DEST);
	theApp.ReadPatchVer(destDestVer, destDestRev, destSupVer, destSupRev);

	// Upper half
	String strSrcPath, strDestPath, strFinalPath, strRevertPath;
	String rev;
	rev.Format(_T("%d"), srcSupVer);
	((CEdit*)GetDlgItem(IDC_TEXT_SRC_SUPPORT))->SetWindowText(rev.c_str());
	rev.Format(_T("%d"), srcSupRev);
	((CEdit*)GetDlgItem(IDC_TEXT_SRC_SUPPORT_REV))->SetWindowText(rev.c_str());
	rev.Format(_T("%d"), srcDestRev);
	((CEdit*)GetDlgItem(IDC_TEXT_SRC_DEST_REV))->SetWindowText(rev.c_str());
	rev.Format(_T("%d"), destDestRev);
	((CEdit*)GetDlgItem(IDC_TEXT_DEST_DEST_REV))->SetWindowText(rev.c_str());

	// Lower half
	// this field decided by 2 component: integration and user input
	String strTemp;
	int bCheck = ((CButton*)GetDlgItem(IDC_CHECK_INTEGRATE))->GetCheck();
	if( !bCheck )
	{
		strTemp.Format(_T("%i"), destDestVer);
		((CEdit*)GetDlgItem(IDC_TEXT_FINAL_DEST))->SetWindowText(strTemp.c_str());
	}
	rev.Format(_T("%i"), destDestRev);
	((CEdit*)GetDlgItem(IDC_TEXT_FINAL_DEST_REV))->SetWindowText(rev.c_str());

	strTemp.Format(_T("%i"), srcSupVer);
	((CEdit*)GetDlgItem(IDC_TEXT_FINAL_SUPPORT))->SetWindowText(strTemp.c_str());
	strTemp.Format(_T("%i"), srcSupRev);
	((CEdit*)GetDlgItem(IDC_TEXT_FINAL_SUPPORT_REV))->SetWindowText(strTemp.c_str());
	if( !bCheck )
		strTemp.Format(_T("%i"), destDestVer);
	else
		strTemp.Format(_T("%i"), GetDlgItemInt(IDC_TEXT_FINAL_DEST));
	((CEdit*)GetDlgItem(IDC_TEXT_REVERT_SUPPORT))->SetWindowText(strTemp.c_str());
	strTemp.Format(_T("%i"), destDestRev);
	((CEdit*)GetDlgItem(IDC_TEXT_REVERT_SUPPORT_REV))->SetWindowText(strTemp.c_str());
	strTemp.Format(_T("%i"), srcSupVer);
	((CEdit*)GetDlgItem(IDC_TEXT_REVERT_DEST))->SetWindowText(strTemp.c_str());
	strTemp.Format(_T("%i"), srcSupRev);
	((CEdit*)GetDlgItem(IDC_TEXT_REVERT_DEST_REV))->SetWindowText(strTemp.c_str());

	strSrcPath.Format(_T("%s\\%i"), theApp.GetConfigure()->path_single.c_str(), srcDestVer);
	((CEdit*)GetDlgItem(IDC_SRC_PATH))->SetWindowText(strSrcPath.c_str());
	strDestPath.Format(_T("%s\\%i"), theApp.GetConfigure()->path_single.c_str(), destDestVer);
	((CEdit*)GetDlgItem(IDC_DEST_PATH))->SetWindowText(strDestPath.c_str());
	
    UINT finalDestVer = GetDlgItemInt(IDC_TEXT_FINAL_DEST);
	if( !bCheck )
	{
		strFinalPath.Format(_T("%s\\%i_%i"), theApp.GetConfigure()->path_batch.c_str(), srcSupVer, destDestVer);
		((CEdit*)GetDlgItem(IDC_FINAL_PATH))->SetWindowText(strFinalPath.c_str());
		strRevertPath.Format(_T("%s\\%i_%i"), theApp.GetConfigure()->path_revertSingle.c_str(), destDestVer, srcSupVer);
		((CEdit*)GetDlgItem(IDC_REVERT_PATH))->SetWindowText(strRevertPath.c_str());
	}else
	{
		strFinalPath.Format(_T("%s\\%i"), theApp.GetConfigure()->path_single.c_str(), finalDestVer);
		((CEdit*)GetDlgItem(IDC_FINAL_PATH))->SetWindowText(strFinalPath.c_str());
		strRevertPath.Format(_T("%s\\%i_%i"), theApp.GetConfigure()->path_revertSingle.c_str(), finalDestVer, srcSupVer);
		((CEdit*)GetDlgItem(IDC_REVERT_PATH))->SetWindowText(strRevertPath.c_str());
	}
}

void CQGLMerger::OnEditSrcDest()
{
	Refresh();
}

void CQGLMerger::OnEditDestDest()
{
	Refresh();
}

void CQGLMerger::OnEditFinalDest()
{
	// if-case to prevent infinite loop caused by non integrate OnChange update
	int bCheck = ((CButton*)GetDlgItem(IDC_CHECK_INTEGRATE))->GetCheck();
	if( bCheck )
		Refresh();
}