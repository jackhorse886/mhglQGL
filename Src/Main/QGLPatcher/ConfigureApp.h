#pragma once
#ifndef _CONFIGUREAPP_H
#define _CONFIGUREAPP_H

#include "common.h"

class CConfigureApp
{
public:
	CConfigureApp();
	~CConfigureApp();

// initialization
public:
	BOOL LoadConfigure();

// called to update
public:
	BOOL UpdatePatcherVersion();
	BOOL SaveLastServer(LPCTSTR lastsrv);
	BOOL SaveLastPort(UInt32 lastport);
	BOOL SaveLastModifiedDate(LPCTSTR lastmodifieddate);
	BOOL SaveLastSelection(UInt32 lastServerID, PCSTRING lastServerName, PCSTRING lastZoneName);

private:
	VOID PrintDebug();
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureCRC(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);

// Attributes
public:
	UInt32 m_patcherversion;
	String m_lastmodifieddate;
	String m_lastservername;
	String m_lastzonename;
	String m_startgame;
	String m_startpatcher;
	String m_startrestart;
	UInt32 m_debug_ui;
	UInt32 m_debug_exportblock;
	UInt32 m_patcherversioncrc;
};

#endif // _CONFIGUREAPP_H
