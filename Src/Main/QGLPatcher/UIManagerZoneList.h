#pragma once
#ifndef _UIMANAGERZONELIST_H
#define _UIMANAGERZONELIST_H

#include "common.h"
#include "ConfigureZoneList.h"

class CUIManagerZoneList
{
public:
	CUIManagerZoneList();
	~CUIManagerZoneList();

public:
	VOID SetTotalSlot(UInt32 totalSlot)			{m_totalSlot = totalSlot;};
	UInt32 GetTotalSlot()						{return m_totalSlot;};
	BOOL LoadContent(CONST ZoneMap* zonemap);
	BOOL ReloadContent(CONST ZoneMap* zonemap);
	VOID FreeContent();
	//BOOL GetSelectedZone(UInt32 zoneId, Zone& outZone);
	bool Notify(UInt zoneId);

public:
	BOOL SetSelect(UInt zoneId);
	BOOL SetSelect(Zone& zone);

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
	BOOL GetCurPageContent(ZoneMap& outZoneMap);

// Attributes
private:
	ZoneMap		m_zoneMap;
	BOOL		m_isPageChanged;
	UInt32		m_curPage;		// zero-based
	UInt32		m_setZoneId;
	UInt32		m_totalSlot;
};

#endif // _UIMANAGERZONELIST_H
