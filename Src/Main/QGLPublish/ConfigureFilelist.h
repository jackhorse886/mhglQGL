#pragma once
#ifndef _CONFIGUREFILELIST_H
#define _CONFIGUREFILELIST_H

#include "common.h"

class CConfigureFilelist
{
public:
	CConfigureFilelist();
	~CConfigureFilelist();

public:
	BOOL LoadConfigure(PCSTRING iniFilename);
	BOOL SavePatch(PCSTRING filename, UInt32 size, UInt32 crc, UInt32 vercrc);

private:
	VOID SetINIFilename(PCSTRING iniFilename)		{m_iniFilename = iniFilename;}
	String GetINIFilename()							{return m_iniFilename;}

// Attributes
public:
	String m_iniFilename;
};

#endif // _CONFIGUREFILELIST_H
