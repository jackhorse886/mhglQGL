#include "GameStringTable.h"
#include "QGLCSVTReader.h"

CGameStringTable::CGameStringTable()
{
}

CGameStringTable::~CGameStringTable()
{
}

#define MAX_TEXT_LEN 512

VOID CGameStringTable::Load(String filename)
{
	CQGLCSVTReader reader;
	TCHAR tmpchr[MAX_TEXT_LEN];
	TCHAR tmpchr2[MAX_TEXT_LEN];
	if( reader.Open(filename.c_str(), true) )
	{
		try{
			while( !reader.IsLineEmpty()  )
			{
				reader.ReadString(_T("MSG_KEY"), tmpchr, MAX_TEXT_LEN);
				reader.ReadString(_T("MSG_CONTENT"), tmpchr2, MAX_TEXT_LEN);
				StringA temp = StringA(tmpchr);
				String temp2 = String(tmpchr2);
				m_stringTable.insert(std::make_pair(temp, temp2));
				reader.MoveNext();
			}
		}
		catch(...)
		{
//			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("Err: GameStringTable::Load: CSV reading error due to \
//						 column format or missing MoveNext(), %s"), filename);
		}
		reader.Close();
	}
}

VOID CGameStringTable::LoadFromBuffer(BYTE* pData, UInt32 pDataLength)
{
	CQGLCSVTReader reader;
	TCHAR tmpchr[MAX_TEXT_LEN];
	TCHAR tmpchr2[MAX_TEXT_LEN];
	if( pDataLength > 0 )
	{
		if( reader.OpenBuf(pData, pDataLength, true) )
		{
			try{
				while( !reader.IsLineEmpty()  )
				{
					reader.ReadString(_T("MSG_KEY"), tmpchr, MAX_TEXT_LEN);
					reader.ReadString(_T("MSG_CONTENT"), tmpchr2, MAX_TEXT_LEN);
					StringA temp = StringA(tmpchr);
					String temp2 = String(tmpchr2);
					m_stringTable.insert(std::make_pair(temp, temp2));
					reader.MoveNext();
				}
			}
			catch(...)
			{
			}
			reader.Close();
		}
	}
}

String CGameStringTable::Get(StringA msgName)
{
	MapString::iterator itr = m_stringTable.find(msgName);
	if( itr!=m_stringTable.end() )
		return itr->second;
	String a;
	return a;
}