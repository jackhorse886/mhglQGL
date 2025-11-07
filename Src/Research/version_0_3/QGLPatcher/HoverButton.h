#if !defined(AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_)
#define AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "MemDC.h"
#include "GDFPackager\GDFcommon.h"

// HoverButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHoverButton by Niek Albers
// Thanks to some people for the tooltip.
// A cool CBitmapButton derived class with 3 states,
// Up/Down/Hover.
class CHoverButton : public CBitmapButton
{
	DECLARE_DYNAMIC(CHoverButton);

	// Construction
public:
	CHoverButton();
	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
	void SetToolTipText(CString* spText, BOOL bActivate = TRUE);
	void SetToolTipText(int nId, BOOL bActivate = TRUE);

// Attributes
protected:
	void ActivateTooltip(BOOL bActivate = TRUE);
	BOOL m_bHover;						// indicates if mouse is over the button
	CSize m_ButtonSize;					// width and height of the button
	CBitmap* mybitmap;
	CBitmap* mybitmaphover;
	CBitmap* mybitmapdown;
	BOOL m_bTracking;
	UInt m_lockstatus;						// 0: normal; 1: Lock on Normal; 2: Lock on Over; 3: Lock on Down
	String m_text;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoverButton)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadBitmap(UINT bitmapid);
	BOOL LoadBitmap(String bmp_normal, String bmp_hover, String bmp_down);
	BOOL ReplaceBitmap(String bmp_normal, String bmp_hover, String bmp_down);
	virtual ~CHoverButton();

	VOID LockStatus(UInt status) // 0: normal; 1: Lock on Normal; 2: Lock on Over; 3: Lock on Down
	{
		if(status>=0 && status<4)
			m_lockstatus = status;
	};
	UInt GetLockStatus() {return m_lockstatus;};
	VOID UnlockStatus() {m_lockstatus = 0;};
	VOID SetText(String& text) {m_text = text;};
	String GetText() {return m_text;};

	// Generated message map functions
protected:
	CToolTipCtrl m_ToolTip;
	void InitToolTip();
	//{{AFX_MSG(CHoverButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	BOOL LoadFileBitmap(CBitmap* pBmp, LPCTSTR szFilename);
	void DrawBitmap(CMemDC* pDC, int xStart, int yStart, int wWidth, int wHeight, CDC* pTmpDC, int xSource, int ySource);
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_)
