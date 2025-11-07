#pragma once
#ifndef _CONFIGUREREVERTLIST_H
#define _CONFIGUREREVERTLIST_H

#include "common.h"

class CConfigureRevertlist
{
public:
	CConfigureRevertlist();
	~CConfigureRevertlist();

public:
	BOOL LoadConfigure(PCSTRING iniFilename);
	BOOL SavePatch(PCSTRING filename, UInt32 size, UInt32 crc, UInt32 revertFrom, UInt32 revertTo, UInt32 revertFromVCRC, UInt32 revertToVCRC);

private:
	VOID SetINIFilename(PCSTRING iniFilename)		{m_iniFilename = iniFilename;}
	String GetINIFilename()							{return m_iniFilename;}

// Attributes
public:
	String m_iniFilename;
};

#endif // _CONFIGUREFILELIST_H
