// HoverButton.cpp : implementation file
//

#include "stdafx.h"
#include "HoverButton.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverButton

CHoverButton::CHoverButton()
{
	m_bHover		= FALSE;
	m_bTracking		= FALSE;
	m_lockstatus	= 0;

	mybitmap		= NULL;
	mybitmaphover	= NULL;
	mybitmapdown	= NULL;
}

CHoverButton::~CHoverButton()
{
}

IMPLEMENT_DYNAMIC(CHoverButton, CBitmapButton)

BEGIN_MESSAGE_MAP(CHoverButton, CBitmapButton)
	//{{AFX_MSG_MAP(CHoverButton)
	ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
 //	CHoverButton message handlers
BOOL CHoverButton::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	RECT brc;
	GetClientRect(&brc);
	return CBitmapButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

		
void CHoverButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	//	TODO: Add your message handler code here and/or call default

	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CBitmapButton::OnMouseMove(nFlags, point);
}

BOOL CHoverButton::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	RECT brc;
	GetClientRect(&brc);

	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);		
	return CButton::PreTranslateMessage(pMsg);
}

// Set the tooltip with a string resource
void CHoverButton::SetToolTipText(int nId, BOOL bActivate)
{
	CString sText;

	// load string resource
	sText.LoadString(nId);
	// If string resource is not empty
	if (sText.IsEmpty() == FALSE) SetToolTipText(&sText, bActivate);

}

// Set the tooltip with a CString
void CHoverButton::SetToolTipText(CString *spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, (LPCTSTR)*spText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText((LPCTSTR)*spText, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CHoverButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

// Activate the tooltip
void CHoverButton::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
} // End of EnableTooltip






void CHoverButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	if( IsWindowVisible() )
	{
		RECT brc;
		GetClientRect(&brc);
		CDC *mydc=CDC::FromHandle(lpDrawItemStruct->hDC);

		CDC * pMemDC = new CDC;
		pMemDC -> CreateCompatibleDC(mydc);

		CBitmap * pOldBitmap;

		CPoint point(0,0);	

		switch( m_lockstatus )
		{
		case 0:
			if(lpDrawItemStruct->itemState & ODS_SELECTED)
			{
				pOldBitmap = pMemDC -> SelectObject(mybitmapdown);
				int width = brc.right-brc.left;
				int height = brc.bottom - brc.top;

				mydc->StretchBlt( 0, 0, width, height, pMemDC, 0, 0, m_ButtonSize.cx, m_ButtonSize.cy, SRCCOPY);
				//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx,0,SRCCOPY);
			}
			else
			{
				if(m_bHover)
				{
					pOldBitmap = pMemDC -> SelectObject(mybitmaphover);
					mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY);
				}else
				{
					pOldBitmap = pMemDC -> SelectObject(mybitmap);
					mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY);
				}	
			}
			break;
		case 1:
			pOldBitmap = pMemDC -> SelectObject(mybitmap);
			mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY);
			break;
		case 2:
			pOldBitmap = pMemDC -> SelectObject(mybitmaphover);
			mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY);
			break;
		case 3:
			pOldBitmap = pMemDC -> SelectObject(mybitmapdown);
			mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY);
			break;
		}

		if( !m_text.IsEmpty() )
		{
			RECT rect;
			rect.left	= 0;
			rect.top	= 10;
			rect.right	= m_ButtonSize.cx;
			rect.bottom	= m_ButtonSize.cy;
			mydc->SetBkMode(TRANSPARENT);
			if( m_lockstatus==3 || (lpDrawItemStruct->itemState & ODS_SELECTED) )
				mydc->SetTextColor(RGB(255,255,255));
			else
				mydc->SetTextColor(RGB(0,0,0));
			mydc->DrawText(m_text.c_str(), &rect, DT_CENTER | DT_NOCLIP | DT_SINGLELINE);
		}

		// clean up
		pMemDC -> SelectObject(pOldBitmap);
		delete pMemDC;
	}
}

// Load a bitmap from the resources in the button, the bitmap has to have 3 buttonsstates next to each other: Up/Down/Hover
BOOL CHoverButton::LoadBitmap(UINT bitmapid)
{
	mybitmap->Attach(::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(bitmapid), IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS));
	BITMAP	bitmapbits;
	mybitmap->GetBitmap(&bitmapbits);
	m_ButtonSize.cy=bitmapbits.bmHeight;
	m_ButtonSize.cx=bitmapbits.bmWidth/3;
	SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE   |SWP_NOOWNERZORDER   );
	return TRUE;
}

BOOL CHoverButton::LoadBitmap(String bmp_normal, String bmp_hover, String bmp_down)
{
	BOOL succeeded	= TRUE;
	if( mybitmap )
	{
		delete mybitmap;
		mybitmap = NULL;
	}
	if( mybitmaphover )
	{
		delete mybitmaphover;
		mybitmaphover = NULL;
	}
	if( mybitmapdown )
	{
		delete mybitmapdown;
		mybitmapdown = NULL;
	}
	mybitmap		= new CBitmap();
	mybitmaphover	= new CBitmap();
	mybitmapdown	= new CBitmap();
	if (!LoadFileBitmap(mybitmap, bmp_normal.c_str()))
	{
		delete mybitmap;
		mybitmap	= NULL;
		succeeded	= FALSE;
	}
	if (!LoadFileBitmap(mybitmaphover, bmp_hover.c_str()))
	{
		delete mybitmaphover;
		mybitmaphover	= NULL;
		succeeded		= FALSE;
	}
	if (!LoadFileBitmap(mybitmapdown, bmp_down.c_str()))
	{
		delete mybitmapdown;
		mybitmapdown	= NULL;
		succeeded		= FALSE;
	}
	if (succeeded)
	{
		BITMAP bmpInfo;
		mybitmap->GetBitmap(&bmpInfo);
		m_ButtonSize.cx=bmpInfo.bmWidth;
		m_ButtonSize.cy=bmpInfo.bmHeight;
		SetWindowPos( NULL, 0, 0, m_ButtonSize.cx, m_ButtonSize.cy, SWP_NOMOVE|SWP_NOOWNERZORDER );
	}
	return succeeded;
}

BOOL CHoverButton::ReplaceBitmap(String bmp_normal, String bmp_hover, String bmp_down)
{
	BOOL succeeded = TRUE;
	if( mybitmap )
	{
		delete mybitmap;
		mybitmap = NULL;
	}
	if( mybitmaphover )
	{
		delete mybitmaphover;
		mybitmaphover = NULL;
	}
	if( mybitmapdown )
	{
		delete mybitmapdown;
		mybitmapdown = NULL;
	}
	mybitmap		= new CBitmap();
	mybitmaphover	= new CBitmap();
	mybitmapdown	= new CBitmap();
	if (!LoadFileBitmap(mybitmap, bmp_normal.c_str()))
	{
		delete mybitmap;
		mybitmap = NULL;
		succeeded = FALSE;
	}
	if (!LoadFileBitmap(mybitmaphover, bmp_hover.c_str()))
	{
		delete mybitmaphover;
		mybitmaphover = NULL;
		succeeded = FALSE;
	}
	if (!LoadFileBitmap(mybitmapdown, bmp_down.c_str()))
	{
		delete mybitmapdown;
		mybitmapdown = NULL;
		succeeded = FALSE;
	}
	if (succeeded)
	{
		BITMAP bmpInfo;
		mybitmap->GetBitmap(&bmpInfo);
		m_ButtonSize.cx=bmpInfo.bmWidth;
		m_ButtonSize.cy=bmpInfo.bmHeight;
		SetWindowPos( NULL, 0, 0, m_ButtonSize.cx, m_ButtonSize.cy, SWP_NOMOVE|SWP_NOOWNERZORDER );
	}
	return succeeded;
}

LRESULT CHoverButton::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	// TODO: Add your message handler code here and/or call default
	m_bHover=TRUE;
	Invalidate();
	return 0;
}


LRESULT CHoverButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHover=FALSE;
	Invalidate();
	return 0;
}

BOOL CHoverButton::LoadFileBitmap(CBitmap* pBmp, LPCTSTR szFilename)
{
	pBmp->DeleteObject();
	return pBmp->Attach(LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
}

void CHoverButton::DrawBitmap(CMemDC* pDC, int xStart, int yStart, int wWidth, int wHeight, CDC* pTmpDC, int xSource, int ySource)
{
	CDC hdcMem;
	hdcMem.CreateCompatibleDC(NULL);
	pDC->BitBlt(xStart, yStart, wWidth, wHeight, pTmpDC, xSource, ySource,SRCCOPY);
	hdcMem.DeleteDC();
}

void CHoverButton::OnDestroy()
{
	if( mybitmap )
	{
		mybitmap->DeleteObject();
		delete mybitmap;
	}
	if( mybitmaphover )
	{
		mybitmaphover->DeleteObject();
		delete mybitmaphover;
	}
	if( mybitmapdown )
	{
		mybitmapdown->DeleteObject();
		delete mybitmapdown;
	}
	CButton::OnDestroy();
}

VOID LockStatus(UInt status)
{
}

VOID UnlockStatus()
{
}
