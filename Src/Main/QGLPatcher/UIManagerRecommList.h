#pragma once
#ifndef _UIMANAGERRECOMMLIST_H
#define _UIMANAGERRECOMMLIST_H

#include "common.h"
#include "UIManagerServerList.h"

class CConfigureServerList;
class CConfigureZoneList;
class CUIManagerZoneList;
class CUIManagerServerList;

class CUIManagerRecommList
{
public:
	CUIManagerRecommList();
	~CUIManagerRecommList();

public:
	void SetTotalSlot(UInt32 totalSlot)			{m_totalSlot = totalSlot;};
	UInt32 GetTotalSlot()						{return m_totalSlot;};
	bool LoadContent(CConfigureServerList* serverList, CConfigureZoneList* zoneList);
	bool SetListener(CUIManagerZoneList* uiZoneList)	{ m_uiZoneList = uiZoneList; };
	bool SetListener(CUIManagerServerList* uiServerList) { m_uiServerList = uiServerList; };
	bool NotifyListener(UInt zoneId, UInt serverId);
	void ResetContent(); 

public:
	bool GetServerInfo(ServerInfo& outServerInfo, UInt serverid);
	bool GetServerInfoBySlot(ServerInfo& outServerInfo, UInt slotId);
	bool GetPageContent(ServerInfoList& outServerLst);

public:
	bool SetSelect(UInt serverId);
	bool SetSelectSlot(UInt slotId);

// Attributes
private:
	ServerInfoList			m_recommList;
	CUIManagerZoneList*		m_uiZoneList;
	CUIManagerServerList*	m_uiServerList;
	UInt					m_totalSlot;
};

#endif // _UIMANAGERRECOMMLIST_H
