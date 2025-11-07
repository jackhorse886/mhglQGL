#pragma once
#ifndef _GAMESTRINGTABLE_H_
#define _GAMESTRINGTABLE_H_

#include <map>
#include <Reuben\System\Reuben.h>

typedef StlMap<StringA, String, std::less<StringA>, ManagedAllocator<std::pair<StringA, String> > > MapString;

class CGameStringTable
{
public:
	CGameStringTable();
	~CGameStringTable();
	String Get(StringA msgName);

	VOID Load(String filename);
	VOID LoadFromBuffer(BYTE* pData, UInt32 pDataLength);

private:
	MapString m_stringTable;
};

#endif // _GAMESTRINGTABLE_H_
