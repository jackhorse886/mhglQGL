// GridCellFileDialog.cpp: implementation of the CGridCellFileDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellFileDialog.h"

#include "GridCtrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridCellFileDialog::CGridCellFileDialog(int nRow, int nCol, const RECT & rect, CWnd * pParentWnd, UINT uID, CCreateContext * pContext /*NULL*/)
{
	// Initialize variables
	m_strFilter = _T("");
	::ZeroMemory(&m_ofn, sizeof(m_ofn));
	::ZeroMemory(&m_szFile, sizeof(m_szFile));
	::ZeroMemory(&m_szFileTitle, sizeof(m_szFileTitle));
	::ZeroMemory(&m_szSelectedFolder, sizeof(m_szSelectedFolder));
	m_nRow = nRow;
	m_nColumn = nCol;
	Create(NULL, NULL, WS_CHILD, rect, pParentWnd, uID, pContext);
}

CGridCellFileDialog::CGridCellFileDialog(int nRow, int nCol, const RECT & rect, CWnd * pParentWnd, UINT uID,
		BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CCreateContext * pContext /*NULL*/)
{
	// Set dialog
	m_nRow = nRow;
	m_nColumn = nCol;
	SetDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd);
	Create(NULL, NULL, WS_CHILD, rect, pParentWnd, uID, pContext);
}

CGridCellFileDialog::~CGridCellFileDialog()
{
}

void CGridCellFileDialog::SetDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd)
{
	// Declare variables
	CString strFilter;
	int nIndex = 0;

	// Store parameters into structure
	m_bOpenFileDialog = bOpenFileDialog;
	m_ofn.lpstrDefExt = lpszDefExt;
	if (lpszFileName != NULL)
	{
		_tcscpy(m_szFile, lpszFileName);
		m_ofn.lpstrFile = m_szFile;
		m_ofn.nMaxFile = MAX_PATH;
	}
	else
	{
		m_ofn.lpstrFile = m_szFile;
		m_ofn.nMaxFile = MAX_PATH;
	}
	m_ofn.lpstrFileTitle = m_szFileTitle;
	m_ofn.nMaxFileTitle = MAX_PATH;
	m_ofn.Flags = dwFlags | OFN_EXPLORER;
	if (lpszFilter != NULL)
	{
		m_strFilter = lpszFilter;
		LPTSTR pch = m_strFilter.GetBuffer(0);
		while ((pch = _tcschr(pch, '|')) != NULL)
		{
			*pch++ = '\0';
		}
		m_ofn.lpstrFilter = m_strFilter;
	}
	if (pParentWnd != NULL)
	{
		m_ofn.hwndOwner = pParentWnd->GetSafeHwnd();
	}
}

int CGridCellFileDialog::DoModal()
{
	// Declare variables
	BOOL bRetValue;
	DWORD dwWinMajor;

	//memset(&m_ofn, 0, sizeof(OPENFILENAME));

	// Get OS version
	// Use GetVersionEx instead of deprecated GetVersion
	#pragma warning(push)
	#pragma warning(disable: 4996)
	OSVERSIONINFO osvi = {};
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (::GetVersionEx(&osvi))
	{
		dwWinMajor = osvi.dwMajorVersion;
	}
	else
	{
		dwWinMajor = 4;  // Default to Windows 95/98 behavior
	}
	#pragma warning(pop)
	if (dwWinMajor >= 5)
	{
		m_ofn.lStructSize = sizeof(m_ofn);
	}
	else
	{
		m_ofn.lStructSize = sizeof(OPENFILENAME);
	}

	// Execute dialog
	if (m_bOpenFileDialog)
	{
		bRetValue = ::GetOpenFileName(&m_ofn);
	}
	else
	{
		bRetValue = ::GetSaveFileName(&m_ofn);
	}

	return (bRetValue ? IDOK : IDCANCEL);
}

CString CGridCellFileDialog::GetPathName() const
{
	return m_ofn.lpstrFile;
}

CString CGridCellFileDialog::GetFileName() const
{
	return m_ofn.lpstrFileTitle;
}

CString CGridCellFileDialog::GetFileTitle() const
{
	TCHAR szTitle[MAX_PATH];

	// Validate
	if (m_ofn.lpstrFile == NULL)
	{
		return  _T("");
	}

	// Split path into components
	_tsplitpath(m_ofn.lpstrFile, NULL, NULL, szTitle, NULL);

	return szTitle;
}

CString CGridCellFileDialog::GetFileExt() const
{
	// Declare variables
	TCHAR szExt[MAX_PATH];

	// Validate
	if (m_ofn.lpstrFile == NULL)
	{
		return  _T("");
	}

	// Split path into components
	_tsplitpath(m_ofn.lpstrFile, NULL, NULL, NULL, szExt);

	memmove(szExt, szExt + 1,_tcslen(szExt) -1);
	szExt[_tcslen(szExt) -1] = '\0';

	return szExt;
}

CString CGridCellFileDialog::GetFileDir() const
{
	// Declare variables
	TCHAR szDrive[MAX_PATH];
	TCHAR szDir[MAX_PATH];

	// Validate
	if (m_ofn.lpstrFile == NULL)
	{
		return  _T("");
	}

	// Split path into components
	_tsplitpath(m_ofn.lpstrFile, szDrive, szDir, NULL, NULL);
	::lstrcat(szDrive, szDir);

	return szDrive;
}

CString CGridCellFileDialog::GetFileDrive() const
{
	// Declare variables
	TCHAR szDrive[MAX_PATH];

	// Validate
	if (m_ofn.lpstrFile == NULL)
	{
		return  _T("");
	}

	// Split path into components
	_tsplitpath(m_ofn.lpstrFile, szDrive, NULL, NULL, NULL);

	return szDrive;
}

POSITION CGridCellFileDialog::GetStartPosition() const
{
	return (POSITION)m_ofn.lpstrFile;
}

CString CGridCellFileDialog::GetNextPathName(POSITION& pos) const
{
	// Declare variables
	BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
	TCHAR chDelimiter;
	LPTSTR lpsz;
	CString strPath;
	LPTSTR lpszPath;
	LPTSTR lpszFileName;
	CString strFileName;

	// Explorer?
	if (bExplorer)
	{
		chDelimiter = _T('\0');
	}
	else
	{
		chDelimiter = _T(' ');
	}

	// Get it
	lpsz = (LPTSTR)pos;
	if (lpsz == m_ofn.lpstrFile) 
	{
		if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == FALSE)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}

		// -- find char pos after first Delimiter
		while(*lpsz != chDelimiter && *lpsz != '\0')
			lpsz = _tcsinc(lpsz);
		lpsz = _tcsinc(lpsz);

		// -- If single selection then return only selection
		if (*lpsz == 0)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}
	}
	strPath = m_ofn.lpstrFile;
	if (!bExplorer)
	{
		lpszPath = m_ofn.lpstrFile;
		while(*lpszPath != chDelimiter)
		{
			lpszPath = _tcsinc(lpszPath);
		}
		strPath = strPath.Left(lpszPath - m_ofn.lpstrFile);
	}
	lpszFileName = lpsz;
	strFileName = lpsz;

	// Find char pos at next Delimiter
	while(*lpsz != chDelimiter && *lpsz != '\0')
	{
		lpsz = _tcsinc(lpsz);
	}

	if (!bExplorer && *lpsz == '\0')
	{
		pos = NULL;
	}
	else
	{
		if (!bExplorer)
		{
			strFileName = strFileName.Left(lpsz - lpszFileName);
		}

		// -- If double terminated then done
		lpsz = _tcsinc(lpsz);
		if (*lpsz == '\0') 
		{
			pos = NULL;
		}
		else
		{
			pos = (POSITION)lpsz;
		}
	}

	// -- Only add '\\' if it is needed
	if (!strPath.IsEmpty())
	{
		// -- check for last back-slash or forward slash (handles DBCS)
		LPCTSTR lpsz = _tcsrchr(strPath, '\\');
		if (lpsz == NULL)
		{
			lpsz = _tcsrchr(strPath, '/');
		}
		// -- if it is also the last character, then we don't need an extra
		if (lpsz != NULL &&	(lpsz - (LPCTSTR)strPath) == strPath.GetLength()-1)
		{
			ASSERT(*lpsz == '\\' || *lpsz == '/');
			return strPath + strFileName;
		}
	}

	return strPath + '\\' + strFileName;
}

int CGridCellFileDialog::SelectFolder(LPCTSTR lpszTitle, LPCTSTR lpszStartPath, UINT ulFlags, CWnd* pParentWnd)
{
	// Declare variables
	LPMALLOC pMalloc;
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	int	nRetValue = IDCANCEL;
	::ZeroMemory(&bi, sizeof(bi));

	// Gets the Shell's default allocator
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		// -- Get help on BROWSEINFO struct - it's got all the bit settings.
		if (pParentWnd != NULL)
		{
			bi.hwndOwner = pParentWnd->GetSafeHwnd();
		}
		bi.pidlRoot = NULL;
		bi.pszDisplayName = m_szSelectedFolder;
		bi.lpszTitle = lpszTitle;
		bi.ulFlags = ulFlags;
		bi.lpfn = BrowseCtrlCallback;
		bi.lParam = (LPARAM)lpszStartPath;
		
		// -- This next call issues the dialog box.
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, m_szSelectedFolder))
			{ 
				// -- -- At this point pszBuffer contains the selected path
				nRetValue = IDOK;
			} 

			// -- -- Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free(pidl);
		}

		// -- Release the shell's allocator.
		pMalloc->Release();
	}

	return nRetValue;
}

int __stdcall CGridCellFileDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED && lpData != NULL)
	{
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}

CString CGridCellFileDialog::GetSelectedFolder() const
{
	return m_szSelectedFolder;
}

void CGridCellFileDialog::EndEdit()
{
	CString str;

    GetWindowText(str);

    // Send Notification to parent
    GV_DISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetSafeHwnd();
    dispinfo.hdr.idFrom   = GetDlgCtrlID();
    dispinfo.hdr.code     = GVN_ENDLABELEDIT;

    dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
    dispinfo.item.row     = m_nRow;
    dispinfo.item.col     = m_nColumn;
    dispinfo.item.strText  = str;
    dispinfo.item.lParam  = (LPARAM)0;

    CWnd* pOwner = GetOwner();
    if (pOwner)
        pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);

    // Close this window (PostNcDestroy will delete this)
    if (IsWindow(GetSafeHwnd()))
        SendMessage(WM_CLOSE, 0, 0);
}
