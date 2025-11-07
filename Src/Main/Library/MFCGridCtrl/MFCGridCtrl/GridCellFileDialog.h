// GridCellFileDialog.h: interface for the CGridCellFileDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLFILEDIALOG_H__CAD95AAB_BE38_42DA_96CA_A59BF0C629B4__INCLUDED_)
#define AFX_GRIDCELLFILEDIALOG_H__CAD95AAB_BE38_42DA_96CA_A59BF0C629B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Structures
struct GC_OPENFILENAMEEX : public OPENFILENAME
{ 
	void*	m_pvReserved;
	DWORD	m_dwReserved;
	DWORD	m_dwFlagsEx;
};

class CGridCellFileDialog : public CWnd
{
public:
	CGridCellFileDialog(int nRow, int nCol, const RECT & rect, CWnd * pParentWnd, UINT uID, CCreateContext * pContext = NULL);
	CGridCellFileDialog(int nRow, int nCol, const RECT & rect, CWnd * pParentWnd, UINT uID,	
		BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CCreateContext * pContext = NULL);
	virtual ~CGridCellFileDialog();

	int SelectFolder(LPCTSTR lpszTitle = NULL, LPCTSTR lpszStartPath = NULL, UINT ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS, CWnd* pParentWnd = NULL);
	POSITION GetStartPosition() const;
	CString GetNextPathName(POSITION& pos) const;
	CString GetPathName() const;
	CString GetFileName() const;
	CString GetFileTitle() const;
	CString GetFileExt() const;
	CString GetFileDir() const;
	CString GetFileDrive() const;
	CString GetSelectedFolder() const;
	virtual int DoModal();
	void SetDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
	void EndEdit();

protected:
	static int __stdcall BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

	OPENFILENAME m_ofn;
	//GC_OPENFILENAMEEX	m_ofn;
	BOOL m_bOpenFileDialog;
	TCHAR m_szFile[MAX_PATH];
	TCHAR m_szFileTitle[MAX_PATH];
	TCHAR m_szSelectedFolder[MAX_PATH];
	CString m_strFilter;
    int     m_nRow;
    int     m_nColumn;
};

#endif // !defined(AFX_GRIDCELLFILEDIALOG_H__CAD95AAB_BE38_42DA_96CA_A59BF0C629B4__INCLUDED_)
