#include "UIManagerServerList.h"

CUIManagerServerList::CUIManagerServerList()
{
	m_isPageChanged		= FALSE;
	m_setServerId		= -1;
	m_curPage			= 0;
	m_totalSlot			= 0;
}

CUIManagerServerList::~CUIManagerServerList()
{
	FreeContent();
}

BOOL CUIManagerServerList::LoadContent(CONST ServerInfoList* serverlist)
{
	if( serverlist==NULL )	return FALSE;

	m_srvList = *serverlist;
	return TRUE;
}

BOOL CUIManagerServerList::ReloadContent(CONST ServerInfoList* serverlist)
{
	FreeContent();
	return LoadContent(serverlist);
}

VOID CUIManagerServerList::FreeContent()
{
	m_curPage		= 0;
	m_setServerId	= -1;
	SetPageChanged();
	m_srvList.clear();
}

BOOL CUIManagerServerList::SetSelect(UInt serverId)
{
	if( serverId >= m_srvList.size() ) return FALSE;

	m_setServerId = serverId;
	return TRUE;
}

BOOL CUIManagerServerList::SetPrevPage()
{
	if( HasPrevPage() )
	{
		--m_curPage;
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerServerList::SetNextPage()
{
	if( HasNextPage() )
	{
		++m_curPage;
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerServerList::JumpSelectPage()
{
	if( GetPageSlot()==0 || m_srvList.size()==0 )	return FALSE;

	UInt targetPageNo = m_setServerId / GetPageSlot();
	if( HasPage(targetPageNo) )
	{
		m_curPage = targetPageNo;
		SetPageChanged();
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerServerList::HasPage(UInt32 pageno)
{
	if( GetPageSlot()==0 || m_srvList.size()==0 )	return FALSE;

	if( pageno < ((m_srvList.size()-1) / GetPageSlot())+1 )
		return TRUE;
	return FALSE;
}

BOOL CUIManagerServerList::HasNextPage()
{
	return HasPage(m_curPage + 1);
}

BOOL CUIManagerServerList::HasPrevPage()
{
	if( m_curPage==0 ) return FALSE;
	return TRUE;
}

BOOL CUIManagerServerList::GetServerInfo(ServerInfo& outServerInfo, UInt serverid)
{
	if( GetPageSlot()==0 || m_srvList.size()==0 )	return FALSE;

	if( serverid < (UInt)m_srvList.size() )
	{
		outServerInfo = m_srvList[serverid];
		return TRUE;
	}
	return FALSE;
}

BOOL CUIManagerServerList::GetServerInfoBySlot(ServerInfo& outServerInfo, UInt slotId)
{
	if( GetPageSlot()==0 || m_srvList.size()==0 )	return FALSE;

	UInt pos = m_curPage * GetPageSlot() + slotId;
	if( pos < (UInt)m_srvList.size() )
	{
		outServerInfo = m_srvList[pos];
		return TRUE;
	}
	return FALSE;
}

UInt32 CUIManagerServerList::GetPageSlot()
{
	return m_totalSlot;
}

BOOL CUIManagerServerList::GetCurPageContent(ServerInfoList& outServerLst)
{
	if( GetPageSlot()==0 || m_srvList.size()==0 )	return FALSE;

	UInt counter = 0;
	outServerLst.clear();
	for( Index i = C_INDEX(0); i < m_srvList.size(); ++i )
	{
		if( i >= m_curPage * GetPageSlot() && i < (m_curPage+1) * GetPageSlot() )
			outServerLst.push_back(m_srvList[i]);
	}
	return TRUE;
}

bool CUIManagerServerList::Notify(UInt serverId)
{
	return false;
}