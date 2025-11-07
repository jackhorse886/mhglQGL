#pragma once
#ifndef _CUTF8FILE_H
#define _CUTF8FILE_H

#include <stdio.h>
#include <tchar.h>

#include "Reuben\System\Reuben.h"

enum UTF8_DATA_MODE
{
	UTF8_DATA_MODE_NULL = 0,
	UTF8_DATA_MODE_FILE_R,
	UTF8_DATA_MODE_FILE_RW,
	UTF8_DATA_MODE_DATA_R
};

class UTF8File
{
public:
	UTF8File();
	~UTF8File();

	int Read(TCHAR &out);
	int Write(TCHAR* src);
	
	int Open(const TCHAR* filename, const char* mode);
	int Open(const char* filename, const char* mode);
	int OpenBuf(const BYTE* pData, unsigned int pDataSize); // read only
	int Close();

	int Rewind();

private:
	int m_readmode; // 0: not initialized; 1: read-only from file; 2: read-write of file; 3: read-only from data stream

	// File mode
	FILE* f0;
	bool HeaderWritten;
	char* buf;

	// Data stream mode
	BYTE* m_Data; // for data stream only
	unsigned int m_DataSize;
	unsigned int m_CurPos;

};

#endif
