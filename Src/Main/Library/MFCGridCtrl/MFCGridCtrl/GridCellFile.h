// GridCellFile.h: interface for the CGridCellFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLFILE_H__05C19A66_1EB0_47BF_A54C_F943FCBF8E01__INCLUDED_)
#define AFX_GRIDCELLFILE_H__05C19A66_1EB0_47BF_A54C_F943FCBF8E01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCell.h"
#include "GridCtrl.h"
#include "GridCellFileDialog.h"

#define OT_FILE_SHOWFULLPATH				0x00040000L				// Show full paths in sample
#define OT_FILE_SHOWFILENAME				0x00000400L				// Show file name in sample
#define OT_FILE_SHOWFILETITLE				0x00000200L				// Show file title in sample
#define OT_FILE_SHOWFILEEXT					0x00000100L				// Show file extention in sample
#define OT_FILE_SHOWFILEDIR					0x00000800L				// Show file directory in sample
#define OT_FILE_SHOWFILEDRIVE				0x00001000L				// Show file drive in sample
#define OT_FILE_OPENDIALOG					0x00020000L				// Open file dialog
#define OT_FILE_SELECTDIALOG				0x00008000L				// Select folder

class CGridCellFile : public CGridCell  
{
	friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellFile)
public:
	CGridCellFile();
	virtual ~CGridCellFile();

// editing cells
public:
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual void EndEdit();

	CString GetSelectedFolder();
	CString GetFileDrive();
	CString GetFileDir();
	CString GetFileExt();
	CString GetFileTitle();
	CString GetFileName();
	CString GetPathName();
	CString GetNextPathName(POSITION& pos);
	POSITION GetStartPosition();
	CString GetDialogTitle();
	void SetDialogTitle(CString strTitle);
	CString GetFilter();
	void SetFilter(CString strFilter);
	CString GetDefaultExtention();
	void SertDefaultExtention(CString strExt);
	DWORD GetDialogFlags();
	void SetDialogFlags(DWORD dwFlags);
	void AddFileName(CString strFile);
	BOOL CreateFileItem(CString strFile, CString strDefExt, CString strFilter, DWORD dwOptions, DWORD dwDlgFlags);

protected:
	CString GetFileDrive(CString strFile);
	CString GetFileDirectory(CString strFile);
	CString GetFileExtention(CString strFile);
	BOOL GetOption(DWORD dwOption);
	void SetOption(DWORD dwOption, BOOL bSet);	
	BOOL GetDialogFlag(DWORD dwOption);
	void SetDialogFlag(DWORD dwOption, BOOL bSet);	
	DWORD m_dwOptions;
	DWORD m_dwDlgFlags;
	long m_lDefaultHeight;
	CGridCellFileDialog * m_pdlgFile;
	CStringArray m_strFileNames;
	CString m_strDefExt;
	CString m_strFilter;
	CString m_strDlgTitle;
	int m_nRow;
	int m_nColumn;
};

#endif // !defined(AFX_GRIDCELLFILE_H__05C19A66_1EB0_47BF_A54C_F943FCBF8E01__INCLUDED_)
