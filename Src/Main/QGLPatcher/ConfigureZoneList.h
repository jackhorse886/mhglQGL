#pragma once
#ifndef _CONFIGUREZONELIST_H
#define _CONFIGUREZONELIST_H

#include "common.h"
#include "ConfigureServerList.h"

typedef StlVector<ServerInfo*, ManagedAllocator<ServerInfo*> >		ServerInfoPtrList;

class Zone
{
public:
	Zone(const String& name)	{m_name = name;};
	~Zone()						{m_serverList.clear();};

public:
	BOOL RegisterServer(ServerInfo* info);
	VOID UnregisterServer(CONST ServerInfo* info);
	VOID UnregisterAllServer();
	UInt32 GetSize()	{return (UInt32)m_serverList.size();};
	String GetName()	{return m_name;};
	VOID GetServerList(ServerInfoPtrList& serverInfoList)	{serverInfoList = m_serverList;};
	VOID GetServerList(ServerInfoList& serverInfoList);

private:
	ServerInfoPtrList m_serverList;
	String m_name;
};

typedef StlMap<UInt32, Zone, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, Zone> > > ZoneMap;

class CConfigureZoneList
{
public:
	CConfigureZoneList();
	~CConfigureZoneList();

// initialization
public:
	BOOL LoadConfigure();
	BOOL ReloadConfigure();
	VOID FreeConfigure();

public:
	Zone* GetZone(UInt32 zoneId);
	ZoneMap* GetZoneMap()	{return &m_zoneMap;};

private:
	VOID PrintDebug();
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	BOOL CheckFileValid();

// Attributes
private:
	ZoneMap m_zoneMap;
};

#endif // _CONFIGUREZONELIST_H
