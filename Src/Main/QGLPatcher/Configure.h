#pragma once
#ifndef _CONFIGURE_H
#define _CONFIGURE_H

#include "common.h"

enum ZoneType
{
	ZONETYPE_ZONE = 0,
	ZONETYPE_SERVER,
};

struct ProgressInfo
{
	String name;
	UInt w;
	UInt h;
	UInt x;
	UInt y;
};

struct LinkInfo
{
	String name;
	String url;
	String btnImage1;
	String btnImage2;
	String btnImage3;
	UInt w;
	UInt h;
	UInt x;
	UInt y;
};

struct FunctionInfo
{
	String name;
	String btnImage1;
	String btnImage2;
	String btnImage3;
	String btnImage4;
	UInt w;
	UInt h;
	UInt x;
	UInt y;
};

struct URLInfo
{
	String name;
	UInt w;
	UInt h;
	UInt x;
	UInt y;
	String url;
};

struct ServerButtonTable
{
	String btnImage1[4];	// normal:normal/busy/v.busy/disabled
	String btnImage2[4];	// on
	String btnImage3[4];	// down
	String btnImage4[4];	// disabled
	UInt w;
	UInt h;
};

struct ZoneButtonTable
{
	String btnImage1;	// normal
	String btnImage2;	// on
	String btnImage3;	// down
	String btnImage4;	// disabled
	UInt w;
	UInt h;
};

struct ZoneInfo
{
	ZoneType zoneType;
	UInt32Vector x;
	UInt32Vector y;
};

struct GraphInfo
{
	String name;
	UInt32 x;
	UInt32 y;
	UInt32 w;
	UInt32 h;
};

typedef StlVector<LinkInfo, ManagedAllocator<LinkInfo> > LinkInfoVector;

class CConfigure
{
public:
	CConfigure();
	~CConfigure();

public:
	BOOL LoadConfigure();

private:
	VOID PrintDebug();
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID ReadFunctionInfo(FunctionInfo& info, LPCTSTR family, LPCTSTR groupName);
	VOID ReadURLInfo(URLInfo& info, LPCTSTR family, LPCTSTR groupName);
	VOID ReadProgressInfo(ProgressInfo& info, LPCTSTR family, LPCTSTR groupName);
	VOID ReadGraphInfo(GraphInfo& info, LPCTSTR family, LPCTSTR groupName);

// Attributes
public:
	// init page (before page 1)
	GraphInfo m_initBg;
	// news page (page 1)
	UInt m_linkCount;
	LinkInfoVector m_linkInfo;
	FunctionInfo m_newsStart;
	FunctionInfo m_newsExit;
	URLInfo m_newsExplorer;
	// server page (page 2)
	ServerButtonTable m_serverButtonTable;
	ZoneButtonTable m_zoneButtonTable;
	ZoneInfo m_zoneinfo1;
	ZoneInfo m_zoneinfo2;
	ZoneInfo m_zoneinfo3;
	FunctionInfo m_serverPrev;
	FunctionInfo m_serverDefault;
	FunctionInfo m_serverNext;
	FunctionInfo m_serverZoneUp;
	FunctionInfo m_serverZoneDown;
	FunctionInfo m_serverServerUp;
	FunctionInfo m_serverServerDown;
	GraphInfo m_serverDefaultContent;
	// patch page (page 3)
	URLInfo m_patchExplorer;
	URLInfo m_eventExplorer;
	FunctionInfo m_patchStart;
	FunctionInfo m_patchExit;
	LinkInfo m_patchLink;
	ProgressInfo m_patchProgress1;
	ProgressInfo m_patchProgress2;
	GraphInfo m_patchCurVer;
	GraphInfo m_patchTarVer;
	GraphInfo m_patchYear;
	GraphInfo m_patchMonth;
	GraphInfo m_patchDay;
	GraphInfo m_patchWeekday;
	GraphInfo m_progressText1;
	GraphInfo m_progressText2;
};

#endif // _CONFIGURE_H
