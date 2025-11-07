#pragma once
#ifndef _CONFIGUREVERSION_H
#define _CONFIGUREVERSION_H

#include "common.h"

typedef StlMap<UInt32, String, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, String> > > PatchMajorVerMap;
typedef StlMap<UInt32, String, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, String> > > PatchSuffixMap;

class CConfigureVersion
{
public:
	CConfigureVersion();
	~CConfigureVersion();

// initialization
public:
	BOOL LoadConfigure();
	BOOL ReloadConfigure();
	VOID FreeConfigure();

public:
	VOID GetMajorVersion(UInt32 curVersion, String& outMajor, UInt32& outSinceVer);
	VOID GetSuffixVersion(UInt32 curVersion, String& outSuffix, UInt32& outSinceVer);

private:
	VOID PrintDebug();
	VOID LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	VOID LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath);
	BOOL CheckFileValid();

// Attributes
private:
	PatchMajorVerMap	m_patchMajorMap;
	PatchSuffixMap		m_patchSuffixMap;
};

#endif // _CONFIGUREVERSION_H
