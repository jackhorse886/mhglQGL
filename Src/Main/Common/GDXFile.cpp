#include "common.h"
#include "GDXFile.h"
#include <Reuben/Platform/File.h>
#include "Reuben/Math/CRC.h"
typedef Reuben::Platform::File File;
typedef Reuben::Platform::FileException FileException;

CGDXFile::CGDXFile()
{
	m_revertMap.clear();
	m_bufData = NULL;
	m_filename.Empty();
}

CGDXFile::~CGDXFile()
{
	CloseFile();
}

BOOL CGDXFile::LoadFile(PCSTRING filename)
{
	TRACE_ERRORDTL_IF(GLOBAL_LOGGER, !filename, _T("CGDXFile::Open Error (1) [filename null]"));
	TRACE_ENSURE(filename);

	TRACE_ERRORDTL_IF(GLOBAL_LOGGER, !m_filename.IsEmpty(), _T("CGDXFile::Open Error (2) [File already opened]"));
	TRACE_ENSURE(m_filename.IsEmpty());

	if( !Reuben::Platform::File::IsExists(filename) )
	{
		File pFile;
		pFile.Open(filename, Reuben::Platform::File::MODE_WRITE);
		if( pFile.IsOpened() )
		{
			pFile.SetPosition(0, File::MOVE_BEGIN);
			char temp[5] = ".GDX";
			GDXHeader header;
			memcpy(header.magic, temp, 4 * sizeof(char));
			header.version		= 1;
			header.numRecord	= 0;
			header.bodyOffset	= 20;
			header.bodyCRC		= 0;
			pFile.Write((BYTE*)&header, 1 * sizeof(GDXHeader));

			pFile.Close();
		}else
		{
			TRACE_ERRORDTL_1(GLOBAL_LOGGER, _T("CGDXFile::Open Error (3) [file cannot be created] filename=%s"), filename);
			return FALSE;
		}
	}

	File pFile;
	pFile.Open(filename, Reuben::Platform::File::MODE_READ_EXISTING);
	if( pFile.IsOpened() )
	{
		pFile.SetPosition(0, File::MOVE_BEGIN);
		pFile.Read((BYTE*)&m_header, 1 * sizeof(GDXHeader));

		if( m_header.numRecord )
		{
			m_bufData = SafeAllocate(GDXToFromData, m_header.numRecord);
			TRACE_ERRORDTL_IF_1(GLOBAL_LOGGER, !m_bufData, _T("CGDXFile::Open Error (3) [Buffer allocation error] numRecord=%d"), m_header.numRecord);
			TRACE_ENSURE(m_bufData);

			pFile.Read((BYTE*)m_bufData, m_header.numRecord * sizeof(GDXToFromData));
			for( Index i = C_INDEX(0); i < m_header.numRecord; ++i )
			{
				GDXFromData tempdat;
				tempdat.SetData(m_bufData[i]);
				m_revertMap.insert(std::make_pair(m_bufData[i].to, tempdat));
			}
		}
		
		pFile.Close();

		m_filename = filename;
		return TRUE;
	}
	return FALSE;
}

BOOL CGDXFile::IsValid()
{
	TRACE_ERRORDTL_IF(GLOBAL_LOGGER, m_filename.IsEmpty(), _T("CGDXFile::IsValid Error (1) [File not opened]"));
	TRACE_ENSURE(!m_filename.IsEmpty());

	if( m_header.numRecord==0 && !m_bufData )
		return TRUE;

	UInt32 crc = 0;
	if( m_header.numRecord )
		Reuben::Math::CRC32((BYTE*)m_bufData, m_header.numRecord * sizeof(GDXToFromData), crc);
	if( m_header.bodyCRC==crc )
		return TRUE;
	return FALSE;
}

BOOL CGDXFile::SaveAs(PCSTRING filename)
{
	TRACE_ERRORDTL_IF(GLOBAL_LOGGER, !filename, _T("CGDXFile::SaveAs Error (1) [filename null]"));
	TRACE_ENSURE(filename);

	TRACE_ERRORDTL_IF(GLOBAL_LOGGER, m_filename.IsEmpty(), _T("CGDXFile::SaveAs Error (2) [File not opened]"));
	TRACE_ENSURE(!m_filename.IsEmpty());

	File pFile;
	pFile.Open(filename, Reuben::Platform::File::MODE_WRITE);
	if( pFile.IsOpened() )
	{
		pFile.SetLength(0);
	
		UInt32 crc = 0;
		if( m_header.numRecord )
			Reuben::Math::CRC32((BYTE*)m_bufData, m_header.numRecord * sizeof(GDXToFromData), crc);
		m_header.bodyCRC = crc;

		pFile.Write((BYTE*)&m_header, sizeof(GDXHeader));
		if( m_header.numRecord )
			pFile.Write((BYTE*)m_bufData, m_header.numRecord * sizeof(GDXToFromData));
		pFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL CGDXFile::AppendSave(UInt32 from, UInt32 to, UInt32 crc, UInt32 size)
{
	TRACE_ERRORDTL_IF(GLOBAL_LOGGER, m_filename.IsEmpty(), _T("CGDXFile::Recreate Error (1) [File not opened]"));
	TRACE_ENSURE(!m_filename.IsEmpty());

	File pFile;
	pFile.Open(GetFilename().c_str(), Reuben::Platform::File::MODE_WRITE_EXISTING);
	if( pFile.IsOpened() )
	{
		m_header.numRecord++;

		GDXToFromData pushData(from, to, crc, size);
		// memory rearrangement
		GDXToFromData* buf = SafeAllocate(GDXToFromData, m_header.numRecord);
		if( m_bufData )
		{
			memcpy(buf, m_bufData, (m_header.numRecord - 1) * sizeof(GDXToFromData));
			SafeDeallocate(m_bufData);
		}
		m_bufData = buf;

		m_bufData[m_header.numRecord-1].SetData(from, to, crc, size);
		// map rearrangement
		GDXFromData memData;
		memData.SetData(m_bufData[m_header.numRecord-1]);
		m_revertMap.insert(std::make_pair(to, memData));

		// header recalculation
		UInt32 crc = 0;
		if( m_header.numRecord )
			Reuben::Math::CRC32((BYTE*)m_bufData, m_header.numRecord * sizeof(GDXToFromData), crc);
		m_header.bodyCRC = crc;

		// file write
		pFile.SetPosition(0, File::MOVE_END);
		pFile.Write((BYTE*)&pushData, sizeof(GDXToFromData));
		pFile.SetPosition(0, File::MOVE_BEGIN);
		pFile.Write((BYTE*)&m_header, sizeof(GDXHeader));

		pFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL CGDXFile::DecreaseByOne()
{
	TRACE_ERRORDTL_IF(GLOBAL_LOGGER, m_filename.IsEmpty(), _T("CGDXFile::DecreaseByOne Error (1) [File not opened]"));
	TRACE_ENSURE(!m_filename.IsEmpty());

	if( !m_header.numRecord )
		return TRUE;

	File pFile;
	pFile.Open(GetFilename().c_str(), Reuben::Platform::File::MODE_WRITE_EXISTING);
	if( pFile.IsOpened() )
	{
		m_header.numRecord--;

		// memory rearrangement
		GDXToFromData popData(m_bufData[m_header.numRecord]);
		if( m_header.numRecord )
		{
			GDXToFromData* buf = SafeAllocate(GDXToFromData, m_header.numRecord);
			memcpy(buf, m_bufData, m_header.numRecord * sizeof(GDXToFromData));
			SafeDeallocate(m_bufData);
			m_bufData = buf;
		}else
		{
			SafeDeallocate(m_bufData);
			m_bufData = NULL;
		}

		// map rearrangement
		for( RevertMap::iterator itr = m_revertMap.begin(); itr != m_revertMap.end(); ++itr )
		{
			CONST GDXFromData& dat = itr->second;
			CONST UInt32& to = itr->first;
			if( dat.from==popData.from && to==popData.to )
			{
				m_revertMap.erase(itr);
				break;
			}
		}

		// header recalculation
		UInt32 crc = 0;
		if( m_header.numRecord )
			Reuben::Math::CRC32((BYTE*)m_bufData, m_header.numRecord * sizeof(GDXToFromData), crc);
		m_header.bodyCRC = crc;

		// file write
		pFile.SetLength(pFile.GetLength() - sizeof(GDXToFromData));
		pFile.SetPosition(0, File::MOVE_BEGIN);
		pFile.Write((BYTE*)&m_header, sizeof(GDXHeader));

		pFile.Close();
		return TRUE;
	}
	return FALSE;
}

VOID CGDXFile::CloseFile()
{
	m_filename.Empty();
	m_revertMap.clear();
	SafeDeallocate(m_bufData);
}

UInt32 CGDXFile::GetFrom(UInt32 to)
{
	RevertMap::iterator itr = m_revertMap.find(to);
	if( itr != m_revertMap.end() )
		return itr->second.from;
	return 0;
}


UInt32 CGDXFile::GetCRCFrom(UInt32 to)
{
	RevertMap::iterator itr = m_revertMap.find(to);
	if( itr != m_revertMap.end() )
		return itr->second.crc;
	return 0;
}
UInt32 CGDXFile::GetSizeFrom(UInt32 to)
{
	RevertMap::iterator itr = m_revertMap.find(to);
	if( itr != m_revertMap.end() )
		return itr->second.size;
	return 0;
}
