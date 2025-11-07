#pragma once
#ifndef _GDFSTREAM_H_
#define _GDFSTREAM_H_

//#include "GDFFile.h"
#include "GDFStreamData.h"

class GDFFile;

enum GDFPos
{
	GDF_BEGIN = 0,
	GDF_CURRENT,
	GDF_END
};

class GDFStream
{
public:
	GDFStream(GDFFile* gdffile, UInt32 blockIndex, UInt32 filesize);
	~GDFStream();

public:
	BOOL Read(BYTE* buf, UInt32 readlen, UInt32& outlen, UInt32 maxLength);
	VOID MoveFirst();
	VOID MovePos(GDFPos relativePos, UInt32 pos);
	VOID MoveLast();
	BOOL IsBegin();
	BOOL IsEnd();
	GDFStreamData* GetStreamData() {return &m_sd;};
	VOID SetStreamData_BlockIndex(UInt32 blockIndex) {m_sd.m_blockIndex = blockIndex;};
	UInt32 GetFileSize() {return m_FileSize;};

private: 
	GDFStreamData m_sd;
	GDFFile* m_gdffile;
	UInt32 m_CurPos;
	UInt32 m_FileSize;
	UInt32 m_BeginOffset;
	UInt32 m_EndOffset;
};

#endif
