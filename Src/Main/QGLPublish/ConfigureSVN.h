#pragma once
#ifndef _CONFIGURESVN_H
#define _CONFIGURESVN_H

#include "common.h"

#define CONFIGURATION_SVN_INI_FILE			_T("Config\\Publish_SVN.ini")

class CConfigureSVN
{
public:
	CConfigureSVN();
	~CConfigureSVN();

public:
	BOOL LoadConfigure();
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);

private:
	VOID PrintDebug();

// Attributes
public:
	UInt32 svnRevision;
};

#endif // _CONFIGURESVN_H
