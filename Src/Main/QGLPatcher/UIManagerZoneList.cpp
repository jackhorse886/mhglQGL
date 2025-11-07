#include "UIManagerZoneList.h"

CUIManagerZoneList::CUIManagerZoneList()
{
	m_isPageChanged		= FALSE;
	m_setZoneId			= -1;
	m_curPage			= 0;
	m_totalSlot			= 0;
}

CUIManagerZoneList::~CUIManagerZoneList()
{
	FreeContent();
}

BOOL CUIManagerZoneList::LoadContent(CONST ZoneMap* zonemap)
{
	if( zonemap==NULL )	return FALSE;

	m_zoneMap = *zonemap;
	return TRUE;
}

BOOL CUIManagerZoneList::ReloadContent(CONST ZoneMap* zonemap)
{
	FreeContent();
	return LoadContent(zonemap);
}

VOID CUIManagerZoneList::FreeContent()
{
	m_curPage		= 0;
	m_setZoneId		= 0;
	SetPageChanged();
	m_zoneMap.clear();
}

BOOL CUIManagerZoneList::SetSelect(Zone& zone)
{
	for( ZoneMap::iterator itr = m_zoneMap.begin(); itr != m_zoneMap.end(); ++itr )
	{
		Zone& tempZone = itr->second;
		if( !_tcscmp(zone.GetName().c_str(), tempZone.GetName().c_str()) )
		{
			m_setZoneId = itr->first;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CUIManagerZoneList::SetSelect(UInt zoneId)
{
	ZoneMap::iterator itr = m_zoneMap.find(zoneId);
	if( itr != m_zoneMap.end() )
	{
		m_setZoneId = zoneId;
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerZoneList::SetPrevPage()
{
	if( HasPrevPage() )
	{
		--m_curPage;
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerZoneList::SetNextPage()
{
	if( HasNextPage() )
	{
		++m_curPage;
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerZoneList::JumpSelectPage()
{
	if( GetPageSlot()==0 || m_zoneMap.size()==0 )	return FALSE;

	BOOL bFound = FALSE;
	UInt counter = 0;
	for( ZoneMap::iterator itr = m_zoneMap.begin(); itr != m_zoneMap.end(); ++itr )
	{
		if( itr->first==m_setZoneId )
		{
			bFound = TRUE;
			break;
		}
		++counter;
	}
	
	if( !bFound )	return FALSE;

	UInt targetPageNo = counter / GetPageSlot();
	if( HasPage(targetPageNo) )
	{
		m_curPage = targetPageNo;
		SetPageChanged();
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerZoneList::HasPage(UInt32 pageno)
{
	if( GetPageSlot()==0 || m_zoneMap.size()==0 )	return FALSE;

	if( pageno < ((m_zoneMap.size()-1) / GetPageSlot())+1 )
		return TRUE;
	return FALSE;
}

BOOL CUIManagerZoneList::HasNextPage()
{
	return HasPage(m_curPage + 1);
}

BOOL CUIManagerZoneList::HasPrevPage()
{
	if( m_curPage==0 ) return FALSE;
	return TRUE;
}

BOOL CUIManagerZoneList::GetCurPageContent(ZoneMap& outZoneMap)
{
	if( GetPageSlot()==0 || m_zoneMap.size()==0 )	return FALSE;

	UInt counter = 0;
	for( ZoneMap::iterator itr = m_zoneMap.begin(); itr != m_zoneMap.end(); ++itr )
	{
		if( counter >= m_curPage * GetPageSlot() && counter < (m_curPage+1) * GetPageSlot() )
			outZoneMap.insert(std::make_pair(itr->first, itr->second));
		++counter;
	}
	return TRUE;
}

UInt32 CUIManagerZoneList::GetPageSlot()
{
	return m_totalSlot;
}

//BOOL CUIManagerZoneList::GetSelectedZone(UInt32 zoneId, Zone& outZone)
//{
//	ZoneMap::iterator itr = m_zoneMap.find(zoneId);
//	if( itr != m_zoneMap.end() )
//	{
//		outZone = itr->second;
//		return TRUE;
//	}
//	return FALSE;
//}
//
bool CUIManagerZoneList::Notify(UInt zoneId)
{
	return false;
}