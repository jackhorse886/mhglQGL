#pragma once
#ifndef _GDXFILE_H_
#define _GDXFILE_H_

struct GDXHeader
{
	char magic[4];
	UInt32 version;
	UInt32 bodyOffset;
	UInt32 numRecord;
	UInt32 bodyCRC;
};

struct GDXToFromData
{
	UInt32 from;
	UInt32 to;
	UInt32 crc;
	UInt32 size;

	GDXToFromData(UInt32 filefrom, UInt32 fileto, UInt32 filecrc, UInt32 filesize)
	{
		from	= filefrom;
		to		= fileto;
		crc		= filecrc;
		size	= filesize;
	};

	VOID SetData(UInt32 filefrom, UInt32 fileto, UInt32 filecrc, UInt32 filesize)
	{
		from	= filefrom;
		to		= fileto;
		crc		= filecrc;
		size	= filesize;
	};

	VOID SetData(GDXToFromData dat)
	{
		from	= dat.from;
		to		= dat.to;
		crc		= dat.crc;
		size	= dat.size;
	};
};

struct GDXFromData
{
	UInt32 from;
	UInt32 crc;
	UInt32 size;

	VOID SetData(GDXToFromData dat)
	{
		from	= dat.from;
		crc		= dat.crc;
		size	= dat.size;
	};
};

typedef StlMap<UInt32, GDXFromData, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, GDXFromData> > > RevertMap;

class CGDXFile
{
public:
	CGDXFile();
	~CGDXFile();

public:
	BOOL LoadFile(PCSTRING filename);
	VOID CloseFile();

	BOOL IsValid();								// crc check
	BOOL SaveAs(PCSTRING filename);				// on invalidate: backup and give up the old records
	BOOL AppendSave(UInt32 from, UInt32 to, UInt32 crc, UInt32 size);	// increase 1 data + header change
	BOOL DecreaseByOne();						// decrease 1 data + header change
	UInt32 GetFrom(UInt32 to);					// get version transition FROM
	UInt32 GetCRCFrom(UInt32 to);
	UInt32 GetSizeFrom(UInt32 to);
	CONST String& GetFilename()		{return m_filename;}

private:
	GDXToFromData*	m_bufData;
	GDXHeader		m_header;
	RevertMap		m_revertMap;
	String			m_filename;
};

#endif	// _GDXFILE_H_
