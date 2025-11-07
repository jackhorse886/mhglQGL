#pragma once
#ifndef _CQGLCSVTREADER_H
#define _CQGLCSVTREADER_H

#include "UTF8File.h"
#include "Reuben\System\Reuben.h"

typedef StlMap<int, String, std::less<int>, ManagedAllocator<std::pair<int, String> > > MapNameColumn;
typedef StlMap<String, String, std::less<String>, ManagedAllocator<std::pair<String, String> > > MapColumnValue;

class CQGLCSVTReader
{
public:
	CQGLCSVTReader();
	~CQGLCSVTReader();

	bool Open(const TCHAR* csvfilename, bool hasMetaData = false);
	bool OpenBuf(const BYTE* pData, unsigned int pDataSize, bool hasMetaData = false);
	bool Close();

	bool IsLineEmpty();
	bool ReadString(String fieldid, TCHAR *out, int stringLength);
	bool Read(String fieldid, Int64 &out);
	bool Read(String fieldid, Int32 &out);
	bool Read(String fieldid, Int16 &out);
	bool Read(String fieldid, Int8  &out);
	bool Read(String fieldid, UInt64 &out);
	bool Read(String fieldid, UInt32 &out);
	bool Read(String fieldid, UInt16 &out);
	bool Read(String fieldid, UInt8  &out);
	bool Read(String fieldid, Boolean &out);
	bool Read(String fieldid, Real32 &out);
	bool Read(String fieldid, Real64 &out);
	bool MoveNext();

private:
	void AddColumnName(String name, int columnno);
	bool BufferCurrLine();
	bool ReadMetaData();
	void ResetFieldValue();
	bool SetFieldValue(int column_id, String value);
	void SkipNext();
	bool AnalyseCurrString(TCHAR* srcString, int len, TCHAR* composedString, int& outlen);

	int colid;
	int quotationmarkfound;
	int justquotationmark;
	int currentwordreadpos;
	TCHAR* a;
	TCHAR* b;
	bool isnewline;
	bool hasmetadata;

	char* filename;
	UTF8File* utf8file;

	bool isFirstLine;
    
	MapNameColumn m_namecolumn;
	MapColumnValue m_columnvalue;
};

#endif