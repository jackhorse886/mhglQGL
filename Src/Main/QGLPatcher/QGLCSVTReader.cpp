#include "common.h"
#include "QGLCSVTReader.h"

CQGLCSVTReader::CQGLCSVTReader()
{
	a = SafeAllocate(TCHAR, 2048);
	b = SafeAllocate(TCHAR, 2048);
	filename = SafeAllocate(char, 1024);
	utf8file = SafeCreateObject(UTF8File);
}

CQGLCSVTReader::~CQGLCSVTReader()
{
	m_columnvalue.clear();
	m_namecolumn.clear();
	SafeDeallocate(a);
	SafeDeallocate(b);
	SafeDeallocate(filename);
	SafeDeleteObject(utf8file);
}

bool CQGLCSVTReader::Open(const TCHAR* csvfilename, bool hasMetaData)
{
	if (!utf8file )
		return false;

	isFirstLine = true;
	colid = 0;
	quotationmarkfound = 0;
	justquotationmark = 0;
	currentwordreadpos = 0;
	a[0] = 0;
	isnewline = false;
	hasmetadata = false;

	m_columnvalue.clear();
	m_namecolumn.clear();

	hasmetadata = hasMetaData;

	if( utf8file->Open(csvfilename, "rt") )
	{
		ReadMetaData();
		BufferCurrLine();
		return true;
	}else
		return false;
}

bool CQGLCSVTReader::OpenBuf(const BYTE* pData, unsigned int pDataSize, bool hasMetaData)
{
	if (!utf8file )
		return false;

	isFirstLine = true;
	colid = 0;
	quotationmarkfound = 0;
	justquotationmark = 0;
	currentwordreadpos = 0;
	a[0] = 0;
	isnewline = false;
	hasmetadata = false;

	m_columnvalue.clear();
	m_namecolumn.clear();

	hasmetadata = hasMetaData;

	if( utf8file->OpenBuf(pData, pDataSize) )
	{
		ReadMetaData();
		BufferCurrLine();
		return true;
	}else
		return false;
}

bool CQGLCSVTReader::Close()
{
	if( !utf8file )
	{
		throw;
		return false;
	}

	utf8file->Close();
	m_columnvalue.clear();
	m_namecolumn.clear();
	return true;
}

bool CQGLCSVTReader::ReadString(String fieldid, TCHAR *out, int stringLength)
{
	MapColumnValue::iterator itr = m_columnvalue.find(fieldid);
	if( itr==m_columnvalue.end() )
		return false;

	// choose smaller length for copying
	int len = MIN(itr->second.GetLength(), stringLength);
	_tcsncpy(out, itr->second.c_str(), len);
	out[len] = 0;
	return true;
}

bool CQGLCSVTReader::Read(String fieldid, Int64 &out)
{
	MapColumnValue::iterator itr = m_columnvalue.find(fieldid);
	if( itr==m_columnvalue.end() )
		return false;
	out = _ttol(itr->second.c_str());
	return true;
}

bool CQGLCSVTReader::Read(String fieldid, UInt64 &out)
{
	return Read(fieldid, (Int64&)out);
}

bool CQGLCSVTReader::Read(String fieldid, Int32 &out)
{
	MapColumnValue::iterator itr = m_columnvalue.find(fieldid);
	if( itr==m_columnvalue.end() )
		return false;
	out = _ttoi(itr->second.c_str());
	return true;
}

bool CQGLCSVTReader::Read(String fieldid, UInt32 &out)
{
	return Read(fieldid, (Int32&)out);
}

bool CQGLCSVTReader::Read(String fieldid, Int16 &out)
{
	return Read(fieldid, (Int32&)out);
}

bool CQGLCSVTReader::Read(String fieldid, UInt16 &out)
{
	return Read(fieldid, (Int32&)out);
}

bool CQGLCSVTReader::Read(String fieldid, Int8 &out)
{
	return Read(fieldid, (Int32&)out);
}

bool CQGLCSVTReader::Read(String fieldid, UInt8 &out)
{
	return Read(fieldid, (Int32&)out);
}

bool CQGLCSVTReader::Read(String fieldid, Boolean &out)
{
	return Read(fieldid, (Int32&)out);
}

bool CQGLCSVTReader::Read(String fieldid, Real64 &out)
{
	MapColumnValue::iterator itr = m_columnvalue.find(fieldid);
	if( itr==m_columnvalue.end() )
		return false;
	out = _tstof(itr->second.c_str());
	return true;
}

bool CQGLCSVTReader::Read(String fieldid, Real32 &out)
{
	MapColumnValue::iterator itr = m_columnvalue.find(fieldid);
	if( itr==m_columnvalue.end() )
		return false;
	out = (Real32)_tstof(itr->second.c_str());
	return true;
}

bool CQGLCSVTReader::MoveNext()
{
	SkipNext();

	ResetFieldValue();
	if( hasmetadata && isFirstLine )
		ReadMetaData();
	else
		BufferCurrLine();

	return true;
}

void CQGLCSVTReader::SkipNext()
{
	quotationmarkfound = 0;
	if( !isnewline )
	{
		while( (utf8file->Read(a[0]) && a[0]!='\n') || quotationmarkfound==1)
		{
			if( a[0]=='"' )
			{
				switch( quotationmarkfound )
				{
				case 0:
					quotationmarkfound = 1;
					break;
				case 1:
					quotationmarkfound = 0;
					break;
				}
			}
		}
	}
	isnewline = true;
	colid = 0;
	quotationmarkfound = 0;
	currentwordreadpos = 0;
	justquotationmark = 0;
}

bool CQGLCSVTReader::ReadMetaData()
{
	if( !hasmetadata )
		return false;

	if( !utf8file )
		throw;

	colid = 0;

	currentwordreadpos = 0;
	while( utf8file->Read(a[currentwordreadpos]) )
	{
		isnewline = false;
		
		if( a[0]=='#' ) // comment
			SkipNext();
		else
		{
			isFirstLine = false;
			if( a[currentwordreadpos]=='"' )
			{
				if( justquotationmark==1 )
				{
					justquotationmark = 0;
					a[currentwordreadpos] = '"';
					currentwordreadpos++;
				}else
				{
					justquotationmark = 1;
				}
				if( quotationmarkfound==0 )
					quotationmarkfound = 1;
				else
					quotationmarkfound = 0;
				continue;
			}else if( a[currentwordreadpos]==',' )
			{
				justquotationmark = 0;
				if( !quotationmarkfound )
				{
					a[currentwordreadpos]='\0';
					currentwordreadpos = 0;
					AddColumnName(a, colid);

					colid++;

				}else
					currentwordreadpos++;
			} else if( a[currentwordreadpos]=='\r' )
			{
				justquotationmark = 0;
				continue;
			} else if( a[currentwordreadpos]=='\n' )
			{
				if( quotationmarkfound==0 )
				{
					a[currentwordreadpos]='\0';
					isnewline = true;
					quotationmarkfound = 0;
					currentwordreadpos = 0;

					AddColumnName(a, colid);
					colid = 0;
					return true;
				}else
				{
					currentwordreadpos++;
				}
			}else
			{
				justquotationmark = 0;
				currentwordreadpos++;
			}
		}
	}
	return true;
}

void CQGLCSVTReader::AddColumnName(String name, int columnno)
{
	m_namecolumn.insert(std::make_pair(columnno, name));
}

bool CQGLCSVTReader::AnalyseCurrString(TCHAR* srcString, int len, TCHAR* composedString, int& outlen)
{
	bool quoted = false;
	int numQuote = 0;
	if( srcString[0]=='"' && srcString[len-1]=='"' )
		quoted = true;

	for( int i = 0; i<len; i++ )
	{
		if( srcString[i]=='"' )
			++numQuote;
	}
	if( quoted && numQuote%2==0 )
	{
		// finished string.  Replace all "" and outputr
		int newpos = 0;
		for( int i = 1; i<len-1; ++i )
		{
			if( srcString[i]=='"' && srcString[i+1]=='"' && i<len-2)
			{
				composedString[newpos] = '"';
				++i;
			}else
			{
				composedString[newpos] = srcString[i];
			}
			++newpos;
		}
		outlen = newpos;
		composedString[outlen] = '\0';
		return true;
	}else if( !quoted && numQuote==0 )
	{
		outlen = len;
		memcpy(composedString, srcString, len * sizeof(TCHAR));
		composedString[outlen] = '\0';
		return true;
	}else
		return false;
}

bool CQGLCSVTReader::BufferCurrLine()
{
	isFirstLine = false;
	if( !utf8file )
		throw;

	colid = 0;

	currentwordreadpos = 0;
	while( utf8file->Read(a[currentwordreadpos]) )
	{
		isnewline = false;
		
		if( a[0]=='#' ) // comment
			SkipNext();
		else
		{
			if( a[currentwordreadpos]==',' )
			{
				a[currentwordreadpos] = '\0';
				int outlen = 0;
				if( AnalyseCurrString(a, currentwordreadpos, b, outlen) )
				{
					currentwordreadpos = 0;
					SetFieldValue(colid, b);
					colid++;
				}else
				{
					a[currentwordreadpos] = ',';
					currentwordreadpos++;
				}
			} else if( a[currentwordreadpos]=='\r' )
			{
				continue;
			} else if( a[currentwordreadpos]=='\n' )
			{
				a[currentwordreadpos]='\0';
				int outlen = 0;
				if( AnalyseCurrString(a, currentwordreadpos, b, outlen) )
				{
					isnewline = true;
					currentwordreadpos = 0;
					SetFieldValue(colid, b);
					colid = 0;
					return true;
				}else
				{
					a[currentwordreadpos]='\n';
					currentwordreadpos++;
				}
			}else
			{
				currentwordreadpos++;
			}
		}
	}
	return true;
}

bool CQGLCSVTReader::SetFieldValue(int column_id, String value)
{
	String columnname;
	MapNameColumn::iterator itr = m_namecolumn.find(column_id);
	if( itr!=m_namecolumn.end() )
	{
		columnname = itr->second;
		m_columnvalue.insert(std::make_pair(columnname, value));
		return true;
	}
	return false;
}

void CQGLCSVTReader::ResetFieldValue()
{
	m_columnvalue.clear();
}

bool CQGLCSVTReader::IsLineEmpty()
{
	if( m_columnvalue.size()!=0 )
	{
		String columnname;
		MapNameColumn::iterator itr = m_namecolumn.find(0);
		if( itr!=m_namecolumn.end() )
		{
			columnname = itr->second;
			MapColumnValue::iterator colitr = m_columnvalue.find(columnname);
			if( colitr!=m_columnvalue.end() )
				return false;
		}
	}
	return true;
}