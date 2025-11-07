// QGLOpenPatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QGLPublish.h"
#include "QGLOpenPatchDlg.h"
#include ".\qglopenpatchdlg.h"


// CQGLOpenPatchDlg dialog

IMPLEMENT_DYNAMIC(CQGLOpenPatchDlg, CDialog)
CQGLOpenPatchDlg::CQGLOpenPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQGLOpenPatchDlg::IDD, pParent)
{
}

CQGLOpenPatchDlg::~CQGLOpenPatchDlg()
{
}

void CQGLOpenPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CQGLOpenPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDirectoryList();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CQGLOpenPatchDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_OPEN_LIST, OnLbnDblchangeOpenList)
END_MESSAGE_MAP()


// CQGLOpenPatchDlg message handlers

VOID CQGLOpenPatchDlg::GetDirectoryList()
{
	CListBox* pLB = (CListBox*)GetDlgItem(IDC_OPEN_LIST);
	if( !pLB ) return;

	m_optionMap.clear();
	for( int i = 0; i < pLB->GetCount(); ++i )
		pLB->DeleteString(0);

	BOOL bFinish = FALSE;
	UInt32 counter = 0;
	String criteria;
	criteria.Format(_T("%s\\*.*"), theApp.GetConfigure()->path_single.c_str());
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = FindFirstFile(criteria.c_str(), &FileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		while (!bFinish) 
		{ 
			if( !(FileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN
				|| !_tcsicmp(FileData.cFileName, _T("."))
				|| !_tcsicmp(FileData.cFileName, _T("..")))
				)
			{
				if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					OptionData option;
					option.revision = 0;
					option.version	= _ttoi(FileData.cFileName);
					if( option.version )
					{
						option.display.Format(_T("%i"), option.version);
						m_optionMap.insert(std::make_pair(counter++, option));
						pLB->AddString(option.display.c_str());
					}
				}
			}
			// find another
			if (!FindNextFile(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinish = TRUE; 
				else
				{
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("CheckVersionInfo Error (1) [Next file find error]"));
				}
			}
		}
		FindClose(hSearch);
	}
}
void CQGLOpenPatchDlg::OnBnClickedOk()
{
	CListBox* pLB = (CListBox*)GetDlgItem(IDC_OPEN_LIST);
	if( !pLB ) return;

	OptionDataMap::iterator itr = m_optionMap.find(pLB->GetCurSel());
	if( itr == m_optionMap.end() )	return;
	
	OptionData& option = itr->second;
	theApp.SetTargetVersion(option.version, option.revision);
	String target;
	target.Format(_T("%s\\%i"), theApp.GetConfigure()->path_single.c_str(), theApp.GetTargetVersion());
	theApp.SetTargetFolder(target.c_str());
	OnOK();
}

void CQGLOpenPatchDlg::OnLbnDblchangeOpenList()
{
	OnBnClickedOk();
}
