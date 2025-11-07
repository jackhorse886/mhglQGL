// GridCellFile.cpp: implementation of the CGridCellFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGridCellFile, CGridCell)

CGridCellFile::CGridCellFile() : CGridCell()
{
	// Initialize variables
	m_pdlgFile = NULL;
	m_dwOptions = NULL;
	m_lDefaultHeight = 0;
	m_dwDlgFlags = NULL;
	m_strDefExt = _T("");
	m_strFilter = _T("");
	m_lDefaultHeight = 0;
	m_nRow = 0;
	m_nColumn = 0;
	m_strDlgTitle = _T("Select Folder");
}

CGridCellFile::~CGridCellFile()
{

}

// Create a control to do the editing
BOOL CGridCellFile::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
	m_pdlgFile = new CGridCellFileDialog(nRow, nCol, rect, GetGrid(), IDC_INPLACE_CONTROL);
	m_bEditing = TRUE;
    m_nRow = nRow;
	m_nColumn = nCol;
    // Declare variables
	BOOL bMultiline = FALSE;
	int nNumItems = 0;
	POSITION psPos;
	CString strFile;

	// Get file
	if (m_strFileNames.GetSize() == 0)
	{
		strFile = _T("");
	}
	else
	{
		strFile = m_strFileNames.GetAt(0);
	}

	// Select folder
	if (GetOption(OT_FILE_SELECTDIALOG) == TRUE)
	{
		if (m_pdlgFile->SelectFolder(m_strDlgTitle, strFile, m_dwDlgFlags, (CWnd*)GetGrid()) == IDOK)
		{
			bMultiline = TRUE;

			// -- Get file names
			m_strFileNames.RemoveAll();
			m_strFileNames.Add(m_pdlgFile->GetSelectedFolder());

			// -- Number of items
			nNumItems = 1;

			GetGrid()->RedrawCell(nRow, nCol);
			EndEdit();
			return TRUE;
		}
	}
	// Normal
	else
	{
		m_pdlgFile->SetDialog(GetOption(OT_FILE_OPENDIALOG), m_strDefExt, strFile, m_dwDlgFlags, m_strFilter, (CWnd*)GetGrid());
		if (m_pdlgFile->DoModal() == IDOK)
		{
			bMultiline = TRUE;

			// -- Get file names
			m_strFileNames.RemoveAll();
			psPos = m_pdlgFile->GetStartPosition();

			while (psPos != NULL)
			{
				m_strFileNames.Add(m_pdlgFile->GetNextPathName(psPos));

				nNumItems++;

				GetGrid()->RedrawCell(nRow, nCol);
				EndEdit();
				return TRUE;
			}
		}
	}

	if (m_pdlgFile)
	{
		delete m_pdlgFile;
		m_pdlgFile = NULL;
	}
	EndEdit();
    return FALSE;
}

void CGridCellFile::EndEdit()
{
	if (!m_bEditing)
		return;

	if (m_pdlgFile)
	{
		m_pdlgFile->SetWindowText(m_strFileNames.GetAt(0));
        m_pdlgFile->EndEdit();
		delete m_pdlgFile;
	}
	m_pdlgFile = NULL;
	OnEndEdit();
}

BOOL CGridCellFile::GetOption(DWORD dwOption)
{
	// Return option
	return (m_dwOptions & dwOption) ? TRUE : FALSE;
}

void CGridCellFile::SetOption(DWORD dwOption, BOOL bSet)
{
	// Set option
	if (bSet == TRUE)
	{
		m_dwOptions |= dwOption;
	}
	else
	{
		m_dwOptions &= ~dwOption;
	}	
}

BOOL CGridCellFile::GetDialogFlag(DWORD dwOption)
{
	// Return option
	return (m_dwDlgFlags & dwOption) ? TRUE : FALSE;
}

void CGridCellFile::SetDialogFlag(DWORD dwOption, BOOL bSet)
{
	// Set option
	if (bSet == TRUE)
	{
		m_dwDlgFlags |= dwOption;
	}
	else
	{
		m_dwDlgFlags &= ~dwOption;
	}
}

BOOL CGridCellFile::CreateFileItem(CString strFile, CString strDefExt, CString strFilter, DWORD dwOptions, DWORD dwDlgFlags)
{
	// Save options
	m_dwOptions = dwOptions;

	// Save dialog flags
	m_dwDlgFlags = dwDlgFlags;

	// Save file
	m_strFileNames.Add(strFile);

	// Save default extention
	m_strDefExt = strDefExt;

	// Save filter
	m_strFilter = strFilter;

	return TRUE;
}

void CGridCellFile::AddFileName(CString strFile)
{
	m_strFileNames.Add(strFile);
}

void CGridCellFile::SetDialogFlags(DWORD dwFlags)
{
	m_dwDlgFlags = dwFlags;
}

DWORD CGridCellFile::GetDialogFlags()
{
	return m_dwDlgFlags;
}

void CGridCellFile::SertDefaultExtention(CString strExt)
{
	m_strDefExt = strExt;
}

CString CGridCellFile::GetDefaultExtention()
{
	return m_strDefExt;
}

void CGridCellFile::SetFilter(CString strFilter)
{
	m_strFilter = strFilter;
}

CString CGridCellFile::GetFilter()
{
	return m_strFilter;
}

void CGridCellFile::SetDialogTitle(CString strTitle)
{
	m_strDlgTitle = strTitle;
}

CString CGridCellFile::GetDialogTitle()
{
	return m_strDlgTitle;
}

CString CGridCellFile::GetFileExtention(CString strFile)
{
	// Declare variables
	TCHAR szExt[MAX_PATH];

	// Split path into components
	_tsplitpath(strFile, NULL, NULL, NULL, szExt);

	memmove(szExt, szExt + 1,_tcslen(szExt) - 1);
	szExt[_tcslen(szExt) - 1] = '\0';

	return szExt;
}

CString CGridCellFile::GetFileDirectory(CString strFile)
{
	// Declare variables
	TCHAR szDrive[MAX_PATH];
	TCHAR szDir[MAX_PATH];

	// Split path into components
	_tsplitpath(strFile, szDrive, szDir, NULL, NULL);
	::lstrcat(szDrive, szDir);

	return szDrive;
}

CString CGridCellFile::GetFileDrive(CString strFile)
{
	// Declare variables
	TCHAR szDrive[MAX_PATH];

	// Split path into components
	_tsplitpath(strFile, szDrive, NULL, NULL, NULL);

	return szDrive;
}

POSITION CGridCellFile::GetStartPosition()
{
	return m_pdlgFile->GetStartPosition();
}

CString CGridCellFile::GetNextPathName(POSITION &pos)
{
	return m_pdlgFile->GetNextPathName(pos);
}

CString CGridCellFile::GetPathName()
{
	return m_pdlgFile->GetPathName();
}

CString CGridCellFile::GetFileName()
{
	return m_pdlgFile->GetFileName();
}

CString CGridCellFile::GetFileTitle()
{
	return m_pdlgFile->GetFileTitle();
}

CString CGridCellFile::GetFileExt()
{
	return m_pdlgFile->GetFileExt();
}

CString CGridCellFile::GetFileDir()
{
	return m_pdlgFile->GetFileDir();
}

CString CGridCellFile::GetFileDrive()
{
	return m_pdlgFile->GetFileDrive();
}

CString CGridCellFile::GetSelectedFolder()
{
	return m_pdlgFile->GetSelectedFolder();
}
