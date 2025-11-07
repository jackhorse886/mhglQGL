#pragma once
#ifndef _CONFIGURESUMMARY_H
#define _CONFIGURESUMMARY_H

#include "common.h"

class CConfigureSummary
{
public:
	CConfigureSummary();
	~CConfigureSummary();

public:
	BOOL LoadConfigure(PCSTRING iniFilename);
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	BOOL SaveVersion(UInt32 value, UInt32 versionCRC);
	BOOL SavePatch(UInt32 sourceVersion, PCSTRING filename);

private:
	VOID PrintDebug();
	VOID SetINIFilename(PCSTRING iniFilename)		{m_iniFilename = iniFilename;}
	String GetINIFilename()							{return m_iniFilename;}

// Attributes
public:
	UInt32 version;

private:
	String m_iniFilename;
};

#endif // _CONFIGURESUMMARY_H
