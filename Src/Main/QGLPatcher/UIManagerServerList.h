#pragma once
#ifndef _UIMANAGERSERVERLIST_H
#define _UIMANAGERSERVERLIST_H

#include "common.h"
#include "ConfigureServerList.h"

class CUIManagerServerList
{
public:
	CUIManagerServerList();
	~CUIManagerServerList();

public:
	VOID SetTotalSlot(UInt32 totalSlot)			{m_totalSlot = totalSlot;};
	BOOL LoadContent(CONST ServerInfoList* serverlist);
	BOOL ReloadContent(CONST ServerInfoList* serverlist);
	VOID FreeContent();
	bool Notify(UInt serverId);

public:
	BOOL SetSelect(UInt serverId);

public:
	BOOL SetPrevPage();
	BOOL SetNextPage();
	BOOL JumpSelectPage();
	BOOL HasPrevPage();
	BOOL HasNextPage();
	BOOL HasPage(UInt32 pageno);

public:
	BOOL IsPageChanged()	{return m_isPageChanged;};

private:
	VOID SetPageChanged()	{m_isPageChanged = TRUE;};
	VOID ResetPageChanged()	{m_isPageChanged = FALSE;};

public:
	UInt32 GetPageSlot();

public:
	BOOL GetServerInfo(ServerInfo& outServerInfo, UInt serverid);
	BOOL GetServerInfoBySlot(ServerInfo& outServerInfo, UInt slotId);
	BOOL GetCurPageContent(ServerInfoList& outServerLst);

// Attributes
private:
	ServerInfoList		m_srvList;
	BOOL		m_isPageChanged;
	UInt32		m_curPage;		// zero-based
	UInt32		m_setServerId;
	UInt32		m_totalSlot;
};

#endif // _UIMANAGERSERVERLIST_H
