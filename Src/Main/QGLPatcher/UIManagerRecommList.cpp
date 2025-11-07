#include "UIManagerRecommList.h"

#include "ConfigureServerList.h"
#include "ConfigureZoneList.h"
#include "UIManagerServerList.h"
#include "UIManagerZoneList.h"

CUIManagerRecommList::CUIManagerRecommList()
{
	m_uiZoneList = NULL;
	m_uiServerList = NULL;
	m_recommList.clear();
}

CUIManagerRecommList::~CUIManagerRecommList()
{
	m_recommList.clear();
}

bool CUIManagerRecommList::LoadContent(CConfigureServerList* serverList, CConfigureZoneList* zoneList)
{
	if( serverList==NULL )	return false;

	ServerInfoList sortList = *serverList->GetServerInfoList();
	// sort server list
	for( Index i = C_INDEX(0); i < sortList.size() - 1; ++i )
	{
		for( Index j = i+1; j < sortList.size(); ++j )
		{
			if( (sortList[i].rank < sortList[j].rank) ||
				(sortList[i].rank==sortList[j].rank && sortList[i].id < sortList[j].id)
				)
			{
				sortList[i].swap(sortList[j]);
			}
		}
	}
	// prepare recommendation list
	for( Index i = C_INDEX(0); i < m_totalSlot && i < sortList.size(); ++i )
	{
		ServerInfo& info = sortList[i];
		if( info.rank )
			m_recommList.push_back(info);
	}
	// zone-id cache assignment
	for( Index i = C_INDEX(0); i < m_recommList.size(); ++i )
	{
		ServerInfo& info = m_recommList[i];
		if( info.zoneList.size() )
		{
			UInt32 zoneid = info.zoneList[0];
			ZoneMap::iterator itr = zoneList->GetZoneMap()->find(zoneid);
			if( itr != zoneList->GetZoneMap()->end() )
			{
				info.zoneId = itr->first;
				info.fullName_cache.Format(_T("%s-%s"), itr->second.GetName().c_str(), info.name.c_str());
			}else
			{
				info.zoneId = -1;
				info.fullName_cache = info.name;
			}
		}else
		{
			info.zoneId = -1;
			info.fullName_cache = info.name;
		}
	}
	return true;
}

void CUIManagerRecommList::ResetContent()
{
	m_recommList.clear();
}

bool CUIManagerRecommList::SetSelect(UInt serverId)
{
	NotifyListener(0, serverId);
	return false;
}

bool CUIManagerRecommList::GetServerInfo(ServerInfo& outServerInfo, UInt serverid)
{
	for( Index i = C_INDEX(0); i < m_recommList.size(); ++i )
	{
		if( m_recommList[i].id==serverid )
		{
			outServerInfo = m_recommList[i];
			return true;
		}
	}
	return false;
}

bool CUIManagerRecommList::GetServerInfoBySlot(ServerInfo& outServerInfo, UInt slotId)
{
	if( slotId >= m_recommList.size() )	return false;

	outServerInfo = m_recommList[slotId];
	return true;
}

bool CUIManagerRecommList::GetPageContent(ServerInfoList& outServerLst)
{
	outServerLst = m_recommList;
	return true;
}

bool CUIManagerRecommList::SetSelectSlot(UInt slotId)
{
	ServerInfo info;
	bool isSuccess = GetServerInfoBySlot(info, slotId);
	if( isSuccess )
		SetSelect(info.id);
	
	return isSuccess;
}

bool CUIManagerRecommList::NotifyListener(UInt zoneId, UInt serverId)
{
	bool isSuccess = true;
	if( m_uiZoneList )
		isSuccess = m_uiZoneList->Notify(zoneId) && isSuccess;
	if( m_uiServerList )
		isSuccess = m_uiServerList->Notify(serverId) && isSuccess;
	return isSuccess;
}
