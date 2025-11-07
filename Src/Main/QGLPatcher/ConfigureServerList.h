#pragma once
#ifndef _CONFIGURESERVERLIST_H
#define _CONFIGURESERVERLIST_H

#include "common.h"

/*
struct WebAccessData
{
	String	ip;
	UInt	port;
	UInt	type; // 0:invalid, 1:http, 2:ftp
	String	path;
	String	file;
};
*/

struct ProxyIP
{
	union
	{
		UInt32 fullip;
		struct
		{
			UInt8 pos1;
			UInt8 pos2;
			UInt8 pos3;
			UInt8 pos4;
		};
	};
	UInt16 port;
};

//typedef StlVector<WebAccessData, ManagedAllocator<WebAccessData> >		WebAccessDataList;
typedef StlVector<ProxyIP, ManagedAllocator<ProxyIP> >					ProxyIPList;

struct ServerInfo
{
	UInt32 id;
	String name;
	UInt32Vector zoneList;
	UInt32 status;
	ProxyIPList proxyList;
	UInt32 rank;
	UInt32 zoneId;
	String fullName_cache;
	StringVector downloadLinkList;

	ServerInfo()
		: id(0)
		, status(0)
		, rank(0)
		, zoneId(0)
	{
		zoneList.clear();
		proxyList.clear();
		downloadLinkList.clear();
	};

	void swap(ServerInfo& info)
	{
		ServerInfo tmp = info;
		info = *this;
		*this = tmp;
	};
};

typedef StlVector<ServerInfo, ManagedAllocator<ServerInfo> >		ServerInfoList;

class CConfigureServerList
{
public:
	CConfigureServerList();
	~CConfigureServerList();

// initialization
public:
	BOOL LoadConfigure();
	BOOL ReloadConfigure();
	VOID FreeConfigure();

public:
	ServerInfoList* GetServerInfoList()	{return &m_serverInfoList;};

private:
	VOID PrintDebug();
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	BOOL CheckFileValid();
	VOID ReadServerInfo(ServerInfo& info, LPCTSTR family);

// Attributes
private:
	ServerInfoList m_serverInfoList;
};

#endif // _CONFIGURESERVERLIST_H
